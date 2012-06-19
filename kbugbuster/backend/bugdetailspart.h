#ifndef BUGDETAILSPART_H
#define BUGDETAILSPART_H

#include <qvaluelist.h>
#include <qdatetime.h>

struct BugDetailsPart
{
    typedef QValueList<BugDetailsPart> List;

    BugDetailsPart () {}
    BugDetailsPart( const Person &_sender, const QDateTime &_date,
                    const QString &_text )
        : sender( _sender ), date( _date ), text( _text ) {}
    
    Person sender;
    QDateTime date;
    QString text;
};

#endif
