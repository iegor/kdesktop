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
// $Id: kimagecanvas.h 465369 2005-09-29 14:33:08Z mueller $

#ifndef _KIMAGECANVAS_H
#define _KIMAGECANVAS_H

#include "kimageviewer/canvas.h"

#include <qscrollview.h>
#include <qwmatrix.h>
#include <qcursor.h>
#include <qrect.h>

#include <kdemacros.h>

class KImageHolder;
class QColor;
class QImage;
class KPixmap;

/**
 * @short KViewCanvas
 * @author Matthias Kretz <kretz@kde.org>
 * @version $Id: kimagecanvas.h 465369 2005-09-29 14:33:08Z mueller $
 */
class KDE_EXPORT KImageCanvas : public QScrollView, public KImageViewer::Canvas
{
	Q_OBJECT
	public:
		/**
		 * KImageCanvas Constructor
		 */
		KImageCanvas( QWidget * parent, const char * name, const QStringList & args );

		/**
		 * KImageCanvas Destructor
		 */
		virtual ~KImageCanvas();

		/**
		 * set the background color of the canvas
		 */
		void setBgColor( const QColor & );

		/**
		 * returns the current background color
		 */
		const QColor & bgColor() const;

		/**
		 * the depth of the contained image
		 */
		int imageDepth() const;

		/**
		 * the size of the unzoomed image
		 */
		QSize imageSize() const;

		/**
		 * the size of the zoomed (current) image
		 */
		QSize currentSize() const;

		/**
		 * returns the zoom factor
		 */
		double zoom() const { return m_zoom; }

		/**
		 * returns the current (unzoomed) image
		 */
		const QImage * image() const;

		/**
		 * Scrolls the content so that the point (x, y) is in the top-left corner.
		 */
		void setXYOffset( int x, int y ) { setContentsPos( x, y ); }

		/**
		 * Returns the leftmost visible X coordinate of the image.
		 */
		int xOffset() const { return contentsX(); }

		/**
		 * Returns the topmost visible Y coordinate of the image.
		 */
		int yOffset() const { return contentsY(); }

		/**
		 * Returns whether to use fast or smooth scaling
		 */
		bool fastScale() const { return m_fastscale; }

		/**
		 * Return whether the image should always be centered.
		 */
		bool centered() const { return m_bCentered; }

		/**
		 * Return the selected rectangle
		 */
		QRect selection() const;

		/**
		 * Returns whether the aspect ratio of the image is kept
		 */
		bool keepAspectRatio() const { return m_keepaspectratio; }

		/**
		 * @return the number of available blend effects
		 */
		unsigned int numOfBlendEffects() const;

		/**
		 * @return the description of the blend effect
		 */
		QString blendEffectDescription( unsigned int ) const;

		/**
		 * @return the current maximum image size
		 */
		const QSize & maximumImageSize() const { return m_maxsize; }

		/**
		 * @return the current minimum image size
		 */
		const QSize & minimumImageSize() const { return m_minsize; }

		/**
		 * @return a pointer to the QWidget interface of this object
		 */
		QWidget * widget() { return static_cast<QWidget *>( this ); }

		bool eventFilter( QObject *, QEvent * );

	signals:
		/**
		 * a mouse button was pressed and a context menu should be openend
		 */
		void contextPress( const QPoint& );

		/**
		 * the size of the image has changed (a new image was loaded, or the
		 * image was zoomed or cropped)
		 *
		 * it passes the new size of the image
		 */
		void imageSizeChanged( const QSize & );

		/**
		 * The zoom of the image has changed.
		 */
		void zoomChanged( double zoom );

		/**
		 * The selection has changed. Connect to this signal if you want to
		 * do something with a selection of the image (e.g. crop).
		 */
		void selectionChanged( const QRect & );

		/**
		 * Emitted when an image is finished being shown. If a blend effect is being used
		 * the signal is emitted when the effect is finished.
		 */
		void showingImageDone();

		/**
		 * This signal is emitted whenever the canvas changes between image/no-image. For
		 * example, if someone calls @ref clear() hasImage( false ) is emitted if an image
		 * was shown before.
		 */
		void hasImage( bool );

		/**
		 * Some methods of the canvas not only change the way the image is shown (e.g. zoom)
		 * but also change the image itself (e.g. rotation) - @ref image() returns something
		 * different. If such a change happens this signal is emitted.
		 * It is not emitted when a new image is set with the @ref setImage() methods.
		 */
		void imageChanged();

		/**
		 * The current mouse cursor position on the image.
		 */
		void cursorPos( const QPoint & );

	public slots:
		/**
		 * Set if the image should always be centered if the canvas is
		 * bigger than the image.
		 */
		void setCentered( bool );

		/**
		 * give the canvas a new image to show. The zoom level is kept.
		 */
		void setImage( const QImage & );

		/**
		 * Give the canvas a new image to show.
		 *
		 * You have to pass the size the image should have when it appears
		 * on screen.
		 */
		void setImage( const QImage &, const QSize & );

		/**
		 * set the zoom to be used when showing the image
		 */
		void setZoom( double );

		/**
		 * Fit the image into the requested width and height.
		 */
		void boundImageTo( const QSize & size );

		/**
		 * Set the maximum size of the image. If this is set the image will
		 * never exceed this size.
		 *
		 * If you set this to 0x0 the image size may be as big as possible
		 */
		void setMaximumImageSize( const QSize & );

		/**
		 * Set the minimum size of the image. If this is set the image will
		 * never be smaller than this size.
		 *
		 * If you set this to 0x0 the image size can be as small as possible
		 */
		void setMinimumImageSize( const QSize & );

		/**
		 * Resize the image to the given size. It will keep the aspect ratio
		 * as long as keepAspectRatio is true (default). The image will be as
		 * large as possible within the given constraints.
		 */
		void resizeImage( const QSize & );

		/**
		 * Hides the scrollbars of the canvas. It's still possible to scroll
		 * by moving the image with the mouse.
		 */
		void hideScrollbars( bool );

		/**
		 * Changes the zoom behaviour: Normally the aspect ratio of the image
		 * won't change, but if you want to allow it you may do.
		 */
		void setKeepAspectRatio( bool );

		/**
		 * If the canvas supports different methods for scaling you may
		 * switch between fast and smooth scaling.
		 *
		 * It defaults to smooth scaling.
		 */
		void setFastScale( bool );

		/**
		 * clears the canvas (no image loaded)
		 */
		void clear();

		/**
		 * flip the image horizontally
		 */
		void flipHorizontal( bool change = false );

		/**
		 * flip the image vertically
		 */
		void flipVertical( bool change = false );

		/**
		 * rotate the image a degrees counterclockwise
		 */
		void rotate( double a, bool change = false );

	protected:
		void checkBounds( QSize & newsize );
		void zoomFromSize( const QSize & );
		void sizeFromZoom( double );
		void updateImage();

		void mouseMoveEvent( QMouseEvent * );
		void resizeEvent( QResizeEvent * );
		void contentsMousePressEvent( QMouseEvent * );
		void contentsWheelEvent( QWheelEvent * );
		void keyPressEvent( QKeyEvent * );
		void timerEvent( QTimerEvent * );

	protected slots:
		void slotUpdateImage();
		void hideCursor();
		void slotImageChanged();
		void loadSettings();

	private slots:
		void selected( const QRect & ); // map rect to unzoomed rect
		void mapCursorPos( const QPoint & );

	private:
		enum BlendEffect {
			NoBlending = 0,
			WipeFromLeft = 1,
			WipeFromRight = 2,
			WipeFromTop = 3,
			WipeFromBottom = 4,
			AlphaBlend = 5
		};
		const KPixmap pixmap();
		void sizeChanged();
		void matrixChanged();
		void center();
		void finishNewClient();
		KImageHolder * createNewClient();

		KImageHolder * m_client;
		KImageHolder * m_oldClient;
		QImage * m_image; //unzoomed copy of the current image
		QImage * m_imageTransformed; //xForm( m_matrix ) copy of the current image
		KPixmap * m_pixmap; //copy of the current pixmap (if ( m_fastscale ) it's unzoomed else it's m_imageTransformed.smoothScale()d)

		QTimer * m_pTimer; // timer for single shot to hide the cursor
		QCursor m_cursor; // the cursor show in the canvas (for auto-hiding)

		QWMatrix m_matrix; // the current transformation matrix
		QSize m_maxsize, m_minsize;
		QSize m_currentsize;

		double m_zoom;
		bool m_fastscale;
		bool m_keepaspectratio;
		bool m_bImageChanged;
		bool m_bSizeChanged;
		bool m_bMatrixChanged;
		bool m_bNeedNewPixmap;
		bool m_bCentered;
		bool m_bImageUpdateScheduled;
		bool m_bNewImage;
		int m_iBlendTimerId;

		QRect m_selection; //unzoomed selection rect
};

// vim:sw=4:ts=4

#endif // _KIMAGECANVAS_H
