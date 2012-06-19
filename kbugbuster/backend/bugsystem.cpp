
#include "bugsystem.h"
#include "packagelistjob.h"
#include "buglistjob.h"
#include "bugmybugsjob.h"
#include "bugdetailsjob.h"
#include "bugcommand.h"

#include <kstaticdeleter.h>
#include <kdebug.h>
#include <klocale.h>
#include <kemailsettings.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kconfig.h>

#include "packageimpl.h"
#include "bugimpl.h"
#include "bugdetailsimpl.h"
#include "mailsender.h"
#include "kbbprefs.h"
#include "bugserver.h"
#include "bugserverconfig.h"
#include "bugcache.h"

KStaticDeleter<BugSystem> bssd;

BugSystem *BugSystem::s_self = 0;

QString BugSystem::mLastResponse;

BugSystem *BugSystem::self()
{
    if ( !s_self )
        s_self = bssd.setObject( s_self, new BugSystem );

    return s_self;
}

BugSystem::BugSystem()
    : m_disconnected( false )
{
    mServer = 0;
}

BugSystem::~BugSystem()
{
    QValueList<BugServer *>::ConstIterator it;
    for( it = mServerList.begin(); it != mServerList.end(); ++it ) {
        delete *it;
    }
}

BugCache *BugSystem::cache()const
{
  return mServer->cache();
}

void BugSystem::setDisconnected( bool disconnected )
{
    m_disconnected = disconnected;
}

bool BugSystem::disconnected() const
{
    return m_disconnected;
}

void BugSystem::retrievePackageList()
{
    mServer->setPackages( mServer->cache()->loadPackageList() );

    if( !mServer->packages().isEmpty() ) {
        emit packageListAvailable( mServer->packages() );
    } else {
        emit packageListCacheMiss();

        if ( !m_disconnected )
        {
            emit packageListLoading();

            PackageListJob *job = new PackageListJob( mServer );
            connect( job, SIGNAL( packageListAvailable( const Package::List & ) ),
                     this, SIGNAL( packageListAvailable( const Package::List & ) ) );
            connect( job, SIGNAL( packageListAvailable( const Package::List & ) ),
                     this, SLOT( setPackageList( const Package::List & ) ) );
            connect( job, SIGNAL( error( const QString & ) ),
                     this, SIGNAL( loadingError( const QString & ) ) );
            connectJob( job );

            registerJob( job );

            job->start();
        }
    }
}

void BugSystem::retrieveBugList( const Package &pkg, const QString &component )
{
    kdDebug() << "BugSystem::retrieveBugList(): " << pkg.name() << endl;

    if ( pkg.isNull() )
        return;

    mServer->setBugs( pkg, component,
                      mServer->cache()->loadBugList( pkg, component,
                                                     m_disconnected ) );

    // Since the GUI stops showing the splash widget after this signal,
    // we should not emit anything on a cache miss...
    if( !mServer->bugs( pkg, component ).isEmpty() )
        emit bugListAvailable( pkg, component, mServer->bugs( pkg, component ) );
    else
    {
        emit bugListCacheMiss( pkg );

        if ( !m_disconnected )
        {
            kdDebug() << "BugSystem::retrieveBugList() starting job" << endl;
            emit bugListLoading( pkg, component );

            BugListJob *job = new BugListJob( mServer );
            connect( job, SIGNAL( bugListAvailable( const Package &, const QString &, const Bug::List & ) ),
                     this, SIGNAL( bugListAvailable( const Package &, const QString &, const Bug::List & ) ) );
            connect( job, SIGNAL( bugListAvailable( const Package &, const QString &, const Bug::List & ) ),
                     this, SLOT( setBugList( const Package &, const QString &, const Bug::List & ) ) );
            connect( job, SIGNAL( error( const QString & ) ),
                     this, SIGNAL( loadingError( const QString & ) ) );
            connectJob( job );

            registerJob( job );

            job->start( pkg, component );
        }
    }
}

void BugSystem::retrieveMyBugsList()
{
    kdDebug() << k_funcinfo << endl;

    if ( m_disconnected )
    {
        // This function is not cached for now
        emit bugListCacheMiss( i18n( "My Bugs" ) );
    }
    else
    {
        kdDebug() << k_funcinfo << "Starting job" << endl;

        emit bugListLoading( i18n( "Retrieving My Bugs list..." ) );

        BugMyBugsJob *job = new BugMyBugsJob( mServer );

        connect( job, SIGNAL( bugListAvailable( const QString &, const Bug::List & ) ),
                 this, SIGNAL( bugListAvailable( const QString &, const Bug::List & ) ) );
        connect( job, SIGNAL( error( const QString & ) ),
                 this, SIGNAL( loadingError( const QString & ) ) );
        connectJob( job );

        registerJob( job );

        job->start();
    }
}

void BugSystem::retrieveBugDetails( const Bug &bug )
{
    if ( bug.isNull() )
        return;

    kdDebug() << "BugSystem::retrieveBugDetails(): " << bug.number() << endl;

    mServer->setBugDetails( bug, mServer->cache()->loadBugDetails( bug ) );

    if ( !mServer->bugDetails( bug ).isNull() ) {
//        kdDebug() << "Found in cache." << endl;
        emit bugDetailsAvailable( bug, mServer->bugDetails( bug ) );
    } else {
//        kdDebug() << "Not found in cache." << endl;
        emit bugDetailsCacheMiss( bug );

        if ( !m_disconnected ) {
            emit bugDetailsLoading( bug );

            BugDetailsJob *job = new BugDetailsJob( mServer );
            connect( job, SIGNAL( bugDetailsAvailable( const Bug &, const BugDetails & ) ),
                     this, SIGNAL( bugDetailsAvailable( const Bug &, const BugDetails & ) ) );
            connect( job, SIGNAL( bugDetailsAvailable( const Bug &, const BugDetails & ) ),
                     this, SLOT( setBugDetails( const Bug &, const BugDetails & ) ) );
            connect( job, SIGNAL( error( const QString & ) ),
                     this, SIGNAL( bugDetailsLoadingError() ) );
            connectJob( job );

            registerJob( job );

            job->start( bug );
        }
    }
}

void BugSystem::connectJob( BugJob *job )
{
    connect( job, SIGNAL( infoMessage( const QString & ) ),
             this, SIGNAL( infoMessage( const QString & ) ) );
    connect( job, SIGNAL( infoPercent( unsigned long ) ),
             this, SIGNAL( infoPercent( unsigned long ) ) );
    connect( job, SIGNAL( jobEnded( BugJob * ) ),
             SLOT( unregisterJob( BugJob * ) ) );
}

void BugSystem::setPackageList( const Package::List &pkgs )
{
    mServer->setPackages( pkgs );

    mServer->cache()->savePackageList( pkgs );
}

void BugSystem::setBugList( const Package &pkg, const QString &component, const Bug::List &bugs )
{
    mServer->setBugs( pkg, component, bugs );
    mServer->cache()->saveBugList( pkg, component, bugs );
}

void BugSystem::setBugDetails( const Bug &bug, const BugDetails &details )
{
    mServer->setBugDetails( bug , details );

    mServer->cache()->saveBugDetails( bug, details );
}

Package::List BugSystem::packageList() const
{
    return mServer->packages();
}

Package BugSystem::package( const QString &pkgname ) const
{
    Package::List::ConstIterator it;
    for( it = mServer->packages().begin(); it != mServer->packages().end(); ++it ) {
        if( pkgname == (*it).name() ) return (*it);
    }
    return Package();
}

Bug BugSystem::bug( const Package &pkg, const QString &component, const QString &number ) const
{
    Bug::List bugs = mServer->bugs( pkg, component );

    Bug::List::ConstIterator it;
    for( it = bugs.begin(); it != bugs.end(); ++it ) {
        if( number == (*it).number() ) return (*it);
    }
    return Bug();
}

void BugSystem::queueCommand( BugCommand *cmd )
{
    if ( mServer->queueCommand( cmd ) ) emit commandQueued( cmd );
}

void BugSystem::clearCommands( const QString &bug )
{
    mServer->clearCommands( bug );

    emit commandCanceled( bug );
}

void BugSystem::clearCommands()
{
    QStringList bugs = mServer->bugsWithCommands();

    QStringList::ConstIterator it;
    for( it = bugs.begin(); it != bugs.end(); ++it ) {
        clearCommands( *it );
    }
}

void BugSystem::sendCommands()
{
    QString recipient = KBBPrefs::instance()->mOverrideRecipient;
    bool sendBCC = KBBPrefs::instance()->mSendBCC;

    KEMailSettings emailSettings;
    QString senderName = emailSettings.getSetting( KEMailSettings::RealName );
    QString senderEmail = emailSettings.getSetting( KEMailSettings::EmailAddress );
    QString smtpServer = emailSettings.getSetting( KEMailSettings::OutServer );

    MailSender::MailClient client = (MailSender::MailClient)KBBPrefs::instance()->mMailClient;

    // ### connect to signals
    MailSender *mailer = new MailSender( client, smtpServer );
    connect( mailer, SIGNAL( status( const QString & ) ),
             SIGNAL( infoMessage( const QString & ) ) );

    mServer->sendCommands( mailer, senderName, senderEmail, sendBCC, recipient );
}

void BugSystem::setServerList( const QValueList<BugServerConfig> &servers )
{
    if ( servers.isEmpty() ) return;

    QString currentServer;
    if ( mServer ) currentServer = mServer->serverConfig().name();
    else currentServer = KBBPrefs::instance()->mCurrentServer;

    killAllJobs();

    QValueList<BugServer *>::ConstIterator serverIt;
    for( serverIt = mServerList.begin(); serverIt != mServerList.end();
         ++serverIt ) {
        delete *serverIt;
    }
    mServerList.clear();

    QValueList<BugServerConfig>::ConstIterator cfgIt;
    for( cfgIt = servers.begin(); cfgIt != servers.end(); ++cfgIt ) {
        mServerList.append( new BugServer( *cfgIt ) );
    }

    setCurrentServer( currentServer );
}

QValueList<BugServer *> BugSystem::serverList()
{
    return mServerList;
}

void BugSystem::setCurrentServer( const QString &name )
{
    killAllJobs();

    BugServer *server = findServer( name );
    if ( server ) {
        mServer = server;
    } else {
        kdError() << "Server '" << name << "' not known." << endl;
        if ( mServerList.isEmpty() ) {
            kdError() << "Fatal error: server list empty." << endl;
        } else {
            mServer = mServerList.first();
        }
    }

    if ( mServer ) {
      KBBPrefs::instance()->mCurrentServer = mServer->serverConfig().name();
    }
}

BugServer *BugSystem::findServer( const QString &name )
{
    QValueList<BugServer *>::ConstIterator serverIt;
    for( serverIt = mServerList.begin(); serverIt != mServerList.end();
         ++serverIt ) {
        if ( (*serverIt)->serverConfig().name() == name ) return *serverIt;
    }
    return 0;
}

void BugSystem::saveQuery( const KURL &url )
{
  mLastResponse = "Query: " + url.url();
  mLastResponse += "\n\n";
}

void BugSystem::saveResponse( const QByteArray &response )
{
  mLastResponse += response;
}

QString BugSystem::lastResponse()
{
  return mLastResponse;
}

void BugSystem::readConfig( KConfig *config )
{
  config->setGroup("Servers");
  QStringList servers = config->readListEntry( "Servers" );

  QValueList<BugServerConfig> serverList;

  if ( servers.isEmpty() ) {
    serverList.append( BugServerConfig() );
  } else {
    QStringList::ConstIterator it;
    for( it = servers.begin(); it != servers.end(); ++it ) {
      BugServerConfig cfg;
      cfg.readConfig( config, *it );
      serverList.append( cfg );
    }
  }

  setServerList( serverList );
}

void BugSystem::writeConfig( KConfig *config )
{
  QValueList<BugServer *>::ConstIterator itServer;
  QStringList servers;
  QValueList<BugServer *> serverList = BugSystem::self()->serverList();
  for( itServer = serverList.begin(); itServer != serverList.end();
       ++itServer ) {
    BugServerConfig serverConfig = (*itServer)->serverConfig();
    servers.append( serverConfig.name() );
    serverConfig.writeConfig( config );
  }

  config->setGroup("Servers");
  config->writeEntry( "Servers", servers );
}

void BugSystem::registerJob( BugJob *job )
{
  mJobs.append( job );
}

void BugSystem::unregisterJob( BugJob *job )
{
  mJobs.removeRef( job );
}

void BugSystem::killAllJobs()
{
  BugJob *job;
  for( job = mJobs.first(); job; job = mJobs.next() ) {
    job->kill();
    unregisterJob( job );
  }
}

#include "bugsystem.moc"

/*
 * vim:sw=4:ts=4:et
 */
