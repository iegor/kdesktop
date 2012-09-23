#ifndef __bug_impl_h__
#define __bug_impl_h__

#include "person.h"
#include "bug.h"

#include <kurl.h>
#include <ksharedptr.h>

struct BugImpl : public KShared
{
public:
    BugImpl( const QString &_title, const Person &_submitter, QString _number,
        uint _age, Bug::Severity _severity, Person _developerTODO,
        Bug::Status _status, const Bug::BugMergeList& _mergedWith  )
    : age( _age ), title( _title ), submitter( _submitter ), number( _number ),
        severity( _severity ), developerTODO( _developerTODO ),
        status( _status ), mergedWith( _mergedWith )
    {
    }

    uint age;
    QString title;
    Person submitter;
    QString number;
    Bug::Severity severity;
    Person developerTODO;
    Bug::Status status;

    Bug::BugMergeList mergedWith;
};

#endif

// vim: set sw=4 ts=4 sts=4 et:

