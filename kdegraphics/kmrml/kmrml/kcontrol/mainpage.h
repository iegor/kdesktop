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

#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <qvbox.h>

#include <kmrml_config.h>

class QCheckBox;
class KComboBox;
class KEditListBox;
class KIntNumInput;
class KLineEdit;
class KProgressDialog;
class KURLRequester;

namespace KMrml
{
    class Config;
}

class ServerConfigWidget;

namespace KMrmlConfig
{
    class Indexer;
    class IndexCleaner;

    class MainPage : public QVBox
    {
        Q_OBJECT

    public:
        MainPage( QWidget *parent, const char *name );
        ~MainPage();

        void resetDefaults();
        void load();
        void save();

    signals:
        void changed( bool );

    private slots:
        void changed() { emit changed( true ); }
        void slotRequesterClicked( KURLRequester * );
        void slotHostChanged();
        void slotUseAuthChanged( bool );
        void slotUserChanged( const QString& );
        void slotPassChanged( const QString& );
        void slotPortChanged( int );
        void slotAutoPortChanged( bool );

        void slotAddClicked();
        void slotRemoveClicked();

        void slotHostActivated( const QString& );

        void slotDirectoriesChanged();

        void slotMaybeIndex();
        void slotIndexingProgress( int percent, const QString& message );
        void slotIndexingFinished( int returnCode );
        void slotCancelIndexing();

        
    private:
        void enableWidgetsFor( const KMrml::ServerSettings& settings );
        void initFromSettings( const KMrml::ServerSettings& settings );

        void processIndexDirs( const QStringList& removedDirs );
        
        QStringList difference( const QStringList& oldIndexDirs,
                                const QStringList& newIndexDirs ) const;

        ServerConfigWidget *m_serverWidget;
        KEditListBox *m_listBox;
        KMrml::Config *m_config;
        KMrmlConfig::Indexer *m_indexer;
        KMrmlConfig::IndexCleaner *m_indexCleaner;
        KProgressDialog *m_progressDialog;

        KMrml::ServerSettings m_settings;
        bool m_performIndexing;
        bool m_locked;
    };

}



#endif // MAINPAGE_H
