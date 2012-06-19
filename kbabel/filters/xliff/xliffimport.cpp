// kate: space-indent on; indent-width 2; replace-tabs on;

/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
                2001-2004 by Stanislav Visnovsky
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


/*******
 design:

 the file name and id data are stored in the comment like this:
 file\n
 id\n
 optional comments
 */ 

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
#include "xliffimport.h"

// ### TODO: what is the kdebug area?
#define KDEBUG_AREA

K_EXPORT_COMPONENT_FACTORY( kbabel_xliffimport, KGenericFactory<XLIFFImportPlugin> ( "kbabelxliffimportfilter" ) )

using namespace KBabel;

XLIFFImportPlugin::XLIFFImportPlugin( QObject * parent, const char * name, const QStringList& )
  : CatalogImportPlugin( parent, name )
{
}

ConversionStatus XLIFFImportPlugin::load( const QString& filename, const QString& )
{
  if ( filename.isEmpty( ) ) {
    kdDebug( KDEBUG_AREA ) << "fatal error: empty filename to open" << endl;
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
      kdError ( KDEBUG_AREA ) << "Parsing error at line " << errorLine << ", column " << errorColumn << ", error " << errorMsg << endl;
      return PARSE_ERROR;
  }
  file.close( );

  extraData.clear();

  const QDomElement documentElement( doc.documentElement() );
  msgcnt = documentElement.elementsByTagName( "trans-unit" ).count();
  
  extraData.append( doc.toString() );

  cnt = 0;
  emit signalClearProgressBar( );
  kdDebug( KDEBUG_AREA ) << "start parsing..." << endl;
  
  parse( documentElement );

  // store full document in extra data at index 0, the rest
  // contains contexts for each entry at(1) for entry 0, at(2) for entry 1 ...
  setCatalogExtraData( extraData );
  
  emit signalProgress( 100 );
  kdDebug( KDEBUG_AREA ) << "finished parsing..." << endl;
  
  setMimeTypes( "application/x-xliff" );
    
  return OK;
}

void XLIFFImportPlugin::parse( const QDomElement& parentElement )
{
  QDomNode node = parentElement.firstChild( );
  
  while ( !node.isNull( ) ) {
    if ( node.isElement( ) ) {
      QDomElement elem = node.toElement( );
      
      if ( elem.tagName( ) == "body" ) {
        // nothing to do here
      } else if ( elem.tagName( ) == "file" ) {
        context = elem.attribute( "original", QString() );
        kdDebug ( KDEBUG_AREA ) << "Found file: " << context << endl;
      } else if ( elem.tagName( ) == "trans-unit" ) {
        CatalogItem item;
        QString comment;
        bool isObsolete = false;
	
	const QString id = elem.attribute ("id");

        QDomNode childNode = node.firstChild();
        for ( ; ! childNode.isNull() ; childNode = childNode.nextSibling() )
        {
            if ( childNode.isElement() )
            {
                const QDomElement elem = childNode.toElement( );
                if ( elem.tagName( ) == "source" ) {
                    item.setMsgid( elem.text( ) );
                } else if ( elem.tagName( ) == "target" ) {
                    item.setMsgstr( elem.text( ) );
                }
            }
        }

	extraData.append ( context + '\n' + id );
        item.setComment( "Context: " + context + '\n' + id + "\n" + comment );
        
        appendCatalogItem( item, isObsolete );
        
        // Update the progress bar.
        cnt++;   
        uint prog = 100*cnt/msgcnt;
        emit signalProgress( prog );
      }
      // recursive parsing
      parse( elem );
    }
    node = node.nextSibling( );
  }
}
