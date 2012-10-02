#ifndef __bug_h__
#define __bug_h__

#include "person.h"

#include <qvaluelist.h>

#include <ksharedptr.h>

class BugImpl;

class Bug
{
public:
    typedef QValueList<Bug> List;
    typedef QValueList<int> BugMergeList;

    enum Severity { SeverityUndefined, Critical, Grave, Major, Crash, Normal,
                    Minor, Wishlist };
    enum Status { StatusUndefined, Unconfirmed, New, Assigned, Reopened,
                  Closed };

    Bug();
    Bug( BugImpl *impl );
    Bug( const Bug &other );
    Bug &operator=( const Bug &rhs );
    ~Bug();

    static QString severityLabel( Severity s );
    /**
      Return string representation of severity. This function is symmetric to
      stringToSeverity().
    */
    static QString severityToString( Severity s );
    /**
      Return severity code of string representation. This function is symmetric
      to severityToString().
    */
    static Severity stringToSeverity( const QString &, bool *ok = 0 );

    static QValueList<Severity> severities();

    uint age() const;
    void setAge( uint days );

    QString title() const;
    void setTitle( QString title );
    Person submitter() const;
    QString number() const;
    Severity severity() const;
    void setSeverity( Severity severity );
    QString severityAsString() const;
    Person developerTODO() const;

    BugMergeList mergedWith() const;

    /**
     * Status of a bug. Currently open or closed.
     * TODO: Should we add a status 'deleted' here ?
     */
    Status status() const;
    void setStatus( Status newStatus );

    static QString statusLabel( Status s );
    /**
      Return string representation of status. This function is symmetric to
      stringToStatus().
    */
    static QString statusToString( Status s );
    /**
      Return status code of string representation. This function is symmetric
      to statusToString().
    */
    static Status stringToStatus( const QString &, bool *ok = 0 );

    bool operator==( const Bug &rhs );
    bool operator<( const Bug &rhs ) const;

    bool isNull() const { return m_impl == 0; }

    static Bug fromNumber( const QString &bugNumber );

private:
    BugImpl *impl() const { return m_impl; }

    KSharedPtr<BugImpl> m_impl;
};

#endif

/* vim: set sw=4 ts=4 et softtabstop=4: */

