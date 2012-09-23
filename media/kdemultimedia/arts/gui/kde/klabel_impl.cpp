/*
    Copyright ( C ) 2002 Arnold Krille <arnold@arnoldarts.de>

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

#include "klabel_impl.h"
#include "klabel_impl.moc"

#include <kdebug.h>
#include <qfont.h>

using namespace Arts;
using namespace std;

KLabel_impl::KLabel_impl( QFrame *widget ) : KFrame_impl( widget ? widget : new RotateLabel( 0 ) ) {
	_label = static_cast<RotateLabel*>( _qwidget );
}

string KLabel_impl::text() {
	return _label->title().utf8().data();
}

void KLabel_impl::text(  const string& newtext ) {
	_label->title( QString::fromUtf8( newtext.c_str() ) );
}

long KLabel_impl::align() { return _label->align(); }
void KLabel_impl::align( long n ) { _label->align( n ); }

void KLabel_impl::fontsize( long n ) { _label->fontsize( n ); }
void KLabel_impl::fontfamily( const std::string& n ) { _label->fontfamily( n.c_str() ); }

Arts::TextBottom KLabel_impl::bottom() { return _label->bottom(); }
void KLabel_impl::bottom( Arts::TextBottom n ) { _label->bottom( n ); }

REGISTER_IMPLEMENTATION( KLabel_impl );

RotateLabel::RotateLabel( QWidget* p, const char* n ) : QFrame( p,n ) {
	_bottom = Arts::South;
	_align = Arts::AlignCenter;
}
void RotateLabel::paintEvent( QPaintEvent* ) {
	QPainter p( this );
	if ( _bottom == Arts::East ) {
		p.rotate( 270 );
		p.drawText( QRect( 0,0, -height(), width() ), _align, _title );
	} else if ( _bottom == Arts::West ) {
		p.rotate( 90 );
		p.drawText( QRect( 0,0, height(), -width() ), _align, _title );
	} else if ( _bottom == Arts::North ) {
		p.rotate( 180 );
		p.drawText( QRect( 0,0, -width(), -height() ), _align, _title );
	} else {
		p.drawText( QRect( 0,0, width(), height() ), _align, _title );
	}
}

void RotateLabel::fontfamily( QString n ) {
	QFont font = this->font();
	font.setFamily( n );
	this->setFont( font );
}
void RotateLabel::fontsize( int n ) {
	QFont font = this->font();
	font.setPixelSize( n );
	this->setFont( font );
}

void RotateLabel::title( QString n ) {
	_title = n;
	QSize size = this->fontMetrics().size( SingleLine, _title );
	if ( _bottom == Arts::East || _bottom == Arts::West )
		this->setMinimumSize( size.height(), size.width() );
	else
		this->setMinimumSize( size );
}

void RotateLabel::align( long n ) {
	_align=n;
	repaint();
}

void RotateLabel::bottom( Arts::TextBottom bottom ) {
	_bottom = bottom;
	title( _title );
	repaint();
}

// vim: sw=4 ts=4
