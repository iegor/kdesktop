#ifndef __package_h__
#define __package_h__

#include "person.h"

#include <qvaluelist.h>

#include <ksharedptr.h>

class PackageImpl;

class Package
{
public:
    typedef QValueList<Package> List;

    Package();
    Package( PackageImpl *impl );
    Package( const Package &other );
    Package &operator=( const Package &rhs );
    ~Package();

    QString name() const;
    QString description() const;
    uint numberOfBugs() const;
    Person maintainer() const;
    const QStringList components() const;

    bool isNull() const { return m_impl == 0; }

    PackageImpl *impl() const { return m_impl; }

    bool operator==( const Package &rhs );
    bool operator<( const Package &rhs ) const;

private:
    KSharedPtr<PackageImpl> m_impl;
};

#endif

/* vim: set sw=4 ts=4 et softtabstop=4: */

