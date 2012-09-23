/*

    Copyright ( C ) 2002 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (  at your option ) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include <artsflow.h>
#include <flowsystem.h>
#include <stdsynthmodule.h>
#include <debug.h>
#include <artsmoduleseffects.h>
#include <connect.h>

#include <kglobal.h>
#include <klocale.h>

namespace Arts {

class MonoToStereo_impl : virtual public MonoToStereo_skel,
                          virtual public StdSynthModule
{
protected:
	float _pan, _pLeft, _pRight;
public:
	MonoToStereo_impl()
	{
		pan( 0 );
	}

	float pan() { return _pan; }
	void pan( float pan )
	{
		if( pan < -1 ) pan = -1;
		if( pan > 1 ) pan = 1;
		_pan = pan;
		_pLeft = _pRight = 1;
		if( _pan < 0 )
			_pRight = 1 + _pan;
		else
			_pLeft = 1 - _pan;
	}

	void calculateBlock( unsigned long samples )
	{
		for( unsigned int i=0; i<samples; i++ )
		{
			outleft[ i ] = inmono[ i ] * _pLeft;
			outright[ i ] = inmono[ i ] * _pRight;
		}
	}
};
REGISTER_IMPLEMENTATION( MonoToStereo_impl );

class StereoToMono_impl : virtual public StereoToMono_skel,
                          virtual public StdSynthModule
{
protected:
	float _pan, _pLeft, _pRight;
public:
	StereoToMono_impl()
	{
		pan( 0 );
	}

	float pan() { return _pan; }
	void pan( float pan ) 
	{
		if( pan < -1 ) pan = -1;
		if( pan > 1 ) pan = 1;
		_pan = pan;
		_pLeft = _pRight = 1;
		if( _pan < 0 )
			_pRight = 1 + _pan;
		else
			_pLeft = 1 - _pan;
	}

	void calculateBlock( unsigned long samples )
	{
		for( unsigned int i=0; i<samples; i++ )
			outmono[ i ] = ( inleft[ i ] * _pLeft + inright[ i ] * _pRight ) / ( _pLeft + _pRight );
	}
};
REGISTER_IMPLEMENTATION( StereoToMono_impl );

class StereoBalance_impl : virtual public StereoBalance_skel,
                           virtual public StdSynthModule
{
protected:
	float _balance, _left, _right;
public:
	StereoBalance_impl() : _balance( 0 ), _left( 1 ), _right( 1 ) { }

	float balance() { return _balance; }
	void balance( float n )
	{
//arts_debug( "StereoBalance::balance( float %f )", n );
		if( n>1 ) n=1;
		if( n<-1 ) n=-1;
		_balance = n;
		_right = _left = 1;
		if( _balance < 0 )
			_right = 1 + _balance;
		else
			_left = 1 - _balance;
	}

	void calculateBlock( unsigned long samples )
	{
//arts_debug( "StereoBalance::calculateBlock( unsigned int %i )", samples );
		for( unsigned long i=0; i<samples; i++ )
		{
		//	outleft[ i ] = inleft[ i ];
		//	outright[ i ] = inright[ i ];
			outleft[ i ] = inleft[ i ] * _left;
			outright[ i ] = inright[ i ] * _right;
		}
	}
};
REGISTER_IMPLEMENTATION( StereoBalance_impl );

class StereoBalanceGuiFactory_impl : virtual public StereoBalanceGuiFactory_skel
{
public:
	Widget createGui( Object object )
	{
		KGlobal::locale()->insertCatalogue( "artsmodules" );

		arts_return_val_if_fail( !object.isNull(), Arts::Widget::null() );
		StereoBalance ch= DynamicCast( object );
		arts_return_val_if_fail( !ch.isNull(), Arts::Widget::null() );

		Poti bal;
		bal.caption( i18n( "Balance" ).utf8().data() );
		bal.min( -1 ); bal.max( 1 );
		bal.value( ch.balance() );
		connect( bal, "value_changed", ch, "balance" );

		return bal;
	}
};
REGISTER_IMPLEMENTATION( StereoBalanceGuiFactory_impl );

}

