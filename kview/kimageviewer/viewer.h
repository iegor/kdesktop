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
// $Id: viewer.h 510693 2006-02-17 20:10:29Z woebbe $

#ifndef KIMAGEVIEWER_VIEWER_H
#define KIMAGEVIEWER_VIEWER_H

#include <kparts/part.h>
#include <kdemacros.h>
namespace KParts
{
	class BrowserExtension;
}

namespace KImageViewer
{
	class Canvas;

/**
 * An image viewer KPart
 *
 * @author Matthias Kretz <kretz@kde.org>
 *
 * You'll find an implementation in kdegraphics (KView). You may
 * still use this interface in your program but it will only work
 * if you have an implementation installed
 *
 * WARNING: This interface is not guaranteed to be kept binary or source compatible
 * until it's finished. So if you're using this interface please get in contact
 * with me.
 */
class KDE_EXPORT Viewer : public KParts::ReadWritePart
{
	Q_OBJECT
	public:
		Viewer( QObject * parent = 0, const char * name = 0 );

		virtual ~Viewer();

		/**
		 * Return the canvas this viewer is using. The interface of the
		 * canvas is defined in kimageviewer/canvas.h
		 */
		virtual Canvas * canvas() const = 0;

		/**
		 * If the Viewer wants to be configurable 
		 */
		//virtual void createConfigurationDialogPages() = 0;

		/**
		 * A pointer to the Browser Extension (if available). You should always
		 * check whether this returns a valid pointer.
		 */
		virtual KParts::BrowserExtension * browserExtension() const { return 0; }

	public slots:
		/**
		 * Set a new Image. Close the old one and change the caption and file
		 * name and url and whatnot accordingly.
		 * So if you want to display a new image (not change the one shown) this
		 * is the method to use. Else take a look at Canvas::setImage().
		 */
		virtual void newImage( const QImage & ) = 0;

		/**
		 * Tell the view to reload the current image. The host for this view
		 * should make an Action available for reloading.
		 */
		virtual void reload() = 0;

	signals:
		/**
		 * Emitted when the viewer opens a new image
		 */
		void imageOpened( const KURL & );

}; //class Viewer
} //namespace KImageViewer

// vim:sw=4:ts=4

#endif // KIMAGEVIEWER_VIEWER_H
