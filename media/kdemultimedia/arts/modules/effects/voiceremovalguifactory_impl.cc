/*  This file is part of the KDE project
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
// $Id: voiceremovalguifactory_impl.cc 239435 2003-07-27 13:50:21Z mueller $

#include "artsmoduleseffects.h"
#include <debug.h>
#include <connect.h>
#include <iostream>

using namespace std;
using namespace Arts;

namespace Arts {

class VoiceRemovalGuiFactory_impl : public VoiceRemovalGuiFactory_skel
{
	public:
		Widget createGui( Object );
};

REGISTER_IMPLEMENTATION( VoiceRemovalGuiFactory_impl );

}

Widget VoiceRemovalGuiFactory_impl::createGui( Object object )
{
	arts_return_val_if_fail( ! object.isNull(), Arts::Widget::null() );

	Synth_VOICE_REMOVAL voiceremoval = DynamicCast( object );
	arts_return_val_if_fail( ! voiceremoval.isNull(), Arts::Widget::null() );

	HBox hbox;
	hbox.width( 140 ); hbox.height( 80 );// hbox.show();

	Poti position;
	position.x( 20 ); position.y( 10 ); position.caption( "position" );
	position.color( "grey" ); position.min( -1 ); position.max( 1 );
	position.value( voiceremoval.position() );
	position.range( 100 );
	position.parent( hbox );
	position.show();
	connect( position, "value_changed", voiceremoval, "position" );
	hbox._addChild( position, "positionWidget" );

	Poti freq;
	freq.x( 80 ); freq.y( 10 ); freq.caption( "freq" );
	freq.color( "red" ); freq.min( 1 ); freq.max( 10000 );
	freq.value( voiceremoval.frequency() );
	freq.range( 400 );
	freq.logarithmic( 2.0 );
	freq.parent( hbox );
	freq.show();
	connect( freq, "value_changed", voiceremoval, "frequency" );
	hbox._addChild( freq, "freqWidget" );


	return hbox;
}

// vim: ts=4 sw=4
