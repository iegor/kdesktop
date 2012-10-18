    /*

    Copyright (C) 2000,2001 Stefan Westerfeld
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

#include "khbox_impl.h"
#include <qhbox.h>

using namespace Arts;

KHBox_impl::KHBox_impl( QHBox * widget )
	: KFrame_impl( widget ? widget : new QHBox )
	, _spacing( 5 )
{
	_qhbox = static_cast<QHBox*>( _qwidget );
	_qhbox->setSpacing( _spacing );
	_qhbox->setMargin( 5 );
}

long KHBox_impl::spacing()
{
	return _spacing;
}

void KHBox_impl::spacing( long s )
{
	_spacing = s;
	_qhbox->setSpacing( s );
}

REGISTER_IMPLEMENTATION(KHBox_impl);

// vim: sw=4 ts=4
