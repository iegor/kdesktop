// (C) 2001, Cornelius Schumacher

#include <qstringlist.h>
#include <qfile.h>

#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kdebug.h>

#include "packageimpl.h"
#include "bugimpl.h"
#include "bugdetailsimpl.h"

#include "bugcache.h"

BugCache::BugCache( const QString &id )
{
    mId = id;

    init();
}

BugCache::~BugCache()
{
    m_cachePackages->sync();
    m_cacheBugs->sync();

    delete m_cachePackages;
    delete m_cacheBugs;
}

void BugCache::init()
{
    mCachePackagesFileName = locateLocal( "appdata", mId + "-packages.cache" );
    mCacheBugsFileName = locateLocal( "appdata", mId + "-bugs.cache" );

    m_cachePackages = new KSimpleConfig( mCachePackagesFileName );
    m_cacheBugs = new KSimpleConfig( mCacheBugsFileName );
}

void BugCache::savePackageList( const Package::List &pkgs )
{
    Package::List::ConstIterator it;
    for (it = pkgs.begin(); it != pkgs.end(); ++it) {
        m_cachePackages->setGroup((*it).name());
        m_cachePackages->writeEntry("description",(*it).description());
        m_cachePackages->writeEntry("numberOfBugs",(*it).numberOfBugs());
        m_cachePackages->writeEntry("components",(*it).components());
        writePerson(m_cachePackages,"Maintainer",(*it).maintainer());
    }
}

Package::List BugCache::loadPackageList()
{
    Package::List pkgs;

    QStringList packages = m_cachePackages->groupList();
    QStringList::ConstIterator it;
    for( it = packages.begin(); it != packages.end(); ++it ) {
        if ((*it) == "<default>") continue;
        if ((*it).contains("/")) continue;

        m_cachePackages->setGroup(*it);

        QString description = m_cachePackages->readEntry("description");
        int numberOfBugs = m_cachePackages->readNumEntry("numberOfBugs");
        Person maintainer = readPerson( m_cachePackages, "Maintainer");
        QStringList components = m_cachePackages->readListEntry("components");
 
        pkgs.append( Package( new PackageImpl( (*it), description, numberOfBugs,
                                                maintainer, components ) ) );
    }

    return pkgs;
}

void BugCache::invalidatePackageList()
{
    // Completely wipe out packages.cache
    QStringList packages = m_cachePackages->groupList();
    QStringList::ConstIterator it;
    for( it = packages.begin(); it != packages.end(); ++it ) {
        if ((*it) == "<default>") continue;
        m_cachePackages->deleteGroup(*it, true);
    }
}

void BugCache::saveBugList( const Package &pkg, const QString &component, const Bug::List &bugs )
{
    QStringList bugList;

    Bug::List::ConstIterator it;
    for( it = bugs.begin(); it != bugs.end(); ++it ) {
        QString number = (*it).number();
        bugList.append( number );
        m_cacheBugs->setGroup( number );
        m_cacheBugs->writeEntry( "Title", (*it).title() );
        m_cacheBugs->writeEntry( "Severity", Bug::severityToString((*it).severity()) );
        m_cacheBugs->writeEntry( "Status", Bug::statusToString((*it).status()) );
        m_cacheBugs->writeEntry( "MergedWith" , (*it).mergedWith() );
        m_cacheBugs->writeEntry( "Age", ( *it ).age() );
        writePerson( m_cacheBugs, "Submitter", (*it).submitter() );
        writePerson( m_cacheBugs, "TODO", (*it).developerTODO() );
    }

    if ( component.isEmpty() )
        m_cachePackages->setGroup( pkg.name() );
    else {
        m_cachePackages->setGroup( pkg.name() + "/" + component );
    }

    m_cachePackages->writeEntry( "bugList", bugList );
}

Bug::List BugCache::loadBugList( const Package &pkg, const QString &component, bool disconnected )
{
//    kdDebug() << "Loading bug list for " << pkg.name() << endl;

    Bug::List bugList;

    if ( component.isEmpty() )
        m_cachePackages->setGroup( pkg.name() );
    else
        m_cachePackages->setGroup( pkg.name() + "/" + component );

    QStringList bugs = m_cachePackages->readListEntry( "bugList" );

//    kdDebug() << "  Bugs: " << (bugs.join(",")) << endl;

    QStringList::ConstIterator it;
    for( it = bugs.begin(); it != bugs.end(); ++it ) {
        if ( m_cacheBugs->hasGroup(*it) )
        {
            m_cacheBugs->setGroup(*it);
            QString title = m_cacheBugs->readEntry("Title");
            if ( !title.isEmpty() ) // dunno how I ended up with an all empty bug in the cache
            {
                Person submitter = readPerson( m_cacheBugs, "Submitter" );
                Bug::Status status = Bug::stringToStatus( m_cacheBugs->readEntry("Status") );
                Bug::Severity severity = Bug::stringToSeverity( m_cacheBugs->readEntry("Severity") );
                Person developerTODO = readPerson( m_cacheBugs, "TODO" );
                Bug::BugMergeList mergedWith = m_cacheBugs->readIntListEntry( "MergedWith" );
                uint age = m_cacheBugs->readUnsignedNumEntry( "Age", 0xFFFFFFFF );
                bugList.append( Bug( new BugImpl( title, submitter, ( *it ), age,
                                                  severity, developerTODO,
                                                  status, mergedWith ) ) );
            }
        } else {
            // This bug is in the package cache's buglist but not in the bugs cache
            // Probably a new bug, we need to fetch it - if we're not in disconnected mode
            kdWarning() << "Bug " << *it << " not in bug cache" << endl;
            if ( !disconnected )
                return Bug::List(); // returning an empty list will trigger a reload of the buglist
        }
    }

    return bugList;
}

void BugCache::invalidateBugList( const Package& pkg, const QString &component )
{
    kdDebug() << "BugCache::invalidateBugList " << pkg.name()
              << " (" << component << ")" << endl;

    // Erase bug list for this package
    if ( component.isEmpty() ) {
        m_cachePackages->setGroup( pkg.name() );
    } else {
        QString key = pkg.name() + "/" + component;
        m_cachePackages->setGroup( key );
        m_cachePackages->setGroup( pkg.name() + "/" + component );
    }

    m_cachePackages->writeEntry("bugList",QString::null);
}

void BugCache::saveBugDetails( const Bug &bug, const BugDetails &details )
{
    m_cacheBugs->setGroup( bug.number() );

    m_cacheBugs->writeEntry( "Version", details.version() );
    m_cacheBugs->writeEntry( "Source", details.source() );
    m_cacheBugs->writeEntry( "Compiler", details.compiler() );
    m_cacheBugs->writeEntry( "OS", details.os() );

    QStringList senders;
    QStringList texts;
    QStringList dates;

    BugDetailsPart::List parts = details.parts();
    BugDetailsPart::List::ConstIterator it;
    for ( it = parts.begin(); it != parts.end(); ++it ) {
        senders.append( (*it).sender.fullName() );
        texts.append( (*it).text );
        dates.append( (*it).date.toString( Qt::ISODate ) );
    }

    m_cacheBugs->writeEntry( "Details", texts );
    m_cacheBugs->writeEntry( "Senders", senders );
    m_cacheBugs->writeEntry( "Dates", dates );
}

bool BugCache::hasBugDetails( const Bug& bug ) const
{
    if ( !m_cacheBugs->hasGroup( bug.number() ) )
        return false;

    m_cacheBugs->setGroup( bug.number() );
    return m_cacheBugs->hasKey( "Details" );
}

BugDetails BugCache::loadBugDetails( const Bug &bug )
{
    if ( !m_cacheBugs->hasGroup( bug.number() ) ) {
        return BugDetails();
    }

    m_cacheBugs->setGroup( bug.number() );

    BugDetailsPart::List parts;

    QStringList texts = m_cacheBugs->readListEntry( "Details" );
    QStringList senders = m_cacheBugs->readListEntry( "Senders" );
    QStringList dates = m_cacheBugs->readListEntry( "Dates" );

    QStringList::ConstIterator itTexts = texts.begin();
    QStringList::ConstIterator itSenders = senders.begin();
    QStringList::ConstIterator itDates = dates.begin();
    while( itTexts != texts.end() ) {
        QDateTime date = QDateTime::fromString( *itDates, Qt::ISODate );
        parts.append( BugDetailsPart( Person(*itSenders), date, *itTexts ) );

        ++itTexts;
        ++itSenders;
        ++itDates;
    }

    if ( parts.count() == 0 ) {
        return BugDetails();
    }

    QString version = m_cacheBugs->readEntry( "Version" );
    QString source = m_cacheBugs->readEntry( "Source" );
    QString compiler = m_cacheBugs->readEntry( "Compiler" );
    QString os = m_cacheBugs->readEntry( "OS" );

    return BugDetails( new BugDetailsImpl( version, source, compiler, os,
                                           parts ) );
}

void BugCache::invalidateBugDetails( const Bug& bug )
{
    m_cacheBugs->deleteGroup( bug.number(), true );
}

void BugCache::clear()
{
    delete m_cachePackages;
    delete m_cacheBugs;

    QFile f1( mCachePackagesFileName );
    f1.remove();

    QFile f2( mCacheBugsFileName );
    f2.remove();

    init();    
}

void BugCache::writePerson( KSimpleConfig *file, const QString &key,
                            const Person &p )
{
    QStringList values;
    values.append(p.name);
    values.append(p.email);
    file->writeEntry( key, values );
}

struct Person BugCache::readPerson( KSimpleConfig *file, const QString &key )
{
    struct Person p;
    QStringList values = file->readListEntry(key);
    if ( values.count() > 0 )
        p.name = values[0];
    if ( values.count() > 1 )
        p.email = values[1];
    return p;
}
