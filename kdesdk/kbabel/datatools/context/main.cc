/* This file is part of KBabel
   based Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
			2002	Stanislav Visnovsky <visnovsky@kde.org>

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
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>

/***************************************************
 *
 * Factory
 *
 ***************************************************/

K_EXPORT_COMPONENT_FACTORY( kbabel_contexttool, KGenericFactory<ContextTool> ( "kbabeldatatool" ) )

using namespace KBabel;

ContextTool::ContextTool( QObject* parent, const char* name, const QStringList & )
    : KDataTool( parent, name ), _cache_origin( 0 )
{
    i18n("what check found errors","context info");
}

bool ContextTool::run( const QString& command, void* data, const QString& datatype, const QString& mimetype )
{
    if ( command != "validate" )
    {
	kdDebug(KBABEL) << "Context Tool does only accept the command 'validate' and 'shortcut'" << endl;
	kdDebug(KBABEL) << "   The commands " << command << " is not accepted" << endl;
	return FALSE;
    }
    
    // Check wether we can accept the data
    if ( datatype != "CatalogItem" )
    {
	kdDebug(KBABEL) << "Context Tool only accepts datatype CatalogItem" << endl;
	return FALSE;
    }

    if ( mimetype != "application/x-kbabel-catalogitem" )
    {
	kdDebug(KBABEL) << "Context Tool only accepts mimetype application/x-kbabel-catalogitem" << endl;
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
	
	bool hasError = false;

	if(!item->isUntranslated() && item->msgid().first().contains(_context) 
	    && item->msgstr().first().contains(_context) )
	{
	    hasError = true;
	}
	
	if(hasError)
	{
	    item->appendError( "context info" );
	}
	else
	{
	    item->removeError( "context info" );
	}
			
	return !hasError;
    }
    return FALSE;
}

#include "main.moc"
