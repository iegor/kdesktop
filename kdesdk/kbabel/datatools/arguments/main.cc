/* This file is part of KBabel
   based Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
		       2002-2003  Stanislav Visnovsky <visnovsky@kde.org>

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

#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kmessagebox.h>

/***************************************************
 *
 * Factory
 *
 ***************************************************/

K_EXPORT_COMPONENT_FACTORY( kbabel_argstool, KGenericFactory<ArgumentTool> ( "kbabeldatatool" ) )

using namespace KBabel;

ArgumentTool::ArgumentTool( QObject* parent, const char* name, const QStringList & )
    : KDataTool( parent, name ), _cache_origin( 0 )
{
    i18n( "what check found errors","arguments");
}

bool ArgumentTool::run( const QString& command, void* data, const QString& datatype, const QString& mimetype )
{
    if ( command != "validate" )
    {
	kdDebug(KBABEL) << "Argument Tool does only accept the command 'validate' and 'shortcut'" << endl;
	kdDebug(KBABEL) << "   The commands " << command << " is not accepted" << endl;
	return FALSE;
    }
    
    // Check wether we can accept the data
    if ( datatype != "CatalogItem" )
    {
	kdDebug(KBABEL) << "Argument Tool only accepts datatype CatalogItem" << endl;
	return FALSE;
    }

    if ( mimetype != "application/x-kbabel-catalogitem" )
    {
	kdDebug(KBABEL) << "Argument Tool only accepts mimetype application/x-kbabel-catalogitem" << endl;
	return FALSE;
    }
    
    if( command == "validate" )
    {
	CatalogItem* item = (CatalogItem*)(data);
	
	if( item->isNoCformat() ) return true;
	bool hasError = false;
	
	if( _cache_origin != item->project() )
	{
    	    _context = item->project()->miscSettings().contextInfo;
	    _checkPlurals = item->project()->identitySettings().checkPluralArgument;
	    _cache_origin = item->project();
	}

	if(!item->isUntranslated())
	{

		QString formatChars="dioxXucsfeEgGp%";
		
		if( _checkPlurals ) formatChars+="n";

		// FIXME: this should care about plural forms in msgid
		QString line=item->msgid().first();
		QStringList argList;
		
//		if( isPluralForm() ) 
		{
		    // FIXME: this is KDE specific
		    if( line.startsWith("_n:" ))
		    {
			// truncate text after first \n to get args only once
			line = line.mid(0,line.find("\\n"));
		    }
		}
		line.replace(_context, "");
		line.replace(QRegExp("\\n"),"");
		line.simplifyWhiteSpace();
		
		// flag, for GNU only we can allow reordering
		bool non_gnu = (item->pluralForm() == KDESpecific);
		
		int index=line.find(QRegExp("%."));
		
		while(index>=0)
		{
			int endIndex=line.find(QRegExp("[^\\d]"),index+1);
			if(endIndex<0)
			{
				endIndex=line.length();
			}
			else if( formatChars.contains(line[endIndex]) )
			{
				endIndex++;
			}
			
			if(endIndex - index > 1 ) {
			    QString arg = line.mid(index,endIndex-index);
			    if( arg.contains( QRegExp("\\d") ) ) {
				non_gnu = true;
			    }
			    argList.append(arg);
				
			}
	
			index=line.find(QRegExp("%."),endIndex);
		}
		
		if( item->pluralForm()==KDESpecific) 
		{
		    // FIXME: this is KDE specific
		    if( _checkPlurals && line.startsWith("_n:" ) && !argList.contains("%n") )
		    {
			argList.append("%n");
		    }
		}
		
		// generate for each plural form to be checked separately
		line=item->msgstr().first();
		QStringList lines;
		
		// FIXME; this is KDE specific
		if( item->pluralForm() == KDESpecific )
		{
		    lines = QStringList::split("\\n",line);
		}
		else 
		{
		    lines.append(line);
		}

		QStringList argCache = argList;
		QStringList foundArgs;
		
		for(QStringList::Iterator i = lines.begin() ; i!=lines.end() ; i++)
		{
		    // initialize for the next plural form
		    foundArgs.clear();
		    argList = argCache;
		    
		    line=(*i);
		    
    		    line.replace(QRegExp("\\n"),"");
		    
		    QRegExp argdesc( 
			"%((["+formatChars+"])"
			+"|(\\d)+"
			+"|(\\d)+\\$(["+formatChars+"])"
			+")" ) ;
		    index = -1;

		    do {
		    
			index = argdesc.search( line, index+1 );
			
			if( index == -1 ) break;
			
			// do not add a redundant argument, if it is non GNU 
			if( !non_gnu || !foundArgs.contains( argdesc.cap(0) ) )
			    foundArgs.append( argdesc.cap( 0 ) );		    
		    } while( true );
		    
		    // now, compare the list    
		    argList = argCache;
		    
		    if( non_gnu ) {
			for ( QStringList::Iterator it = foundArgs.begin(); it != foundArgs.end(); ++it ) {
			    if( argList.find( *it ) == argList.end() ) {
				hasError = true;
				break;
			    } else {
				argList.remove( *it );
			    }
			}
			
			if( ! argList.isEmpty() ) {
			    hasError = true;
			}
		    }
		    else 
		    {
			// handle GNU with replacements
			QStringList::Iterator oit = argList.begin();
			for ( QStringList::Iterator it = foundArgs.begin(); it != foundArgs.end(); ++it , ++oit) {
			    if( *it == *oit ) {
				// argument is the same, mark as used
				*oit = QString::null;
			    } else {
				// try to strip replacement
				int index = (*it).find( '$' );
				if( index == -1 ) {
				    // there is no replacement, this is wrong
				    hasError = true;
				    break;
				}
				QString place = (*it).mid( 1, index-1 );
				QString arg = (*it).right( index );
				arg[0] = '%';
				QStringList::Iterator a = argList.at( place.toInt()-1 );
				if( a != argList.end() && (*a) == arg ) 
				{
				    (*a) = QString::null;
				}
				else 
				{
				    // duplicate or index is too high
				    hasError = true;
				}
			    }
			}

			for ( QStringList::Iterator it = argList.begin(); it != argList.end(); ++it) {
			    if( ! (*it).isNull () ) {
				// argument is the same, mark as used
				hasError = true;
				break;
			    }
			}
		    }
		}
	}
	
	if(hasError)
	{
	    item->appendError( "Arguments" );
	}
	else
	{
	    item->removeError( "Arguments" );
	}
	
	return !hasError;
    }
    return FALSE;
}

#include "main.moc"
