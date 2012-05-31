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
// $Id: kframe_impl.cpp 256191 2003-10-04 14:42:11Z akrille $

#include "kframe_impl.h"
#include <qframe.h>
#include <debug.h>
#include <stdio.h>

using namespace Arts;
using namespace std;

KFrame_impl::KFrame_impl( QFrame * widget )
	: KWidget_impl( widget ? widget : new QFrame )
{
	_qframe = static_cast<QFrame*>( _qwidget );
	assert( _qframe );
}

long KFrame_impl::margin()
{
	return _qframe->margin();
}

void KFrame_impl::margin( long m )
{
	_qframe->setMargin( m );
}

long KFrame_impl::linewidth()
{
	return _qframe->lineWidth();
}

void KFrame_impl::linewidth( long lw )
{
	_qframe->setLineWidth( lw );
}

long KFrame_impl::midlinewidth()
{
	return _qframe->midLineWidth();
}

void KFrame_impl::midlinewidth( long mlw )
{
	_qframe->setMidLineWidth( mlw );
}

long KFrame_impl::framestyle()
{
	return _qframe->frameStyle();
}

void KFrame_impl::framestyle( long fs )
{
	_qframe->setFrameStyle( fs );
}

Shape KFrame_impl::frameshape()
{
	return ( Shape )_qframe->frameShape();
}

void KFrame_impl::frameshape( Shape fs )
{
	_qframe->setFrameShape( ( QFrame::Shape )fs );
}

Shadow KFrame_impl::frameshadow()
{
	return ( Shadow )_qframe->frameShadow();
}

void KFrame_impl::frameshadow( Shadow fs )
{
	_qframe->setFrameShadow( ( QFrame::Shadow )fs );
}

REGISTER_IMPLEMENTATION(KFrame_impl);

// vim: sw=4 ts=4
