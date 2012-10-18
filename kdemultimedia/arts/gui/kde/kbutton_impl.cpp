    /*

    Copyright (C) 2000       Stefan Westerfeld
                             stefan@space.twc.de
                  2001, 2002 Matthias Kretz
                             kretz@kde.org

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

#include "kbutton_impl.h"
#include "kbutton_impl.moc"

using namespace Arts;
using namespace std;

KButtonMapper::KButtonMapper( KButton_impl *_impl, QPushButton *but )
	: QObject( but, "KButtonMapper" )
	, impl( _impl )
	, button( but )
{
	connect( but, SIGNAL( pressed() ), this, SLOT( pressed() ) );
	connect( but, SIGNAL( released() ), this, SLOT( released() ) );
	connect( but, SIGNAL( toggled( bool ) ), this, SLOT( toggled( bool ) ) );
	connect( but, SIGNAL( clicked() ), this, SLOT( clicked() ) );
}

void KButtonMapper::pressed()
{
	if( ! button->isToggleButton() )
		impl->changeState(true);
}

void KButtonMapper::released()
{
	if( ! button->isToggleButton() )
		impl->changeState(false);
}

void KButtonMapper::toggled( bool b )
{
	if( button->isToggleButton() )
		impl->changeState( b );
}

void KButtonMapper::clicked()
{
	impl->emitClicked();
}

KButton_impl::KButton_impl( QPushButton * widget )
	: KWidget_impl( widget ? widget : new QPushButton( 0 ) )
	, _clicked( false )
{
	_qpushbutton = static_cast<QPushButton*>( _qwidget );
	( void )new KButtonMapper( this, _qpushbutton );
}

void KButton_impl::constructor( Widget p )
{
	parent( p );
}

void KButton_impl::constructor( const string & t, Widget p )
{
	parent( p );
	text( t );
}

void KButton_impl::emitClicked()
{
	_clicked = true;
	clicked_changed( true );
}

string KButton_impl::text()
{
	return _qpushbutton->text().utf8().data();
}

void KButton_impl::text(const string& newText)
{
	_qpushbutton->setText(QString::fromUtf8(newText.c_str()));
}

bool KButton_impl::toggle()
{
	return _qpushbutton->isToggleButton();
}

void KButton_impl::toggle(bool newToggle)
{
	_qpushbutton->setToggleButton(newToggle);
}

bool KButton_impl::pressed()
{
	if( _qpushbutton->isToggleButton() )
		return _qpushbutton->isOn();
	else
		return _qpushbutton->isDown();
}

bool KButton_impl::clicked()
{
	if( _clicked )
	{
		_clicked = false;
		return true;
	}
	return false;
}

void KButton_impl::changeState(bool newState)
{
	pressed_changed(newState);
}

REGISTER_IMPLEMENTATION(KButton_impl);

// vim:sw=4:ts=4
