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

#include <qdir.h>
#include <qfile.h>
#include <qtextcodec.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include "kmrml_config.h"

#include <kdeversion.h>
#if KDE_VERSION < 307
  #define QUOTE( x ) x
#else
  #define QUOTE( x ) KProcess::quote( x )
#endif

using namespace KMrml;

// #define DEFAULT_ADDCOLLECTION_CMD "gift-add-collection.pl --thumbnail-dir=%t --local-encoding %d"
#define DEFAULT_ADDCOLLECTION_CMD "gift-add-collection.pl --gift-home=%h --thumbnail-dir=%t --local-encoding=%e %d"
#define DEFAULT_REMOVECOLLECTION_CMD "gift-add-collection.pl --gift-home=%h --local-encoding=%e --remove-collection %d"

#define DEFAULT_MRMLD_CMD "gift --port %p --datadir %d"
#define DEFAULT_MRMLD_CMD_AUTOPORT "gift --datadir %d"

#define CONFIG_GROUP "MRML Settings"
#define DEFAULT_HOST "localhost"
#define DEFAULT_USER "kmrml"
#define DEFAULT_PASS "none"
#define DEFAULT_AUTH false
#define DEFAULT_AUTOPORT true
const int DEFAULT_PORT = 12789;

Config::Config()
{
    m_ownConfig = new KConfig( "kio_mrmlrc", false, false );
    m_config = m_ownConfig;

    init();
}

Config::Config( KConfig *config )
    : m_config( config ),
      m_ownConfig( 0L )
{
    init();
}

Config::~Config()
{
    delete m_ownConfig;
}

void Config::init()
{
    m_config->setGroup( CONFIG_GROUP );
    m_defaultHost = m_config->readEntry( "Default Host" );
    if ( m_defaultHost.isEmpty() )
        m_defaultHost = DEFAULT_HOST;

    m_hostList = m_config->readListEntry( "Host List" );
    if ( m_hostList.isEmpty() )
        m_hostList.append( DEFAULT_HOST );
    
    m_serverStartedIndividually = 
        m_config->readBoolEntry( "ServerStartedIndividually", false );
}

bool Config::sync()
{
    bool notifySlaves = m_config->isDirty();
    m_config->sync();
    return notifySlaves;

    // This moved to kcontrol/MainPage::save() so we don't have to link against
    // KIO and need a full KApplication instance to work (so that the tiny
    // mrmlsearch binary can also use this class)
    // tell the ioslaves about the new configuration
//     if ( notifySlaves )
//         KIO::SlaveConfig::self()->reset();
}

void Config::setDefaultHost( const QString& host )
{
    m_defaultHost = host.isEmpty() ?
                    QString::fromLatin1(DEFAULT_HOST) : host;

    m_config->setGroup( CONFIG_GROUP );
    m_config->writeEntry( "Default Host", m_defaultHost );
}

ServerSettings Config::settingsForLocalHost() const
{
    return settingsForHost( "localhost" );
}

ServerSettings Config::settingsForHost( const QString& host ) const
{
    KConfigGroup config( m_config, settingsGroup( host ) );
    ServerSettings settings;

    settings.host = host;
    settings.configuredPort = config.readUnsignedNumEntry( "Port",
                                                           DEFAULT_PORT );
    settings.autoPort = (host == "localhost") &&
                        config.readBoolEntry("Automatically determine Port",
                                             DEFAULT_AUTOPORT );
    settings.user = config.readEntry( "Username", DEFAULT_USER );
    settings.pass = config.readEntry( "Password", DEFAULT_PASS );
    settings.useAuth = config.readBoolEntry( "Perform Authentication",
                                                DEFAULT_AUTH );

    return settings;
}

void Config::addSettings( const ServerSettings& settings )
{
    QString host = settings.host;
    if ( m_hostList.find( host ) == m_hostList.end() )
        m_hostList.append( host );

    m_config->setGroup( CONFIG_GROUP );
    m_config->writeEntry( "Host List", m_hostList );

    m_config->setGroup( settingsGroup( host ) );
    m_config->writeEntry( "Host", host );
    m_config->writeEntry( "Port", settings.configuredPort );
    m_config->writeEntry( "Automatically determine Port", settings.autoPort );
    m_config->writeEntry( "Username", settings.user );
    m_config->writeEntry( "Password", settings.pass );
    m_config->writeEntry( "Perform Authentication", settings.useAuth );
}

bool Config::removeSettings( const QString& host )
{
    bool success = m_config->deleteGroup( settingsGroup( host ) );
    if ( success )
    {
        m_hostList.remove( host );
        m_config->setGroup( CONFIG_GROUP );
    }

    return success;
}

QStringList Config::indexableDirectories() const
{
    m_config->setGroup( CONFIG_GROUP );
    return m_config->readListEntry( "Indexable Directories" );
}

void Config::setIndexableDirectories( const QStringList& dirs )
{
    m_config->setGroup( CONFIG_GROUP );
    m_config->writeEntry( "Indexable Directories", dirs );
}

QString Config::addCollectionCommandLine() const
{
    m_config->setGroup( CONFIG_GROUP );
    QString cmd = m_config->readEntry( "AddCollection Commandline",
                                       DEFAULT_ADDCOLLECTION_CMD );
    int index = cmd.find( "%h" );
    if ( index != -1 )
        cmd.replace( index, 2, QUOTE( mrmldDataDir() ) );

    index = cmd.find( "%e" );
    if ( index != -1 )
        cmd.replace( index, 2, QTextCodec::codecForLocale()->mimeName() );

    return cmd;
}

void Config::setAddCollectionCommandLine( const QString& cmd )
{
    m_config->setGroup( CONFIG_GROUP );
    m_config->writeEntry( "AddCollection Commandline", cmd );
}

QString Config::removeCollectionCommandLine() const
{
    m_config->setGroup( CONFIG_GROUP );
    QString cmd = m_config->readEntry( "RemoveCollection Commandline",
                                       DEFAULT_REMOVECOLLECTION_CMD );
    int index = cmd.find( "%h" );
    if ( index != -1 )
        cmd.replace( index, 2, QUOTE( mrmldDataDir() ) );

    index = cmd.find( "%e" );
    if ( index != -1 )
        cmd.replace( index, 2, QTextCodec::codecForLocale()->mimeName() );

    return cmd;
}

void Config::setRemoveCollectionCommandLine( const QString& cmd )
{
    m_config->setGroup( CONFIG_GROUP );
    m_config->writeEntry( "RemoveCollection Commandline", cmd );
}

QString Config::mrmldCommandline() const
{
    ServerSettings settings = settingsForLocalHost();

    m_config->setGroup( CONFIG_GROUP );
    QString cmd = m_config->readEntry( "MrmmlDaemon Commandline",
                                       settings.autoPort ?
                                       DEFAULT_MRMLD_CMD_AUTOPORT :
                                       DEFAULT_MRMLD_CMD );

    // add data directory and port to the commandline
    int index = cmd.find( "%p" );
    if ( index != -1 )
    {
        QString port = settings.autoPort ?
                  QString::null : QString::number( settings.configuredPort );
        cmd.replace( index, 2, port );
    }
    index = cmd.find( "%d" );
    if ( index != -1 )
    {
        cmd.replace( index, 2, QUOTE( mrmldDataDir() ) );
    }

    qDebug("***** commandline: %s", cmd.latin1());

    return cmd;
}

QString Config::mrmldDataDir()
{
    QString dir = KGlobal::dirs()->saveLocation( "data",
                                                 "kmrml/mrmld-data/" );
    if ( dir.isEmpty() ) // fallback
        dir = QDir::homeDirPath() + "/";

    return dir;
}

void Config::setMrmldCommandLine( const QString& cmd )
{
    m_config->setGroup( CONFIG_GROUP );
    m_config->writeEntry( "MrmmlDaemon Commandline", cmd );
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

ServerSettings::ServerSettings()
    : configuredPort( 0 ),
      autoPort( true ),
      useAuth( false )
{
}

ServerSettings::ServerSettings( const QString& host, unsigned short int port,
                                bool autoPort, bool useAuth,
                                const QString& user, const QString& pass )
{
    this->host = host;
    this->configuredPort = port;
    this->autoPort = autoPort;
    this->useAuth = useAuth;
    this->user = user;
    this->pass = pass;
}

// static
ServerSettings ServerSettings::defaults()
{
    return ServerSettings( DEFAULT_HOST, DEFAULT_PORT,
                           (!strcmp(DEFAULT_HOST, "localhost") && DEFAULT_PORT),
                           DEFAULT_AUTH, DEFAULT_USER, DEFAULT_PASS );
}

KURL ServerSettings::getUrl() const
{
    KURL url;
    url.setProtocol( "mrml" );
    url.setHost( host );
    if ( !autoPort )
        url.setPort( configuredPort );

    if ( useAuth && user.isEmpty() )
    {
        url.setUser( user );
        url.setPass( pass );
    }

    return url;
}

unsigned short int ServerSettings::port() const
{
    if ( autoPort )
    {
        QString portsFile = Config::mrmldDataDir() + "gift-port.txt";
        QFile file( portsFile );
        if ( file.open( IO_ReadOnly ) )
        {
            QString line;
            (void) file.readLine( line, 6 );
//             qDebug("**** read: %s", line.latin1());

            file.close();

            bool ok;
            unsigned short int p = line.toUShort( &ok );
            if ( ok )
                return p;
        }
        else
            kdWarning() << "Can't open \"" << portsFile << "\" to automatically determine the gift port" << endl;
    }

    return configuredPort;
}
