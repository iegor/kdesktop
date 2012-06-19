/* This file is part of KBabel
   based Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
			2002	Stanislav Visnovsky <visnovsky@kde.org>
			2003	Dwayne Bailey <dwayne@translate.org.za>

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

K_EXPORT_COMPONENT_FACTORY( kbabel_nottranslatedtool, KGenericFactory<NotTranslatedTool> ( "kbabeldatatool" ) )

using namespace KBabel;

NotTranslatedTool::NotTranslatedTool( QObject* parent, const char* name, const QStringList & )
    : KDataTool( parent, name ) , _cache_origin( 0 )
{
    i18n("which check found errors","English text in translation");
}

bool NotTranslatedTool::run( const QString& command, void* data, const QString& datatype, const QString& mimetype )
{

    if ( command != "validate" )
	{
		kdDebug(KBABEL) << "Not Translated Tool only accepts the 'validate' command" << endl;
		kdDebug(KBABEL) << "   The command " << command << " is not accepted" << endl;
		return false;
    }
    
    // Check wether we can accept the data
    if ( datatype != "CatalogItem" )
    {
		kdDebug(KBABEL) << "Not Translated Tool only accept the CatalogItem datatype" << endl;
		return false;
    }

    if ( mimetype != "application/x-kbabel-catalogitem" )
    {
		kdDebug(KBABEL) << "Not Translated Tool only accept the 'application/x-kbabel-catalogitem' mimetype" << endl;
		return false;
    }
    
    if( command == "validate" )
    {
		CatalogItem* item = (CatalogItem*)(data);
		bool hasError = false;

		if(!item->isUntranslated()) {
			if( _cache_origin != item->project() )
			{
			    _context = item->project()->miscSettings().contextInfo;
		    	    _plurals = item->project()->miscSettings().singularPlural;
			    _cache_origin = item->project();
			}

			//FIXME Expand this to do substring matching of non-translation
			QStringList id, str;
			if( item->pluralForm() == KDESpecific ) {
				str = QStringList::split( "\\n", item->msgstr().first(), true );
				id = QStringList::split( "\\n", 
					item->msgid().first().replace( QRegExp(_plurals), ""), true );
			} else {
				str = item->msgstr();
				id = item->msgid();
			}
			for( QStringList::Iterator i = id.begin() ; i != id.end() ; i++ ) {
				QString id_str = (*i).replace( QRegExp(_context), "");
				for( QStringList::Iterator j = str.begin() ; j != str.end() ; j++ ) {
					QString str_str = (*j);
					hasError = hasError || ( id_str == str_str );
				}
			}
				
		}
	
		if(hasError)
	   		item->appendError( "english text in translation" );
		else
			item->removeError( "english text in translation" );
			
		return !hasError;
    }
    return false;
}

#include "main.moc"
