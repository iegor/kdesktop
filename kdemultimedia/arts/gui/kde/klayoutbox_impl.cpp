/*
    Copyright ( C ) 2002, 2003 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or ( at your option ) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#include "klayoutbox_impl.h"

#include <qframe.h>
#include <qlayout.h>
#include <kdebug.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qpen.h>
#include "kwidgetrepo.h"

using namespace Arts;
using namespace std;

KLayoutBox_impl::KLayoutBox_impl( QFrame *widget ) : KFrame_impl( widget ? widget :new QFrame( 0 ) )
{
	_qframe = static_cast<QFrame*>( _qwidget );
	_layout = new QBoxLayout( _qframe, QBoxLayout::LeftToRight );
}
KLayoutBox_impl::~KLayoutBox_impl() {
}

void KLayoutBox_impl::addWidget( Arts::Widget widget, long stretch, long align ) {
	widget.parent( self() );
	this->_addChild( widget, "layoutbox_item" );
	QWidget * tmp = KWidgetRepo::the()->lookupQWidget( widget.widgetID() );
	_layout->addWidget( tmp, stretch, align );
}

void KLayoutBox_impl::insertWidget( long index, Arts::Widget widget, long stretch, long align ) {
	widget.parent( self() );
	this->_addChild( widget, "layoutbox_item" );
	QWidget * tmp = KWidgetRepo::the()->lookupQWidget( widget.widgetID() );
	_layout->insertWidget( index, tmp, stretch, align );
}

void KLayoutBox_impl::addStretch( long n ) { _layout->addStretch( n ); }
void KLayoutBox_impl::addSpace( long n ) { _layout->addSpacing( n ); }
void KLayoutBox_impl::addStrut( long n ) { _layout->addStrut( n ); }
void KLayoutBox_impl::addSeparator( long stretch, long align ) {
	_layout->addWidget( new KLayoutBox_Separator( _qframe ), stretch, align );
}
void KLayoutBox_impl::addLine( long width, long space, long stretch, long align ) {
	_layout->addWidget( new KLayoutBox_Line( width, space, _qframe ), stretch, align );
}

long KLayoutBox_impl::spacing() { return _layout->spacing(); }
void KLayoutBox_impl::spacing( long n ) { _layout->setSpacing( n ); }

long KLayoutBox_impl::layoutmargin() { return _layout->margin(); }
void KLayoutBox_impl::layoutmargin( long n ) { _layout->setMargin( n ); this->margin( n ); }

Direction KLayoutBox_impl::direction() { return Arts::Direction( _layout->direction() ); }
void KLayoutBox_impl::direction( Direction d ) { _layout->setDirection( QBoxLayout::Direction( d ) ); }

REGISTER_IMPLEMENTATION( KLayoutBox_impl );

KLayoutBox_Separator::KLayoutBox_Separator( QWidget* p, const char* n ) : QWidget( p,n ) {
//kdDebug() << k_funcinfo << endl;
}

void KLayoutBox_Separator::resizeEvent( QResizeEvent* ) { kdDebug() << k_funcinfo << size() << endl; }

void KLayoutBox_Separator::paintEvent( QPaintEvent* ) {
//kdDebug() << k_funcinfo << size() << endl;
	QPainter p( this );
	QStyle::SFlags flags = QStyle::Style_Default;
	if ( width() < height() ) flags |= QStyle::Style_Horizontal;
	style().drawPrimitive( QStyle::PE_Splitter, &p, rect(), colorGroup(), flags );
}

QSize KLayoutBox_Separator::minimumSizeHint() const {
	int wh = style().pixelMetric( QStyle::PM_SplitterWidth, this );
	return QSize( wh, wh );
}

KLayoutBox_Line::KLayoutBox_Line( int width, int space, QWidget* p, const char* n )
  : QWidget( p,n )
  , _width( width )
  , _space( space )
{
//kdDebug() << k_funcinfo << size() << endl;
}


void KLayoutBox_Line::paintEvent( QPaintEvent* ) {
//kdDebug() << k_funcinfo << size() << endl;
	QPainter p( this );
	p.setPen( QPen( colorGroup().foreground(), _width ) );
	if ( width() > height() ) p.drawLine( 0, height()/2, width(), height()/2 );
		else p.drawLine( width()/2, 0, width()/2, height() );
}

QSize KLayoutBox_Line::minimumSizeHint() const {
//kdDebug() << k_funcinfo << size() << endl;
	int wh = _width + 2* _space;
	return QSize( wh, wh );
}

#include <klayoutbox_impl.moc>

// vim: sw=4 ts=4

