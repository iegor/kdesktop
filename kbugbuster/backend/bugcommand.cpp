#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>

#include "bugcommand.h"

QString BugCommand::name()
{
    return i18n("Unknown");
}

QString BugCommand::details()
{
    return QString::null;
}

BugCommand *BugCommand::load( KConfig *config, const QString &type )
{
    QString bugNumber = config->group();
    // ### this sucks. we better let Bug implement proper persistance,
    // because this way of instantiating a bug object doesn't bring back
    // properties like title, package, etc. (Simon)
    Bug bug = Bug::fromNumber( bugNumber );
    Package pkg = Package(); // hack

    if ( type == "Close" ) {
        return new BugCommandClose( bug, config->readEntry( type ), pkg );
    } else if ( type == "Reopen" ) {
        return new BugCommandReopen( bug, pkg );
    } else if ( type == "Merge" ) {
        return new BugCommandMerge( config->readListEntry( type ), pkg );
    } else if ( type == "Unmerge" ) {
        return new BugCommandUnmerge( bug, pkg );
    } else if ( type == "Reassign" ) {
        return new BugCommandReassign( bug, config->readEntry( type ), pkg );
    } else if ( type == "Retitle" ) {
        return new BugCommandRetitle( bug, config->readEntry( type ), pkg );
    } else if ( type == "Severity" ) {
        return new BugCommandSeverity( bug, config->readEntry( type ), pkg );
    } else if ( type == "Reply" ) {
        return new BugCommandReply( bug, config->readEntry( type ), config->readNumEntry("Recipient",Normal) );
    } else if ( type == "ReplyPrivate" ) {
        QStringList args = config->readListEntry( type );
        if ( args.count() != 2 ) return 0;
        return new BugCommandReplyPrivate( bug, *(args.at(0)), *(args.at(1)) );
    } else {
      kdDebug() << "Warning! Unknown bug command '" << type << "'" << endl;
      return 0;
    }
}

///////////////////// Close /////////////////////

QString BugCommandClose::controlString() const
{
    if (m_message.isEmpty()) {
        return "close " + m_bug.number();
    } else {
        return QString::null;
    }
}

QString BugCommandClose::mailAddress() const
{
    kdDebug() << "BugCommandClose::mailAddress(): number: " << m_bug.number() << endl;

    if (m_message.isEmpty()) {
        return QString::null;
    } else {
        return m_bug.number() + "-done@bugs.kde.org";
    }
}

QString BugCommandClose::mailText() const
{
    if (m_message.isEmpty()) {
        return QString::null;
    } else {
        return m_message;
    }
}

QString BugCommandClose::name()
{
    return i18n("Close");
}

QString BugCommandClose::details() const
{
    return m_message;
}

void BugCommandClose::save( KConfig *config )
{
    config->writeEntry( "Close",m_message );
}

///////////////////// Close Silently /////////////////////

QString BugCommandCloseSilently::controlString() const
{
    return "done " + m_bug.number();
}

QString BugCommandCloseSilently::name()
{
    return i18n("Close Silently");
}

void BugCommandCloseSilently::save( KConfig *config )
{
    config->writeEntry( "CloseSilently", true );
}

///////////////////// Reopen /////////////////////

QString BugCommandReopen::controlString() const
{
    return "reopen " + m_bug.number();
}

QString BugCommandReopen::name()
{
    return i18n("Reopen");
}

void BugCommandReopen::save( KConfig *config )
{
    config->writeEntry( "Reopen", true );
}

///////////////////// Retitle /////////////////////

QString BugCommandRetitle::controlString() const
{
    return "retitle " + m_bug.number() + " " + m_title;
}

QString BugCommandRetitle::name()
{
    return i18n("Retitle");
}

QString BugCommandRetitle::details() const
{
    return m_title;
}

void BugCommandRetitle::save( KConfig *config )
{
    config->writeEntry( "Retitle", m_title );
}

///////////////////// Merge /////////////////////

QString BugCommandMerge::controlString() const
{
    return "merge " + m_bugNumbers.join(" ");
}

QString BugCommandMerge::name()
{
    return i18n("Merge");
}

QString BugCommandMerge::details() const
{
    return m_bugNumbers.join(", ");
}

void BugCommandMerge::save( KConfig *config )
{
    config->writeEntry( "Merge", m_bugNumbers );
}

///////////////////// Unmerge /////////////////////

QString BugCommandUnmerge::controlString() const
{
    return "unmerge " + m_bug.number();
}

QString BugCommandUnmerge::name()
{
    return i18n("Unmerge");
}

void BugCommandUnmerge::save( KConfig *config )
{
    config->writeEntry( "Unmerge", true );
}

///////////////////// Reply /////////////////////

QString BugCommandReply::mailAddress() const
{
    return m_bug.number() + "@bugs.kde.org";
#if 0
    switch ( m_recipient ) {
      case Normal:
        return m_bug.number() + "@bugs.kde.org";
      case Maintonly:
        return m_bug.number() + "-maintonly@bugs.kde.org";
      case Quiet:
        return m_bug.number() + "-quiet@bugs.kde.org";
    }
    return QString::null;
#endif
}

QString BugCommandReply::mailText() const
{
    return m_message;
}

QString BugCommandReply::name()
{
    return i18n("Reply");
#if 0
    switch ( m_recipient ) {
      case Normal:
        return i18n("Reply");
      case Maintonly:
        return i18n("Reply (Maintonly)");
      case Quiet:
        return i18n("Reply (Quiet)");
    }
    return QString::null;
#endif
}

QString BugCommandReply::details() const
{
    return m_message;
}

void BugCommandReply::save( KConfig *config )
{
    config->writeEntry( "Reply", m_message );
#if 0
    config->writeEntry( "Recipient", m_recipient );
#endif
}

///////////////////// Reply Private /////////////////////

QString BugCommandReplyPrivate::mailAddress() const
{
    return m_address;
}

QString BugCommandReplyPrivate::mailText() const
{
    return m_message;
}

QString BugCommandReplyPrivate::name()
{
    return i18n("Private Reply");
}

QString BugCommandReplyPrivate::details() const
{
    return m_message;
}

void BugCommandReplyPrivate::save( KConfig *config )
{
    QStringList args;
    args << m_address;
    args << m_message;
    config->writeEntry( "ReplyPrivate", args );
}

///////////////////// Severity /////////////////////

QString BugCommandSeverity::controlString() const
{
    return "severity " + m_bug.number() + " " + m_severity.lower();
}

QString BugCommandSeverity::name()
{
    return i18n("Severity");
}

QString BugCommandSeverity::details() const
{
    return m_severity;
}

void BugCommandSeverity::save( KConfig *config )
{
    config->writeEntry( "Severity", m_severity );
}

///////////////////// Reassign /////////////////////

QString BugCommandReassign::controlString() const
{
    return "reassign " + m_bug.number() + " " + m_package;
}

QString BugCommandReassign::name()
{
    return i18n("Reassign");
}

QString BugCommandReassign::details() const
{
    return m_package;
}

void BugCommandReassign::save( KConfig *config )
{
    config->writeEntry( "Reassign", m_package );
}
