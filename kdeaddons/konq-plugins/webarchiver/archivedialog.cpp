/*
   Copyright (C) 2001 Andreas Schlapbach <schlpbch@iam.unibe.ch>
   Copyright (C) 2003 Antonio Larrosa <larrosa@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "archivedialog.h"
#include <qwidget.h>
#include <khtml_part.h>
#include "archiveviewbase.h"
#include <kinstance.h>
#include <ktempfile.h>
#include <ktar.h>

#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kpassivepopup.h>
#include <klocale.h>
#include <kio/netaccess.h>
#include <khtml_part.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kactivelabel.h>
#include <qstylesheet.h>
#include <qiodevice.h>
#include <klistview.h>
#include <kio/job.h>
#include <kapplication.h>
#include <kurllabel.h>
#include <kprogress.h>
#include <kstringhandler.h>
#include <qpushbutton.h>

#undef DEBUG_WAR

#define CONTENT_TYPE "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"

ArchiveDialog::ArchiveDialog(QWidget *parent, const QString &filename,
        KHTMLPart *part) :
        KDialogBase(parent, "WebArchiveDialog", false, i18n("Web Archiver"),
          KDialogBase::Ok | KDialogBase::Cancel | KDialogBase::User1 ),
        m_bPreserveWS(false), m_tmpFile(0), m_url(part->url())
{
   m_widget=new ArchiveViewBase(this);
   setMainWidget(m_widget);
   setWFlags(getWFlags() | WDestructiveClose);

   m_widget->urlLabel->setText(QString("<a href=\"")+m_url.url()+"\">"+KStringHandler::csqueeze( m_url.url(), 80 )+"</a>");
   m_widget->targetLabel->setText(QString("<a href=\"")+filename+"\">"+KStringHandler::csqueeze( filename, 80 )+"</a>");

   if(part->document().ownerDocument().isNull())
      m_document = part->document();
   else
      m_document = part->document().ownerDocument();

   enableButtonOK( false );
   showButton( KDialogBase::User1, false );
   setButtonOK( KStdGuiItem::close() );

   m_tarBall = new KTar(filename,"application/x-gzip");
}

void ArchiveDialog::archive()
{
   m_iterator=0;
   m_currentLVI=0;
   if (m_tarBall->open(IO_WriteOnly)) {
#ifdef DEBUG_WAR
      kdDebug(90110) << "Web Archive opened " << endl;
#endif

      m_linkDict.insert(QString("index.html"), QString(""));
      saveFile("index.html");

   } else {
     const QString title = i18n( "Unable to Open Web-Archive" );
     const QString text = i18n( "Unable to open \n %1 \n for writing." ).arg(m_tarBall->fileName());
     KMessageBox::sorry( 0L, text, title );
   }
}

ArchiveDialog::~ArchiveDialog()
{
   delete m_tarBall;
}

/* Store the HTMLized DOM-Tree to a temporary file and add it to the Tar-Ball */

void ArchiveDialog::saveFile( const QString&)
{
   KTempFile tmpFile;
   if (!(tmpFile.status())) {

     QString temp;

     m_state=Retrieving;
     QTextStream *tempStream = new QTextStream(&temp, IO_ReadOnly);

     saveToArchive(tempStream);

     delete tempStream;

     m_downloadedURLDict.clear();

     m_state=Downloading;
     m_widget->progressBar->setTotalSteps(m_urlsToDownload.count());
     m_widget->progressBar->setProgress(0);
     downloadNext();

   } else {
     const QString title = i18n( "Could Not Open Temporary File" );
     const QString text = i18n( "Could not open a temporary file" );
     KMessageBox::sorry( 0, text, title );
   }
}

void ArchiveDialog::setSavingState()
{
   KTempFile tmpFile;
   QTextStream* textStream = tmpFile.textStream();
   textStream->setEncoding(QTextStream::UnicodeUTF8);

   m_widget->progressBar->setProgress(m_widget->progressBar->totalSteps());

   m_state=Saving;
   saveToArchive(textStream);

   tmpFile.close();

   QString fileName="index.html";
   QFile file(tmpFile.name());
   file.open(IO_ReadOnly);
   m_tarBall->writeFile(fileName, QString::null, QString::null, file.size(), file.readAll());
#ifdef DEBUG_WAR
   kdDebug(90110) << "HTML-file written: " << fileName << endl;
#endif
   file.close();

  // Cleaning up
   file.remove();
   m_tarBall->close();

   KPassivePopup::message( m_url.prettyURL() , i18n( "Archiving webpage completed." ), this );

   enableButtonOK(true);
   setEscapeButton(Ok);
   actionButton(Ok)->setFocus();
   enableButtonCancel(false);
}

/* Recursively travers the DOM-Tree */

void ArchiveDialog::saveToArchive(QTextStream* _textStream)
{
   if (!_textStream) return;

   // Add a doctype

   (*_textStream) <<"<!-- saved from:" << endl << m_url.url() << " -->" << endl;

   try
   {
      saveArchiveRecursive(m_document.documentElement(), m_url, _textStream, 0);
   }
   catch (...)
   {
      kdDebug(90110) << "exception" << endl;
   }
}

static bool hasAttribute(const DOM::Node &pNode, const QString &attrName, const QString &attrValue)
{
   const DOM::Element element = (const DOM::Element) pNode;
   DOM::Attr attr;
   DOM::NamedNodeMap attrs = element.attributes();
   unsigned long lmap = attrs.length();
   for( unsigned int j=0; j<lmap; j++ ) {
      attr = static_cast<DOM::Attr>(attrs.item(j));
      if ((attr.name().string().upper() == attrName) &&
          (attr.value().string().upper() == attrValue))
         return true;
   }
   return false;
}

static bool hasChildNode(const DOM::Node &pNode, const QString &nodeName)
{
   DOM::Node child;
   try
   {
     // We might throw a DOM exception
     child = pNode.firstChild();
   }
   catch (...)
   {
     // No children, stop recursion here
     child = DOM::Node();
   }

   while(!child.isNull()) {
     if (child.nodeName().string().upper() == nodeName)
        return true;
     child = child.nextSibling();
   }
   return false;
}

/* Transform DOM-Tree to HTML */

void ArchiveDialog::saveArchiveRecursive(const DOM::Node &pNode, const KURL& baseURL,
					     QTextStream* _textStream, int indent)
{
   const QString nodeNameOrig(pNode.nodeName().string());
   const QString nodeName(pNode.nodeName().string().upper());
   QString text;
   QString strIndent;
   strIndent.fill(' ', indent);
   const DOM::Element element = (const DOM::Element) pNode;
   DOM::Node child;

   if ( !element.isNull() ) {
      if (nodeName.at(0)=='-') {
        /* Don't save khtml internal tags '-konq..'
         * Approximating it with <DIV>
         */
        text += "<DIV> <!-- -KONQ_BLOCK -->";
      } else if (nodeName == "BASE") {
        /* Skip BASE, everything is relative to index.html
         * Saving SCRIPT but they can cause trouble!
         */
      } else if ((nodeName == "META") && hasAttribute(pNode, "HTTP-EQUIV", "CONTENT-TYPE")) {
        /* Skip content-type meta tag, we provide our own.
         */
      } else {
        if (!m_bPreserveWS) {
          if (nodeName == "PRE") {
            m_bPreserveWS = true;
          }
          text = strIndent;
        }
        text += "<" + nodeNameOrig;
        QString attributes;
        QString attrNameOrig, attrName, attrValue;
        DOM::Attr attr;
        DOM::NamedNodeMap attrs = element.attributes();
        unsigned long lmap = attrs.length();
        for( unsigned int j=0; j<lmap; j++ ) {
           attr = static_cast<DOM::Attr>(attrs.item(j));
           attrNameOrig = attr.name().string();
           attrName = attrNameOrig.upper();
           attrValue = attr.value().string();

#if 0
        if ((nodeName == "FRAME" || nodeName == "IFRAME") && attrName == "SRC") {
          //attrValue = handleLink(baseURL, attrValue);

          /* Going recursively down creating a DOM-Tree for the Frame, second Level of recursion */
          //## Add Termination criteria, on the other hand frames are not indefinetly nested, are they :)

          KHTMLPart* part = new KHTMLPart();
          KURL absoluteURL = getAbsoluteURL(baseURL, attrValue);
          part->openURL(absoluteURL);
          saveFile(getUniqueFileName(absoluteURL.fileName()), part);
          delete part;

        } else if
#endif
           if ((nodeName == "LINK" && attrName == "HREF") || // Down load stylesheets, js-script, ..
             ((nodeName == "FRAME" || nodeName == "IFRAME") && attrName == "SRC") ||
             ((nodeName == "IMG" || nodeName == "INPUT" || nodeName == "SCRIPT") && attrName == "SRC") ||
             ((nodeName == "BODY" || nodeName == "TABLE" || nodeName == "TH" || nodeName == "TD") && attrName == "BACKGROUND")) {
           // Some people use carriage return in file names and browsers support that!
              attrValue = handleLink(baseURL, attrValue.replace(QRegExp("\\s"), ""));
           }
        /*
         * ## Make recursion level configurable
         */
        /*
        } else if (nodeName == "A" && attrName == "HREF") {
           attrValue = handleLink(baseURL, attrValue);
        */

           attributes += " " + attrName + "=\"" + attrValue + "\"";
        }
        if (!(attributes.isEmpty())){
  	   text += " ";
        }
        text += attributes.simplifyWhiteSpace();
        text += ">";

        if (nodeName == "HTML") {
          /* Search for a HEAD tag, if not found, generate one.
           */
          if (!hasChildNode(pNode, "HEAD"))
            text += "\n" + strIndent + "  <HEAD>" CONTENT_TYPE "</HEAD>";
        }
        else if (nodeName == "HEAD") {
          text += "\n" + strIndent + "  " + CONTENT_TYPE;
        }
     }
   } else {
     const QString& nodeValue(pNode.nodeValue().string());
     if (!(nodeValue.isEmpty())) {
     // Don't escape < > in JS or CSS
       QString parentNodeName = pNode.parentNode().nodeName().string().upper();
       if (parentNodeName == "STYLE") {
          text = analyzeInternalCSS(baseURL, pNode.nodeValue().string());
       } else if (m_bPreserveWS) {
          text = QStyleSheet::escape(pNode.nodeValue().string());
       } else if (parentNodeName == "SCRIPT") {
          text = pNode.nodeValue().string();
       } else {
          text = strIndent + QStyleSheet::escape(pNode.nodeValue().string());
       }
     }
   }

#ifdef DEBUG_WAR
   kdDebug(90110) << "text:" << text << endl;
#endif
   if (!(text.isEmpty())) {
     (*_textStream) << text;
     if (!m_bPreserveWS) {
       (*_textStream) << endl;
     }
   }

   try
   {
     // We might throw a DOM exception
     child = pNode.firstChild();
   }
   catch (...)
   {
     // No children, stop recursion here
     child = DOM::Node();
   }

   while(!child.isNull()) {
     saveArchiveRecursive(child, baseURL, _textStream, indent+2);
     child = child.nextSibling();
   }

   if (!(element.isNull())) {
     if (nodeName == "AREA" || nodeName == "BASE" || nodeName == "BASEFONT" ||
	nodeName == "BR" || nodeName == "COL" || nodeName == "FRAME" ||
	nodeName == "HR" || nodeName == "IMG" || nodeName == "INPUT" ||
	nodeName == "ISINDEX" || nodeName == "META" || nodeName == "PARAM") {

      /* Closing Tag is forbidden, see HTML 4.01 Specs: Index of Elements  */

     } else {
        if (!m_bPreserveWS) {
           text = strIndent;
        } else {
           text ="";
        }
        if (nodeName.at(0)=='-') {
           text += "</DIV> <!-- -KONQ_BLOCK -->";
        } else {
	   text += "</" + pNode.nodeName().string() + ">";
	   if (nodeName == "PRE") {
	      m_bPreserveWS = false;
           }
        }
#ifdef DEBUG_WAR
        kdDebug(90110) << text << endl;
#endif
        if (!(text.isEmpty())) {
           (*_textStream) << text;
	   if (!m_bPreserveWS) {
	      (*_textStream) << endl;
	   }
        }
     }
   }
}

/* Extract the URL, download it's content and return an unique name for the link */

QString ArchiveDialog::handleLink(const KURL& _url, const QString& _link)
{
   KURL url(getAbsoluteURL(_url, _link));

   QString tarFileName;
   if (kapp->authorizeURLAction("redirect", _url, url))
   {
       if (m_state==Retrieving)
           m_urlsToDownload.append(url);
       else if (m_state==Saving)
           tarFileName = m_downloadedURLDict[url.url()];
   }

   return tarFileName;
}

void ArchiveDialog::downloadNext()
{
   if (m_iterator>=m_urlsToDownload.count())
   {
      // We've already downloaded all the files we wanted, let's save them
      setSavingState();
      return;
   }

   KURL url=m_urlsToDownload[m_iterator];

#ifdef DEBUG_WAR
   kdDebug(90110) << "URL : " << url.url() << endl;
#endif
   QString tarFileName;

  // Only download file once
   if (m_downloadedURLDict.contains(url.url())) {
      tarFileName = m_downloadedURLDict[url.url()];
#ifdef DEBUG_WAR
      kdDebug(90110) << "File already downloaded: " << url.url()
		<< m_downloadedURLDict.count() << endl;
#endif
      m_iterator++;
      downloadNext();
      return;
   } else {

      // Gets the name of a temporary file into m_tmpFileName
      delete m_tmpFile;
      m_tmpFile=new KTempFile();
      m_tmpFile->close();
      QFile::remove(m_tmpFile->name());
      kdDebug(90110) << "downloading: " << url.url() <<  " to: " << m_tmpFile->name() << endl;
      KURL dsturl;
      dsturl.setPath(m_tmpFile->name());
      KIO::Job *job=KIO::file_copy(url, dsturl, -1, false, false, false);
      job->addMetaData("cache", "cache"); // Use entry from cache if available.
      connect(job, SIGNAL(result( KIO::Job *)), this, SLOT(finishedDownloadingURL( KIO::Job *)) );

      m_currentLVI=new QListViewItem(m_widget->listView, url.prettyURL());
      m_widget->listView->insertItem( m_currentLVI );
      m_currentLVI->setText(1,i18n("Downloading"));
   }
#ifdef DEBUG_WAR
   kdDebug(90110) << "TarFileName: [" << tarFileName << "]" << endl << endl;
#endif
}

void ArchiveDialog::finishedDownloadingURL( KIO::Job *job )
{
   if ( job->error() )
   {
//     QString s=job->errorString();
     m_currentLVI->setText(1,i18n("Error"));
   }
   else
     m_currentLVI->setText(1,i18n("Ok"));

   m_widget->progressBar->advance(1);


   KURL url=m_urlsToDownload[m_iterator];

   QString tarFileName = getUniqueFileName(url.fileName());

   // Add file to Tar-Ball
   QFile file(m_tmpFile->name());
   file.open(IO_ReadOnly);
   m_tarBall->writeFile(tarFileName, QString::null, QString::null, file.size(), file.readAll());
   file.close();
   m_tmpFile->unlink();
   delete m_tmpFile;
   m_tmpFile=0;

   // Add URL to downloaded URLs

   m_downloadedURLDict.insert(url.url(), tarFileName);
   m_linkDict.insert(tarFileName, QString(""));

   m_iterator++;
   downloadNext();
}

/* Create an absolute URL for download */

KURL ArchiveDialog::getAbsoluteURL(const KURL& _url, const QString& _link)
{
   // Does all the magic for me
   return KURL(_url, _link);
}

/* Adds an id to a fileName to make it unique relative to the Tar-Ball */

QString ArchiveDialog::getUniqueFileName(const QString& fileName)
{
  // Name clash -> add unique id
   static int id=2;
   QString uniqueFileName(fileName);

#ifdef DEBUG_WAR
   kdDebug(90110) << "getUniqueFileName(..): [" << fileName << "]" << endl;
#endif

   while (uniqueFileName.isEmpty() || m_linkDict.contains(uniqueFileName))
      uniqueFileName = QString::number(id++) + fileName;

   return uniqueFileName;
}

/* Search for Images in CSS, extract them and adjust CSS */

QString ArchiveDialog::analyzeInternalCSS(const KURL& _url, const QString& string)
{
#ifdef DEBUG_WAR
   kdDebug ()  << "analyzeInternalCSS" << endl;
#endif

   QString str(string);
   int pos = 0;
   int startUrl = 0;
   int endUrl = 0;
   int length = string.length();
   while (pos < length && pos >= 0) {
      pos = str.find("url(", pos);
      if (pos!=-1) {
         pos += 4; // url(

         if (str[pos]=='"' || str[pos]=='\'') // CSS 'feature'
	    pos++;
         startUrl = pos;
         pos = str.find(")",startUrl);
         endUrl = pos;
         if (str[pos-1]=='"' || str[pos-1]=='\'') // CSS 'feature'
            endUrl--;
         QString url = str.mid(startUrl, endUrl-startUrl);

#ifdef DEBUG_WAR
         kdDebug () << "url: " << url << endl;
#endif

         url = handleLink(_url, url);

#ifdef DEBUG_WAR
         kdDebug () << "url: " << url << endl;
#endif

         str = str.replace(startUrl, endUrl-startUrl, url);
         pos++;
      }
   }
   return str;
}

#include "archivedialog.moc"
