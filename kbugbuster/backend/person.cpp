#include <kdebug.h>

#include "person.h"

Person::Person( const QString &fullName )
{
    int emailPos = fullName.find( '<' );
    if ( emailPos < 0 ) {
        email = fullName;
    } else {
        email = fullName.mid( emailPos + 1, fullName.length() - 1 );
        name = fullName.left( emailPos - 1 );
    }
}

QString Person::fullName(bool html) const
{
    if( name.isEmpty() )
    {
        if( email.isEmpty() )
            return i18n( "Unknown" );
        else
            return email;
    }
    else
    {
        if( email.isEmpty() )
            return name;
        else
            if ( html ) {
                return name + " &lt;" + email + "&gt;";
            } else {
                return name + " <" + email + ">";
            }
    }
}

Person Person::parseFromString( const QString &_str )
{
    Person res;

    QString str = _str;

    int ltPos = str.find( '<' );
    if ( ltPos != -1 )
    {
        int gtPos = str.find( '>', ltPos );
        if ( gtPos != -1 )
        {
            res.name = str.left( ltPos - 1 );
            str = str.mid( ltPos + 1, gtPos - ( ltPos + 1 ) );
        }
    }
   
    int atPos = str.find( '@' );
    int spacedAtPos = str.find( QString::fromLatin1( " at " ) );
    if ( atPos == -1 && spacedAtPos != -1 )
        str.replace( spacedAtPos, 4, QString::fromLatin1( "@" ) );

    int spacePos = str.find( ' ' );
    while ( spacePos != -1 )
    {
        str[ spacePos ] = '.'; 
        spacePos = str.find( ' ', spacePos );
    }
  
    res.email = str;

    return res;
}

/**
 * vim:et:ts=4:sw=4
 */
