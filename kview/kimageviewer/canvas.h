/*  This file is part of the KDE project
    Copyright (C) 2001-2002 Matthias Kretz <kretz@kde.org>

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
// $Id$

#ifndef KIMAGEVIEWER_CANVAS_H
#define KIMAGEVIEWER_CANVAS_H

class QColor;
class QSize;
class QImage;
class QRect;
class QPoint;
class QWidget;

#include <qstring.h>
#include <qobjectdefs.h>
#include <kdemacros.h>
namespace KImageViewer
{

/**
 * An image canvas widget
 *
 * @author Matthias Kretz <kretz@kde.org>
 *
 * You'll find an implementation in kdegraphics (KView). You may
 * still use this interface in your program but it will only work
 * if you have an implementation installed.
 *
 * This is what you have to do to get an image canvas embedded in
 * your program:
 *
 * <pre>
   QWidget * widget = KParts::ComponentFactory::createInstanceFromQuery<QWidget>( "KImageViewer/Canvas", QString::null, this );
   m_canvas = static_cast<KImageViewer::Canvas *>( widget->qt_cast( "KImageViewer::Canvas" ) );
   if( ! ( widget && m_canvas ) )
   {
     KMessageBox::error( this, "Could not find the Canvas!" );
     kapp->quit();
   }
   setCentralWidget( widget );
   connect( widget, SIGNAL( contextPress( const QPoint & ) ), SLOT( mySlot( const QPoint & ) ) );
   connect( m_canvas->widget(), SIGNAL( contextPress( const QPoint & ) ), SLOT( mySlot( const QPoint & ) ) );
   </pre>
 *
 * You can't connect signals or slots using KImageViewer::Canvas, because this interface
 * doesn't inherit from QObject. But you can cast to QWidget and use that instead (or just
 * keep the original pointer to QWidget around, like shown in the example).
 *
 * This interface is not guaranteed to be kept binary or source compatible
 * until it's finished. So if you're using this interface please get in contact
 * with me.
 */
class KDE_EXPORT Canvas
{
	public:
		/**
		 * set the background color of the canvas
		 */
		virtual void setBgColor( const QColor & ) = 0;

		/**
		 * returns the current background color
		 */
		virtual const QColor & bgColor() const = 0;

		/**
		 * the depth of the contained image
		 */
		virtual int imageDepth() const = 0;

		/**
		 * the size of the unzoomed image
		 */
		virtual QSize imageSize() const = 0;

		/**
		 * the size of the zoomed (current) image
		 */
		virtual QSize currentSize() const = 0;

		/**
		 * returns the zoom factor
		 */
		virtual double zoom() const = 0;

		/**
		 * @return The current (unzoomed) image
		 * Take care that the canas may delete the image so you probably need to
		 * make a copy of the image if you want to keep it around.
		 */
		virtual const QImage * image() const = 0;

		/**
		 * Scrolls the content so that the point (x, y) is in the top-left corner.
		 */
		virtual void setXYOffset( int x, int y ) = 0;

		/**
		 * Returns the leftmost visible X coordinate of the image.
		 */
		virtual int xOffset() const = 0;

		/**
		 * Returns the topmost visible Y coordinate of the image.
		 */
		virtual int yOffset() const = 0;

		/**
		 * Returns whether to use fast or smooth scaling
		 */
		virtual bool fastScale() const = 0;

		/**
		 * Return whether the image should always be centered.
		 */
		virtual bool centered() const = 0;

		/**
		 * Return the selected rectangle. If nothing is selected the rectangle is
		 * empty but doesn't have to be null.
		 */
		virtual QRect selection() const = 0;

		/**
		 * Returns whether the aspect ratio of the image is kept
		 */
		virtual bool keepAspectRatio() const = 0;

		/**
		 * @return the number of available blend effects
		 */
		virtual unsigned int numOfBlendEffects() const { return 0; }

		/**
		 * @return the description of the blend effect
		 */
		virtual QString blendEffectDescription( unsigned int ) const { return QString::null; }

		/**
		 * Sets the blending effect used to create a transition between images
		 */
		virtual void setBlendEffect( unsigned int idx ) { m_iBlendEffect = idx; }

		/**
		 * @return the current blend effect index
		 */
		virtual unsigned int blendEffect() const { return m_iBlendEffect; }

		/**
		 * @return the current maximum image size
		 */
		virtual const QSize & maximumImageSize() const = 0;

		/**
		 * @return the current minimum image size
		 */
		virtual const QSize & minimumImageSize() const = 0;

		/**
		 * @return a pointer to the QWidget interface of this object
		 */
		virtual QWidget * widget() = 0;

	signals:
		/**
		 * a mouse button was pressed and a context menu should be openend
		 */
		virtual void contextPress( const QPoint & ) = 0;

		/**
		 * the size of the image has changed (a new image was loaded, or the
		 * image was zoomed or cropped)
		 *
		 * it passes the new size of the image
		 */
		virtual void imageSizeChanged( const QSize & ) = 0;

		/**
		 * The zoom of the image has changed.
		 */
		virtual void zoomChanged( double zoom ) = 0;

		/**
		 * The selection has changed. Connect to this signal if you want to
		 * do something with a selection of the image (e.g. crop).
		 */
		virtual void selectionChanged( const QRect & ) = 0;

		/**
		 * Emitted when an image is finished being shown. If a blend effect is being used
		 * the signal is emitted when the effect is finished.
		 */
		virtual void showingImageDone() = 0;

		/**
		 * This signal is emitted whenever the canvas changes between image/no-image. For
		 * example, if someone calls @ref clear() hasImage( false ) is emitted if an image
		 * was shown before.
		 */
		virtual void hasImage( bool ) = 0;

		/**
		 * Some methods of the canvas not only change the way the image is shown (e.g. zoom)
		 * but also change the image itself (e.g. rotation) - @ref image() returns something
		 * different. If such a change happens this signal is emitted.
		 * It is not emitted when a new image is set with the @ref setImage() methods.
		 */
		virtual void imageChanged() = 0;

		/**
		 * The current mouse cursor position on the image.
		 */
		virtual void cursorPos( const QPoint & ) = 0;

	public slots:
		/**
		 * Set if the image should always be centered if the canvas is
		 * bigger than the image.
		 */
		virtual void setCentered( bool ) = 0;

		/**
		 * Give the canvas a new image to show. The zoom level is kept.
		 */
		virtual void setImage( const QImage & ) = 0;

		/**
		 * Give the canvas a new image to show.
		 *
		 * You have to pass the size the image should have when it appears
		 * on screen.
		 */
		virtual void setImage( const QImage &, const QSize & ) = 0;

		/**
		 * Set the zoom to be used when showing the image.
		 */
		virtual void setZoom( double ) = 0;

		/**
		 * Fit the image into the requested width and height.
		 */
		virtual void boundImageTo( const QSize & size ) = 0;

		/**
		 * Set the maximum size of the image. If this is set the image will
		 * never exceed this size.
		 *
		 * If you set this to 0x0 the image size may be as big as possible
		 */
		virtual void setMaximumImageSize( const QSize & ) = 0;

		/**
		 * Set the minimum size of the image. If this is set the image will
		 * never be smaller than this size.
		 *
		 * If you set this to 0x0 the image size can be as small as possible
		 */
		virtual void setMinimumImageSize( const QSize & ) = 0;

		/**
		 * Resize the image to the given size. It will keep the aspect ratio
		 * as long as keepAspectRatio is true (default). The image will be as
		 * large as possible within the given constraints.
		 */
		virtual void resizeImage( const QSize & ) = 0;

		/**
		 * Hides the scrollbars of the canvas. It's still possible to scroll
		 * by moving the image with the mouse.
		 */
		virtual void hideScrollbars( bool ) = 0;

		/**
		 * Changes the zoom behaviour: Normally the aspect ratio of the image
		 * won't change, but if you want to allow it you may do.
		 */
		virtual void setKeepAspectRatio( bool ) = 0;

		/**
		 * If the canvas supports different methods for scaling you may
		 * switch between fast and smooth scaling.
		 *
		 * It defaults to smooth scaling.
		 */
		virtual void setFastScale( bool ) = 0;

		/**
		 * clears the canvas (no image loaded)
		 */
		virtual void clear() = 0;

		/**
		 * flip the image horizontally
		 *
		 * @param change  If set to true the internal image will be changed, else
		 *                only the shown image changes and @ref image() still returns
		 *                the same as before this call.
		 */
		virtual void flipHorizontal( bool change = false ) = 0;

		/**
		 * flip the image vertically
		 *
		 * @param change  If set to true the internal image will be changed, else
		 *                only the shown image changes and @ref image() still returns
		 *                the same as before this call.
		 */
		virtual void flipVertical( bool change = false ) = 0;

		/**
		 * rotate the image a degrees counterclockwise
		 *
		 * @param angle   The angle in degrees that the image should be rotated.
		 * @param change  If set to true the internal image will be changed, else
		 *                only the shown image changes and @ref image() still returns
		 *                the same as before this call.
		 */
		virtual void rotate( double angle, bool change = false ) = 0;
		
	protected:
		Canvas();
		virtual ~Canvas();
		unsigned int m_iBlendEffect;

}; //class Canvas
} //namespace KImageViewer

// vim:sw=4:ts=4

#endif // KIMAGEVIEWER_CANVAS_H
