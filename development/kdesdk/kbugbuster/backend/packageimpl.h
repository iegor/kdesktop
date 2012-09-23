#ifndef __packageimpl_h__
#define __packageimpl_h__

#include "person.h"

#include <qstringlist.h>
#include <kurl.h>
#include <ksharedptr.h>

struct PackageImpl : public KShared
{
public:
    PackageImpl( const QString &_name, const QString &_description, 
                 uint _numberOfBugs,  const Person &_maintainer,
                 const QStringList &_components )
        : name( _name ), description( _description ),numberOfBugs( _numberOfBugs ),
          maintainer( _maintainer ), components(_components)
    {}
    
    QString name;
    QString description;
    uint numberOfBugs;
    Person maintainer;
    QStringList components;
};

#endif

/*
 * vim:sw=4:ts=4:et
 */
