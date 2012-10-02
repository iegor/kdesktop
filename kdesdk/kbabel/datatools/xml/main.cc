/* This file is part of KBabel
   based Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
			2002-2003 Stanislav Visnovsky <visnovsky@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

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

*/

#include <resources.h>
#include "catalogitem.h"
#include "catalogsettings.h"
#include "main.h"

#include <qdom.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>

/***************************************************
 *
 * Factory
 *
 ***************************************************/

K_EXPORT_COMPONENT_FACTORY( kbabel_xmltool, KGenericFactory<XMLTool> ( "kbabeldatatool" ) )

using namespace KBabel;

XMLTool::XMLTool( QObject* parent, const char* name, const QStringList & )
    : KDataTool( parent, name ), _cache_origin( 0 )
{
    i18n( "what check found errors", "XML tags" );
}

bool XMLTool::run( const QString& command, void* data, const QString& datatype, const QString& mimetype )
{
    if ( command != "validate" )
    {
	kdDebug(KBABEL) << "XML Tool does only accept the command 'validate' and 'shortcut'" << endl;
	kdDebug(KBABEL) << "   The commands " << command << " is not accepted" << endl;
	return FALSE;
    }
    
    // Check wether we can accept the data
    if ( datatype != "CatalogItem" )
    {
	kdDebug(KBABEL) << "XML Tool only accepts datatype CatalogItem" << endl;
	return FALSE;
    }

    if ( mimetype != "application/x-kbabel-catalogitem" )
    {
	kdDebug(KBABEL) << "XML Tool only accepts mimetype application/x-kbabel-catalogitem" << endl;
	return FALSE;
    }
    
    if( command == "validate" )
    {
	CatalogItem* item = (CatalogItem*)(data);
	
	if( _cache_origin != item->project() )
	{
	    _context = item->project()->miscSettings().contextInfo;
	    _cache_origin = item->project();
	}
	
	uint correctnessLevel = 0;
	QString msgid = item->msgid().first();
	msgid.replace( "\\\"", "\"" ); // Change '\"' to '"'
	msgid.replace( QRegExp( "&(?![a-zA-Z0-9]+;)" ), "&amp;" );
	msgid.replace( _context, "" );
	msgid.replace("\n","");	// delete newlines
	
	if( _levelCache.contains(msgid) )
	{
	    correctnessLevel = _levelCache[msgid];
	}
	else
	{
	    // identify the level of correctness
	    if( isFullyCompliant(msgid) )
	    {
		correctnessLevel = 0;
	    }
	    else if( isNonCaseCompliant(msgid) )
	    {
		correctnessLevel = 1;
	    }
	    else if( isNonCaseWithoutCommonCompliant(msgid) )
	    {
		correctnessLevel = 2;
	    }
	    else
	    {
		correctnessLevel = 3;
	    }
	    
	    _levelCache[msgid] = correctnessLevel;
	}
	
	bool hasError = false;

	if(!item->isUntranslated())
	{
	    QStringList str = item->msgstr(true);
    	    for( QStringList::Iterator form = str.begin() ; form != str.end() ; form++ )
	    {
	        QString text=(*form);
		text.replace( "\\\"", "\"" ); // Change '\"' to '"'
		text.replace( QRegExp( "&(?![a-zA-Z0-9]+;)" ), "&amp;" );
		
		// isNonCaseWithoutCommonCompliant can fail
		// even though higher level checks works
		// see case 2.
		switch( correctnessLevel )
		{
		    case 0: hasError = !isFullyCompliant(text); break;
		    case 1: hasError = !isNonCaseCompliant(text); break;
		    case 2: hasError = !isNonCaseWithoutCommonCompliant(text);
			if (hasError)
			{
			    hasError = !isNonCaseCompliant(text) ||
				!isFullyCompliant(text);
			}
			break;
		    case 3: hasError = false; break;	// the original is broken
		    default: kdWarning() << "No compliance level, this should not happen" << endl;
		}
	    }
	}
	
	if(hasError)
	{
	    item->appendError( "XML tags" );
	}
	else
	{
	    item->removeError( "XML tags" );
	}
	
	return !hasError;
    }
    return FALSE;
}

bool XMLTool::isFullyCompliant( const QString& text)
{
    QDomDocument doc;
    return doc.setContent("<para>" + text + "</para>" );
}

bool XMLTool::isNonCaseCompliant( const QString& text)
{
    QDomDocument doc;
    QString test = text.lower();
    return doc.setContent("<para>" + test + "</para>" );
}

bool XMLTool::isNonCaseWithoutCommonCompliant( const QString& text)
{
    QDomDocument doc;
    QString test = text.lower();
    QRegExp rx( "(<br>)|(<hr>)|(<p>)||(<\\w+@(\\w+.)*\\w+>)" );
    test.replace( rx, "" );

    QString a;
    do 
    {
	a = test;
	test.replace( QRegExp("<[^_:A-Za-z/]"), "" );
    } while( a!=test);
		
    test.replace( QRegExp("<$"), "" );
		
    return doc.setContent("<para>" + test + "</para>" );
}

#include "main.moc"
