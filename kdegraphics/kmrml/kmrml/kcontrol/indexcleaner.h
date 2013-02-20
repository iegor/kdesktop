/****************************************************************************
** $Id: indexcleaner.h 239366 2003-07-27 00:25:16Z mueller $
**
** Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>
**
****************************************************************************/

#ifndef INDEXCLEANER_H
#define INDEXCLEANER_H

#include <qobject.h>
#include <qstringlist.h>

class KProcess;

namespace KMrml
{
    class Config;
}

namespace KMrmlConfig
{
    class IndexCleaner : public QObject
    {
        Q_OBJECT

    public:
        IndexCleaner( const QStringList& dirs, const KMrml::Config *config,
                      QObject *parent = 0, const char *name = 0 );
        ~IndexCleaner();

        void start();

    signals:
        void advance( int value );
        void finished();

    private slots:
        void slotExited( KProcess * );

    private:
        int m_stepSize;
        void startNext();

        QStringList m_dirs;
        const KMrml::Config *m_config;
        KProcess *m_process;
    };

}


#endif // INDEXCLEANER_H
