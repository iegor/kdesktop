
#include "package.h"

#include "packageimpl.h"

Package::Package()
{
}

Package::Package( PackageImpl *impl )
    : m_impl( impl )
{
}

Package::Package( const Package &other )
{
    (*this) = other;
}

Package &Package::operator=( const Package &rhs )
{
    m_impl = rhs.m_impl;
    return *this;
}

Package::~Package()
{
}

QString Package::name() const
{
    if ( !m_impl )
        return QString::null;
    
    return m_impl->name;
}

QString Package::description() const
{
    if ( !m_impl )
        return QString::null;
    
    return m_impl->description;
}

uint Package::numberOfBugs() const
{
    if ( !m_impl )
        return 0;

    return m_impl->numberOfBugs;
}

Person Package::maintainer() const
{
    if ( !m_impl )
        return Person();

    return m_impl->maintainer;
}

const QStringList Package::components() const
{
    if ( !m_impl )
        return QStringList();
    
    return m_impl->components;
}

bool Package::operator==( const Package &rhs )
{
    return m_impl == rhs.m_impl;
}

bool Package::operator<( const Package &rhs ) const
{
    return m_impl < rhs.m_impl;
}

/**
 * vim:ts=4:sw=4:et
 */
