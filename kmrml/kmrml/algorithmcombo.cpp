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

#include "algorithmcombo.h"

#include <kdatastream.h>

using namespace KMrml;

// ### copycat of CollectionCombo... moc can't handle templates unfortunately..
// could use base-class MrmlElement....

AlgorithmCombo::AlgorithmCombo( QWidget *parent, const char *name )
    : KComboBox( false, parent, name ),
      m_algorithms( 0L )
{
    connect( this, SIGNAL( activated( const QString& ) ),
             SLOT( slotActivated( const QString& ) ));
}

AlgorithmCombo::~AlgorithmCombo()
{
}

void AlgorithmCombo::setAlgorithms( const AlgorithmList *algorithms )
{
    assert( algorithms != 0L );

    clear();
    m_algorithms = algorithms;
    insertStringList( algorithms->itemNames() );
    // #### block signals here?
}

void AlgorithmCombo::setCurrent( const Algorithm& coll )
{
    setCurrentItem( coll.name() );
}

Algorithm AlgorithmCombo::current() const
{
    return m_algorithms->findByName( currentText() );
}

void AlgorithmCombo::slotActivated( const QString& name )
{
    Algorithm coll = m_algorithms->findByName( name );
    emit selected( coll );
}

#include "algorithmcombo.moc"
