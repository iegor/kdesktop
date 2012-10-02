#ifndef BUGCACHE_H
#define BUGCACHE_H

class KSimpleConfig;

#include "package.h"
#include "bug.h"
#include "bugdetails.h"

class BugCache
{
    public:
        BugCache( const QString &id );
        ~BugCache();

        void savePackageList( const Package::List &pkgs );
        Package::List loadPackageList();
        void invalidatePackageList();

        void saveBugList( const Package &pkg, const QString &component, const Bug::List & );
        Bug::List loadBugList( const Package &pkg, const QString &component, bool disconnected );
        void invalidateBugList( const Package &pkg, const QString &component );

        void saveBugDetails( const Bug &bug, const BugDetails & );
        BugDetails loadBugDetails( const Bug &bug );
        void invalidateBugDetails( const Bug &bug );
        bool hasBugDetails( const Bug& bug ) const;

        void clear();

    private:
        void init();
    
        void writePerson( KSimpleConfig *file, const QString &key,
                          const Person &p );
        struct Person readPerson (KSimpleConfig *file, const QString &key );

        QString mId;

        KSimpleConfig *m_cachePackages;
        KSimpleConfig *m_cacheBugs;

        QString mCachePackagesFileName;
        QString mCacheBugsFileName;
};

#endif
