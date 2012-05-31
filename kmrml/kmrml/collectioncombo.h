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

#ifndef COLLECTIONCOMBO_H
#define COLLECTIONCOMBO_H

#include <kcombobox.h>

#include "mrml_elements.h"

namespace KMrml
{

    class CollectionCombo : public KComboBox
    {
        Q_OBJECT

    public:
        CollectionCombo( QWidget *parent, const char *name = 0 );
        ~CollectionCombo();

        void setCollections( const CollectionList * collections );
        void setCurrent( const Collection& coll );

        Collection current() const;

    signals:
        void selected( const Collection& );

    private slots:
        void slotActivated( const QString& );

    private:
        const CollectionList *m_collections;
    };

    QDataStream& operator<<( QDataStream& stream, const CollectionCombo& );
    QDataStream& operator>>( QDataStream& stream, CollectionCombo& );

}

#endif // COLLECTIONCOMBO_H
