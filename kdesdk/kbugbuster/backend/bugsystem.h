#ifndef __bugsystem_h__
#define __bugsystem_h__

#include "package.h"
#include "bug.h"
#include "bugdetails.h"
#include "bugcache.h"

#include <kurl.h>

#include <qobject.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qpair.h>

class KConfig;

class BugCommand;
class BugServer;
class BugServerConfig;
class BugJob;

class BugSystem : public QObject
{
    Q_OBJECT
    friend class BugJob;
  public:
    BugSystem();
    virtual ~BugSystem();

    static BugSystem *self();

    BugCache *cache()const;
    BugServer *server() const { return mServer; }

    /**
      BugSystem takes ownership of the BugServerConfig objects.
    */
    void setServerList( const QValueList<BugServerConfig> &servers );
    QValueList<BugServer *> serverList();

    void setCurrentServer( const QString & );

    void retrievePackageList();
    void retrieveBugList( const Package &, const QString &component );
    void retrieveBugDetails( const Bug & );

    /**
     * Load the bugs the user reported himself, or for which he is the assigned to person
     */
    void retrieveMyBugsList();

    /**
      Queue a new command.
    */
    void queueCommand( BugCommand * );
    /**
      Forget all commands for a given bug.
    */
    void clearCommands( const QString &bug );
    /**
      Forget all commands for all bugs.
    */
    void clearCommands();
    /**
      Send all commands (generate the mails).
    */
    void sendCommands();

    void setDisconnected( bool );
    bool disconnected() const;

    Package::List packageList() const;

    Package package( const QString &pkgname ) const;
    Bug bug( const Package &pkg, const QString &component, const QString &number ) const;

    static void saveQuery( const KURL &url );
    static void saveResponse( const QByteArray &d );
    static QString lastResponse();

    void readConfig( KConfig * );
    void writeConfig( KConfig * );

  signals:
    void packageListAvailable( const Package::List &pkgs );
    void bugListAvailable( const Package &pkg, const QString &component, const Bug::List & );
    void bugListAvailable( const QString &label, const Bug::List & );
    void bugDetailsAvailable( const Bug &, const BugDetails & );

    void packageListLoading();
    void bugListLoading( const Package &, const QString &component );
    void bugListLoading( const QString &label );
    void bugDetailsLoading( const Bug & );

    void packageListCacheMiss();
    void bugListCacheMiss( const Package &package );
    void bugListCacheMiss( const QString &label );
    void bugDetailsCacheMiss( const Bug & );

    void bugDetailsLoadingError();

    void infoMessage( const QString &message );
    void infoPercent( unsigned long percent );

    void commandQueued( BugCommand * );
    void commandCanceled( const QString & );

    void loadingError( const QString &text );

  protected:
    BugServer *findServer( const QString &name );

    void registerJob( BugJob * );

    void connectJob( BugJob * );

    void killAllJobs();

  protected slots:
    void unregisterJob( BugJob * );

  private slots:
    void setPackageList( const Package::List &pkgs );
    void setBugList( const Package &pkg, const QString &component, const Bug::List &bugs );
    void setBugDetails( const Bug &bug, const BugDetails &details );

  private:
    bool m_disconnected;

    BugServer *mServer;

    QValueList<BugServer *> mServerList;

    QPtrList<BugJob> mJobs;

    static BugSystem *s_self;

    static QString mLastResponse;
};

#endif

/*
 * vim:sw=4:ts=4:et
 */
