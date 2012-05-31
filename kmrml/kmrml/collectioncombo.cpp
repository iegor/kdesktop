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

#include "collectioncombo.h"

#include <kdatastream.h>

using namespace KMrml;

CollectionCombo::CollectionCombo( QWidget *parent, const char *name )
    : KComboBox( false, parent, name ),
      m_collections( 0L )
{
    connect( this, SIGNAL( activated( const QString& ) ),
             SLOT( slotActivated( const QString& ) ));
}

CollectionCombo::~CollectionCombo()
{
}

void CollectionCombo::setCollections( const CollectionList *collections )
{
    assert( collections != 0L );

    clear();
    m_collections = collections;
    insertStringList( collections->itemNames() );
    // #### block signals here?
}

void CollectionCombo::setCurrent( const Collection& coll )
{
    setCurrentItem( coll.name() );
}

Collection CollectionCombo::current() const
{
    return m_collections->findByName( currentText() );
}

void CollectionCombo::slotActivated( const QString& name )
{
    Collection coll = m_collections->findByName( name );
    emit selected( coll );
}

QDataStream& KMrml::operator<<( QDataStream& stream,
                                const CollectionCombo& combo )
{
    int count = combo.count();
    stream << count;
    for ( int i = 0; i < count; i++ )
        stream << combo.text( i );

    stream << combo.currentItem();
    return stream;
}

QDataStream& KMrml::operator>>( QDataStream& stream, CollectionCombo& combo )
{
    combo.clear();

    int count;
    stream >> count;
    QString text;
    for ( int i = 0; i < count; i++ )
    {
        stream >> text;
        combo.insertItem( text );
    }

    int current;
    stream >> current;
    combo.setCurrentItem( current );

    return stream;
}

#include "collectioncombo.moc"
