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

#ifndef INDEXER_H
#define INDEXER_H

#include <qobject.h>

#include <kmrml_config.h>

class KProcess;
class KProcIO;

namespace KMrmlConfig
{
    class Indexer : public QObject
    {
        Q_OBJECT

    public:
        Indexer( const KMrml::Config *config,
                 QObject *parent = 0L, const char *name = 0 );
        ~Indexer();

        void startIndexing( const QStringList& dirs );
        void stop();

    signals:
        void progress( int percent, const QString& text );
        void finished( int returnCode );


    private slots:
        void slotCanRead( KProcIO * );
        void processFinished( KProcess * );

    private:
        void processNext();

        KProcIO *m_process;
        const KMrml::Config *m_config;

        uint m_dirCount;
        QStringList m_dirs;
        QString m_currentDir;

    };


}


#endif // INDEXER_H
