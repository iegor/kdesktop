/** 
 * Copyright (C) 2001-2002 the KGhostView authors. See file AUTHORS.
 * 	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <qbitmap.h>
#include <qdrawutil.h>
#include <qpainter.h>
#include <qregion.h>

#include "kgvpagedecorator.h"
    
KGVPageDecorator::KGVPageDecorator( QWidget* parent, const char* name ) : 
    QHBox( parent, name ), 
    _margin( 5 ), 
    _borderWidth( 1 ), 
    _shadowOffset( 2, 2 )
{
    setFrameStyle( Box | Plain );
    setLineWidth( _margin + _borderWidth );
    setBackgroundMode( NoBackground );
    setAutoMask( true );
}

bool KGVPageDecorator::eventFilter( QObject* o, QEvent* e )
{
    switch( e->type() ) {
    case QEvent::MouseButtonPress: 
    case QEvent::MouseButtonRelease: 
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
	return event( e );
    default:
	;
    }
    return QHBox::eventFilter( o, e );
}

void KGVPageDecorator::childEvent( QChildEvent* e )
{
    if( e->child()->isWidgetType() && e->inserted() )
	e->child()->installEventFilter( this );
}

void KGVPageDecorator::drawFrame( QPainter* p )
{
    QRect r( frameRect().topLeft()     + QPoint(_margin,_margin), 
	     frameRect().bottomRight() - QPoint(_margin,_margin) );
	     
    if( !r.isValid() ) 
	return;
    
    const QColorGroup& cg = colorGroup();

    r.moveCenter( r.center() + _shadowOffset );
    qDrawPlainRect( p, r, cg.shadow(), _shadowOffset.manhattanLength() );
    
    r.moveCenter( r.center() - _shadowOffset );
    qDrawPlainRect( p, r, cg.foreground(), _borderWidth );
}

void KGVPageDecorator::drawMask( QPainter* p )
{
    QRect r( frameRect().topLeft()     + QPoint(_margin,_margin), 
	     frameRect().bottomRight() - QPoint(_margin,_margin) );

    if( !r.isValid() )
	return;

    QColorGroup cg( color1, color1, color1, color1, color1, color1, color1,
		    color1, color0 );
    QBrush brush( cg.foreground() );
    
    r.moveCenter( r.center() + _shadowOffset );
    qDrawPlainRect( p, r, cg.foreground(), _shadowOffset.manhattanLength() );
    
    r.moveCenter( r.center() - _shadowOffset );
    qDrawPlainRect( p, r, cg.foreground(), _borderWidth, &brush );
}

void KGVPageDecorator::updateMask()
{
    QBitmap bm( size() );
    bm.fill( color0 );
    QPainter p( &bm, this );
    p.setPen( color1 );
    p.setBrush( color1 );
    drawMask( &p );
    p.end();
    setMask( bm );
}

// vim:sw=4:sts=4:ts=8:noet
