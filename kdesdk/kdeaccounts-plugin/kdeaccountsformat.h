/****************************************************************************
** $Id: kdeaccountsformat.h 174706 2002-08-30 22:44:18Z pfeiffer $
**
** Created : 2001
**
** Copyright (C) 2001 Carsten Pfeiffer <pfeiffer@kde.org>
**
****************************************************************************/

#ifndef KDEACCOUNTSPARSER_H
#define KDEACCOUNTSPARSER_H

#include <kabc/formatplugin.h>

namespace KABC {
    class AddressBook;
}

class KDEAccountsFormat : public KABC::FormatPlugin
{
public:
    KDEAccountsFormat() {}
    ~KDEAccountsFormat() {}

    virtual bool loadAll( KABC::AddressBook *, 
                          KABC::Resource *resource, QFile *file );

    virtual bool load( KABC::Addressee&, QFile *)
    {
        qDebug("*** KDE Accounts format: load single entry not supported.");
        return false;
    }
    virtual void save( const KABC::Addressee&, QFile *)
    {
        qDebug("*** KDE Accounts format: save not supported.");
    }
    virtual void saveAll( KABC::AddressBook *, KABC::Resource *, QFile *)
    {
        qDebug("*** KDE Accounts format: save not supported.");
    }
    virtual bool checkFormat( QFile *file ) const
    {
        if ( file->name().endsWith( "/accounts" ) )
            return true; // lame, but works for me :)

        return false;
    }

};

#endif // KDEACCOUNTSPARSER_H
