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


#include <qfile.h>
#include "qregexp.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <kgenericfactory.h>

#include "catalog.h"
#include "catalogitem.h"
#include "catalogsettings.h"
#include "xliffexport.h"

K_EXPORT_COMPONENT_FACTORY( kbabel_xliffexport, KGenericFactory<XLIFFExportPlugin> ( "kbabelxliffexportfilter" ) )

using namespace KBabel;

XLIFFExportPlugin::XLIFFExportPlugin( QObject * parent, const char * name, const QStringList& )
  : CatalogExportPlugin( parent, name )
{
}

ConversionStatus XLIFFExportPlugin::save( const QString& filename, const QString&, const Catalog * catalog )
{
  // Check whether we know how to handle the extra data.
  if ( catalog->importPluginID( ) != "XLIFF 1.1" )
    return UNSUPPORTED_TYPE;
  
  QFile file( filename );
  if ( !file.open( IO_WriteOnly ) )
    return OS_ERROR;
    
  SaveSettings settings = catalog->saveSettings( );

  // New DOM document.  
  QDomDocument doc( "" );
  
  extraData = catalog->catalogExtraData();
  kdDebug () << "Setting the document data: " << extraData.first () << endl;  
  doc.setContent ( extraData.first () );

  // Regular messages.
  for ( uint i = 0; i < catalog->numberOfEntries( ); i++ ) {
    QDomElement element = extractComment( doc, *(extraData.at( i+1 )) );
    createMessage( doc, element, catalog->msgid( i ).join( "" ), catalog->msgstr( i ).join( "" ) );
  }
  
  QTextStream stream( &file );
  doc.save( stream, 2 );
  file.close( );
  
  return OK;
}

QDomElement XLIFFExportPlugin::extractComment( QDomDocument& doc, const QString& s )
{
    QString comment( s );
  
    if ( comment.isEmpty () )
    {
	kdError () << "Empty comment, should not happen" << endl;
    }
  
    // Extract the context and the actual comment.
    comment.remove( QRegExp( "^Context:[\\s]*" ) );
    QString newContext;
    QStringList commentlines = QStringList::split ( '\n', comment);
    
    QString file = *(commentlines.at(0));
    QString id = *(commentlines.at(1));
    
    kdDebug () << "Looking for file " << file << endl;

    return getContext( doc, file, id );
}

void XLIFFExportPlugin::createMessage( QDomDocument& doc, QDomElement& translationElement, const QString& msgid, 
     const QString& msgstr )
{
    // for empty messages, don't store anything
    if (msgstr.isEmpty ())
	return;
	
    // find the trans element
    QDomNode node = translationElement.firstChild( );
    while ( !node.isNull( ) ) {
	kdDebug () << node.nodeName () << endl;
	if ( node.isElement()  && node.toElement().tagName( ) == "target") {
	    kdDebug () << "Found the target: " << 
	    node.firstChild().nodeName () << endl;
	    // set the new translation
	    node.firstChild().toText().setData (msgstr);
	    break;
	}
	node = node.nextSibling( );
    }

    if ( node.isNull () )
    {
	// no target tag, create one
	node = doc.createElement ("target");
	translationElement.appendChild (node);
	
	QDomText data = doc.createTextNode(msgstr );
	node.appendChild( data );
    }    
}

QDomElement XLIFFExportPlugin::getContext( QDomDocument& doc, const QString& file, const QString& id )
{
  // Find out whether there is already such a context in the QDomDocument.
  QDomNode parentelem = doc.documentElement();
  QDomNode elem = doc.documentElement( ).firstChild( );
  while ( !elem.isNull( ) ) {
      if ( elem.isElement( ) && elem.toElement().tagName( ) == "file" && elem.toElement().attribute ("original") == file ) {
        kdDebug () << "We have found the file" << endl;
	break;
      }
      elem = elem.nextSibling( );
  }
  
  if (elem.isNull ())
  {
      kdError () << "File not found at all, creating" << endl;
      QDomElement newelem = doc.createElement ("file");
      newelem.setAttribute ("original", file);
      parentelem.appendChild (newelem);
      elem = newelem;
  }
  
  // lookup body tag
  parentelem = elem;
  elem = elem.firstChild ();
  while ( !elem.isNull( ) ) {
      if ( elem.isElement( ) && elem.toElement().tagName( ) == "body" ) {
        kdDebug () << "We have found the file body" << endl;
	break;
      }
      elem = elem.nextSibling( );
  }
  
  if (elem.isNull ())
  {
      kdError () << "File body not found at all, creating" << endl;
      QDomElement newelem = doc.createElement ("body");
      parentelem.appendChild (newelem);
      elem = newelem;
  }
  
  elem = findTransUnit (elem, id);
  
  if (elem.isNull ())
  {
      kdError () << "Trans-unit not found at all, creating" << endl;
      QDomElement newelem = doc.createElement ("trans-unit");
      newelem.setAttribute ("id", id);
      parentelem.appendChild (newelem);
      elem = newelem;
  }
  
  return elem.toElement ();
}

QDomElement XLIFFExportPlugin::findTransUnit( QDomNode& group, const QString& id )
{
  QDomNode elem = group.firstChild( );

  // lookup correct trans-unit tag
  while ( !elem.isNull( ) ) {
      if ( elem.isElement( ) && elem.toElement().tagName() == "group" )
      {
        // search recursively
	QDomElement res = findTransUnit( elem, id );
	if (! res.isNull () )
	    return res.toElement();
      }
      else if ( elem.isElement( ) && elem.toElement().tagName( ) == "trans-unit" && elem.toElement().attribute ("id") == id ) {
        kdDebug () << "We have found the trans-unit" << endl;
	return elem.toElement ();
      }
      elem = elem.nextSibling( );
  }
  
  return elem.toElement ();
}
