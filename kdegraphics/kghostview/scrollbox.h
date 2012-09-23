/** 
 * Copyright (C) 1997-2002 the KGhostView authors. See file AUTHORS.
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

#ifndef __SCROLLBOX_H__
#define __SCROLLBOX_H__

#include <qframe.h>
#include <qimage.h>

class ScrollBox: public QFrame
{
    Q_OBJECT

public:
    ScrollBox( QWidget* parent = 0, const char* name = 0 );

public slots:
    void setPageSize( const QSize& );
    void setViewSize( const QSize& );
    void setViewPos( const QPoint& );
    void setViewPos( int x, int y ) { setViewPos( QPoint( x, y ) ); }
    void setThumbnail( QPixmap img );
    void clear();

signals:
    void valueChanged( const QPoint& );
    void valueChangedRelative( int dx, int dy );
    void button2Pressed();
    void button3Pressed();

protected:
    void mousePressEvent( QMouseEvent *);
    void mouseMoveEvent( QMouseEvent *);
    void drawContents( QPainter *);
    void resizeEvent( QResizeEvent * );

private:
    QPoint viewpos, mouse;
    QSize  pagesize;
    QSize  viewsize;
};

#endif

// vim:sw=4:sts=4:ts=8:noet
