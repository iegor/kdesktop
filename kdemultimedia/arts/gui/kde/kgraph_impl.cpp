    /*

    Copyright (C) 2001 Stefan Westerfeld
                       stefan@space.twc.de
                  2002 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "kgraph_impl.h"
#include "anyref.h"
#include "stdio.h"

using namespace Arts;
using namespace std;

KGraph_impl::KGraph_impl( KGraph * widget )
	: KWidget_impl( widget ? widget : new KGraph )
{
	_minx = 0.0; _maxx = 1.0; _miny = 0.0; _maxy = 1.0;
	_kgraph = static_cast<KGraph*>( _qwidget );
	_kgraph->setFixedSize( 300, 200 );
}

string KGraph_impl::caption()
{
	return _caption.utf8().data();
}

void KGraph_impl::caption(const string& newCaption)
{
	_caption = QString::fromUtf8(newCaption.c_str());
	// FIXME: do something with the caption here
}

float KGraph_impl::minx()
{
	return _minx;
}

void KGraph_impl::minx(float newMin)
{
	_minx = newMin;
}

float KGraph_impl::maxx()
{
	return _maxx;
}

void KGraph_impl::maxx(float newMax)
{
	_maxx = newMax;
}

float KGraph_impl::miny()
{
	return _miny;
}

void KGraph_impl::miny(float newMin)
{
	_miny = newMin;
}

float KGraph_impl::maxy()
{
	return _maxy;
}

void KGraph_impl::maxy(float newMax)
{
	_maxy = newMax;
}

namespace Arts { REGISTER_IMPLEMENTATION(KGraph_impl); }

// vim: sw=4 ts=4
