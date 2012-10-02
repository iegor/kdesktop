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
#ifndef BUGSERVER_H
#define BUGSERVER_H

#include <qstring.h>

#include <kurl.h>

#include "bug.h"
#include "package.h"
#include "bugsystem.h"
#include "bugserverconfig.h"

class Processor;
class BugCache;
class MailSender;
class BugServerConfig;

class BugServer
{
  public:
    BugServer();
    BugServer( const BugServerConfig & );
    ~BugServer();

    /**
      BugServer takes ownership of the BugServerConfig object.
    */
    void setServerConfig( const BugServerConfig & );
    BugServerConfig &serverConfig();

    QString identifier();

    BugCache *cache() const { return mCache; }

    KURL packageListUrl();

    KURL bugListUrl( const Package &, const QString &component );

    KURL bugDetailsUrl( const Bug & );

    KURL bugLink( const Bug & );
    KURL attachmentViewLink( const QString &id );
    KURL attachmentEditLink( const QString &id );
    
    Bug::Status bugStatus( const QString & );

    Bug::Severity bugSeverity( const QString & );

    Processor *processor() const;

    void readConfig( KConfig * );
    
    void writeConfig( KConfig * );

    /**
      Queue a new command.
    */
    bool queueCommand( BugCommand * );
    /**
      Return all the commands for a given bug.
    */
    QPtrList<BugCommand> queryCommands( const Bug & ) const;
    /**
      Return true if we have a least one command for this bug.
    */
    bool hasCommandsFor( const Bug &bug ) const;
    /**
      Send all commands (generate the mails).
    */
    void sendCommands( MailSender *, const QString &senderName,
                       const QString &senderEmail, bool sendBCC,
                       const QString &recipient );
    /**
      Forget all commands for a given bug.
    */
    void clearCommands( const QString &bug );
    /**
      Return true if any command has been created.
    */
    bool commandsPending() const;
    /**
      List all pending commands.
    */
    QStringList listCommands() const;
    /**
      Return numbers of all bugs having at least one command queued.
    */
    QStringList bugsWithCommands() const;

    void saveCommands() const;
    void loadCommands();

    void setPackages( const Package::List & );
    const Package::List &packages() const;

    void setBugs( const Package &, const QString &component,
                  const Bug::List & );
    const Bug::List &bugs( const Package &, const QString &component );

    void setBugDetails( const Bug &, const BugDetails & );
    const BugDetails &bugDetails( const Bug & );

  private:
    void init();

    BugServerConfig mServerConfig;
    
    Processor *mProcessor;

    BugCache *mCache;

    Package::List mPackages;
    // Map package -> list of bugs
    typedef QMap< QPair<Package, QString>, Bug::List > BugListMap;
    BugListMap mBugs;
    // Map bug -> bug details (i.e. contents of the report)
    typedef QMap< Bug, BugDetails > BugDetailsMap;
    BugDetailsMap mBugDetails;
    // Map bug-number -> list of commands
    typedef QMap< QString, QPtrList<BugCommand> > CommandsMap;
    CommandsMap mCommands;

    KSimpleConfig *mCommandsFile;
};

#endif

/*
 * vim:sw=4:ts=4:et
 */
