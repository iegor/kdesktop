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

#ifndef ALGORITHMCOMBO_H
#define ALGORITHMCOMBO_H

#include <kcombobox.h>

#include "mrml_elements.h"

namespace KMrml
{

    class AlgorithmCombo : public KComboBox
    {
        Q_OBJECT

    public:
        AlgorithmCombo( QWidget *parent, const char *name = 0 );
        ~AlgorithmCombo();

        void setAlgorithms( const AlgorithmList * algorithms );
        void setCurrent( const Algorithm& coll );

        Algorithm current() const;

    signals:
        void selected( const Algorithm& );

    private slots:
        void slotActivated( const QString& );

    private:
        const AlgorithmList *m_algorithms;
    };

}

#endif // ALGORITHMCOMBO_H
