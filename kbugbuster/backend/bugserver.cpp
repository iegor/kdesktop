/*
    This file is part of KBugBuster.
    Copyright (c) 2002,2003 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include "bugserver.h"
#include "kbbprefs.h"
#include "rdfprocessor.h"
#include "bugcache.h"
#include "bugcommand.h"
#include "mailsender.h"
#include "bugserverconfig.h"
#include "htmlparser.h"

#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kdebug.h>

BugServer::BugServer()
{
  init();
}

BugServer::BugServer( const BugServerConfig &cfg )
  : mServerConfig( cfg )
{
  init();
}

void BugServer::init()
{
  mCache = new BugCache( identifier() );

  QString commandsFile = locateLocal( "appdata", identifier() + "commands" );
  mCommandsFile = new KSimpleConfig( commandsFile );

  QString bugzilla = mServerConfig.bugzillaVersion();

  if ( bugzilla == "KDE" ) mProcessor = new DomProcessor( this );
  else if ( bugzilla == "2.10" ) mProcessor = new HtmlParser_2_10( this );
  else if ( bugzilla == "2.14.2" ) mProcessor = new HtmlParser_2_14_2( this );
  else if ( bugzilla == "2.17.1" ) mProcessor = new HtmlParser_2_17_1( this );
  else mProcessor = new HtmlParser( this );

  loadCommands();
}

BugServer::~BugServer()
{
  saveCommands();

  delete mProcessor;
  delete mCommandsFile;
  delete mCache;
}

void BugServer::setServerConfig( const BugServerConfig &cfg )
{
  mServerConfig = cfg;
}

BugServerConfig &BugServer::serverConfig()
{
  return mServerConfig;
}

QString BugServer::identifier()
{
  QString id = mServerConfig.baseUrl().host();
  return id;
}

Processor *BugServer::processor() const
{
  return mProcessor;
}

KURL BugServer::packageListUrl()
{
  KURL url = mServerConfig.baseUrl();

  mProcessor->setPackageListQuery( url );

  return url;
}

KURL BugServer::bugListUrl( const Package &product, const QString &component )
{
  KURL url = mServerConfig.baseUrl();

  mProcessor->setBugListQuery( url, product, component );

  return url;
}

KURL BugServer::bugDetailsUrl( const Bug &bug )
{
  KURL url = mServerConfig.baseUrl();

  mProcessor->setBugDetailsQuery( url, bug );

  return url;
}

KURL BugServer::bugLink( const Bug &bug )
{
  KURL url = mServerConfig.baseUrl();

  url.setFileName( "show_bug.cgi" );
  url.setQuery( "id=" + bug.number() );

  kdDebug() << "URL: " << url.url() << endl;

  return url;
}

KURL BugServer::attachmentViewLink( const QString &id )
{
  KURL url = mServerConfig.baseUrl();

  url.setFileName( "attachment.cgi" );
  url.setQuery( "id=" + id + "&action=view" );

  return url;
}

KURL BugServer::attachmentEditLink( const QString &id )
{
  KURL url = mServerConfig.baseUrl();

  url.setFileName( "attachment.cgi" );
  url.setQuery( "id=" + id + "&action=edit" );

  return url;
}

Bug::Status BugServer::bugStatus( const QString &str )
{
  if ( str == "UNCONFIRMED" ) {
    return Bug::Unconfirmed;
  } else if ( str == "NEW" ) {
    return Bug::New;
  } else if ( str == "ASSIGNED" ) {
    return Bug::Assigned;
  } else if ( str == "REOPENED" ) {
    return Bug::Reopened;
  } else if ( str == "RESOLVED" ) {
    return Bug::Closed;
  } else if ( str == "VERIFIED" ) {
    return Bug::Closed;
  } else if ( str == "CLOSED" ) {
    return Bug::Closed;
  } else {
    return Bug::StatusUndefined;
  }
}

Bug::Severity BugServer::bugSeverity( const QString &str )
{
  if ( str == "critical" ) {
    return Bug::Critical;
  } else if ( str == "grave" ) {
    return Bug::Grave;
  } else if ( str == "major" ) {
    return Bug::Major;
  } else if ( str == "crash" ) {
    return Bug::Crash;
  } else if ( str == "normal" ) {
    return Bug::Normal;
  } else if ( str == "minor" ) {
    return Bug::Minor;
  } else if ( str == "wishlist" ) {
    return Bug::Wishlist;
  } else {
    return Bug::SeverityUndefined;
  }
}

void BugServer::readConfig( KConfig * /*config*/ )
{
}

void BugServer::writeConfig( KConfig * /*config*/ )
{
}

bool BugServer::queueCommand( BugCommand *cmd )
{
    // mCommands[bug] is a QPtrList. Get or create, set to autodelete, then append command.
    mCommands[cmd->bug().number()].setAutoDelete( true );
    QPtrListIterator<BugCommand> cmdIt( mCommands[cmd->bug().number()] );
    for ( ; cmdIt.current(); ++cmdIt )
        if ( cmdIt.current()->type() == cmd->type() )
            return false;
    mCommands[cmd->bug().number()].append( cmd );
    return true;
}

QPtrList<BugCommand> BugServer::queryCommands( const Bug &bug ) const
{
    CommandsMap::ConstIterator it = mCommands.find( bug.number() );
    if (it == mCommands.end()) return QPtrList<BugCommand>();
    else return *it;
}

bool BugServer::hasCommandsFor( const Bug &bug ) const
{
    CommandsMap::ConstIterator it = mCommands.find( bug.number() );
    return it != mCommands.end();
}

void BugServer::sendCommands( MailSender *mailer, const QString &senderName,
                              const QString &senderEmail, bool sendBCC,
                              const QString &recipient )
{
    // Disable mail commands for non-KDE servers
    if ( mServerConfig.baseUrl() != KURL( "http://bugs.kde.org" ) ) return;

    QString controlText;

    // For each bug that has commands.....
    CommandsMap::ConstIterator it;
    for(it = mCommands.begin(); it != mCommands.end(); ++it ) {
        Bug bug;
        Package pkg;
        // And for each command....
        QPtrListIterator<BugCommand> cmdIt( *it );
        for ( ; cmdIt.current() ; ++cmdIt ) {
            BugCommand* cmd = cmdIt.current();
            bug = cmd->bug();
            if (!cmd->package().isNull())
                pkg = cmd->package();
            if (!cmd->controlString().isNull()) {
                kdDebug() << "control@bugs.kde.org: " << cmd->controlString() << endl;
                controlText += cmd->controlString() + "\n";
            } else {
                kdDebug() << cmd->mailAddress() << ": " << cmd->mailText() << endl;
                // ### hm, should probably re-use the existing mailer instance and
                // implement message queueing for smtp
                MailSender *directMailer = mailer->clone();
#if 0
                connect( directMailer, SIGNAL( status( const QString & ) ),
                         this, SIGNAL( infoMessage( const QString & ) ) );
#endif
                if (!directMailer->send( senderName, senderEmail, cmd->mailAddress(),
                                    cmd->bug().title().prepend( "Re: " ),
                                    cmd->mailText(), sendBCC, recipient )) {
                    delete mailer;
                    return;
                }
            }
        }
        if (!bug.isNull()) {
            mCommandsFile->deleteGroup( bug.number(), true ); // done, remove command
            mCache->invalidateBugDetails( bug );
            if ( !pkg.isNull() ) {
                mCache->invalidateBugList( pkg, QString::null ); // the status of the bug comes from the buglist...

                QStringList::ConstIterator it2;
                for (it2 = pkg.components().begin();it2 != pkg.components().end();++it2) {
                    mCache->invalidateBugList( pkg, (*it2) ); // the status of the bug comes from the buglist...
                }
            }
        }
    }

    if (!controlText.isEmpty()) {
        kdDebug() << "control@bugs.kde.org doesn't work anymore" << endl;
#if 0        
        if ( !mailer->send( senderName, senderEmail, "control@bugs.kde.org",
                      i18n("Mail generated by KBugBuster"), controlText,
                      sendBCC, recipient ))
            return;
#endif
    } else {
        delete mailer;
    }

    mCommands.clear();
}

void BugServer::clearCommands( const QString &bug )
{
    mCommands.remove( bug );
    mCommandsFile->deleteGroup( bug, true );
}

bool BugServer::commandsPending() const
{
    if ( mCommands.count() > 0 ) return true;
    else return false;
}

QStringList BugServer::listCommands() const
{
    QStringList result;
    CommandsMap::ConstIterator it;
    for(it = mCommands.begin(); it != mCommands.end(); ++it ) {
        QPtrListIterator<BugCommand> cmdIt( *it );
        for ( ; cmdIt.current() ; ++cmdIt ) {
            BugCommand* cmd = cmdIt.current();
            if (!cmd->controlString().isNull())
                result.append( i18n("Control command: %1").arg(cmd->controlString()) );
            else
                result.append( i18n("Mail to %1").arg(cmd->mailAddress()) );
        }
    }
    return result;
}

QStringList BugServer::bugsWithCommands() const
{
    QStringList bugs;

    CommandsMap::ConstIterator it;
    for(it = mCommands.begin(); it != mCommands.end(); ++it ) {
        bugs.append( it.key() );
    }

    return bugs;
}

void BugServer::saveCommands() const
{
    CommandsMap::ConstIterator it;
    for(it = mCommands.begin(); it != mCommands.end(); ++it ) {
        mCommandsFile->setGroup( it.key() );
        QPtrListIterator<BugCommand> cmdIt( *it );
        for ( ; cmdIt.current() ; ++cmdIt ) {
            BugCommand* cmd = cmdIt.current();
            cmd->save( mCommandsFile );
        }
    }

    mCommandsFile->sync();
}

void BugServer::loadCommands()
{
    mCommands.clear();

    QStringList bugs = mCommandsFile->groupList();
    QStringList::ConstIterator it;
    for( it = bugs.begin(); it != bugs.end(); ++it ) {
        mCommandsFile->setGroup( *it );
        QMap<QString, QString> entries = mCommandsFile->entryMap ( *it );
        QMap<QString, QString>::ConstIterator it;
        for( it = entries.begin(); it != entries.end(); ++it ) {
            QString type = it.key();
            BugCommand *cmd = BugCommand::load( mCommandsFile, type );
            if ( cmd ) {
                mCommands[cmd->bug().number()].setAutoDelete(true);
                mCommands[cmd->bug().number()].append(cmd);
            }
        }
    }
}

void BugServer::setPackages( const Package::List &packages )
{
  mPackages = packages;
}

const Package::List &BugServer::packages() const
{
  return mPackages;
}

void BugServer::setBugs( const Package &pkg, const QString &component,
                         const Bug::List &bugs )
{
  QPair<Package, QString> pkg_key = QPair<Package, QString>(pkg, component);
  mBugs[ pkg_key ] = bugs;
}

const Bug::List &BugServer::bugs( const Package &pkg, const QString &component )
{
  QPair<Package, QString> pkg_key = QPair<Package, QString>(pkg, component);
  return mBugs[ pkg_key ];
}

void BugServer::setBugDetails( const Bug &bug, const BugDetails &details )
{
  mBugDetails[ bug ] = details;
}

const BugDetails &BugServer::bugDetails( const Bug &bug )
{
  return mBugDetails[ bug ];
}

/*
 * vim:sw=4:ts=4:et
 */
