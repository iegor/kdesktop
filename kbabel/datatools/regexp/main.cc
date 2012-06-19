/*	Copyright (C) 2005	Albert Cervera i Areny <albertca at hotpop dot com>

	Based on Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
		    2002 	Stanislav Visnovsky <visnovsky@kde.org>
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
*/

#include <resources.h>
#include "catalog.h"
#include "catalogitem.h"
#include "catalogsettings.h"
#include "main.h"
#include <math.h>

#include <qdir.h>
#include <qfile.h>
#include <qdom.h>
#include <qstringlist.h>
#include <qregexp.h>
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

K_EXPORT_COMPONENT_FACTORY( kbabel_regexptool, KGenericFactory<RegExpTool> ( "kbabeldatatool" ) )

using namespace KBabel;

RegExpTool::RegExpTool( QObject* parent, const char* name, const QStringList & )
    : KDataTool( parent, name )
{
	i18n("which check found errors","translation has inconsistent length");
	loadExpressions();
	if ( ! _error.isNull() )
		KMessageBox::error( (QWidget*)parent, i18n( "Error loading data (%1)" ).arg( _error ) );
}

bool RegExpTool::run( const QString& command, void* data, const QString& datatype, const QString& mimetype )
{
	if ( command != "validate" )
	{
		kdDebug(KBABEL) << "RegExpTool only accepts the 'validate' command" << endl;
		kdDebug(KBABEL) << "   The command " << command << " is not accepted" << endl;
		return false;
	}
	// Check wether we can accept the data
	if ( datatype != "CatalogItem" )
	{
		kdDebug(KBABEL) << "RegExpTool only accepts the CatalogItem datatype" << endl;
		return false;
	}
	if ( mimetype != "application/x-kbabel-catalogitem" )
	{
		kdDebug(KBABEL) << "RegExpTool only accepts the 'application/x-kbabel-catalogitem' mimetype" << endl;
		return false;
	}
	
	bool hasError = false;
	if( command == "validate" )
	{
		CatalogItem* item = (CatalogItem*)(data);

		if(!item->isUntranslated()) {
			ExpressionList::Iterator it( _list.begin() );
			ExpressionList::Iterator end( _list.end() );
			QStringList msgs = item->msgstr();
			QStringList results;
			for ( ; it != end; ++it ) {
				results.clear();
				results = msgs.grep( (*it).regExp() );
				if ( results.size() > 0 ) {
					hasError = true;
					break;
				}
			}
		}
		if(hasError) {
			item->appendError( "regexp" );
		} else {
			item->removeError( "regexp" );
		}
	}
	return !hasError;
}


void RegExpTool::loadExpressions()
{
	// TODO: Change file path
	QFile file( QDir::homeDirPath() + "/.kde/share/apps/kbabel/regexplist.xml" );
	QDomDocument doc;
	
	if ( ! file.open( IO_ReadOnly ) ) {
		kdDebug() << "File not found" << endl;
		_error = i18n( "File not found" );
		return;
	}
	if ( ! doc.setContent( &file ) ) {
		kdDebug() << "Could not set content of xml file" << endl;
		_error = i18n( "The file is not a XML" );
		return;
	}
	file.close();

	QDomElement docElem = doc.documentElement();
	QDomNode n = docElem.firstChild();
	while( !n.isNull() ) {
		QDomElement e = n.toElement();
		if( !e.isNull() )
			elementToExpression( e );
		if ( ! _error.isNull() )
			break;
		n = n.nextSibling();
	}
}

void RegExpTool::elementToExpression( const QDomElement& e )
{
	QString name;
	QString exp;
	bool cs = false; //Expressions are case insensitive by default

	if ( e.tagName().compare( "item" ) != 0 ) {
		_error = i18n( "Expected tag 'item'" );
		return;
	}

	QDomNode n = e.firstChild();
	if ( n.isNull() ) {
		_error = i18n( "First child of 'item' is not a node" );
		return;
	}

	QDomElement el = n.toElement();
	if ( el.isNull() || el.tagName().compare( "name" ) != 0 ) {
		_error = i18n( "Expected tag 'name'" );
		return;
	}
	name = el.text();

	n = n.nextSibling();
	el = n.toElement();
	if ( el.isNull() || el.tagName().compare( "exp" ) != 0 ) {
		_error = i18n( "Expected tag 'exp'" );
		return;
	}
	exp = el.text();

	n = n.nextSibling();
	el = n.toElement();
	if ( ! el.isNull() )
		cs = true;

	kdDebug(KBABEL) << "RegExpTool: Adding expression: " << name << endl;
	_list.append( Expression( name, QRegExp( exp, cs ) ) );
}

#include "main.moc"
