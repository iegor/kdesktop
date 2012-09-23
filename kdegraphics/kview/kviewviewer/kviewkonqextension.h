/*  This file is part of the KDE project
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
                  2001 Matthias Kretz <kretz@kde.org>

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
#ifndef __kviewkonqextension_h__
#define __kviewkonqextension_h__

#include <kparts/browserextension.h>

namespace KImageViewer { class Canvas; }
class KViewViewer;

class KViewKonqExtension: public KParts::BrowserExtension
{
	friend class KViewViewer; // it can emit our signals
	Q_OBJECT
	public:
		KViewKonqExtension( KImageViewer::Canvas *, KViewViewer *parent, const char *name = 0 );
		virtual ~KViewKonqExtension() {}

		virtual void setXYOffset( int x, int y );
		virtual int xOffset();
		virtual int yOffset();

	public slots:
		// Automatically detected by konqueror and kview
		void print();
		void del();

	private:
		KViewViewer * m_pViewer;
		KImageViewer::Canvas * m_pCanvas;
};

// vim:sw=4:ts=4

#endif
