/*  This file is part of the KDE project
    Copyright (C) 2001-2002 Matthias Kretz <kretz@kde.org>

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

#include "kimagecanvas.h"
#include "kimageholder.h"
#include "version.h"
#include "config/defaults.h"

#include <qcolor.h>
#include <qimage.h>
#include <qapplication.h>
#include <qwmatrix.h>
#include <qtimer.h>

#include <kpixmap.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <ksettings/dispatcher.h>
#include <kconfig.h>

#define KIMAGECANVAS_WIPESIZE 5

const int MOUSECURSORHIDETIME = 3000;

//extern bool qt_use_xrender;

typedef KGenericFactory<KImageCanvas> KImageCanvasFactory;
K_EXPORT_COMPONENT_FACTORY( libkviewcanvas,
		KImageCanvasFactory( "kviewcanvas" ) )

KImageCanvas::KImageCanvas( QWidget * parent, const char * name, const QStringList & )
	: QScrollView( parent, name, WResizeNoErase | WStaticContents )
	, m_client( 0 )
	, m_oldClient( 0 )
	, m_image( 0 )
	, m_imageTransformed( 0 )
	, m_pixmap( 0 )
	, m_pTimer( new QTimer( this, "KImageCanvas/Timer" ) )
	, m_maxsize( Defaults::maxSize )
	, m_minsize( Defaults::minSize )
	, m_currentsize( 0, 0 )
	, m_zoom( 1.0 )
	, m_fastscale( ! Defaults::smoothScaling )
	, m_keepaspectratio( Defaults::keepAspectRatio )
	, m_bImageChanged( false )
	, m_bSizeChanged( false )
	, m_bNeedNewPixmap( false )
	, m_bCentered( Defaults::centerImage )
	, m_bImageUpdateScheduled( false )
	, m_bNewImage( false )
	, m_iBlendTimerId( 0 )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	setFrameStyle( QFrame::NoFrame );
	setResizePolicy( QScrollView::Manual );
	setMinimumSize( 0, 0 );
	setBgColor( Defaults::bgColor );

	connect( this, SIGNAL( imageChanged() ), this, SLOT( slotImageChanged() ) );
	connect( m_pTimer, SIGNAL( timeout() ), this, SLOT( hideCursor() ) );

	KSettings::Dispatcher::self()->registerInstance(
			KImageCanvasFactory::instance(), this,
			SLOT( loadSettings() ) );

	viewport()->setFocusProxy( this );
	clear();

	QWidget::setMouseTracking( true );
	viewport()->setMouseTracking( true );
	m_cursor.setShape( Qt::CrossCursor );
	viewport()->setCursor( m_cursor );
	m_pTimer->start( MOUSECURSORHIDETIME, true );

	loadSettings();
}

KImageCanvas::~KImageCanvas()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	delete m_image; m_image = 0;
	delete m_pixmap; m_pixmap = 0;
}

void KImageCanvas::setBgColor( const QColor & color )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	viewport()->setPaletteBackgroundColor( color );
	if( m_client )
		m_client->setPaletteBackgroundColor( color );
}

const QColor & KImageCanvas::bgColor() const
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	return viewport()->paletteBackgroundColor();
}

int KImageCanvas::imageDepth() const
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( ! m_image )
		return 0;

	return m_image->depth();
}

QSize KImageCanvas::imageSize() const
{
	//kdDebug( 4620 ) << k_funcinfo << endl;
	if( ! m_image )
		return QSize( 0, 0 );

	return m_matrix.isIdentity() ? m_image->size() : m_matrix.mapRect( QRect( QPoint(), m_image->size() ) ).size();
}

QSize KImageCanvas::currentSize() const
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( ! m_image )
		return QSize( 0, 0 );

	return m_currentsize;
}

const QImage * KImageCanvas::image() const
{
	if( m_imageTransformed )
		return m_imageTransformed;
	return m_image;
}

QRect KImageCanvas::selection() const
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( m_client )
		return m_selection;
	else
		return QRect();
}

void KImageCanvas::setCentered( bool centered )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( m_bCentered != centered )
	{
		m_bCentered = centered;
		center();
	}
}

void KImageCanvas::setImage( const QImage & newimage )
{
	bool emitHasImage = m_image ? false : true;
	m_matrix.reset();
	matrixChanged();
	delete m_image;
	m_image = new QImage( newimage );
	m_bNewImage = true;
	// don't emit the signal here - call the slot directly
	slotImageChanged();

	sizeFromZoom( m_zoom );
	updateImage();
	if( emitHasImage && m_image )
		emit hasImage( true );
}

void KImageCanvas::setImage( const QImage & newimage, const QSize & size )
{
	kdDebug( 4620 ) << k_funcinfo << size << endl;
	bool emitHasImage = m_image ? false : true;
	m_matrix.reset();
	matrixChanged();
	delete m_image;
	m_image = new QImage( newimage );
	m_bNewImage = true;
	// don't emit the signal here - call the slot directly
	slotImageChanged();

	resizeImage( size );
	updateImage();
	if( emitHasImage && m_image )
		emit hasImage( true );
}

void KImageCanvas::setZoom( double zoom )
{
	kdDebug( 4620 ) << k_funcinfo << zoom << endl;
	if( m_image == 0 )
		return;

	if( zoom > 0.0 && m_zoom != zoom )
	{
		m_zoom = zoom;
		sizeFromZoom( m_zoom );
		emit zoomChanged( m_zoom );
		updateImage();
	}
}

void KImageCanvas::boundImageTo( const QSize & size )
{
	bool keepAspectRatio = m_keepaspectratio;
	m_keepaspectratio = true;
	resizeImage( size );
	m_keepaspectratio = keepAspectRatio;
}

void KImageCanvas::setMaximumImageSize( const QSize & maxsize )
{
	kdDebug( 4620 ) << k_funcinfo << maxsize << endl;
	if( ( ! m_minsize.isEmpty() ) &&
			( maxsize.width() < m_minsize.width() || maxsize.height() < m_minsize.height() ) )
	{
		kdWarning( 4620 ) << "the new maximum image size is smaller than the minimum size" << endl;
		return;
	}

	m_maxsize = maxsize;

	resizeImage( m_currentsize );
}

void KImageCanvas::setMinimumImageSize( const QSize & minsize )
{
	kdDebug( 4620 ) << k_funcinfo << minsize << endl;
	if( ( ! m_maxsize.isEmpty() ) &&
			( minsize.width() > m_maxsize.width() || minsize.height() > m_maxsize.height() ) )
	{
		kdWarning( 4620 ) << "the new minimum image size is greater than the maximum size" << endl;
		return;
	}

	m_minsize = minsize;

	resizeImage( m_currentsize );
}

void KImageCanvas::resizeImage( const QSize & newsize )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( m_image == 0 )
		return;

	QSize size = newsize;

	// check that it fits into min and max sizes
	checkBounds( size );

	// calculate the new zoom factor
	zoomFromSize( size );

	if( size != m_currentsize )
	{
		m_currentsize = size;
		sizeChanged();

		updateImage();
	}
}

void KImageCanvas::hideScrollbars( bool hidden )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( hidden )
	{
		setVScrollBarMode( AlwaysOff );
		setHScrollBarMode( AlwaysOff );
	}
	else
	{
		setVScrollBarMode( Auto );
		setHScrollBarMode( Auto );
	}
}

void KImageCanvas::setKeepAspectRatio( bool aspect )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	m_keepaspectratio = aspect;
}

unsigned int KImageCanvas::numOfBlendEffects() const
{
	return Defaults::numOfBlendEffects;
}

QString KImageCanvas::blendEffectDescription( unsigned int idx ) const
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	switch( idx )
	{
		case NoBlending:
			kdWarning( 4620 ) << k_funcinfo << " shouldn't be called with an index of 0 - That's always not really defined\n";
			return i18n( Defaults::blendEffectDescription[ 0 ] );
		case AlphaBlend:
			return i18n( Defaults::blendEffectDescription[ 5 ] );
		case WipeFromLeft:
			return i18n( Defaults::blendEffectDescription[ 1 ] );
		case WipeFromRight:
			return i18n( Defaults::blendEffectDescription[ 2 ] );
		case WipeFromTop:
			return i18n( Defaults::blendEffectDescription[ 3 ] );
		case WipeFromBottom:
			return i18n( Defaults::blendEffectDescription[ 4 ] );
	}
	kdError( 4620 ) << "Effect description for effect with index " << idx << " doesn't exist\n";
	return QString::null;
}

bool KImageCanvas::eventFilter( QObject * obj, QEvent * ev )
{
	if( ( obj == m_client || obj == m_oldClient ) && ev->type() == QEvent::MouseMove )
		mouseMoveEvent( static_cast<QMouseEvent*>( ev ) );
	return QScrollView::eventFilter( obj, ev );
}

void KImageCanvas::setFastScale( bool fastscale )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	m_fastscale = fastscale;
	if( m_fastscale )
	{
		// wo do scaling with a matrix now, so the m_imageTransformed isn't needed anymore
		delete m_imageTransformed;
		m_imageTransformed = 0;
	}
	else
	{
		matrixChanged(); // set the flag to dirty so that a new m_imageTransformed will be created
						 // else we very relyably get a crash
	}
	updateImage();
}

void KImageCanvas::clear()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	bool emitHasImage = m_image ? true : false;
	delete m_image;
	m_image = 0;
	m_currentsize -= m_currentsize; //zero size
	if( m_client )
		m_client->clear();
	if( emitHasImage && ! m_image )
		emit hasImage( false );
}

void KImageCanvas::flipHorizontal( bool change )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( m_image == 0 )
		return;

	if( change )
	{
		QWMatrix matrix( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F );
		*m_image = m_image->xForm( matrix );
		emit imageChanged();
	}
	else
	{
		m_matrix.scale( 1.0, -1.0 );
		matrixChanged();
	}
	// size didn't change
	updateImage();
}

void KImageCanvas::flipVertical( bool change )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( m_image == 0 )
		return;

	if( change )
	{
		QWMatrix matrix( -1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F );
		*m_image = m_image->xForm( matrix );
		emit imageChanged();
	}
	else
	{
		m_matrix.scale( -1.0, 1.0 );
		matrixChanged();
	}
	// size didn't change
	updateImage();
}

void KImageCanvas::rotate( double a, bool change )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( m_image == 0 )
		return;

	if( change )
	{
		QWMatrix matrix;
		matrix.rotate( a );
		*m_image = m_image->xForm( matrix );
		emit imageChanged();
	}
	else
	{
		m_matrix.rotate( a );
		matrixChanged();
	}
	//adjust m_currentsize
	sizeFromZoom( m_zoom );
	updateImage();
}

void KImageCanvas::checkBounds( QSize & newsize )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( m_keepaspectratio )
	{
		// check that the new size has the same aspect ratio the original image had
		QSize origsize = imageSize();
		double x1 = double( origsize.height() ) / double( newsize.height() );
		double x2 = double( origsize.width() ) / double( newsize.width() );
		if( ( newsize * x1 != origsize ) || ( newsize * x2 != origsize ) )
		{
			// not OK
			kdDebug( 4620 ) << "checkBounds: the aspect ratio wasn't kept changing from " << newsize << endl;
			// the user want's that the aspect ratio doesn't change. The
			// question is: make it larger or smaller?
			// we make it smaller (we depend on that in boundImageTo)
			newsize = origsize / KMAX( x1, x2 );
			kdDebug( 4620 ) << "checkBounds: to " << newsize << endl;
		}
	}
	if( ( ! m_maxsize.isEmpty() ) &&
			( newsize.width() > m_maxsize.width() || newsize.height() > m_maxsize.height() ) )
	{
		kdDebug( 4620 ) << "checkBounds: the new size is bigger than the max size" << endl;
		if( m_keepaspectratio )
		{
			double x1 = double( m_maxsize.height() ) / double( newsize.height() );
			double x2 = double( m_maxsize.width() ) / double( newsize.width() );
			double x = KMIN( x1, x2 );//( x1 > x2 ) ? x2 : x1;
			newsize *= x;
		}
		else
			newsize = newsize.boundedTo( m_maxsize );
	}
	if( ( ! m_minsize.isEmpty() ) &&
			( newsize.width() < m_minsize.width() || newsize.height() < m_minsize.height() ) )
	{
		kdDebug( 4620 ) << "checkBounds: the new size is smaller than the min size" << endl;
		if( m_keepaspectratio )
		{
			double x1 = double( m_minsize.height() ) / double( newsize.height() );
			double x2 = double( m_minsize.width() ) / double( newsize.width() );
			double x = KMAX( x1, x2 );//( x1 > x2 ) ? x1 : x2;
			newsize *= x;
		}
		else
			newsize = newsize.expandedTo( m_minsize );
	}
	// if it still won't fit we have a problem: we can't keep the aspect ratio or we have
	// to violate the min/max settings
	if( ( ! m_maxsize.isEmpty() ) &&
			( newsize.width() > m_maxsize.width() || newsize.height() > m_maxsize.height() ) )
	{
		kdDebug( 4620 ) << "checkBounds: Sorry, I can't keep the aspect ratio." << endl;
		newsize = newsize.boundedTo( m_maxsize );
	}
}

void KImageCanvas::zoomFromSize( const QSize & newsize )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( ! m_image )
		return;

	QSize originalsize = imageSize();
	double widthzoom = double( newsize.width() ) / double( originalsize.width() );
	double heightzoom = double( newsize.height() ) / double( originalsize.height() );
	double zoom = ( widthzoom + heightzoom ) / 2;
	if( zoom != m_zoom )
	{
		m_zoom = zoom;
		emit zoomChanged( m_zoom );
	}
}

void KImageCanvas::sizeFromZoom( double zoom )
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( ! m_image )
		return;

	QSize newsize = zoom * imageSize();
	kdDebug( 4620 ) << "change size from " << imageSize() << " to " << newsize << endl;
	resizeImage( newsize );
}

void KImageCanvas::updateImage()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( ! m_bImageUpdateScheduled )
		QTimer::singleShot( 0, this, SLOT( slotUpdateImage() ) );
	m_bImageUpdateScheduled = true;
}

void KImageCanvas::slotUpdateImage()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	m_bImageUpdateScheduled = false;
	if( m_image == 0 )
		return;

	//only update if something was changed
	if( m_bImageChanged || m_bSizeChanged || m_bMatrixChanged )
	{
		kdDebug( 4620 ) << "actually updating the image now" << endl;
		QApplication::setOverrideCursor( WaitCursor );
		if( m_bNewImage || ! m_client )
		{
			finishNewClient();
			m_oldClient = m_client;
			m_client = createNewClient();
		}
		m_client->setImage( pixmap() );

		if( m_bSizeChanged || m_bNewImage )
		{
			QSize sh = m_client->sizeHint();
			if( ! sh.isValid() )
				sh = QSize( 0, 0 );
			m_client->resize( sh );
			resizeContents( sh.width(), sh.height() );
			center();
		}
		QRect drawRect = m_client->drawRect();
		switch( m_iBlendEffect )
		{
			case NoBlending:
				break;
			case AlphaBlend:
				break;
			case WipeFromLeft:
				drawRect.setRight( KIMAGECANVAS_WIPESIZE + contentsX() );
				m_client->setDrawRect( drawRect );
				break;
			case WipeFromRight:
				drawRect.rLeft() += KMIN( drawRect.width() - KIMAGECANVAS_WIPESIZE, contentsX() + visibleWidth() );
				m_client->setDrawRect( drawRect );
				break;
			case WipeFromTop:
				drawRect.setBottom( KIMAGECANVAS_WIPESIZE + contentsY() );
				m_client->setDrawRect( drawRect );
				break;
			case WipeFromBottom:
				drawRect.setTop( KMIN( drawRect.height() - KIMAGECANVAS_WIPESIZE, contentsY() + visibleHeight() ) );
				m_client->setDrawRect( drawRect );
				break;
		}
		m_client->update();
		m_iBlendTimerId = startTimer( 5 );
		QApplication::restoreOverrideCursor();
	}

	m_bNewImage = false;
	m_bImageChanged = false;
	m_bSizeChanged = false;
	m_bMatrixChanged = false;
}

void KImageCanvas::mouseMoveEvent( QMouseEvent * )
{
	if( m_cursor.shape() == Qt::BlankCursor )
	{
		m_cursor.setShape( Qt::CrossCursor );
		viewport()->setCursor( m_cursor );
		if( m_client )
			m_client->setCursor( m_cursor );
	}
	m_pTimer->start( MOUSECURSORHIDETIME, true );
}


void KImageCanvas::resizeEvent( QResizeEvent * ev )
{
	kdDebug( 4620 ) << "KImageCanvas resized to " << ev->size() << endl;
	QScrollView::resizeEvent( ev );
	center();
}

void KImageCanvas::contentsMousePressEvent( QMouseEvent * ev )
{
	if ( ev->button() == RightButton )
		emit contextPress( ev->globalPos() );
	QScrollView::contentsMousePressEvent( ev );
}

void KImageCanvas::contentsWheelEvent( QWheelEvent * ev )
{
	//kdDebug( 4620 ) << k_funcinfo << endl;
    // Ctrl+Wheelmouse changes the zoom.
    // Wheelmouse scrolls around
    if ( ev->state() & ControlButton )
    {
		int delta = ev->delta() / 120;
		double zoom = m_zoom;
		// make zoom a value of 1/16, 1/15, 1/14, .. , 1/2, 1, 2, 3, .. , 15, 16
		bool done = false;
		for( int i = 15; i > 0; --i )
		{
			if( zoom <= ( 1.0 / i ) )
			{
				if( zoom < ( 1.0 / ( i + 0.5 ) ) )
					zoom = ( 1.0 / ( i + 1 ) );
				else
					zoom = ( 1.0 / i );
				done = true;
				// zoom = 1/16, 1/15, .. , 1/2, 1
				double x = 1.0 / zoom - delta;
				if( x == 0 )
					zoom = 2.0;
				else
					zoom = 1.0 / x;
				break;
			}
		}
		if( ! done )
			for( int i = 2; i < 17; ++i )
			{
				if( zoom < (double)i )
				{
					if( zoom < ( i - 0.5 ) )
						zoom = i - 1.0;
					else
						zoom = (double)i;
					done = true;
					// zoom = 1, 2, .., 15, 16
					zoom = zoom + delta;
					if( zoom < 0.9 )
						zoom = 0.5;
					break;
				}
			}
		if( ! done )
		{
			zoom = 16.0;
			zoom = zoom + delta;
			if( zoom > 16.0 )
				zoom = 16.0;
		}
		kdDebug( 4620 ) << "Mousewheel: oldzoom = " << m_zoom << " newzoom = " << zoom << endl;
		ev->accept();
		bool oldscale = fastScale();
		setFastScale( true );
		setZoom( zoom );
		setFastScale( oldscale );
    }
    else
        QScrollView::contentsWheelEvent( ev );
}

void KImageCanvas::keyPressEvent( QKeyEvent * ev )
{
	//kdDebug( 4620 ) << k_funcinfo << endl;
	switch( ev->key() )
	{
		case Key_Down:
			ev->accept();
			verticalScrollBar()->addLine();
			break;
		case Key_Up:
			ev->accept();
			verticalScrollBar()->subtractLine();
			break;
		case Key_Left:
			ev->accept();
			horizontalScrollBar()->subtractLine();
			break;
		case Key_Right:
			ev->accept();
			horizontalScrollBar()->addLine();
			break;
		case Key_PageUp:
			ev->accept();
			verticalScrollBar()->subtractPage();
			break;
		case Key_PageDown:
			ev->accept();
			verticalScrollBar()->addPage();
			break;
		default:
			ev->ignore();
			break;
	}
}

void KImageCanvas::timerEvent( QTimerEvent * ev )
{
	if( ev->timerId() == m_iBlendTimerId )
	{
		QRect drawRect = m_client->drawRect();
		switch( m_iBlendEffect )
		{
			case NoBlending:
				finishNewClient();
				break;
			case AlphaBlend:
				finishNewClient();
				//if( qt_use_xrender )
				//{
				//}
				//else
				//{
					//kdWarning( 4620 ) << "no XRender" << endl;
					//finishNewClient();
				//}
				break;
			case WipeFromLeft:
				drawRect.rRight() += KIMAGECANVAS_WIPESIZE;
				m_client->setDrawRect( drawRect );
				m_client->update( drawRect.right() - KIMAGECANVAS_WIPESIZE, 0, KIMAGECANVAS_WIPESIZE, m_client->height() );
				if( drawRect.right() >= contentsX() + visibleWidth() )
					finishNewClient();
				break;
			case WipeFromRight:
				drawRect.rLeft() -= KIMAGECANVAS_WIPESIZE;
				m_client->setDrawRect( drawRect );
				m_client->update( drawRect.left(), 0, KIMAGECANVAS_WIPESIZE, m_client->height() );
				if( drawRect.left() <= contentsX() )
					finishNewClient();
				break;
			case WipeFromTop:
				drawRect.rBottom() += KIMAGECANVAS_WIPESIZE;
				m_client->setDrawRect( drawRect );
				m_client->update( 0, drawRect.bottom() - KIMAGECANVAS_WIPESIZE, m_client->width(), KIMAGECANVAS_WIPESIZE );
				if( drawRect.bottom() >= contentsY() + visibleHeight() )
					finishNewClient();
				break;
			case WipeFromBottom:
				drawRect.rTop() -= KIMAGECANVAS_WIPESIZE;
				m_client->setDrawRect( drawRect );
				m_client->update( 0, drawRect.top(), m_client->width(), KIMAGECANVAS_WIPESIZE );
				if( drawRect.top() <= contentsY() )
					finishNewClient();
				break;
			default:
				kdFatal( 4620 ) << "unknown Blend Effect" << endl;
				break;
		}
	}
	else
		killTimer( ev->timerId() );
}

void KImageCanvas::hideCursor()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	m_cursor.setShape( Qt::BlankCursor );
	viewport()->setCursor( m_cursor );
	if( m_client )
		m_client->setCursor( m_cursor );
}

const KPixmap KImageCanvas::pixmap()
{
	kdDebug( 4620 ) << k_funcinfo << ( m_bNeedNewPixmap ? "convert from Image" : "use old copy" ) << endl;
	// create a new Pixmap in m_pixmap if needed
	if( m_bNeedNewPixmap )
	{
		// only do it again if requested
		m_bNeedNewPixmap = false;
		// ok, the old one may go now
		delete m_pixmap;

		// if smoothscaling is wanted and the transformation matrix or the image
		// itself changed...
		if( ! m_fastscale && ( m_bMatrixChanged || m_bImageChanged ) )
		{
			delete m_imageTransformed;
			// we create a new image transformed by the matrix
			m_imageTransformed = new QImage( m_matrix.isIdentity() ? *m_image : m_image->xForm( m_matrix ) );
			kdDebug( 4620 ) << "Size of m_image: " << m_image->size() << endl;
			kdDebug( 4620 ) << "Size of m_imageTransformed: " << m_imageTransformed->size() << endl;
		}
		// smoothScale or fast scaling via m_matrix
		m_pixmap = new KPixmap();
		m_pixmap->convertFromImage( m_fastscale ? *m_image : m_imageTransformed->smoothScale( m_currentsize ), KPixmap::ColorOnly );
	}
	if( m_fastscale )
	{
		// fast scaling is needed so we need to scale now
		QWMatrix matrix( m_matrix );
		matrix.scale( m_zoom, m_zoom );
		if( ! matrix.isIdentity() )
			return m_pixmap->xForm( matrix );
	}
	return *m_pixmap;
}

void KImageCanvas::slotImageChanged()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	m_bImageChanged = true;
	m_bNeedNewPixmap = true;
}

void KImageCanvas::loadSettings()
{
	KConfigGroup cfgGroup( KImageCanvasFactory::instance()->config(),
			"Settings" );
	setFastScale( ! cfgGroup.readBoolEntry( "Smooth Scaling", ! fastScale() ) );
	setKeepAspectRatio( cfgGroup.readBoolEntry( "Keep Aspect Ratio",
				keepAspectRatio() ) );
	setCentered( cfgGroup.readBoolEntry( "Center Image", centered() ) );

	setBgColor( cfgGroup.readColorEntry( "Background Color", &bgColor() ) );

	setMinimumImageSize( QSize( cfgGroup.readNumEntry( "Minimum Width",
					minimumImageSize().width() ), cfgGroup.readNumEntry(
					"Minimum Height", minimumImageSize().height() ) ) );
	setMaximumImageSize( QSize( cfgGroup.readNumEntry( "Maximum Width",
					maximumImageSize().width() ), cfgGroup.readNumEntry(
					"Maximum Height", maximumImageSize().height() ) ) );

	KConfigGroup blendConfig( KImageCanvasFactory::instance()->config(),
			"Blend Effects" );
	/* TODO
	m_vEffects.clear();
	for( unsigned int i = 1; i <= numOfBlendEffects(); ++i )
	{
		if( blendConfig.readBoolEntry( QString::number( i ), false ) )
			m_vEffects.push_back( i );
	}
	// and now tell the canvas what blend effect to use
	switchBlendEffect();
	*/
}

void KImageCanvas::selected( const QRect & rect )
{
	//kdDebug( 4620 ) << k_funcinfo << rect << endl;
	m_selection = rect;
	if( ! m_selection.isNull() )
	{
		m_selection.setTop( int( ( m_selection.top() + 0.5 ) / m_zoom ) );
		m_selection.setLeft( int( ( m_selection.left() + 0.5 ) / m_zoom ) );
		m_selection.setRight( int( ( m_selection.right() + 0.5 ) / m_zoom ) );
		m_selection.setBottom( int( ( m_selection.bottom() + 0.5 ) / m_zoom ) );
	}
	//kdDebug( 4620 ) << "m_selection = " << m_selection << endl;
	emit selectionChanged( m_selection );
}

void KImageCanvas::mapCursorPos( const QPoint & pos )
{
	QPoint mapped( static_cast<int>( ( pos.x() + 0.5 ) / m_zoom ), static_cast<int>( ( pos.y() + 0.5 ) / m_zoom ) );
	//kdDebug( 4620 ) << k_funcinfo << pos << " -> " << mapped << endl;
	emit cursorPos( mapped );
}

void KImageCanvas::sizeChanged()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	m_bSizeChanged = true;
	if( ! m_fastscale )
		m_bNeedNewPixmap = true;
	emit imageSizeChanged( m_currentsize );
}

void KImageCanvas::matrixChanged()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	m_bMatrixChanged = true;
	m_bNeedNewPixmap = true;
}

void KImageCanvas::center()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	if( m_bCentered && m_client )
	{
		int x = 0;
		int y = 0;

		int scrollbarwidth = ( height() >= m_currentsize.height() ) ? 0 : verticalScrollBar()->width();
		int scrollbarheight = ( width() - scrollbarwidth >= m_currentsize.width() ) ? 0 : horizontalScrollBar()->height();
		scrollbarwidth = ( height() - scrollbarheight >= m_currentsize.height() ) ? 0 : verticalScrollBar()->width();

		int availheight = height() - scrollbarheight;
		int availwidth = width() - scrollbarwidth;

		if( availwidth > m_currentsize.width() )
			x = ( availwidth - m_currentsize.width() ) / 2;
		if( availheight > m_currentsize.height() )
			y = ( availheight - m_currentsize.height() ) / 2;

		kdDebug( 4620 ) << "center with left top at " << x << ", " << y << endl;
		moveChild( m_client, x, y );
	}
}

void KImageCanvas::finishNewClient()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	killTimer( m_iBlendTimerId );
	if( m_client )
		m_client->setDrawRect( m_client->rect() );
	delete m_oldClient;
	m_oldClient = 0;
	emit showingImageDone();
}

KImageHolder * KImageCanvas::createNewClient()
{
	kdDebug( 4620 ) << k_funcinfo << endl;
	KImageHolder * client = new KImageHolder( viewport() );
	client->setMinimumSize( 0, 0 );
	client->setMouseTracking( true );
	client->installEventFilter( this );
	setFocusProxy( client );
	client->setFocusPolicy( QWidget::StrongFocus );
	client->setFocus();

	addChild( client, 0, 0 );

	connect( client, SIGNAL( contextPress( const QPoint& ) ), SIGNAL( contextPress( const QPoint& ) ) );
	connect( client, SIGNAL( cursorPos( const QPoint & ) ), SLOT( mapCursorPos( const QPoint & ) ) );
	connect( client, SIGNAL( selected( const QRect & ) ), SLOT( selected( const QRect & ) ) );
	connect( client, SIGNAL( wannaScroll( int, int ) ), SLOT( scrollBy( int, int ) ) );

	return client;
}

#include "kimagecanvas.moc"

// vim:sw=4:ts=4
