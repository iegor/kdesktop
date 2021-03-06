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

#ifndef KGVMAINWIDGET_H
#define KGVMAINWIDGET_H

#include <qwidget.h>

class KURL;

class KGVMainWidget : public QWidget
{
    Q_OBJECT
    
public:
    KGVMainWidget( QWidget* parent = 0, const char* name = 0 );

signals:
    void spacePressed();
    void urlDropped( const KURL& );

protected:
    virtual void keyPressEvent( QKeyEvent* );
    virtual void dragEnterEvent( QDragEnterEvent* );
    virtual void dropEvent( QDropEvent* );
};

#endif

// vim:sw=4:sts=4:ts=8:noet
