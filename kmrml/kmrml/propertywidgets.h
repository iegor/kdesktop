/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PROPERTYWIDGETS_H
#define PROPERTYWIDGETS_H

#include <qhbox.h>

#include "propertysheet.h"

namespace KMrml
{
    class IntegerWidget : public QHBox
    {
        Q_OBJECT
        
    public:
        IntegerWidget( const PropertySheet& sheet, 
                       QWidget *parent = parent, const char *name = 0L );
        ~IntegerWidget();

        int value() const;
        
    private:
        
    };
    
    class ComboWidget : public QHBox
    {
        Q_OBJECT
        
    public:
        ComboWidget( const PropertySheet& sheet, 
                     QWidget *parent = parent, const char *name = 0L );
        ~ComboWidget();
        
        QString value() const;
        
    private:
        
    };
    
    class CheckBoxWidget : public QHBox
    {
        Q_OBJECT
        
    public:
        CheckBoxWidget( const PropertySheet& sheet, 
                        QWidget *parent = parent, const char *name = 0L );
        ~CheckBoxWidget();
        
        bool value();
        
    private:
        
        
    };
    
    
    class LineEditWidget : public QHBox
    {
        Q_OBJECT
        
    public:
        LineEditWidget( const PropertySheet& sheet, 
                        QWidget *parent = parent, const char *name = 0L );
        ~LineEditWidget();
        
        QString value();
        
    private:
        
    };
    
    class ListBoxWidget : public QHBox
    {
        Q_OBJECT
        
    public:
        ListBoxWidget( const PropertySheet& sheet, 
                       QWidget *parent = parent, const char *name = 0L );
        ~ListBoxWidget();
        
        QStringList values();
        
    private:
        
    };
    
};


#endif // PROPERTYWIDGETS_H
