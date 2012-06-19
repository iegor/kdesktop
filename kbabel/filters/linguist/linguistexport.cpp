/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
                2001-2002 by Stanislav Visnovsky
                            <visnovsky@kde.org>
                2002-2003 by Marco Wegner 
                            <mail@marcowegner.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */


#include <qfile.h>
#include "qregexp.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <kgenericfactory.h>

#include "catalog.h"
#include "catalogitem.h"
#include "catalogsettings.h"
#include "linguistexport.h"

K_EXPORT_COMPONENT_FACTORY( kbabel_linguistexport, KGenericFactory<LinguistExportPlugin> ( "kbabellinguistexportfilter" ) )

using namespace KBabel;

LinguistExportPlugin::LinguistExportPlugin( QObject * parent, const char * name, const QStringList& )
  : CatalogExportPlugin( parent, name )
{
}

ConversionStatus LinguistExportPlugin::save( const QString& filename, const QString&, const Catalog * catalog )
{
  // Check whether we know how to handle the extra data.
  if ( catalog->importPluginID( ) != "Qt translation source" )
    return UNSUPPORTED_TYPE;
  
  QFile file( filename );
  if ( !file.open( IO_WriteOnly ) )
    return OS_ERROR;
    
  SaveSettings settings = catalog->saveSettings( );

  // New DOM document.  
  QDomDocument doc( "TS" );
  // Create the root element.
  doc.appendChild( doc.createElement( "TS" ) );
  
  // Create a default context just in case none is given in the messages.
  context = "Default"; // ### FIXME: Qt's default seems named "@default"

  bool fuzzy;
  
  // Regular messages.
  for ( uint i = 0; i < catalog->numberOfEntries( ); i++ ) {
    QString comment( extractComment( doc, catalog->comment( i ), fuzzy ) );
    createMessage( doc, catalog->msgid( i ).join( "" ), catalog->msgstr( i ).join( "" ), 
                   comment, false, fuzzy );
  }
  
  // Obsolete messages.
  if ( settings.saveObsolete ) {
    QValueList<CatalogItem> obsMessages = catalog->obsoleteEntries( );
    QValueList<CatalogItem>::Iterator it;
    for ( it = obsMessages.begin( ); it != obsMessages.end( ); ++it ) {
      QString comment( extractComment( doc, (*it).comment( ), fuzzy ) );
      createMessage( doc, (*it).msgid( true ).join( "" ), (*it).msgstr( true ).join( "" ), 
                     comment, true, fuzzy );
    }
  }
  
  QTextStream stream( &file );
  doc.save( stream, 2 );
  file.close( );
  
  return OK;
}

const QString LinguistExportPlugin::extractComment( QDomDocument& doc, const QString& s, bool& fuzzy )
{
  fuzzy = false;
  QString comment( s );
  if ( !comment.isEmpty( ) ) {
    // Extract the context and the actual comment.
    comment.remove( QRegExp( "^Context:[\\s]*" ) );
    /*
     * HACK
     *
     * KBabel has not any flag to tell "this entry is fuzzy!"
     * but it uses the corresponding Gettext comment instead.
     *
     * Therefore we have little choice, but to use "#, fuzzy" in the comment,
     * even if it is very Linguist-unlike. So the "#, fuzzy" must be removed before
     * writing the comment for Linguist
     */

    int pos = comment.find("#, fuzzy");
    if ( pos >= 0) {
        fuzzy = true;
        comment.remove("#, fuzzy");
    }
    
    QString newContext;
    pos = comment.find( '\n' );
    if ( pos >= 0 ) {
      newContext = comment.left( pos );
      comment.replace( 0, pos + 1, "" ); // ### TODO: use QString::remove
    } else {
      newContext = comment;
      comment = ""; // ### TODO: use QString() instead of ""
    }
    setContext( doc, newContext );
  }
  return comment;
}

void LinguistExportPlugin::createMessage( QDomDocument& doc, const QString& msgid, 
                                          const QString& msgstr, const QString& comment,
                                          const bool obsolete, const bool fuzzy )
{
    QDomElement elem;
    QDomText text;
    
    QDomElement messageElement = doc.createElement( "message" );
    
    elem = doc.createElement( "source" );
    text = doc.createTextNode( msgid );
    elem.appendChild( text );
    messageElement.appendChild( elem );
    
    if ( !comment.isEmpty( ) ) {
      elem = doc.createElement( "comment" );
      text = doc.createTextNode( comment );
      elem.appendChild( text );
      messageElement.appendChild( elem );
    }
    
    elem = doc.createElement( "translation" );
    if ( obsolete )
      elem.setAttribute( "type", "obsolete" );
    else if ( msgstr.isEmpty( ) || fuzzy ) {
      elem.setAttribute( "type", "unfinished" );
    }
    if ( !msgstr.isEmpty())
    {
        text = doc.createTextNode( msgstr );
        elem.appendChild( text );
    }
    messageElement.appendChild( elem );
    contextElement.appendChild( messageElement );
}

void LinguistExportPlugin::setContext( QDomDocument& doc, QString newContext )
{
  // Nothing to do here.
  if ( newContext == context )
    return;
    
  // Find out whether there is already such a context in the QDomDocument.
  QDomNode node = doc.documentElement( ).firstChild( );
  while ( !node.isNull( ) ) {
    if ( node.isElement( ) ) {
      QDomElement elem = node.firstChild( ).toElement( );
      if ( elem.isElement( ) && elem.tagName( ) == "name" && elem.text( ) == newContext ) {
        // We found the context.
        context = newContext;
        contextElement = node.toElement( );
        // Nothing more to do.
        return;
      }
    }
    node = node.nextSibling( );
  }
  
  // Create new context element.
  contextElement = doc.createElement( "context" );
  doc.documentElement( ).appendChild( contextElement );
  // Appropriate name element.
  QDomElement nameElement = doc.createElement( "name" );
  QDomText text = doc.createTextNode( newContext );
  nameElement.appendChild( text );
  contextElement.appendChild( nameElement );
  // Store new context.
  context = newContext;
}
