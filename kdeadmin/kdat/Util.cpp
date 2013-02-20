// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <qstringlist.h>
#include <kglobal.h>
#include <klocale.h>

#include "Util.h"

QString Util::bytesToString( uint bytes )
{
    return KGlobal::locale()->formatNumber(bytes, 0);
}

QString Util::kbytesToString( uint kbytes )
{
    return KGlobal::locale()->formatNumber(kbytes, 0) + 'k';
}

QString Util::longestCommonPath( const QStringList& files )
{
    QStringList filesTmp = files;
    QStringList::Iterator i = filesTmp.begin();

    uint minLen = (*i).length();
    for ( ; i != filesTmp.end(); ++i ) {
        if ( minLen > (*i).length() ) {
            minLen = (*i).length();
        }
    }
    uint j;
    for ( j = 0; j < minLen; j++ ) {
        i = filesTmp.begin();
        QString first = *i;
        for ( ; i != filesTmp.end(); ++i ) {
            if ( first.left(j) != (*i).left(j) ) {
                // Prefixes are NOT the same.
                break;
            }
        }
        if ( i != filesTmp.end() ) {
            // The common prefix is 0 to j-1, inclusive.
            break;
        }
    }
    i = filesTmp.begin();
    QString prefix = *i;
    int idx = prefix.findRev( '/', j );
    if ( idx > -1 ) {
        prefix = prefix.left( prefix.findRev( '/', j ) );
    } else {
        prefix = "";
    }

    return prefix;
}
