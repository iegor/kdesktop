/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

/* $Id: kviewpresenter.cpp 465369 2005-09-29 14:33:08Z mueller $ */

#include "kviewpresenter.h"
#include "imagelistdialog.h"
#include "imagelistitem.h"

#include <qvbox.h>
#include <qobjectlist.h>
#include <qsignalslotimp.h>
#include <qtimer.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qstringlist.h>

#include <kpushbutton.h>
#include <kapplication.h>
#include <kaction.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kimageviewer/viewer.h>
#include <kimageviewer/canvas.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <ktempfile.h>
#include <kurldrag.h>

typedef KGenericFactory<KViewPresenter> KViewPresenterFactory;
K_EXPORT_COMPONENT_FACTORY( kview_presenterplugin, KViewPresenterFactory( "kviewpresenterplugin" ) )

KViewPresenter::KViewPresenter( QObject* parent, const char* name, const QStringList & )
	: Plugin( parent, name )
	, m_pImageList( new ImageListDialog() )
	, m_paFileOpen( 0 )
	, m_bDontAdd( false )
	, m_pCurrentItem( 0 )
	, m_pSlideshowTimer( new QTimer( this ) )
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	m_imagelist.setAutoDelete( true );

	QObjectList * viewerList = parent->queryList( 0, "KImageViewer Part", false, false );
	m_pViewer = static_cast<KImageViewer::Viewer *>( viewerList->getFirst() );
	delete viewerList;
	if( m_pViewer )
	{
		( void ) new KAction( i18n( "&Image List..." ), 0, 0,
							this, SLOT( slotImageList() ),
							actionCollection(), "plugin_presenter_imageList" );
		m_paSlideshow = new KToggleAction( i18n( "Start &Slideshow" ), Key_S, actionCollection(), "plugin_presenter_slideshow" );
		( void ) new KAction( i18n( "&Previous Image in List" ), "previous", ALT+Key_Left,
							  this, SLOT( prev() ),
							  actionCollection(), "plugin_presenter_prev" );
		( void ) new KAction( i18n( "&Next Image in List" ), "next", ALT+Key_Right,
							  this, SLOT( next() ),
							  actionCollection(), "plugin_presenter_next" );

		connect( m_paSlideshow, SIGNAL( toggled( bool ) ), m_pImageList->m_pSlideshow, SLOT( setOn( bool ) ) );
		connect( m_pImageList->m_pSlideshow, SIGNAL( toggled( bool ) ), m_paSlideshow, SLOT( setChecked( bool ) ) );

		// search for file_open action
		KXMLGUIClient * parentClient = static_cast<KXMLGUIClient*>( parent->qt_cast( "KXMLGUIClient" ) );
		if( parentClient )
		{
			m_paFileOpen = parentClient->actionCollection()->action( "file_open" );
			m_paFileClose = parentClient->actionCollection()->action( "file_close" );
		}
		if( m_paFileClose )
			connect( m_paFileClose, SIGNAL( activated() ), this, SLOT( slotClose() ) );
		if( m_paFileOpen )
		{
			disconnect( m_paFileOpen, SIGNAL( activated() ), parent, SLOT( slotOpenFile() ) );
			connect( m_paFileOpen, SIGNAL( activated() ), this, SLOT( slotOpenFiles() ) );
		}
		else
		{
			(void) new KAction( i18n( "Open &Multiple Files..." ), "queue", CTRL+SHIFT+Key_O,
							this, SLOT( slotOpenFiles() ),
							actionCollection(), "plugin_presenter_openFiles" );
		}
		connect( m_pViewer, SIGNAL( imageOpened( const KURL & ) ),
				SLOT( slotImageOpened( const KURL & ) ) );
	}
	else
		kdWarning( 4630 ) << "no KImageViewer interface found - the presenter plugin won't work" << endl;

	//( void )new KViewPresenterConfModule( this );

	connect( m_pImageList->m_pListView, SIGNAL( executed( QListViewItem* ) ),
			this, SLOT( changeItem( QListViewItem* ) ) );
	connect( m_pImageList->m_pPrevious, SIGNAL( clicked() ),
			this, SLOT( prev() ) );
	connect( m_pImageList->m_pNext, SIGNAL( clicked() ),
			this, SLOT( next() ) );
	connect( m_pImageList->m_pListView, SIGNAL( spacePressed( QListViewItem* ) ),
			this, SLOT( changeItem( QListViewItem* ) ) );
	connect( m_pImageList->m_pListView, SIGNAL( returnPressed( QListViewItem* ) ),
			this, SLOT( changeItem( QListViewItem* ) ) );
	connect( m_pImageList->m_pSlideshow, SIGNAL( toggled( bool ) ),
			this, SLOT( slideshow( bool ) ) );
	connect( m_pImageList->m_pInterval, SIGNAL( valueChanged( int ) ),
			this, SLOT( setSlideshowInterval( int ) ) );
	connect( m_pImageList->m_pShuffle, SIGNAL( clicked() ),
			this, SLOT( shuffle() ) );
	connect( m_pImageList->m_pLoad, SIGNAL( clicked() ),
			this, SLOT( loadList() ) );
	connect( m_pImageList->m_pSave, SIGNAL( clicked() ),
			this, SLOT( saveList() ) );
	connect( m_pImageList->m_pCloseAll, SIGNAL( clicked() ),
			this, SLOT( closeAll() ) );

	// allow drop on the dialog
	m_pImageList->installEventFilter( this );
	m_pImageList->m_pListView->installEventFilter( this );
	m_pImageList->m_pListView->viewport()->installEventFilter( this );

	// grab drops on the main view
	m_pViewer->widget()->installEventFilter( this );

	connect( m_pSlideshowTimer, SIGNAL( timeout() ),
			this, SLOT( next() ) );
}

KViewPresenter::~KViewPresenter()
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	if( m_paFileOpen )
	{
		disconnect( m_paFileOpen, SIGNAL( activated() ), this, SLOT( slotOpenFiles() ) );
		// If the parent() doesn't exist we either leave the "File Open" action
		// in an unusable state or KView was just shutting down and therefor we
		// can ignore this. I've only seen the second one happening and to get
		// rid of the QObject::connect warning we do the parent() check.
		if( parent() )
			connect( m_paFileOpen, SIGNAL( activated() ), parent(), SLOT( slotOpenFile() ) );
	}
}

bool KViewPresenter::eventFilter( QObject *obj, QEvent *ev )
{
	if( obj == m_pImageList || obj == m_pImageList->m_pListView || obj == m_pImageList->m_pListView->viewport() || obj == m_pViewer->widget() )
	{
		switch( ev->type() )
		{
			case QEvent::DragEnter:
			case QEvent::DragMove:
			{
				// drag enter event in the image list
				//kdDebug( 4630 ) << "DragEnterEvent in the image list: " << obj->className() << endl;
				QDragEnterEvent * e = static_cast<QDragEnterEvent*>( ev );
				//for( int i = 0; e->format( i ); ++i )
					//kdDebug( 4630 ) << " - " << e->format( i ) << endl;
				if( KURLDrag::canDecode( e ) )// || QImageDrag::canDecode( e ) )
				{
					e->accept();
					return true;
				}
			}
			case QEvent::Drop:
			{
				// drop event in the image list
				kdDebug( 4630 ) << "DropEvent in the image list: " << obj->className() << endl;
				QDropEvent * e = static_cast<QDropEvent*>( ev );
				QStringList l;
				//QImage image;
				if( KURLDrag::decodeToUnicodeUris( e, l ) )
				{
					for( QStringList::const_iterator it = l.begin(); it != l.end(); ++it )
					{
						ImageInfo * info = new ImageInfo( KURL( *it ) );
						if( ! m_imagelist.contains( info ) )
						{
							m_imagelist.inSort( info );
							( void )new ImageListItem( m_pImageList->m_pListView, KURL( *it ) );
						}
						else
							delete info;
					}
					return true;
				}
				//else if( QImageDrag::decode( e, image ) )
					//newImage( image );
			}
			default: // do nothing
				break;
		}
	}
	return KParts::Plugin::eventFilter( obj, ev );
}

void KViewPresenter::slotImageOpened( const KURL & url )
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	if( ! m_bDontAdd )
	{
		kdDebug( 4630 ) << k_funcinfo << "imagelist:" << endl;
		ImageInfo * info = new ImageInfo( url );
		if( ! m_imagelist.contains( info ) )
		{
			m_imagelist.inSort( info );
			QListViewItem * item = new ImageListItem( m_pImageList->m_pListView, url );
			makeCurrent( item );
		}
		else
			delete info;
	}
}

void KViewPresenter::slotImageList()
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	m_pImageList->show();
}

void KViewPresenter::slotOpenFiles()
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	KURL::List urls = KFileDialog::getOpenURLs( ":load_image", KImageIO::pattern( KImageIO::Reading ), m_pViewer->widget() );

	if( urls.isEmpty() )
		return;

	KURL::List::Iterator it = urls.begin();
	m_pViewer->openURL( *it );
	for( ++it; it != urls.end(); ++it )
	{
		ImageInfo * info = new ImageInfo( *it );
		if( ! m_imagelist.contains( info ) )
		{
			m_imagelist.inSort( info );
			( void )new ImageListItem( m_pImageList->m_pListView, *it );
		}
		else
			delete info;
	}
}

void KViewPresenter::slotClose()
{
	QListViewItem * next = m_pCurrentItem->itemBelow() ? m_pCurrentItem->itemBelow() : m_pImageList->m_pListView->firstChild();
	if( next == m_pCurrentItem )
		next = 0;

	ImageInfo info( m_pCurrentItem->url() );
	m_imagelist.remove( &info );
	delete m_pCurrentItem;
	m_pCurrentItem = 0;

	if( next )
		changeItem( next );
}

void KViewPresenter::changeItem( QListViewItem * qitem )
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	if( qitem->rtti() == 48294 )
	{
		ImageListItem * item = static_cast<ImageListItem*>( qitem );
		if( ! item->url().isEmpty() )
		{
			if( item->url().isLocalFile() && ! QFile::exists( item->url().path() ) )
			{
				kdDebug( 4630 ) << "file doesn't exist. removed." << endl;
				ImageInfo info( item->url() );
				m_imagelist.remove( &info );
				if( m_pCurrentItem == item )
				{
					QListViewItem * next = m_pCurrentItem->itemBelow() ? m_pCurrentItem->itemBelow() : m_pImageList->m_pListView->firstChild();
					if( next->rtti() != 48294 )
						kdWarning( 4630 ) << "unknown ListView item" << endl;
					else
						m_pCurrentItem = static_cast<ImageListItem*>( next );

					if( m_pCurrentItem == item )
						m_pCurrentItem = 0; // don't create a dangling pointer
					delete item;
					if( m_pCurrentItem )
						changeItem( m_pCurrentItem );
				}
				else
				{
					delete item;
					next();
				}
				return;
			}
			kdDebug( 4630 ) << "got url" << endl;
			makeCurrent( qitem );

			bool dontadd = m_bDontAdd;
			m_bDontAdd = true;
			m_pViewer->openURL( item->url() );
			m_bDontAdd = dontadd;
		}
		else
			kdWarning( 4630 ) << "got nothing" << endl;
	}
	else
		kdWarning( 4630 ) << "unknown ListView item" << endl;
}

void KViewPresenter::prev()
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	if( m_pCurrentItem )
	{
		QListViewItem * prev = m_pCurrentItem->itemAbove() ? m_pCurrentItem->itemAbove() : m_pImageList->m_pListView->lastItem();
		if( prev )
			changeItem( prev );
	}
}

void KViewPresenter::next()
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	if( m_pCurrentItem )
	{
		QListViewItem * next = m_pCurrentItem->itemBelow() ? m_pCurrentItem->itemBelow() : m_pImageList->m_pListView->firstChild();
		if( next )
			changeItem( next );
	}
}

void KViewPresenter::makeCurrent( QListViewItem * item )
{
	if( m_pCurrentItem )
		m_pCurrentItem->setPixmap( 0, QPixmap() );
	if( item->rtti() != 48294 )
		kdWarning( 4630 ) << "unknown ListView item" << endl;
	else
	{
		m_pCurrentItem = static_cast<ImageListItem*>( item );
		m_pCurrentItem->setPixmap( 0, KGlobal::iconLoader()->loadIcon( "1rightarrow", KIcon::Small ) );
		m_pImageList->m_pListView->ensureItemVisible( m_pCurrentItem );
	}
}

void KViewPresenter::slideshow( bool running )
{
	if( running )
	{
		m_pSlideshowTimer->start( m_pImageList->m_pInterval->value() );
		actionCollection()->action( "plugin_presenter_slideshow" )->setText( i18n( "Stop &Slideshow" ) );
		m_pImageList->m_pSlideshow->setText( i18n( "Stop &Slideshow" ) );
	}
	else
	{
		m_pSlideshowTimer->stop();
		actionCollection()->action( "plugin_presenter_slideshow" )->setText( i18n( "Start &Slideshow" ) );
		m_pImageList->m_pSlideshow->setText( i18n( "Start &Slideshow" ) );
	}
}

void KViewPresenter::setSlideshowInterval( int msec )
{
	if( m_pSlideshowTimer->isActive() )
		m_pSlideshowTimer->changeInterval( msec );
}

void KViewPresenter::shuffle()
{
	m_pImageList->noSort();
	KListView * listview = m_pImageList->m_pListView;
	QPtrList<QListViewItem> items;
	for( QListViewItem * item = listview->firstChild(); item; item = listview->firstChild() )
	{
		items.append( item );
		listview->takeItem( item );
	}
	while( ! items.isEmpty() )
		listview->insertItem( items.take( KApplication::random() % items.count() ) );
}

void KViewPresenter::closeAll()
{
	m_imagelist.clear();
	m_pImageList->m_pListView->clear();
	m_pCurrentItem = 0;
	if( m_pViewer->closeURL() )
		m_pViewer->canvas()->clear();
}

void KViewPresenter::loadList()
{
	KURL url = KFileDialog::getOpenURL( ":load_list", QString::null, m_pImageList );
	if( url.isEmpty() )
		return;

	QString tempfile;
	if( ! KIO::NetAccess::download( url, tempfile, m_pViewer->widget() ) )
	{
		KMessageBox::error( m_pImageList, i18n( "Could not load\n%1" ).arg( url.prettyURL() ) );
		return;
	}
	QFile file( tempfile );
	if( file.open( IO_ReadOnly ) )
	{
		QTextStream t( &file );
		if( t.readLine() == "[KView Image List]" )
		{
			//clear old image list
			closeAll();

			QStringList list;
			if( ! t.eof() )
				m_pViewer->openURL( KURL( t.readLine() ) );
			while( ! t.eof() )
			{
				KURL url ( t.readLine() );
				ImageInfo * info = new ImageInfo( url );
				if( ! m_imagelist.contains( info ) )
				{
					m_imagelist.inSort( info );
					( void )new ImageListItem( m_pImageList->m_pListView, url );
				}
				else
					delete info;
			}
		}
		else
		{
			KMessageBox::error( m_pImageList, i18n( "Wrong format\n%1" ).arg( url.prettyURL() ) );
		}
		file.close();
	}
	KIO::NetAccess::removeTempFile( tempfile );
}

void KViewPresenter::saveList()
{
	KURL url = KFileDialog::getSaveURL( ":save_list", QString::null, m_pImageList );

	if( url.isEmpty() )
		return;

	QString tempfile;
	if( url.isLocalFile() )
		tempfile = url.path();
	else
	{
		KTempFile ktempf;
		tempfile = ktempf.name();
	}

	QFile file( tempfile );
	if( file.open( IO_WriteOnly ) )
	{
		QTextStream t( &file );
		// write header
		t << "[KView Image List]" << endl;
		QListViewItem * item = m_pImageList->m_pListView->firstChild();
		while( item )
		{
			if( item->rtti() == 48294 )
				t << static_cast<ImageListItem*>( item )->url().url() << endl;
			item = item->itemBelow();
		}
		file.close();

		if( ! url.isLocalFile() )
		{
			KIO::NetAccess::upload( tempfile, url, m_pViewer->widget() );
			KIO::NetAccess::removeTempFile( tempfile );
		}
	}
}

// vim:sw=4:ts=4
#include "kviewpresenter.moc"
