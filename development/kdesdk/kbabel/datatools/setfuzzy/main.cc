/* This file is part of KBabel
   based Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
			2003	Stanislav Visnovsky <visnovsky@kde.org>

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
#include "catalog.h"
#include "main.h"

#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>

/***************************************************
 *
 * Factory
 *
 ***************************************************/

K_EXPORT_COMPONENT_FACTORY( kbabel_setfuzzytool, KGenericFactory<SetFuzzyTool> ( "kbabeldatatool" ) )

using namespace KBabel;

SetFuzzyTool::SetFuzzyTool( QObject* parent, const char* name, const QStringList & )
    : KDataTool( parent, name )
{
}

bool SetFuzzyTool::run( const QString& command, void* data, const QString& datatype, const QString& mimetype )
{
    if ( command != "allfuzzy" )
    {
	kdDebug(KBABEL) << "Fuzzy Toggling Tool does only accept the command 'allfuzzy'" << endl;
	kdDebug(KBABEL) << "   The commands " << command << " is not accepted" << endl;
	return FALSE;
    }
    
    // Check wether we can accept the data
    if ( datatype != "Catalog" )
    {
	kdDebug(KBABEL) << "Fuzzy Toggling Tool  only accepts datatype Catalog" << endl;
	return FALSE;
    }

    if ( mimetype != "application/x-kbabel-catalog" )
    {
	kdDebug(KBABEL) << "Plural Forms Tool only accepts mimetype application/x-kbabel-catalog" << endl;
	return FALSE;
    }
    
    if( command == "allfuzzy" )
    {
	Catalog* catalog = (Catalog*)(data);
	
	catalog->applyBeginCommand(0,Msgstr,0);
	
	for( uint index=0; index < catalog->numberOfEntries(); index++ )
	{
	    if( !catalog->isUntranslated(index) )
	    {
		catalog->setFuzzy(index,true);
	    }
	}

	catalog->applyEndCommand(0,Msgstr,0);
    }
    return TRUE;
}

#include "main.moc"
