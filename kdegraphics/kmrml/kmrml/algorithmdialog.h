/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef ALGORITHMDIALOG_H
#define ALGORITHMDIALOG_H

#include <kdialogbase.h>

#include "mrml_elements.h"

namespace KMrml
{
    class AlgorithmCombo;
    class CollectionCombo;

    class AlgorithmDialog : public KDialogBase
    {
        Q_OBJECT

    public:
        AlgorithmDialog( const AlgorithmList&, const CollectionList&,
                         const Collection& currentColl,
                         QWidget *parent = 0, const char *name = 0 );
        ~AlgorithmDialog();

    private slots:
        void collectionChanged( const Collection& );
        void initGUI( const Algorithm& algo );

    private:
        Algorithm     m_algo;
        AlgorithmList m_allAlgorithms;
        AlgorithmList m_algosForCollection;
        CollectionList m_collections;

        CollectionCombo *m_collectionCombo;
        AlgorithmCombo *m_algoCombo;

        QFrame *m_view;
    };

}

#endif // ALGORITHMDIALOG_H
