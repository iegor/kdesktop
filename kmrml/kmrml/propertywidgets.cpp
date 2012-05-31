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

#include "propertywidgets.h"

IntegerWidget::IntegerWidget( const PropertySheet& sheet,
                              QWidget *parent, const char *name )
    : QHBox( parent, name )
{
    
}

IntegerWidget::~IntegerWidget()
{
    
}

int IntegerWidget::value() const
{
    
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

ComboWidget::ComboWidget( const PropertySheet& sheet,
                              QWidget *parent, const char *name )
    : QHBox( parent, name )
{
    
}

ComboWidget::~ComboWidget()
{
    
}

QString ComboWidget::value() const
{
    
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

CheckBoxWidget::CheckBoxWidget( const PropertySheet& sheet,
                              QWidget *parent, const char *name )
    : QHBox( parent, name )
{
    
}

CheckBoxWidget::~CheckBoxWidget()
{
    
}

bool CheckBoxWidget::value() const
{
    
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

LineEditWidget::LineEditWidget( const PropertySheet& sheet,
                              QWidget *parent, const char *name )
    : QHBox( parent, name )
{
    
}

LineEditWidget::~LineEditWidget()
{
    
}

QString LineEditWidget::value() const
{
    
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

ListBoxWidget::ListBoxWidget( const PropertySheet& sheet,
                              QWidget *parent, const char *name )
    : QHBox( parent, name )
{
    
}

ListBoxWidget::~ListBoxWidget()
{
    
}

QStringList ListBoxWidget::value() const
{
    
}

#include "propertywidgets.moc"
