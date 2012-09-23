/***************************************************************************
                          kcheckeduca.cpp  -  description
                             -------------------
    begin                : Tue Sep 5 2000
    copyright            : (C) 2000 by Javier Campos Morales
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kcheckeduca.h"
#include "kcheckeduca.moc"

#include <qstyle.h>
#include <kaccelmanager.h>

KCheckEduca::KCheckEduca(QWidget *parent, const char *name ) : QCheckBox(parent,name)
{
    _doc = 0;
    KAcceleratorManager::setNoAccel( this );
}

KCheckEduca::~KCheckEduca()
{
    delete _doc;
}

/**  */
void KCheckEduca::drawButtonLabel( QPainter *p){
    int x, y, w, h;
    int extra_width, indicator_width;

    extra_width = 8;
    indicator_width = style().pixelMetric(QStyle::PM_IndicatorWidth, 0);

    y = 0;
    x = indicator_width + extra_width; //###
    w = width() - x;
    h = height();

    QRect cr(x,y,w,h);

    _doc = new QSimpleRichText( text(), font() );
    _doc->setWidth( 10 );

    _doc->setWidth(p, cr.width() );
    int rw = _doc->widthUsed() + extra_width;
    int rh = _doc->height();
    int xo = 0;
    int yo = 0;

    resize( width(), rh);

    yo = (cr.height()-rh)/2;

    if ( !isEnabled() ) {
        QColorGroup cg = colorGroup();
        cg.setColor( QColorGroup::Text, cg.light() );
        _doc->draw(p, cr.x()+xo+1, cr.y()+yo+1, cr, cg, 0);
    }
    _doc->draw(p, cr.x()+xo, cr.y()+yo, cr, colorGroup(), 0);

    if ( hasFocus() ) {
//	QRect br = style().itemRect( p, x, y, rw, rh+yo,
//				   AlignLeft|AlignVCenter|ShowPrefix,
//				   isEnabled(),
//				   pixmap(), text().visual() );
        QRect br(x,y,rw,rh+yo);

        br.setLeft( br.left()-3 );
        br.setRight( br.right()+2 );
        br.setTop( br.top()-2 );
        br.setBottom( br.bottom()+2);
        br = br.intersect( QRect(0,0,rw, rh+yo ) );

        style().drawPrimitive( QStyle::PE_FocusRect, p, br, colorGroup());
    }
}
