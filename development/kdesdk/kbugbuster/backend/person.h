#ifndef __person_h__
#define __person_h__

#include <qstring.h>
#include <klocale.h>

struct Person
{
    Person() {}
    Person( const QString &fullName );
    Person( const QString &_name, const QString &_email )
        : name( _name ), email( _email ) {}

    QString name;
    QString email;

    QString fullName( bool html = false ) const;

    static Person parseFromString( const QString &str );
};

#endif

/*
 * vim:sw=4:ts=4:et
 */
