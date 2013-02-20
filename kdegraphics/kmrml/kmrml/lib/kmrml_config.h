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

#ifndef KMRML_CONFIG_H
#define KMRML_CONFIG_H

class KConfig;

#include <qstringlist.h>
#include <kurl.h>

namespace KMrml
{
    class ServerSettings
    {
    public:
        ServerSettings();
        ServerSettings(const QString& host, unsigned short int port,
                       bool autoPort, bool useAuth, const
                       QString& user, const QString& pass);

        // does NOT set the port in the KURL object, if autoPort is selected
        // kio_mrml is going to determine itself (via ServerSettings::port()).
        // This deuglifies the mrml:/ url a bit (no port is shown)
        KURL getUrl() const;

        QString host;
        QString user;
        QString pass;
        unsigned short int configuredPort;
        bool autoPort :1; // only possible with host == localhost
        bool useAuth  :1;

        static ServerSettings defaults();

        // returns configuredPort or the automatically determined port,
        // depending on the value of autoPort
        unsigned short int port() const;
    };

    class Config
    {
    public:
        Config();
        Config( KConfig *config ); // does not take ownership of KConfig
        ~Config();

        bool sync();

        ServerSettings defaultSettings() const
        {
            return settingsForHost( m_defaultHost );
        }

        ServerSettings settingsForLocalHost() const;
        ServerSettings settingsForHost( const QString& host ) const;

        void setDefaultHost( const QString& host );

        /**
         * Indexed by the hostname -- ensures there are no dupes
         */
        void addSettings( const ServerSettings& settings );

        bool removeSettings( const QString& host );

        QStringList hosts() const { return m_hostList; }

        /**
         * The list of indexable directories -- only applicable to "localhost"
         */
        QStringList indexableDirectories() const;
        void setIndexableDirectories( const QStringList& dirs );

        QString addCollectionCommandLine() const;
        void setAddCollectionCommandLine( const QString& cmd );

        QString removeCollectionCommandLine() const;
        void setRemoveCollectionCommandLine( const QString& cmd );

        void setMrmldCommandLine( const QString& cmd );
        QString mrmldCommandline() const;

        // e.g. Wolfgang needs this :)
        bool serverStartedIndividually() const { 
            return m_serverStartedIndividually; 
        }
        
        static QString mrmldDataDir();

    private:
        void init();

        QString settingsGroup( const QString& host ) const
        {
            return QString::fromLatin1( "SettingsFor: " ).append( host );
        }

        bool m_serverStartedIndividually;
        QString m_defaultHost;
        QStringList m_hostList;

        KConfig *m_config;
        KConfig *m_ownConfig;
    };
}

#endif // KMRML_CONFIG_H
