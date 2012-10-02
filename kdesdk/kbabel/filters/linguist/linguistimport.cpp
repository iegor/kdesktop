// kate: space-indent on; indent-width 2; replace-tabs on;

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

// Qt include files
#include <qfile.h>
#include <qfileinfo.h>
#include <qobject.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
// KDE include files
#include <kdebug.h>
#include <kgenericfactory.h>
// Project specific include files
#include "catalogitem.h"
#include "linguistimport.h"

K_EXPORT_COMPONENT_FACTORY( kbabel_linguistimport, KGenericFactory<LinguistImportPlugin> ( "kbabellinguistimportfilter" ) )

using namespace KBabel;

LinguistImportPlugin::LinguistImportPlugin( QObject * parent, const char * name, const QStringList& )
  : CatalogImportPlugin( parent, name )
{
}

ConversionStatus LinguistImportPlugin::load( const QString& filename, const QString& )
{
  if ( filename.isEmpty( ) ) {
    kdDebug( ) << "fatal error: empty filename to open" << endl;
    return NO_FILE;
  }
    
  QFileInfo info( filename );
  
  if ( !info.exists( ) || info.isDir( ) )
    return NO_FILE;
    
  if ( !info.isReadable( ) )
    return NO_PERMISSIONS;
    
  QFile file( filename );
  if ( !file.open( IO_ReadOnly ) )
    return NO_PERMISSIONS;
    
  QString errorMsg;
  int errorLine, errorColumn;
  
  QDomDocument doc;
  if ( !doc.setContent( &file, &errorMsg, &errorLine, &errorColumn ) ) {
      file.close( );
      kdError() << "Parsing error at line " << errorLine << ", column " << errorColumn << ", error " << errorMsg << endl;
      return PARSE_ERROR;
  }
  file.close( );

  const QDomElement documentElement( doc.documentElement() );
  // Count the number of messages in this file. This is needed for updating 
  // the progress bar correctly.
  msgcnt = documentElement.elementsByTagName( "message" ).count();

  if ( documentElement.tagName() != "TS" )
    return UNSUPPORTED_TYPE;

  cnt = 0;
  emit signalClearProgressBar( );
  kdDebug( ) << "start parsing..." << endl;
  
  parse( documentElement );
  //setCatalogExtraData( obsMessages );
  
  emit signalProgress( 100 );
  kdDebug( ) << "finished parsing..." << endl;
  
  setMimeTypes( "application/x-linguist" );
    
  return OK;
}

void LinguistImportPlugin::parse( const QDomElement& parentElement )
{
  QDomNode node = parentElement.firstChild( );
  
  while ( !node.isNull( ) ) {
    if ( node.isElement( ) ) {
      QDomElement elem = node.toElement( );
      
      if ( elem.tagName( ) == "context" ) {
        // nothing to do here
      } else if ( elem.tagName( ) == "name" ) {
        context = elem.text( );
      } else if ( elem.tagName( ) == "message" ) {
        CatalogItem item;
        QString comment;
        bool isObsolete = false;
        bool isFuzzy = false;

        QDomNode childNode = node.firstChild();
        for ( ; ! childNode.isNull() ; childNode = childNode.nextSibling() )
        {
          const QDomElement elem = childNode.toElement();

          if ( elem.isNull() )
            continue;

          if ( elem.tagName( ) == "source" ) {
              item.setMsgid( elem.text( ) );
          } else if ( elem.tagName( ) == "translation" ) {
              item.setMsgstr( elem.text( ) );
              if ( elem.attribute( "type" ) == "unfinished" ) {
                // Only mark as fuzzy if there is a translation
                isFuzzy = !elem.text().isEmpty();
              } else if ( elem.attribute( "type" ) == "obsolete" ) {
              isObsolete = true;
              }
          } else if ( elem.tagName( ) == "comment" ) {
              if ( !elem.text( ).isEmpty( ) )
              comment = elem.text( );
          }
        }

        QString fullComment = "Context: " + context;
        if ( isFuzzy )
        {
          /*
           * HACK
           *
           * KBabel has not any flag to tell "this entry is fuzzy!"
           * but it uses the corresponding Gettext comment instead.
           *
           * Therefore we have little choice, but to add "#, fuzzy" to the comment,
           * even if it is very Linguist-unlike
           */
          fullComment += "\n";
          fullComment += "#, fuzzy";
        }
        if ( !comment.isEmpty() )
        {
          fullComment += "\n";
          fullComment += comment;
        }
        item.setComment( fullComment );
        
        appendCatalogItem( item, isObsolete );
        
        // Update the progress bar.
        cnt++;   
        uint prog = 100*cnt/msgcnt;
        emit signalProgress( prog );
      }
      // ### TODO: avoid recursing
      // recursive parsing
      parse( elem );
    }
    node = node.nextSibling( );
  }
}
