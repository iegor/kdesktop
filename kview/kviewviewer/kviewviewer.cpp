/*  This file is part of the KDE project

    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
                  2001-2003 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include "kviewviewer.h"
#include "kimageviewer/canvas.h"
#include "version.h"

#include <ksettings/dispatcher.h>

#include <unistd.h>
#include <assert.h>

#include <qbuffer.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qregexp.h>

#include <kpushbutton.h>
#include <kpassivepopup.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kxmlguifactory.h>
#include <kfiledialog.h>
#include <kparts/genericfactory.h>
#include <kparts/componentfactory.h>
#include <kparts/plugin.h>
#include <kmessagebox.h>
#include <ktempfile.h>
#include <kapplication.h>
#include <kimageio.h>
#include <kstdaccel.h>
#include <kdirwatch.h>
#include <kurldrag.h>
#include <krecentdocument.h>

typedef KParts::GenericFactory<KViewViewer> KViewViewerFactory;
K_EXPORT_COMPONENT_FACTORY( libkviewviewer, KViewViewerFactory )

KViewViewer::KViewViewer( QWidget *parentWidget, const char * /*widgetName*/,
		QObject *parent, const char *name, const QStringList & )
	: KImageViewer::Viewer( parent, name )
	, m_pParentWidget( parentWidget )
	, m_pJob( 0 )
	, m_pExtension( 0 )
	, m_pCanvas( 0 )
	, m_pTempFile( 0 )
	, m_pBuffer( 0 )
	, m_pFileWatch( new KDirWatch( this ) )
{
	KImageIO::registerFormats();

	QWidget * widget =  KParts::ComponentFactory::createInstanceFromQuery<QWidget>(
			"KImageViewer/Canvas", QString::null, m_pParentWidget );
	m_pCanvas = static_cast<KImageViewer::Canvas *>( widget->qt_cast( "KImageViewer::Canvas" ) );
	kdDebug( 4610 ) << "KImageViewer::Canvas at " << m_pCanvas << endl;
	if( ! ( widget && m_pCanvas ) )
	{
		if( ! widget )
			KMessageBox::error( m_pParentWidget, i18n( "Unable to find a suitable Image Canvas. This probably means that KView was not installed properly." ) );
		else
			KMessageBox::error( m_pParentWidget, i18n( "Accessing the KImageViewer interface of the Image Canvas failed. Something in your setup is broken (a component claims to be a KImageViewer::Canvas but it is not)." ) );
	}
	else
	{
		// create the extension before the plugins are loaded, because plugins might want to use it
		m_pExtension = new KViewKonqExtension( m_pCanvas, this );

		setPluginLoadingMode( LoadPluginsIfEnabled );
		setInstance( KViewViewerFactory::instance() );

		// m_url isn't set from ReadOnlyPart so we set it here to the CWD
		m_url = QDir::currentDirPath() + "/";
		m_sCaption = i18n( "Title caption when no image loaded", "no image loaded" );

		setWidget( widget );

		// Drag and Drop for the Canvas
		widget->setAcceptDrops( true );
		widget->installEventFilter( this );

		setupActions();

		if( isReadWrite() )
			setXMLFile( "kviewviewer.rc" );
		else
			setXMLFile( "kviewviewer_ro.rc" );

		connect( widget, SIGNAL( contextPress( const QPoint & ) ),
				this, SLOT( slotPopupMenu( const QPoint & ) ) );
		connect( widget, SIGNAL( zoomChanged( double ) ),
				this, SLOT( zoomChanged( double ) ) );
		connect( widget, SIGNAL( showingImageDone() ),
				this, SLOT( switchBlendEffect() ) );
		connect( widget, SIGNAL( hasImage( bool ) ),
				this, SLOT( hasImage( bool ) ) );
		connect( widget, SIGNAL( imageChanged() ),
				this, SLOT( setModified() ) );

		connect( m_pFileWatch, SIGNAL( dirty( const QString & ) ),
				this, SLOT( slotFileDirty( const QString & ) ) );

		KSettings::Dispatcher::self()->registerInstance( instance(), this, SLOT( readSettings() ) );

		// by default disable progress info (so it won't open the dialog in Konqueror)
		setProgressInfoEnabled( false );

		m_popupDoc = KXMLGUIFactory::readConfigFile( "kviewpopup.rc", true, instance() );

		KConfigGroup cfgGroup( instance()->config(), "Settings" );
		bool hideBars = cfgGroup.readBoolEntry( "hideScrollbars", false );
		m_pCanvas->hideScrollbars( hideBars );
		m_paShowScrollbars->setChecked( ! hideBars );
		m_vEffects.resize( m_pCanvas->numOfBlendEffects() );

		readSettings();
	}
}

KViewViewer::~KViewViewer()
{
	kdDebug( 4610 ) << k_funcinfo << endl;

	writeSettings();
	instance()->config()->sync();

	/*abortLoad(); //just in case
	if( isModified() && isReadWrite() )
	{
		int res = m_url.isEmpty() ?
			KMessageBox::warningYesNo( widget(),
				i18n( "This is a new document.\nDo you want to save it ?" ),
				QString::null, KStdGuiItem::saveAs(), KStdGuiItem::discard() ):
			KMessageBox::warningYesNo( widget(),
				i18n( "The document has been modified.\nDo you want to save it ?" ),
				QString::null, KStdGuiItem::saveAs(), KStdGuiItem::dontSave() );

		if( res == KMessageBox::Yes )
		{
			KURL url = KFileDialog::getSaveURL();
			if( ! url.isEmpty() )
				saveAs( url );
		}
	}
	ReadOnlyPart::closeURL();*/

	abortLoad();
	delete m_pTempFile;
	delete m_pBuffer;
}

KAboutData * KViewViewer::createAboutData()
{
	KAboutData * aboutData  = new KAboutData( "kviewviewer", I18N_NOOP( "KView" ),
			KVIEW_VERSION, I18N_NOOP( "KDE Image Viewer Part" ),
			KAboutData::License_GPL,
			I18N_NOOP("(c) 1997-2002, The KView Developers") );
	aboutData->addAuthor( "Matthias Kretz", I18N_NOOP( "Maintainer" ), "kretz@kde.org" );
	aboutData->addAuthor( "Sirtaj Singh Kang", I18N_NOOP( "started it all" ), "taj@kde.org" );
	aboutData->addAuthor( "Simon Hausmann", 0, "hausmann@kde.org" );
	return aboutData;
}

void KViewViewer::setReadWrite( bool readwrite )
{
	KImageViewer::Viewer::setReadWrite( readwrite );
	if( readwrite )
		setXMLFile( "kviewviewer.rc" );
	else
		setXMLFile( "kviewviewer_ro.rc" );
}

bool KViewViewer::saveAs( const KURL & kurl )
{
	kdDebug( 4610 ) << k_funcinfo << endl;
	if( !kurl.isValid() )
		return KParts::ReadWritePart::saveAs( kurl ); // sets m_bClosing = false

	// if the image wasn't modified and should be saved in the same format we just copy the file - no need
	// to lose some quality or information by calling QImage::save()
	if( ! ( isModified() && isReadWrite() ) && m_mimeType == m_newMimeType )
	{
		kdDebug( 4610 ) << "copy image from " << m_file << " to " << kurl.prettyURL() << endl;

		KIO::Job * job = KIO::copy( KURL( m_file ), kurl, isProgressInfoEnabled() );
		emit started( job );
		connect( job, SIGNAL( result( KIO::Job * ) ),
				this, SLOT( slotResultSaveAs( KIO::Job * ) ) );
		return true;
	}
	kdDebug( 4610 ) << "call KParts::ReadWritePart::saveAs( " << kurl.prettyURL() << " )" << endl;

	bool ret = KParts::ReadWritePart::saveAs( kurl );
	if( ret == false )
		KMessageBox::error( m_pParentWidget, i18n( "The image could not be saved to disk. A possible causes is that you don't have permission to write to that file." ) );
	return ret;
}

void KViewViewer::setModified( bool modified )
{
	m_paSave->setEnabled( modified );
	KImageViewer::Viewer::setModified( modified );
}

bool KViewViewer::openURL( const KURL & url )
{
	kdDebug( 4610 ) << k_funcinfo << endl;
	if ( !url.isValid() )
	{
		kdWarning( 4610 ) << "malformed URL " << url.prettyURL() << endl;
		return false;
	}
	if ( !closeURL() )
	{
		kdDebug( 4610 ) << "closeURL didn't work out" << endl;
		return false;
	}
	setModified( false );
	m_url = url;
	m_mimeType = m_pExtension->urlArgs().serviceType; // if this fails to
	// find the right mime type it'll be determined in openFile()
	if ( m_url.isLocalFile() )
	{
		emit started( 0 );
		m_file = m_url.path();
		kdDebug( 4610 ) << "open local file " << m_file << endl;
		bool ret = openFile();
		if( ret )
		{
			m_sCaption = m_url.prettyURL();
			emit setWindowCaption( m_sCaption );
			emit completed();
		}
		return ret;
	}
	else
	{
		m_sCaption = m_url.prettyURL();
		emit setWindowCaption( m_sCaption );
		m_bTemp = true;
		// Use same extension as remote file. This is important for mimetype-determination (e.g. koffice)
		QString extension;
		QString fileName = url.fileName();
		int extensionPos = fileName.findRev( '.' );
		if ( extensionPos != -1 )
			extension = fileName.mid( extensionPos ); // keep the '.'
		delete m_pTempFile;
		m_pTempFile = new KTempFile( QString::null, extension );
		m_file = m_pTempFile->name();

		m_pJob = KIO::get( m_url, m_pExtension->urlArgs().reload, isProgressInfoEnabled() );
		emit started( m_pJob );
		connect( m_pJob, SIGNAL( result( KIO::Job * ) ), SLOT( slotJobFinished ( KIO::Job * ) ) );
		connect( m_pJob, SIGNAL( data( KIO::Job *, const QByteArray & ) ), SLOT( slotData( KIO::Job *, const QByteArray & ) ) );
		return true;
	}
}

bool KViewViewer::closeURL()
{
	kdDebug( 4610 ) << k_funcinfo << endl;
	abortLoad();
	QString file = m_file;
	bool ret = KParts::ReadWritePart::closeURL();
	if( ret )
		if( ! file.isEmpty() )
		{
			kdDebug( 4610 ) << "remove " << file << " from KDirWatch\n";
			m_pFileWatch->removeFile( file );
		}
	return ret;
}

void KViewViewer::newImage( const QImage & newimg )
{
	if( closeURL() )
	{
		m_url = "";
		m_file = QString::null;
		m_sCaption = i18n( "Title caption when new image selected", "new image" );
		m_pCanvas->setImage( newimg );
		if( isReadWrite() )
			setModified( true );
	}
}

void KViewViewer::reload()
{
	kdDebug( 4610 ) << k_funcinfo << endl;

	if( isReadWrite() && isModified() )
		if( ! queryClose() )
			return;

	//load from file
	QImage image( m_file );
	m_pCanvas->setImage( image );
	setModified( false );
}

bool KViewViewer::eventFilter( QObject * o, QEvent * e )
{
	KImageViewer::Canvas * canvas = static_cast<KImageViewer::Canvas*>( o->qt_cast( "KImageViewer::Canvas" ) );
	if( canvas )
	{
		// intercept drops onto the Canvas
		switch( e->type() )
		{
			case QEvent::DragEnter:
			{
				QDragEnterEvent * ev = static_cast<QDragEnterEvent*>( e );
				kdDebug( 4610 ) << "DragEnter Event in the Canvas: " << endl;
				for( int i = 0; ev->format( i ); ++i )
					kdDebug( 4610 ) << " - " << ev->format( i ) << endl;
				ev->accept( KURLDrag::canDecode( ev ) || QImageDrag::canDecode( ev ) );
				return true;
			}
			case QEvent::Drop:
			{
				QDropEvent * ev = static_cast<QDropEvent*>( e );
				kdDebug( 4610 ) << "Drop Event in the Canvas" << endl;
				QStringList l;
				QImage image;
				if( KURLDrag::decodeToUnicodeUris( ev, l ) )
					openURL( KURL( l.first() ) );
				else if( QImageDrag::decode( ev, image ) )
					newImage( image );
				return true;
			}
			default:
				break;
		}
	}
	return KImageViewer::Viewer::eventFilter( o, e );
}

void KViewViewer::abortLoad()
{
	if ( m_pJob )
	{
		m_pJob->kill();
		m_pJob = 0;
	}
}

bool KViewViewer::openFile()
{
	//m_pCanvas->setMaximumImageSize( QSize( widget()->width(), widget()->height() ) );
	if( m_pBuffer )
	{
		kdDebug( 4610 ) << k_funcinfo << " load from buffer\n";
		//load from buffer
		m_pBuffer->close();
		if( m_pTempFile )
		{
			m_pTempFile->dataStream()->writeRawBytes( m_pBuffer->buffer().data(), m_pBuffer->buffer().size() );
			m_pTempFile->close();
		}

		// determine Mime Type
		if( m_mimeType.isNull() )
		{
			m_mimeType = KImageIO::mimeType( m_url.fileName() );
			kdDebug( 4610 ) << "MimeType: " << m_mimeType << endl;
			if( m_mimeType.isNull() )
			{
				m_mimeType = KMimeType::findByContent( m_pBuffer->buffer() )->name();
				kdDebug( 4610 ) << "MimeType: " << m_mimeType << endl;
			}
		}

		QImage image( m_pBuffer->buffer() );
		delete m_pBuffer;
		m_pBuffer = 0;
		if( ! image.isNull() )
		{
			QSize size = image.size();
			m_pCanvas->setImage( image, size );
		}
		else
		{
			emit setStatusBarText( i18n( "Unknown image format: %1" ).arg( m_url.prettyURL() ) );
			return false;
		}
	}
	else
	{ //load from local file
		kdDebug( 4610 ) << k_funcinfo << " load from file: " << m_file << endl;
		if( ! QFile::exists( m_file ) )
		{
			emit setStatusBarText( i18n( "No such file: %1" ).arg( m_file ) );
			return false;
		}
		if( QImage::imageFormat( m_file ) == 0 )
		{
			emit setStatusBarText( i18n( "Unknown image format: %1" ).arg( m_file ) );
			return false;
		}
		// determine Mime Type
		if( m_mimeType.isNull() )
		{
			m_mimeType = KImageIO::mimeType( m_file );
			kdDebug( 4610 ) << "MimeType: " << m_mimeType << endl;
		}

		//load from file
		QImage image( m_file );
		QSize size = image.size();
		m_pCanvas->setImage( image, size );
	}
	m_pFileWatch->addFile( m_file );
	emit imageOpened( m_url );
	return true;
}

bool KViewViewer::saveFile()
{
	kdDebug( 4610 ) << k_funcinfo << endl;
	// two possibilities:
	//
	// 1. The image should be saved as an exact copy of the loaded one (this
	// is useful if you downloaded an image from the net and want to save it
	// locally) - this can happen only in saveAs() so only 2. applies here:
	//
	// 2. The image should be saved in a different format or was modified, so
	// that copying isn't possible anymore.

	const QImage * image = m_pCanvas->image();
	if( ! image )
		return false;

	if( ! m_newMimeType.isNull() )
	{
		m_mimeType = m_newMimeType;
		m_newMimeType = QString::null;
	}
	QString type = KImageIO::typeForMime( m_mimeType );
	kdDebug( 4610 ) << "save m_pCanvas->image() to " << m_file << " as " << type << endl;
	int quality = 100; // TODO: ask user for quality. Keep it at 100 so that our
	// users don't lose quality when working with KView.

	m_pFileWatch->removeFile( m_file );
	bool ret = image->save( m_file, type.latin1(), quality );
	m_pFileWatch->addFile( m_file );
	return ret;
}

void KViewViewer::setupActions()
{
	m_paZoomIn = new KAction( i18n( "Zoom In" ), "viewmag+", KStdAccel::shortcut( KStdAccel::ZoomIn ), this,
			SLOT( slotZoomIn() ), actionCollection(), "zoomin" );
	m_paZoomOut = new KAction( i18n( "Zoom Out" ), "viewmag-", KStdAccel::shortcut( KStdAccel::ZoomOut ), this,
			SLOT( slotZoomOut() ), actionCollection(), "zoomout" );

	m_paZoom = new KSelectAction( i18n( "Zoom" ), "viewmag", 0, actionCollection(), "view_zoom" );
	connect( m_paZoom, SIGNAL( activated( const QString & ) ), this, SLOT( setZoom( const QString & ) ) );
	m_paZoom->setEditable( true );
	m_paZoom->clear();
	m_paZoom->setItems( QStringList::split( '|', "20%|25%|33%|50%|75%|100%|125%|150%|200%|250%|300%|350%|400%|450%|500%" ) );
	m_paZoom->setCurrentItem( 5 );

	m_paFlipMenu = new KActionMenu( i18n( "&Flip" ), actionCollection(), "flip" );
	m_paFlipV = new KAction( i18n( "&Vertical" ), Key_V, this, SLOT( slotFlipV() ), actionCollection(), "flip_vertical" );
	m_paFlipH = new KAction( i18n( "&Horizontal" ), Key_H, this, SLOT( slotFlipH() ), actionCollection(), "flip_horizontal" );
	m_paFlipMenu->insert( m_paFlipV );
	m_paFlipMenu->insert( m_paFlipH );

	m_paRotateCCW = new KAction( i18n( "Ro&tate Counter-Clockwise" ), "rotate_ccw", 0, this,
			SLOT( slotRotateCCW() ), actionCollection(), "rotateCCW" );
	m_paRotateCW = new KAction( i18n( "Rotate Clockwise" ), "rotate_cw", 0, this,
			SLOT( slotRotateCW() ), actionCollection(), "rotateCW" );
	m_paSave = KStdAction::save( this, SLOT( slotSave() ), actionCollection() );
	m_paSave->setEnabled( false );
	m_paSaveAs = KStdAction::saveAs( this, SLOT( slotSaveAs() ), actionCollection() );

	m_paFitToWin = new KAction( i18n( "Fit Image to Window" ), 0, 0, this,
			SLOT( slotFitToWin() ), actionCollection(), "fittowin" );
	m_paZoomIn->setEnabled( false );
	m_paZoomOut->setEnabled( false );
	m_paZoom->setEnabled( false );
	m_paRotateCCW->setEnabled( false );
	m_paRotateCW->setEnabled( false );
	m_paSaveAs->setEnabled( false );
	m_paFitToWin->setEnabled( false );
	m_paFlipMenu->setEnabled( false );
	m_paFlipV->setEnabled( false );
	m_paFlipH->setEnabled( false );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paZoomIn, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paZoomOut, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paZoom, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paRotateCCW, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paRotateCW, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paSaveAs, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paFitToWin, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paFlipMenu, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paFlipV, SLOT( setEnabled( bool ) ) );
	connect( widget(), SIGNAL( hasImage( bool ) ), m_paFlipH, SLOT( setEnabled( bool ) ) );

	m_paShowScrollbars = new KToggleAction( i18n( "Show Scrollbars" ), 0, this, SLOT( slotToggleScrollbars() ),
	 actionCollection(), "show_scrollbars" );
	m_paShowScrollbars->setCheckedState(i18n("Hide Scrollbars"));
}

void KViewViewer::guiActivateEvent( KParts::GUIActivateEvent * event )
{
	ReadWritePart::guiActivateEvent( event );
	bool enabled = m_pCanvas->image() ? true : false;
	emit m_pExtension->enableAction( "del", enabled );
	emit m_pExtension->enableAction( "print", enabled );
	if( ! enabled )
	{
		m_sCaption = i18n( "Title caption when no image loaded", "no image loaded" );
		emit setWindowCaption( m_sCaption );
	}
}

void KViewViewer::readSettings()
{
	KConfigGroup cfgGroup( instance()->config(), "Settings" );
	m_pCanvas->setFastScale( ! cfgGroup.readBoolEntry( "Smooth Scaling", ! m_pCanvas->fastScale() ) );
	m_pCanvas->setKeepAspectRatio( cfgGroup.readBoolEntry( "Keep Aspect Ratio", m_pCanvas->keepAspectRatio() ) );
	m_pCanvas->setCentered( cfgGroup.readBoolEntry( "Center Image", m_pCanvas->centered() ) );

	m_pCanvas->setBgColor( cfgGroup.readColorEntry( "Background Color", &m_pCanvas->bgColor() ) );

	m_pCanvas->setMinimumImageSize( QSize( cfgGroup.readNumEntry( "Minimum Width", m_pCanvas->minimumImageSize().width() ),
				cfgGroup.readNumEntry( "Minimum Height", m_pCanvas->minimumImageSize().height() ) ) );
	m_pCanvas->setMaximumImageSize( QSize( cfgGroup.readNumEntry( "Maximum Width", m_pCanvas->maximumImageSize().width() ),
				cfgGroup.readNumEntry( "Maximum Height", m_pCanvas->maximumImageSize().height() ) ) );

	KConfigGroup blendConfig( instance()->config(), "Blend Effects" );
	m_vEffects.clear();
	for( unsigned int i = 1; i <= m_pCanvas->numOfBlendEffects(); ++i )
	{
		if( blendConfig.readBoolEntry( QString::number( i ), false ) )
			m_vEffects.push_back( i );
	}
	// and now tell the canvas what blend effect to use
	switchBlendEffect();
	loadPlugins();
}

void KViewViewer::writeSettings()
{
	KConfigGroup cfgGroup( instance()->config(), "Settings" );
	cfgGroup.writeEntry( "hideScrollbars", ! m_paShowScrollbars->isChecked() );
}

void KViewViewer::zoomChanged( double zoom )
{
	kdDebug( 4610 ) << k_funcinfo << endl;
	emit setWindowCaption( m_sCaption + QString( " (%1%)" ).arg( zoom * 100, 0, 'f', 0 ) );
	updateZoomMenu( zoom );
}

void KViewViewer::slotJobFinished( KIO::Job * job )
{
	assert( job == m_pJob );
	m_pJob = 0;
	if (job->error())
		emit canceled( job->errorString() );
	else
	{
		openFile();
		emit completed();
	}
}

void KViewViewer::slotData( KIO::Job *, const QByteArray & data )
{
	if( ! m_pBuffer )
	{
		m_pBuffer = new QBuffer();
		m_pBuffer->open( IO_ReadWrite );
	}
	m_pBuffer->writeBlock( data.data(), data.size() );

	//show partial image XXX: is this really the way to do it?
	//No. :) It takes forever like this.
	//OK. So I really have to look at khtml...
	//later...
	//m_pCanvas->setImage( QImage( m_pBuffer->buffer() ) );
}

void KViewViewer::slotSave()
{
	kdDebug( 4610 ) << k_funcinfo << endl;
	if( ! save() )
		KMessageBox::error( m_pParentWidget, i18n( "The image could not be saved to disk. A possible causes is that you don't have permission to write to that file." ) );
}

void KViewViewer::slotSaveAs()
{
	kdDebug( 4610 ) << k_funcinfo << endl;
	KFileDialog dlg( QString::null, QString::null, widget(), "filedialog", true );
	dlg.setMimeFilter( KImageIO::mimeTypes( KImageIO::Writing ) );
	dlg.setSelection( m_url.fileName() );
	dlg.setCaption( i18n( "Save As" ) );
	dlg.setOperationMode( KFileDialog::Saving );
	dlg.exec();
	KURL url = dlg.selectedURL();
	m_newMimeType = dlg.currentMimeFilter();
	if( m_newMimeType.isEmpty() )
		m_newMimeType = KImageIO::mimeType( url.path() );
	kdDebug( 4610 ) << k_funcinfo << "m_newMimeType = " << m_newMimeType << endl;
	if( url.isValid() )
		KRecentDocument::add( url );
	saveAs( url );
}

void KViewViewer::slotZoomIn()
{
	double zoom = m_pCanvas->zoom() * 1.1;
	if (zoom < 0.01) zoom=0.01 ;
	m_pCanvas->setZoom( zoom );
}

void KViewViewer::slotZoomOut()
{
	double zoom = m_pCanvas->zoom() / 1.1;
	if (zoom < 0.01) zoom=0.01 ;
	m_pCanvas->setZoom( zoom );
}

void KViewViewer::setZoom( const QString & newZoom )
{
	kdDebug( 4610 ) << k_funcinfo << newZoom << endl;
	double zoom;
	QString z = newZoom;
	z.remove( z.find( '%' ), 1 );
	if( newZoom == "33%" )
		zoom = 1.0 / 3.0;
	else
		zoom = KGlobal::locale()->readNumber( z ) / 100;

	m_pCanvas->setZoom( zoom );
}

void KViewViewer::updateZoomMenu( double zoom )
{
	QStringList lst;
	if( zoom > 0.0 )
	{
		//lst << i18n( "Maxpect" );
		QValueList<int> list;
		QString z;
		int val;
		bool ok;
		QStringList itemsList = m_paZoom->items();
		for( QStringList::Iterator it = itemsList.begin(); it != itemsList.end(); ++it )
		{
			z = ( *it ).replace( QRegExp( "%" ), "" );
			z = z.simplifyWhiteSpace();
			val = z.toInt( &ok );
			if( ok && val > 0 && list.contains( val ) == 0 )
				list << val;
		}
		val = QString::number( zoom * 100, 'f', 0 ).toInt(); // round/lround from math.h doesn't work - dunno
		if( list.contains( val ) == 0 )
			list.append( val );

		qHeapSort( list );

		for( QValueList<int>::Iterator it = list.begin(); it != list.end(); ++it )
			lst << QString::number( *it ) + '%';
		m_paZoom->setItems( lst );
	}

	// first look if it's a new value (not in the list yet)
	QString z = QString( "%1%" ).arg( zoom * 100, 0, 'f', 0 );
	QStringList items = m_paZoom->items();
	int idx = items.findIndex( z );
	if( -1 == idx )
	{
		// not found XXX: remove when done
		kdDebug( 4610 ) << "zoom value not in the list: " << z << endl;
		kdDebug( 4610 ) << "here's the list:\n- " << items.join( "\n- " ) << endl;
	}
	else
	{
		// already in the list
		kdDebug( 4610 ) << "zoom value already in the list: " << z << endl;
		// make that one selected
		m_paZoom->setCurrentItem( idx );
	}
}

void KViewViewer::slotFlipH()
{
	m_pCanvas->flipVertical( isReadWrite() );
}

void KViewViewer::slotFlipV()
{
	m_pCanvas->flipHorizontal( isReadWrite() );
}

void KViewViewer::slotRotateCCW()
{
	m_pCanvas->rotate( -90.0, isReadWrite() );
}

void KViewViewer::slotRotateCW()
{
	m_pCanvas->rotate( 90.0, isReadWrite() );
}

void KViewViewer::slotFitToWin()
{
	m_pCanvas->boundImageTo( widget()->size() );
}

void KViewViewer::slotDel()
{
	//just in case m_url get's modified while closing
	KURL urltodel = m_url;
	if( closeURL() )
	{
		setModified( false );
		KIO::file_delete( urltodel );
		m_pCanvas->clear();
	}
}

class PopupGUIClient : public KXMLGUIClient
{
	public:
		PopupGUIClient( KInstance *inst, const QString &doc )
		{
			setInstance( inst );
			setXML( doc );
		}
};

void KViewViewer::slotPopupMenu( const QPoint &pos )
{
	KXMLGUIClient *popupGUIClient = new PopupGUIClient( instance(), m_popupDoc );

	(void) new KAction( i18n( "Save Image As..." ), 0, this, SLOT( slotSaveAs() ),
						popupGUIClient->actionCollection(), "saveimageas" );

	// ### HACK treat the image as dir to get the back/fwd/reload buttons (Simon)
	emit m_pExtension->popupMenu( popupGUIClient, pos, m_url, m_mimeType, S_IFDIR );

	delete popupGUIClient;
}

void KViewViewer::slotResultSaveAs( KIO::Job *job )
{
	if ( job->error() )
	{
		emit canceled( job->errorString() );
		//job->showErrorDialog( widget() );
	}
	else
	{
		emit completed();
		KIO::CopyJob * cjob = ::qt_cast<KIO::CopyJob*>( job );
		if( cjob )
		{
			m_url = cjob->destURL();
			m_sCaption = m_url.prettyURL();
		}
		else
			m_sCaption = "";
		emit setWindowCaption( m_sCaption );
	}

	// Now we have to delete the tempfile if it exists and if the current
	// image position is on the local HD. If it's somewhere else we keep the
	// tempfile in case the user calls saveAs() again.
	if( m_url.isLocalFile() )
	{
		if( m_bTemp )
		{
			unlink( QFile::encodeName( m_file ) );
			m_bTemp = false;
		}
		m_file = m_url.path();
	}
}

void KViewViewer::slotFileDirty( const QString & )
{
	if( isModified() && isReadWrite() )
	{
		KPassivePopup * pop = new KPassivePopup( m_pParentWidget );
		QVBox * vb = pop->standardView( i18n( "Load changed image? - %1" ).arg( kapp->aboutData()->programName() ),
				QString::null, kapp->miniIcon() );
		( void )new QLabel( i18n( "The image %1 which you have modified has changed on disk.\n"
								  "Do you want to reload the file and lose your changes?\n"
								  "If you don't and subsequently save the image, you will lose the\n"
								  "changes that have already been saved." ).arg( url().fileName() ), vb );
		QWidget * hb = new QWidget( vb );
		QHBoxLayout * layout = new QHBoxLayout( hb );
		layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
		KPushButton * yes = new KPushButton( i18n("Reload"), hb );
		layout->addWidget( yes );
		layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
		KPushButton * no = new KPushButton( i18n("Do Not Reload"), hb );
		layout->addWidget( no );
		layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
		connect( yes, SIGNAL( clicked() ), this, SLOT( slotReloadUnmodified() ) );
		connect( yes, SIGNAL( clicked() ), pop, SLOT( hide() ) );
		connect( no, SIGNAL( clicked() ), pop, SLOT( hide() ) );
		pop->setView( vb );
		pop->setTimeout( 0 );
		pop->setAutoDelete( true );
		pop->show();
		kdDebug( 4610 ) << "popup returned\n";
	}
	else
		reload();
}

void KViewViewer::slotReloadUnmodified()
{
	setModified( false );
	reload();
	kdDebug( 4610 ) << "reload done\n";
}

void KViewViewer::slotToggleScrollbars()
{
	m_pCanvas->hideScrollbars( ! m_paShowScrollbars->isChecked() );
}

void KViewViewer::loadPlugins()
{
	KImageViewer::Viewer::loadPlugins( this, this, instance() );
	if( factory() )
	{
		QPtrList<KParts::Plugin> plugins = KParts::Plugin::pluginObjects( this );
		KParts::Plugin * plugin;
		for( plugin = plugins.first(); plugin; plugin = plugins.next() )
			factory()->addClient( plugin );
	}
}

void KViewViewer::switchBlendEffect()
{
	if( m_vEffects.empty() )
		m_pCanvas->setBlendEffect( 0 );
	else
	{
		unsigned int num = KApplication::random() % m_vEffects.size();
		m_pCanvas->setBlendEffect( m_vEffects[ num ] );
	}
}

void KViewViewer::hasImage( bool b )
{
	emit m_pExtension->enableAction( "del", b );
	emit m_pExtension->enableAction( "print", b );
	if( ! b )
	{
		m_sCaption = i18n( "Title caption when no image loaded", "No Image Loaded" );
		emit setWindowCaption( m_sCaption );
	}
}

// vim:sw=4:ts=4

#include "kviewviewer.moc"

