/*
 * urllistview.cpp
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kurl.h>
#include <kglobalsettings.h>
#include <kfiledialog.h>
#include <kpopupmenu.h>

#include <qfontmetrics.h>
#include <qdragobject.h>

#include "mrl.h"
#include "playlistitem.h"
#include "urllistview.h"
#include "urllistview.moc"


UrlListView::UrlListView(QWidget *parent, const char *name ) : KListView(parent,name),
 m_listCleared(true), m_itemOfContextMenu(NULL)
{
  m_contextMenu = new KPopupMenu(this);
  m_contextMenu->insertItem(KGlobal::iconLoader()->loadIconSet("player_play", KIcon::Small), i18n("Play"), this, SLOT(slotPlayItem()));
  m_contextMenu->insertItem(i18n("Play Next/Add to Queue"), this, SLOT(slotPlayNext()));
  m_contextMenu->insertSeparator();

  m_contextMenu->insertItem(KGlobal::iconLoader()->loadIconSet("editcut", KIcon::Small), i18n("C&ut"), this, SIGNAL(signalCut()), CTRL+Key_X);
  m_contextMenu->insertItem(KGlobal::iconLoader()->loadIconSet("editcopy", KIcon::Small), i18n("&Copy"), this, SIGNAL(signalCopy()), CTRL+Key_C);
  m_contextMenu->insertItem(KGlobal::iconLoader()->loadIconSet("editpaste", KIcon::Small), i18n("&Paste"), this, SIGNAL(signalPaste()), CTRL+Key_V);
  m_contextMenu->insertItem(i18n("Select &All"), this, SIGNAL(signalSelectAll()), CTRL+Key_A);
  m_contextMenu->insertItem(i18n("Create Playlist From Selected"), this, SIGNAL(signalPlaylistFromSelected()));
  m_contextMenu->insertSeparator();
  m_contextMenu->insertItem(KGlobal::iconLoader()->loadIconSet("indent", KIcon::Small), i18n("Add Sub&title..."), this, SLOT(slotAddSubtitle()),QKeySequence(),100 );
  m_contextMenu->insertSeparator();
  m_contextMenu->insertItem(KGlobal::iconLoader()->loadIconSet("edit", KIcon::Small), i18n("&Edit Title"), this, SLOT(slotEditTitle()));
  m_contextMenu->insertItem(KGlobal::iconLoader()->loadIconSet("info", KIcon::Small), i18n("&Info"), this, SLOT(slotShowInfo()));

/* width of the "length"-column */
  QFontMetrics met(KGlobalSettings::generalFont());
  int w1 = met.width(i18n("Length"));
  int w2 =  met.width("5:55:55") + 2;

  m_column5Width = w1 > w2 ? w1 : w2;
  m_column5Width += 30;

/* width of the "track"-column */
  w1 = met.width(i18n("Track"));
  w2 = met.width("9999") + 2;

  m_column4Track = w1 > w2 ? w1 : w2;
  m_column4Track += 36;

  connect(this, SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&, int)),
          this, SLOT(slotShowContextMenu(QListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(currentChanged(QListViewItem*)),
          this, SLOT(slotCurrentChanged(QListViewItem*)));
  connect(this, SIGNAL(clicked( QListViewItem*, const QPoint&, int )),
          this, SLOT(slotClicked( QListViewItem*, const QPoint&, int )));
}

UrlListView::~UrlListView()
{
}

bool UrlListView::acceptDrag(QDropEvent* event) const
{
 return QUriDrag::canDecode(event) || QTextDrag::canDecode(event) || KListView::acceptDrag(event);
}

QDragObject* UrlListView::dragObject()
{
 // get selected items
  QPtrList<QListViewItem> selected;
  QStrList urlList;

  selected = selectedItems();
  for (uint i = 0; i<selected.count(); i++)
  {
    urlList.append(dynamic_cast<PlaylistItem*>(selected.at(i))->url().ascii());
  }

  return new QUriDrag(urlList, viewport());
}

void UrlListView::resizeEvent(QResizeEvent* rev)
{
  int width = contentsRect().width() - m_column5Width - m_column4Track - 69;
  setColumnWidth(0, 20);  /* mime */
  setColumnWidth(1, ((width * 5 / 12) + 50));  /* title */
  setColumnWidth(2, (width * 3 / 12));  /* artist */
  setColumnWidth(3, (width * 4 / 12)); /* album */
  setColumnWidth(4, m_column4Track);  /* track */
  setColumnWidth(5, m_column5Width);  /* width of "length" column */

  KListView::resizeEvent(rev);
}

void UrlListView::clear()
{
  m_listCleared = true;
  m_itemOfContextMenu = NULL;
  setSorting(-1);
  KListView::clear();
}

void UrlListView::setCleared(bool cl)
{
  m_listCleared = cl;
}

bool UrlListView::getCleared() const   /* was playlist cleared ? */
{
  return m_listCleared;
}

/********** context menu **********/

void UrlListView::slotShowContextMenu(QListViewItem* item, const QPoint& pos, int)
{
  if (!item)
  {
    m_itemOfContextMenu = NULL;
    disableSubEntry();
  }
  else
  {
    m_itemOfContextMenu = dynamic_cast<PlaylistItem *>(item);
    if (m_itemOfContextMenu->mime().contains("video"))
      enableSubEntry();
    else
      disableSubEntry();
  }

  m_contextMenu->popup(pos);
}

void UrlListView::slotPlayItem()
{
  if (m_itemOfContextMenu)
    emit signalPlayItem(m_itemOfContextMenu);
}

void UrlListView::slotPlayNext()
{
  if (m_itemOfContextMenu)
    emit signalAddToQueue(m_itemOfContextMenu->toMRL());
}

void UrlListView::slotEditTitle()
{
  if (m_itemOfContextMenu)
  {
    m_itemOfContextMenu->setRenameEnabled(1, true);
    m_itemOfContextMenu->startRename(1);
    m_itemOfContextMenu->setRenameEnabled(1, false);
  }
}

//Pretty print item info
void UrlListView::slotShowInfo()
{
  if (!m_itemOfContextMenu)
    return;

  QString num;
  num = num.setNum(childCount());
  QString info = "<qt><table width=\"90%\">";
  info = info + "<tr><td colspan=\"2\"><center><b>" + m_itemOfContextMenu->title() + "</b></center></td></tr>";
  info = info + "<tr><td><b>" + i18n("URL")+ ":</b></td><td>" + m_itemOfContextMenu->url() + "</td></tr>";
  info = info + "<tr><td><b>" + i18n("Artist") + ":</b></td><td>" + m_itemOfContextMenu->artist() + "</td></td>";
  info = info + "<tr><td><b>" + i18n("Album") + ":</b></td><td>" + m_itemOfContextMenu->album() + "</td></td>";
  info = info + "<tr><td><b>" + i18n("Track") + ":</b></td><td>" + m_itemOfContextMenu->track() + "</td></td>";
  info = info + "<tr><td><b>" + i18n("Year") + ":</b></td><td>" + m_itemOfContextMenu->year() + "</td></td>";
  info = info + "<tr><td><b>" + i18n("Genre") + ":</b></td><td>" + m_itemOfContextMenu->genre() + "</td></td>";
  info = info + "<tr><td><b>" + i18n("Length") + ":</b></td><td>" + m_itemOfContextMenu->length() + "</td></tr>";
  if(!m_itemOfContextMenu->subtitles().isEmpty())
  {
     info = info + "<tr><td><b>" + i18n("Subtitles") + ":</b></td><td>";

     for(uint i = 0; i < m_itemOfContextMenu->subtitles().count(); i++ )
     {
       info = info + ""+m_itemOfContextMenu->subtitles()[i];
       if(m_itemOfContextMenu->currentSubtitle() == (int)i)
          info = info + "<small> ("+i18n("in use")+")</small>";
       info = info + "<br>";
     }
     info = info + "</ul></td></tr></table></qt>";
  }

  KMessageBox::information(this, info);
}

void UrlListView::slotClicked(QListViewItem*, const QPoint&, int)
{
  /*if ( (item) && (col == 3) )
  {
    m_itemOfContextMenu = dynamic_cast<PlaylistItem *>(item);
    if (!m_itemOfContextMenu) return;
    slotShowInfo();
  } */
}

#include <qdom.h>
#include <kio/job.h>

void UrlListView::slotAddSubtitle()
{
        /*QDomDocument reqdoc;

        QDomElement methodCall = reqdoc.createElement( "methodCall" );
        QDomElement methodName = reqdoc.createElement( "methodName" );
        QDomText methodNameValue = reqdoc.createTextNode( "LogIn" );
        methodName.appendChild( methodNameValue );
        methodCall.appendChild( methodName );
        reqdoc.appendChild( methodCall );

        QDomElement params = reqdoc.createElement( "params" );

        QDomElement param1 = reqdoc.createElement( "param" );
        QDomElement value1 = reqdoc.createElement( "value" );
        QDomElement type1 = reqdoc.createElement( "string" );
        QDomText param1Value = reqdoc.createTextNode( "" );
        type1.appendChild( param1Value );
        value1.appendChild( type1 );
        param1.appendChild( value1 );
        params.appendChild( param1 );

        QDomElement param2 = reqdoc.createElement( "param" );
        QDomElement value2 = reqdoc.createElement( "value" );
        QDomElement type2 = reqdoc.createElement( "string" );
        QDomText param2Value = reqdoc.createTextNode( "" );
        type2.appendChild( param2Value );
        value2.appendChild( type2 );
        param2.appendChild( value2 );
        params.appendChild( param2 );

        QDomElement param3 = reqdoc.createElement( "param" );
        QDomElement value3 = reqdoc.createElement( "value" );
        QDomElement type3 = reqdoc.createElement( "string" );
        QDomText param3Value = reqdoc.createTextNode( "en" );
        type3.appendChild( param3Value );
        value3.appendChild( type3 );
        param3.appendChild( value3 );
        params.appendChild( param3 );

        methodCall.appendChild( params );

        const QString xmlRequest = "<?xml version=\"1.0\"?>\n" + reqdoc.toString();

        QByteArray postData;
        QDataStream stream( postData, IO_WriteOnly );
        stream.writeRawBytes( xmlRequest.utf8(), xmlRequest.utf8().length() );

        openSubJobBuf = QByteArray();

        openSubJob = KIO::http_post( "http://test.opensubtitles.org/xml-rpc", postData, false );
        openSubJob->addMetaData( "content-type", "Content-Type: text/xml" );

        connect( openSubJob, SIGNAL(result(KIO::Job*)), this, SLOT(openSubResult(KIO::Job*)) );
        connect( openSubJob, SIGNAL(data(KIO::Job*,const QByteArray&) ), this, SLOT(openSubData(KIO::Job*,
        	const QByteArray&)) );

        kdDebug() << "\n\n" << xmlRequest << "\n\n" << endl;*/


   if (!m_itemOfContextMenu)
     return;

   QString openURL = m_itemOfContextMenu->url();
   QString subtitleURL = KFileDialog::getOpenURL(openURL,
                         i18n("*.smi *.srt *.sub *.txt *.ssa *.asc|Subtitle Files\n*.*|All Files"),
                         0, i18n("Select Subtitle File")).path();
   if(!(subtitleURL.isEmpty()) && !(m_itemOfContextMenu->subtitles().contains(subtitleURL)) && QFile::exists(subtitleURL))
   {
     m_itemOfContextMenu->addSubtitle(subtitleURL);
     emit signalPlayItem(m_itemOfContextMenu);
   }

}

void UrlListView::openSubResult( KIO::Job* job )
{
	if ( openSubJob!=job )
		return;
	if ( job->error() ) {
		kdDebug() << "OpenSubtiltes error : " << job->error() << endl;
		return;
	}

	QDomDocument document;
	if ( !document.setContent( openSubJobBuf ) ) {
		kdDebug() << "Couldn't read similar artists response" << endl;
		return;
	}
	kdDebug() << "\n\n" << document.toString() << "\n\n" << endl;
	openSubJob = 0;
}

void UrlListView::openSubData( KIO::Job* job, const QByteArray& data )
{
	if ( openSubJob != job )
		return;

	unsigned int bufsize = openSubJobBuf.size();
	openSubJobBuf.resize( bufsize + data.size() );
	memcpy( openSubJobBuf.data()+bufsize, data.data(), data.size() );
}


void UrlListView::slotCurrentChanged(QListViewItem * item)
{
   if(item == 0) //All items deleted
      m_itemOfContextMenu = NULL;
   else
      m_itemOfContextMenu = dynamic_cast<PlaylistItem *>(item);
}

void UrlListView::enableSubEntry()
{
  m_contextMenu->setItemEnabled(100, true);
}

void UrlListView::disableSubEntry()
{
  m_contextMenu->setItemEnabled(100, false);
}
