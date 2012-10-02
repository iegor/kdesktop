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
#include "catalogitem.h"
#include "catalogsettings.h"
#include "main.h"

#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>

/***************************************************
 *
 * Factory
 *
 ***************************************************/

K_EXPORT_COMPONENT_FACTORY( kbabel_punctuationtool, KGenericFactory<PunctuationTool> ( "kbabeldatatool" ) )

using namespace KBabel;

PunctuationTool::PunctuationTool( QObject* parent, const char* name, const QStringList & )
    : KDataTool( parent, name )
{
    // bogus translation just for allowing the translation
    i18n("what check found errors","punctuation");
}

bool PunctuationTool::run( const QString& command, void* data, const QString& datatype, const QString& mimetype )
{
    if ( command != "validate" )
    {
	kdDebug(KBABEL) << "Punctuation Tool does only accept the command 'validate'" << endl;
	kdDebug(KBABEL) << "   The commands " << command << " is not accepted" << endl;
	return FALSE;
    }
    
    // Check wether we can accept the data
    if ( datatype != "CatalogItem" )
    {
	kdDebug(KBABEL) << "Punctuation Tool only accepts datatype CatalogItem" << endl;
	return FALSE;
    }

    if ( mimetype != "application/x-kbabel-catalogitem" )
    {
	kdDebug(KBABEL) << "Punctuation Tool only accepts mimetype application/x-kbabel-catalogitem" << endl;
	return FALSE;
    }
    
    if( command == "validate" )
    {
	CatalogItem* item = (CatalogItem*)(data);
	
	bool hasError = false;

	if(!item->isUntranslated())
	{
		QString lineid=item->msgid().first();

		// lookup punctuation in original text
		QRegExp punc("[\\.!\\?:]+$");
		int i = lineid.find(punc);
		
		QString t("");
		
		if( i != -1 ) t = lineid.right(lineid.length()-i);
		
		if( item->pluralForm() != NoPluralForm )
		{
		    // check, that both plural forms contain the same punctuation
		    QString pl = *(item->msgid().at(1));
		    int j = pl.find(punc);
		    
		    QString tp("");
		    if( j != -1 ) tp = pl.right(pl.length()-j);
		    
		    if( tp != t )
		    {
			kdWarning() << "Singular and plural form do not have the same punctuation" << endl;
		    }
		}
		
		QStringList forms = item->msgstr(true);
		if( item->pluralForm() == KDESpecific ) {
		    forms = QStringList::split("\\n",*item->msgstr(true).at(0));
		}

		for( QStringList::Iterator form = forms.begin() ; form != forms.end(); form++ )
		{
		    QString linestr=(*form);
		    
		    int j = linestr.find(punc);
		    
		    // there is no punctuation in original, but one in the translation
		    if( i == -1 && j != i ) 
		    {
			hasError = true;
			break;
		    }
		    
		    // there is punctuation in original, but not same as in the translation
		    if( i != -1 && linestr.right(linestr.length()-j) != t )
		    {
			hasError = true;
			break;
		    }
		}
	}
	
	if(hasError)
	{
	    item->appendError( "punctuation" );
	}
	else
	{
	    item->removeError( "punctuation" );
	}
			
	return !hasError;
    }
    return FALSE;
}

#include "main.moc"
