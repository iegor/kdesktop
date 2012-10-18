/*
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/
// $Id: kframe_impl.h 376198 2005-01-07 18:38:40Z mlaurent $

#ifndef KFRAME_IMPL
#define KFRAME_IMPL
#include "kwidget_impl.h"
#include <kdelibs_export.h>
class QFrame;

namespace Arts {

class KDE_EXPORT KFrame_impl : virtual public Arts::Frame_skel,
                    public Arts::KWidget_impl
{
	protected:
		QFrame * _qframe;

	public:
		KFrame_impl( QFrame * widget = 0 );
		inline void constructor( Widget p ) { parent( p ); }

		long margin();
		void margin( long m );
		long linewidth();
		void linewidth( long lw );
		long midlinewidth();
		void midlinewidth( long mlw );
		long framestyle();
		void framestyle( long fs );
		Shape frameshape();
		void frameshape( Shape fs );
		Shadow frameshadow();
		void frameshadow( Shadow fs );
}; //class
} //namespace

// vim: sw=4 ts=4
#endif
