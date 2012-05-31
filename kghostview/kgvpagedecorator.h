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

#ifndef KGVPAGEDECORATOR_H
#define KGVPAGEDECORATOR_H

#include <qhbox.h>

class KGVPageDecorator : public QHBox
{
public:
    KGVPageDecorator( QWidget* parent = 0, const char* name = 0 );
    ~KGVPageDecorator() { ; }

    unsigned int margin()      const;
    unsigned int borderWidth() const;
    
    /**
     * Reimplemented from QObject to let mouse events from child widgets
     * appear to come from this widget.
     */
    bool eventFilter( QObject*, QEvent* );

protected:
    /**
     * Reimplemented from QObject to automatically insert an event filter
     * on child widgets.
     */
    virtual void childEvent( QChildEvent* );

    /**
     * Reimplemented from QFrame to draw a pageshadow like frame.
     */
    virtual void drawFrame( QPainter* );
    
    /**
     * Draw the mask of both the frame and the contents in order to create a 
     * partially transparent frame.
     */ 
    virtual void drawMask( QPainter* );
    
    /**
     * Reimplemented from QWidget. It uses @ref drawMask() to draw the mask
     * of the frame when transparency is required.
     */
    virtual void updateMask();

private:
    unsigned int _margin;
    unsigned int _borderWidth;
    QPoint _shadowOffset;
};

inline unsigned int KGVPageDecorator::margin() const
{
    return _margin;
}

inline unsigned int KGVPageDecorator::borderWidth() const
{
    return _borderWidth;
}

#endif

// vim:sw=4:sts=4:ts=8:noet
