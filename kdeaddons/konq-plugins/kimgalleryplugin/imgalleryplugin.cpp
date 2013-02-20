/* This file is part of the KDE project

Copyright (C) 2001, 2003 Lukas Tinkl <lukas@kde.org>
Andreas Schlapbach <schlpbch@iam.unibe.ch>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#include <qdir.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qfont.h>
#include <qdatetime.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qprogressdialog.h>
#include <qtextcodec.h>
#include <qstylesheet.h>

#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcharsets.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kapplication.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kio/netaccess.h>
#include <kpushbutton.h>

#include "imgallerydialog.h"
#include "imgalleryplugin.h"

typedef KGenericFactory<KImGalleryPlugin> KImGalleryPluginFactory;
K_EXPORT_COMPONENT_FACTORY( libkimgallery, KImGalleryPluginFactory( "imgalleryplugin" ) )

KImGalleryPlugin::KImGalleryPlugin( QObject* parent, const char* name, const QStringList & )
        : KParts::Plugin( parent, name ), m_commentMap(0)
{
    new KAction( i18n( "&Create Image Gallery..." ), "imagegallery", CTRL+Key_I, this,
                 SLOT( slotExecute() ), actionCollection(), "create_img_gallery" );
}

void KImGalleryPlugin::slotExecute()
{
    m_progressDlg=0L;
    if ( !parent() || !parent()->inherits("KonqDirPart"))
    {
        KMessageBox::sorry( 0L, i18n("Could not create the plugin, please report a bug."));
        return;
    }
    m_part = static_cast<KonqDirPart *>(parent());
    if (!m_part->url().isLocalFile()) {	//TODO support remote URLs too?
        KMessageBox::sorry(m_part->widget(), i18n("Creating an image gallery works only on local folders."));
        return;
    }
    kdDebug(90170) << "dialog is ok" << endl;
    m_configDlg = new KIGPDialog(m_part->widget(), m_part->url().path(+1));

    if ( m_configDlg->exec() == QDialog::Accepted ) {
        kdDebug(90170) << "dialog is ok" << endl;
        m_configDlg->writeConfig();
        m_copyFiles = m_configDlg->copyOriginalFiles();
        m_recurseSubDirectories = m_configDlg->recurseSubDirectories();
        m_useCommentFile = m_configDlg->useCommentFile();
        m_imagesPerRow = m_configDlg->getImagesPerRow();

        KURL url(m_configDlg->getImageName());
        if ( !url.isEmpty() && url.isValid()) {
            m_progressDlg = new QProgressDialog(m_part->widget(), "progressDlg", true );
            QObject::connect(m_progressDlg, SIGNAL( cancelled() ), this, SLOT( slotCancelled() ) );

            m_progressDlg->setLabelText( i18n("Creating thumbnails") );
            m_progressDlg->setCancelButton(new KPushButton(KStdGuiItem::cancel(),m_progressDlg));
            m_cancelled = false;
            m_progressDlg->show();
            if ( createHtml( url, m_part->url().path(), m_configDlg->recursionLevel() > 0 ? m_configDlg->recursionLevel() + 1 : 0 , m_configDlg->getImageFormat()) ) {
                kapp->invokeBrowser(url.url());	// Open a browser to show the result
            } else {
                deleteCancelledGallery(url, m_part->url().path(), m_configDlg->recursionLevel() > 0 ? m_configDlg->recursionLevel() + 1 : 0, m_configDlg->getImageFormat());
            }
        }
    } else {
        kdDebug(90170) << "dialog is not ok" << endl;
    }
    delete m_progressDlg;
}

bool KImGalleryPlugin::createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName)
{
    if (!thumb_dir.exists()) {
        thumb_dir.setPath( imgGalleryDir);
        if (!(thumb_dir.mkdir(dirName, false))) {
            KMessageBox::sorry(m_part->widget(), i18n("Couldn't create folder: %1").arg(thumb_dir.path()));
            return false;
        } else {
            thumb_dir.setPath( imgGalleryDir + "/" + dirName + "/" );
            return true;
        }
    } else {
        return true;
    }
}

void KImGalleryPlugin::createHead(QTextStream& stream)
{
    const QString chsetName = QTextCodec::codecForLocale()->mimeName();

    stream << "<?xml version=\"1.0\" encoding=\"" +  chsetName + "\" ?>" << endl;
    stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl;
    stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;
    stream << "<head>" << endl;
    stream << "<title>" << QStyleSheet::escape(m_configDlg->getTitle()) << "</title>" << endl;
    stream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=" << chsetName << "\"/>" << endl;
    stream << "<meta name=\"GENERATOR\" content=\"KDE Konqueror KImgallery plugin version " KDE_VERSION_STRING "\"/>" << endl;
    createCSSSection(stream);
    stream << "</head>" << endl;
}

void KImGalleryPlugin::createCSSSection(QTextStream& stream)
{
    const QString backgroundColor = m_configDlg->getBackgroundColor().name();
    const QString foregroundColor = m_configDlg->getForegroundColor().name();
    //adding a touch of style
    stream << "<style type='text/css'>\n";
    stream << "BODY {color: " << foregroundColor << "; background: " << backgroundColor << ";" << endl;
    stream << "          font-family: " << m_configDlg->getFontName() << ", sans-serif;" << endl;
    stream << "          font-size: " << m_configDlg->getFontSize() << "pt; margin: 8%; }" << endl;
    stream << "H1       {color: " << foregroundColor << ";}" << endl;
    stream << "TABLE    {text-align: center; margin-left: auto; margin-right: auto;}" << endl;
    stream << "TD       { color: " << foregroundColor << "; padding: 1em}" << endl;
    stream << "IMG      { border: 1px solid " << foregroundColor << "; }" << endl;
    stream << "</style>" << endl;
}


QString KImGalleryPlugin::extension(const QString& imageFormat)
{
	if (imageFormat == "PNG")
		return ".png";
	if (imageFormat == "JPEG")
		return ".jpg";
	Q_ASSERT(false);
	return QString::null;
}

void KImGalleryPlugin::createBody(QTextStream& stream, const QString& sourceDirName, const QStringList& subDirList,
                                  const QDir& imageDir, const KURL& url, const QString& imageFormat)
{
    int numOfImages = imageDir.count();
    const QString imgGalleryDir = url.directory();
    const QString today(KGlobal::locale()->formatDate(QDate::currentDate()));

    stream << "<body>\n<h1>" << QStyleSheet::escape(m_configDlg->getTitle()) << "</h1><p>" << endl;
    stream << i18n("<i>Number of images</i>: %1").arg(numOfImages) << "<br/>" << endl;
    stream << i18n("<i>Created on</i>: %1").arg(today) << "</p>" << endl;

    stream << "<hr/>" << endl;

    if (m_recurseSubDirectories && subDirList.count() > 2) { //subDirList.count() is always >= 2 because of the "." and ".." directories
        stream << i18n("<i>Subfolders</i>:") << "<br>" << endl;
        for (QStringList::ConstIterator it = subDirList.begin(); it != subDirList.end(); it++) {
            if (*it == "." || *it == "..")
                continue; //disregard the "." and ".." directories
            stream << "<a href=\"" << *it << "/" << url.fileName()
                   << "\">" << *it << "</a><br>" << endl;
        }
        stream << "<hr/>" << endl;
    }

    stream << "<table>" << endl;

    //table with images
    int imgIndex;
    QFileInfo imginfo;
    QPixmap  imgProp;
    for (imgIndex = 0; !m_cancelled && (imgIndex < numOfImages);) {
        stream << "<tr>" << endl;

        for (int col=0; !m_cancelled && (col < m_imagesPerRow) && (imgIndex < numOfImages); col++) {
            const QString imgName = imageDir[imgIndex];

            if (m_copyFiles) {
                stream << "<td align='center'>\n<a href=\"images/" << imgName << "\">";
            } else {
                stream << "<td align='center'>\n<a href=\"" << imgName << "\">";
            }


            if (createThumb(imgName, sourceDirName, imgGalleryDir, imageFormat)) {
                const QString imgPath("thumbs/" + imgName + extension(imageFormat));
                stream << "<img src=\"" << imgPath << "\" width=\"" << m_imgWidth << "\" ";
                stream << "height=\"" << m_imgHeight << "\" alt=\"" << imgPath << "\"/>";
                m_progressDlg->setLabelText( i18n("Created thumbnail for: \n%1").arg(imgName) );
            } else {
                kdDebug(90170) << "Creating thumbnail for " << imgName << " failed" << endl;
                m_progressDlg->setLabelText( i18n("Creating thumbnail for: \n%1\n failed").arg(imgName) );
            }
            stream << "</a>" << endl;

            if (m_configDlg->printImageName()) {
                stream << "<div>" << imgName << "</div>" << endl;
            }

            if (m_configDlg->printImageProperty()) {
                imgProp.load( imageDir.absFilePath(imgName,true) );
                stream << "<div>" << imgProp.width() << " x " << imgProp.height() << "</div>" << endl;
            }

            if (m_configDlg->printImageSize()) {
                imginfo.setFile( imageDir, imgName );
                stream << "<div>(" << (imginfo.size() / 1024) << " " <<  i18n("KB") << ")" << "</div>" << endl;
            }

            if (m_useCommentFile) {
                QString imgComment = (*m_commentMap)[imgName];
                stream << "<div>" << QStyleSheet::escape(imgComment) << "</div>" << endl;
            }
            stream << "</td>" << endl;

            m_progressDlg->setTotalSteps( numOfImages );
            m_progressDlg->setProgress( imgIndex );
            kapp->processEvents();
            imgIndex++;
        }
        stream << "</tr>" << endl;
    }
    //close the HTML
    stream << "</table>\n</body>\n</html>" << endl;
}


bool KImGalleryPlugin::createHtml(const KURL& url, const QString& sourceDirName, int recursionLevel, const QString& imageFormat)
{
    if(m_cancelled) return false;


    if( !parent() || !parent()->inherits("KonqDirPart"))
        return false;
    KonqDirPart * part = static_cast<KonqDirPart *>(parent());

    QStringList subDirList;
    if (m_recurseSubDirectories && (recursionLevel >= 0)) { //recursionLevel == 0 means endless
        QDir toplevel_dir = QDir( sourceDirName );
        toplevel_dir.setFilter( QDir::Dirs | QDir::Readable | QDir::Writable );
        subDirList = toplevel_dir.entryList();

        for (QStringList::ConstIterator it = subDirList.begin(); it != subDirList.end() && !m_cancelled; it++) {
            const QString currentDir = *it;
            if (currentDir == "." || currentDir == "..") { continue;} //disregard the "." and ".." directories
            QDir subDir = QDir( url.directory() + "/" + currentDir );
            if (!subDir.exists()) {
                subDir.setPath( url.directory() );
                if (!(subDir.mkdir(currentDir, false))) {
                    KMessageBox::sorry(part->widget(), i18n("Couldn't create folder: %1").arg(subDir.path()));
                    continue;
                } else {
                    subDir.setPath( url.directory() + "/" + currentDir );
                }
            }
            if(!createHtml( KURL( subDir.path() + "/" + url.fileName() ), sourceDirName + "/" + currentDir,
                            recursionLevel > 1 ? recursionLevel - 1 : 0, imageFormat)) { return false; }
        }
    }

    if (m_useCommentFile) {
        loadCommentFile();
    }

    kdDebug(90170) << "sourceDirName: " << sourceDirName << endl;
    //We're interested in only the patterns, so look for the first |
    //#### perhaps an accessor should be added to KImageIO instead?
    QString filter = KImageIO::pattern(KImageIO::Reading).section('|', 0, 0);

    QDir imageDir( sourceDirName, filter.latin1(),
                   QDir::Name|QDir::IgnoreCase, QDir::Files|QDir::Readable);

    const QString imgGalleryDir = url.directory();
    kdDebug(90170) << "imgGalleryDir: " << imgGalleryDir << endl;

    // Create the "thumbs" subdirectory if necessary
    QDir thumb_dir( imgGalleryDir + QString::fromLatin1("/thumbs/"));
    if (createDirectory(thumb_dir, imgGalleryDir, "thumbs") == false)
        return false;

    // Create the "images" subdirectory if necessary
    QDir images_dir( imgGalleryDir + QString::fromLatin1("/images/"));
    if (m_copyFiles) {
        if (createDirectory(images_dir, imgGalleryDir, "images") == false)
            return false;
    }

    QFile file( url.path() );
    kdDebug(90170) << "url.path(): " << url.path() << ", thumb_dir: "<< thumb_dir.path()
              << ", imageDir: "<< imageDir.path() << endl;

    if ( imageDir.exists() && file.open(IO_WriteOnly) ) {
        QTextStream stream(&file);
        stream.setEncoding(QTextStream::Locale);

        createHead(stream);
        createBody(stream, sourceDirName, subDirList, imageDir, url, imageFormat); //ugly

        file.close();

        return !m_cancelled;

    } else {
        KMessageBox::sorry(m_part->widget(),i18n("Couldn't open file: %1").arg(url.path(+1)));
        return false;
    }
}

void KImGalleryPlugin::deleteCancelledGallery(const KURL& url, const QString& sourceDirName, int recursionLevel, const QString& imageFormat)
{
    if (m_recurseSubDirectories && (recursionLevel >= 0)) {
        QStringList subDirList;
        QDir toplevel_dir = QDir( sourceDirName );
        toplevel_dir.setFilter( QDir::Dirs );
        subDirList = toplevel_dir.entryList();

        for (QStringList::ConstIterator it = subDirList.begin(); it != subDirList.end(); it++) {
            if (*it == "." || *it == ".." || *it == "thumbs" || (m_copyFiles && *it == "images")) {
                continue; //disregard the "." and ".." directories
            }
            deleteCancelledGallery( KURL( url.directory() + "/" + *it + "/" + url.fileName() ),
                                    sourceDirName + "/" + *it,
                                    recursionLevel > 1 ? recursionLevel - 1 : 0, imageFormat);
        }
    }

    const QString imgGalleryDir = url.directory();
    QDir thumb_dir( imgGalleryDir + QString::fromLatin1("/thumbs/"));
    QDir images_dir( imgGalleryDir + QString::fromLatin1("/images/"));
    QDir imageDir( sourceDirName, "*.png *.PNG *.gif *.GIF *.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP",
                   QDir::Name|QDir::IgnoreCase, QDir::Files|QDir::Readable);
    QFile file( url.path() );

    // Remove the image file ..
    file.remove();
    // ..all the thumbnails ..
    for (uint i=0; i < imageDir.count(); i++) {
        const QString imgName = imageDir[i];
        const QString imgNameFormat = imgName + extension(imageFormat);
        bool isRemoved = thumb_dir.remove(imgNameFormat);
        kdDebug(90170) << "removing: " << thumb_dir.path() << "/" << imgNameFormat << "; "<< isRemoved << endl;
    }
    // ..and the thumb directory
    thumb_dir.rmdir(thumb_dir.path());

    // ..and the images directory if images were to be copied
    if (m_copyFiles) {
        for (uint i=0; i < imageDir.count(); i++) {
            const QString imgName = imageDir[i];
            bool isRemoved = images_dir.remove(imgName);
            kdDebug(90170) << "removing: " << images_dir.path() << "/" << imgName << "; "<< isRemoved << endl;
        }
        images_dir.rmdir(images_dir.path());
    }
}

void KImGalleryPlugin::loadCommentFile()
{
    QFile file(m_configDlg->getCommentFile());
    if (file.open(IO_ReadOnly)) {
        kdDebug(90170) << "File opened."<< endl;

        QTextStream* m_textStream = new QTextStream(&file);
        m_textStream->setEncoding(QTextStream::Locale);

        delete m_commentMap;
        m_commentMap = new CommentMap;

        QString picName, picComment, curLine, curLineStripped;
        while (!m_textStream->eof()) {
            curLine = m_textStream->readLine();
            curLineStripped = curLine.stripWhiteSpace();
            // Lines starting with '#' are comment
            if (!(curLineStripped.isEmpty()) && !curLineStripped.startsWith("#")) {
                if (curLineStripped.endsWith(":")) {
                    picComment = QString::null;
                    picName = curLineStripped.left(curLineStripped.length()-1);
                    kdDebug(90170) << "picName: " << picName << endl;
                } else {
                    do {
                        //kdDebug(90170) << "picComment" << endl;
                        picComment += curLine + "\n";
                        curLine = m_textStream->readLine();
                    } while (!m_textStream->eof() && !(curLine.stripWhiteSpace().isEmpty()) &&
                             !curLine.stripWhiteSpace().startsWith("#"));
                    //kdDebug(90170) << "Pic comment: " << picComment << endl;
                    m_commentMap->insert(picName, picComment);
                }
            }
        }
        CommentMap::Iterator it;
        for( it = m_commentMap->begin(); it != m_commentMap->end(); ++it ) {
            kdDebug(90170) << "picName: " << it.key() << ", picComment: " << it.data() << endl;
        }
        file.close();
        kdDebug(90170) << "File closed." << endl;
        delete m_textStream;
    } else {
        KMessageBox::sorry(m_part->widget(), i18n("Couldn't open file: %1").arg(m_configDlg->getCommentFile()));
        m_useCommentFile = false;
    }
}

bool KImGalleryPlugin::createThumb( const QString& imgName, const QString& sourceDirName,
                                    const QString& imgGalleryDir, const QString& imageFormat)
{
    QImage img;
    const QString pixPath = sourceDirName + QString::fromLatin1("/") + imgName;

    if (m_copyFiles) {
        KURL srcURL = KURL::fromPathOrURL(pixPath);
        //kdDebug(90170) << "srcURL: " << srcURL << endl;
        KURL destURL = KURL::fromPathOrURL(imgGalleryDir + QString::fromLatin1("/images/") + imgName);
        //kdDebug(90170) << "destURL: " << destURL << endl;
        KIO::NetAccess::copy(srcURL, destURL, static_cast<KParts::Part *>(parent())->widget());
    }

    const QString imgNameFormat = imgName + extension(imageFormat);
    const QString thumbDir = imgGalleryDir + QString::fromLatin1("/thumbs/");
    int extent = m_configDlg->getThumbnailSize();

    // this code is stolen from kdebase/kioslave/thumbnail/imagecreator.cpp
    // (c) 2000 gis and malte

    m_imgWidth = 120; // Setting the size of the images is
    m_imgHeight = 90; // required to generate faster 'loading' pages
    if ( img.load( pixPath ) )
    {
        int w = img.width(), h = img.height();
        // scale to pixie size
        // kdDebug(90170) << "w: " << w << " h: " << h << endl;
        // Resizing if to big
        if(w > extent || h > extent)
        {
            if(w > h)
            {
                h = (int)( (double)( h * extent ) / w );
                if ( h == 0 ) h = 1;
                w = extent;
                Q_ASSERT( h <= extent );
            }
            else
            {
                w = (int)( (double)( w * extent ) / h );
                if ( w == 0 ) w = 1;
                h = extent;
                Q_ASSERT( w <= extent );
            }
            const QImage scaleImg(img.smoothScale( w, h ));
            if ( scaleImg.width() != w || scaleImg.height() != h )
            {
                kdDebug(90170) << "Resizing failed. Aborting." << endl;
                return false;
            }
            img = scaleImg;
            if (m_configDlg->colorDepthSet() == true )
            {
                const QImage depthImg(img.convertDepth(m_configDlg->getColorDepth()));
                img = depthImg;
            }
        }
        kdDebug(90170) << "Saving thumbnail to: " << thumbDir + imgNameFormat  << endl;
        if (!img.save(thumbDir + imgNameFormat, imageFormat.latin1()))
        {
            kdDebug(90170) << "Saving failed. Aborting." << endl;
            return false;
        }
        m_imgWidth = w;
        m_imgHeight = h;
        return true;
    }
    return false;
}

void KImGalleryPlugin::slotCancelled()
{
    m_cancelled = true;
}

#include "imgalleryplugin.moc"
