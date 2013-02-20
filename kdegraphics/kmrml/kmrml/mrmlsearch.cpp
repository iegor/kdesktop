/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// This little baby is called from Konqueror's popupmenu, when you hit
// "Search for similar images...". This program simply gets the URLs
// from Konqueror and creates a query of the form
// mrml://host.com/?relevant=url1;url2;url3;url4....
// By default, the mrml URL is mrml://localhost", but you can override that
// by editing ~/.kde/share/config/kio_mrmlrc and adding
// [MRML Settings]
// Default URL=mrml://url.to.your.giftserver.com
//
// mrmlsearch will then invoke "kfmclient openURL query" to start open
// a new Konqueror window and perform the query.

#include <unistd.h>

#include <qfile.h>
#include <qstring.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kinstance.h>
#include <kurl.h>

#include <kmrml_config.h>

extern "C" KDE_EXPORT int kdemain( int argc, char **argv )
{
    QString query;

    for ( int i = 1; i < argc; i++ ) {
        if ( i > 1 )
            query += ';';
        QString path = QFile::decodeName( argv[i] );
        if ( path.at( 0 ) == '/' ) {
            KURL u;
            u.setPath( path );
            path = u.url();
        }
        query.append( path );
    }

    KInstance instance( "kio_mrml" );

    KMrml::Config config( instance.config() );
    KMrml::ServerSettings settings = config.defaultSettings();
    KURL url;
    url.setProtocol( "mrml" );
    url.setHost( settings.host );

    query = KURL::encode_string_no_slash( query );
    query.prepend( "?relevant=" ); // this is not encoded!
    url.setQuery( query );
    qDebug("***** Query: %s ** URL: %s", query.latin1(), url.url().latin1());

    return execlp( "kfmclient",
                   "kfmclient", "openURL", QFile::encodeName(url.url()).data(),
                   "text/mrml", (void *)0 );
}
