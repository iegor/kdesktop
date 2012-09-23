
#include "bug.h"

#include "bugimpl.h"

#include <assert.h>
#include <kdebug.h>

Bug::Bug()
: m_impl( NULL )
{
}

Bug::Bug( BugImpl *impl )
: m_impl( impl )
{
}

Bug::Bug( const Bug &other )
{
    (*this) = other;
}

Bug Bug::fromNumber( const QString &bugNumber )
{
    return new BugImpl( QString::null, Person(), bugNumber, 0xFFFFFFFF, Normal, Person(),
                        Unconfirmed, Bug::BugMergeList() );
}

Bug &Bug::operator=( const Bug &rhs )
{
    m_impl = rhs.m_impl;
    return *this;
}

Bug::~Bug()
{
}

QString Bug::severityLabel( Bug::Severity s )
{
    switch ( s )
    {
        case Critical: return i18n("Critical");
        case Grave: return i18n("Grave");
        case Major: return i18n("Major");
        case Crash: return i18n("Crash");
        case Normal: return i18n("Normal");
        case Minor: return i18n("Minor");
        case Wishlist: return i18n("Wishlist");
        default:
        case SeverityUndefined: return i18n("Undefined");
    }
}

QString Bug::severityToString( Bug::Severity s )
{
  switch ( s ) {
    case Critical: return QString::fromLatin1( "critical" );
    case Grave: return QString::fromLatin1( "grave" );
    case Major: return QString::fromLatin1( "major" );
    case Crash: return QString::fromLatin1( "crash" );
    case Normal: return QString::fromLatin1( "normal" );
    case Minor: return QString::fromLatin1( "minor" );
    case Wishlist: return QString::fromLatin1( "wishlist" );
    default:
      kdWarning() << "Bug::severityToString invalid severity " << s << endl;
      return QString::fromLatin1( "<invalid>" );
   }
}

Bug::Severity Bug::stringToSeverity( const QString &s, bool *ok )
{
   if ( ok )
      *ok = true;

   if ( s == "critical" ) return Critical;
   else if ( s == "grave" ) return Grave;
   else if ( s == "major" ) return Major;
   else if ( s == "crash" || s == "drkonqi" ) return Crash;
   else if ( s == "normal" ) return Normal;
   else if ( s == "minor" ) return Minor;
   else if ( s == "wishlist" ) return Wishlist;

   kdWarning() << "Bug::stringToSeverity: invalid severity: " << s << endl;
   if ( ok )
       *ok = false;
   return SeverityUndefined;
}

QValueList<Bug::Severity> Bug::severities()
{
    QValueList<Severity> s;
    s << Critical << Grave << Major << Crash << Normal << Minor << Wishlist;
    return s;
}

QString Bug::statusLabel( Bug::Status s )
{
    switch ( s )
    {
        case Unconfirmed: return i18n("Unconfirmed");
        case New: return i18n("New");
        case Assigned: return i18n("Assigned");
        case Reopened: return i18n("Reopened");
        case Closed: return i18n("Closed");
        default:
        case StatusUndefined: return i18n("Undefined");
    }
}

QString Bug::statusToString( Bug::Status s )
{
  switch ( s ) {
    case Unconfirmed: return QString::fromLatin1( "unconfirmed" );
    case New: return QString::fromLatin1( "new" );
    case Assigned: return QString::fromLatin1( "assigned" );
    case Reopened: return QString::fromLatin1( "reopened" );
    case Closed: return QString::fromLatin1( "closed" );
    default:
      kdWarning() << "Bug::statusToString invalid status " << s << endl;
      return QString::fromLatin1( "<invalid>" );
   }
}

Bug::Status Bug::stringToStatus( const QString &s, bool *ok )
{
   if ( ok )
      *ok = true;

   if ( s == "unconfirmed" ) return Unconfirmed;
   else if ( s == "new" ) return New;
   else if ( s == "assigned" ) return Assigned;
   else if ( s == "reopened" ) return Reopened;
   else if ( s == "closed" ) return Closed;

   kdWarning() << "Bug::stringToStatus: invalid status: " << s << endl;
   if ( ok )
       *ok = false;
   return StatusUndefined;
}

QString Bug::title() const
{
    if ( !m_impl )
        return QString::null;

    return m_impl->title;
}

void Bug::setTitle( QString title)
{
    if ( m_impl )
        m_impl->title = title;
}

uint Bug::age() const
{
    if ( !m_impl )
        return 0;

    return m_impl->age;
}

void Bug::setAge( uint age )
{
    if ( m_impl )
        m_impl->age = age;
}

struct Person Bug::submitter() const
{
    if ( !m_impl )
        return Person( QString::null, QString::null );

    return m_impl->submitter;
}

QString Bug::number() const
{
    if ( !m_impl )
        return QString::null;

    return m_impl->number;
}

Bug::Severity Bug::severity() const
{
    if ( !m_impl )
        return Normal;

    return m_impl->severity;
}

void Bug::setSeverity( Bug::Severity severity )
{
    if ( m_impl )
        m_impl->severity = severity;
}

Bug::BugMergeList Bug::mergedWith() const
{
    if ( !m_impl )
        return Bug::BugMergeList();

    return m_impl->mergedWith;
}


Bug::Status Bug::status() const
{
    if ( !m_impl )
        return StatusUndefined;

    return m_impl->status;
}

QString Bug::severityAsString() const
{
    return severityToString( severity() );
}

Person Bug::developerTODO() const
{
  return (m_impl == NULL) ? Person( QString::null, QString::null ) :
                            m_impl->developerTODO;
}

bool Bug::operator==( const Bug &rhs )
{
    return m_impl == rhs.m_impl;
}

bool Bug::operator<( const Bug &rhs ) const
{
    return m_impl < rhs.m_impl;
}

/* vim: set ts=4 sw=4 et softtabstop=4: */

