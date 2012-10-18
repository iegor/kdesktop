    /*

    Copyright ( C ) 2002-2003 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "artsmodulesmixers.h"
#include "artsmoduleseffects.h"

#include <flowsystem.h>
#include <stdsynthmodule.h>
#include <connect.h>
#include <debug.h>
#include "artsgui.h"

#include <klocale.h>

using namespace Arts;

namespace Arts {

class LittleStereoMixerChannel_impl : virtual public LittleStereoMixerChannel_skel,
                                      virtual public StdSynthModule
{
protected:
	std::string _name;
	Arts::StereoBalance _balance;
	Arts::StereoVolumeControl _volume;
public:
	LittleStereoMixerChannel_impl()
	{
	arts_debug( "LittleStereo startup" );
	if( _balance.isNull() ) arts_debug( "\n\nCouldn't create StereoBalance!!!!\n\n" );
	if( _volume.isNull() ) arts_debug( "\n\nCouldn't create StereoVolumeControl!!!!\n\n" );
		_balance.balance( 0 );
		_volume.scaleFactor( 1 );
	arts_debug( "startup ok\n" );
	}

	std::string name() { return _name; }
	void name( const std::string& n ) { arts_debug( "Name = %s", n.c_str() ); _name = n; }

	//Arts::StereoBalance balance() { return _balance; }

	//Arts::StereoVolumeControl volume() { return _volume; }

	float balance() { return _balance.balance(); }
	void balance( float n ) { _balance.balance( n ); }

	float volume() { return _volume.scaleFactor(); }
	void volume( float n ) { _volume.scaleFactor( n ); }

	void streamInit()
	{
	arts_debug( "LittleStereo::streamInit()" );
	if( _balance.isNull() ) arts_warning( "Couldn't create StereoBalance!!!!\n" );
	if( _volume.isNull() ) arts_warning( "Couldn't create StereoVolumeControl!!!!\n" );

	arts_debug( "LittleStereo::streamInit() starts" );
		_balance.start();
		_volume.start();

	arts_debug( "LittleStereo::streamInit() first connects" );
		_node()->virtualize( "inleft", _balance._node(), "inleft" );
		_node()->virtualize( "inright", _balance._node(), "inright" );
	arts_debug( "LittleStereo::streamInit() middle connects" );
		connect( _balance, "outleft", _volume, "inleft" );
		connect( _balance, "outright", _volume, "inright" );
	arts_debug( "LittleStereo::streamInit() last connects" );
		_node()->virtualize( "outleft", _volume._node(), "outleft" );
		_node()->virtualize( "outright", _volume._node(), "outright" );
	arts_debug( "LittleStereo::streamInit() finished.\nbye" );
		_balance.balance( 0 );
		_volume.scaleFactor( 1 );
	}

	void streamEnd()
	{
		_balance.stop();
		_volume.stop();
	}

};
REGISTER_IMPLEMENTATION( LittleStereoMixerChannel_impl );

class LittleStereoMixerChannelGuiFactory_impl : virtual public LittleStereoMixerChannelGuiFactory_skel
{
public:
	Widget createGui( Object object )
	{
		arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());
		LittleStereoMixerChannel ch= DynamicCast(object);
		arts_return_val_if_fail(!ch.isNull(), Arts::Widget::null());

		Arts::LayoutBox vbox;
		vbox.direction( Arts::TopToBottom );

		Poti pan;
		pan.caption( i18n( "pan" ).utf8().data() );
		pan.color( "grey" ); pan.min( -1.0 ); pan.max( 1.0 );
		pan.value( ch.balance() );
		connect( pan, "value_changed", ch, "balance" );
		vbox.addWidget( pan );

		Fader volume;
		volume.caption( i18n( "volume" ).utf8().data() );
		volume.color( "red" ); volume.min( 0.01 ); volume.max( 2 );
		//volume.logarithmic( 2 );
		volume.value( ch.volume() );
		connect( volume, "value_changed", ch, "volume" );
		vbox.addWidget( volume );

		return vbox;
	}
};
REGISTER_IMPLEMENTATION( LittleStereoMixerChannelGuiFactory_impl );

}

