    /*

    Copyright (  C ) 2002, 2003 Arnold Krille <arnold@arnoldarts.de>

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

#include "kpopupbox_impl.h"
#include "kpopupbox_private.h"

#include <qlayout.h>

using namespace Arts;

KPopupBox_impl::KPopupBox_impl( KPopupBox_widget *w ) : KFrame_impl( w ? w : new KPopupBox_widget )
{
	self().framestyle( Box ); self().margin( 1 ); self().linewidth( 1 );
	self().vSizePolicy( spFixed ); self().hSizePolicy( spFixed );

	if( !w ) w = static_cast<KPopupBox_widget *>( _qframe );

	_widget = w;
//	_mapper = new KPopupBoxEventMapper( _widget, this );
}
KPopupBox_impl::~KPopupBox_impl() {
}

Direction KPopupBox_impl::direction() { return _widget->direction(); }
void KPopupBox_impl::direction( Direction n ) { _widget->direction( n ); }

void KPopupBox_impl::widget( Arts::Widget widget ) {
	widget.parent( self() );
	this->_addChild( widget, "PopupBox_child" );
	_widget->setWidget( widget );
}
Arts::Widget KPopupBox_impl::widget() { return _widget->getWidget(); }

std::string KPopupBox_impl::name() { return _name; }
void KPopupBox_impl::name( const std::string& n ) { _name = ""; _name = n; _widget->name( n ); }

// Following the private class:

KPopupBox_widget::KPopupBox_widget( QWidget *parent, const char* name ) : QFrame( parent,name )
{
	this->setFrameShape( QFrame::Box );
	this->setMargin( 1 ); this->setLineWidth( 1 );

	_titlebar = new QFrame( this );
	_titlebarlayout = new QBoxLayout( _titlebar, QBoxLayout::BottomToTop );
	_titlebarlayout->setAutoAdd( true );

	_showbutton = new ShowButton( _titlebar );
	connect( _showbutton, SIGNAL( toggled( bool ) ), this, SLOT( hide( bool ) ) );
	_drag = new HandleDrag( _titlebar );
	connect( _drag, SIGNAL( clicked() ), _showbutton, SLOT( toggle() ) );
	_ownbutton = new OwnButton( _titlebar );
	connect( _ownbutton, SIGNAL( toggled( bool ) ), this, SLOT( own( bool ) ) );

	_artswidget = new OwnWidget( _showbutton, this );

	_layout = new QBoxLayout( this, QBoxLayout::LeftToRight );
	_layout->addWidget( _titlebar , -1 );
	_layout->addWidget( _artswidget, 20 );
	_layout->addStretch( 0 );
}
KPopupBox_widget::~KPopupBox_widget() {
}

Arts::Direction KPopupBox_widget::direction() {
	return Arts::Direction( _layout->direction() );
}

void KPopupBox_widget::direction( Arts::Direction n ) {
	_layout->setDirection( QBoxLayout::Direction( n ) );
	_showbutton->direction( QBoxLayout::Direction( n ) );
	switch( n ) {
		case LeftToRight:
		case RightToLeft:
			_titlebarlayout->setDirection( QBoxLayout::BottomToTop );
			_drag->setMinimumHeight( 30 );
			_drag->setMinimumWidth( 0 );
			break;
		case TopToBottom:
		case BottomToTop:
			_titlebarlayout->setDirection( QBoxLayout::RightToLeft );
			_drag->setMinimumHeight( 0 );
			_drag->setMinimumWidth( 30 );
	}
}

void KPopupBox_widget::setWidget( Arts::Widget widget ) { _artswidget->setContent( widget ); }
Arts::Widget KPopupBox_widget::getWidget() { return _artswidget->content(); }

void KPopupBox_widget::hide( bool n ) {
	if( n )
		_artswidget->hide();
	else
		_artswidget->show();
}

void KPopupBox_widget::own( bool n ) {
	if ( n )
		_artswidget->reparent( 0, _artswidget->mapToGlobal( _artswidget->pos() ), !( _artswidget->isHidden() ) );
	else
	{
		_artswidget->reparent( this, QPoint( 0,0 ), !( _artswidget->isHidden() ) );
		_layout->insertWidget( 1, _artswidget, 20 );
	}
}

void KPopupBox_widget::name( std::string n ) {
	_artswidget->setCaption( n.c_str() );
}

REGISTER_IMPLEMENTATION( KPopupBox_impl );

#include "kpopupbox_private.moc"

// vim: sw=4 ts=4

