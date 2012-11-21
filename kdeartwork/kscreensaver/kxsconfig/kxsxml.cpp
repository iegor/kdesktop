//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 2002
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "kxsxml.h"
#include "kxscontrol.h"
#include <qobject.h>
#include <qfile.h>
#include <qvbox.h>
#include <qhbox.h>
#include <stdio.h>

KXSXml::KXSXml( QWidget *p )
    : parent(p), handler(0)
{
}

bool KXSXml::parse( const QString &filename )
{
    QFile file( filename );
    handler = new KXSXmlHandler( parent );
    QXmlInputSource source( &file );
    QXmlSimpleReader reader;
    reader.setContentHandler( handler );
    if ( !reader.parse( &source, FALSE ) )
	return FALSE;

    return true;
}

const QPtrList<KXSConfigItem> *KXSXml::items() const
{
    if ( handler )
	return handler->items();
    return 0;
}

QString KXSXml::description() const
{
    if ( handler )
	return handler->description();
    return QString();
}

//===========================================================================

KXSXmlHandler::KXSXmlHandler( QWidget *p )
    : QXmlDefaultHandler(), parent(p), selItem(0), inDesc(false)
{
    mParentStack.push( p );
}

bool KXSXmlHandler::startDocument()
{
    return true;
}

bool KXSXmlHandler::startElement( const QString&, const QString&, 
                                    const QString& qName, 
                                    const QXmlAttributes &atts )
{
    KXSConfigItem *i = 0;
    QString id = atts.value("id");
    if ( qName == "number" ) {
	QString sLow = atts.value( "low" );
	QString sHigh = atts.value( "high" );
	if ( sLow.contains( '.' ) || sHigh.contains( '.' ) ) {
	    if ( parent )
		i = new KXSDoubleRangeControl( parent, id, atts );
	    else
		i = new KXSDoubleRangeItem( id, atts );
	} else {
	    if ( parent )
		i = new KXSRangeControl( parent, id, atts );
	    else
		i = new KXSRangeItem( id, atts );
	}
    } else if ( qName == "boolean" ) {
	if ( parent )
	    i = new KXSCheckBoxControl( parent, id, atts );
	else
	    i = new KXSBoolItem( id, atts );
    } else if ( qName == "string" ) {
	if ( parent )
	    i = new KXSLineEditControl( parent, id, atts );
	else
	    i = new KXSStringItem( id, atts );
    } else if ( qName == "file" ) {
	if ( parent )
	    i = new KXSFileControl( parent, id, atts );
	else
	    i = new KXSStringItem( id, atts );
    } else if ( qName == "_description" ) {
	inDesc = true;
    } else if ( qName == "select" ) {
	if ( parent )
	    selItem = new KXSDropListControl( parent, id, atts );
	else
	    selItem = new KXSSelectItem( id, atts );
	i = selItem;
    } else if ( qName == "option" && selItem ) {
	selItem->addOption( atts );
    } else if ( qName == "hgroup" && parent ) {
	QHBox *hb = new QHBox( parent );
	mParentStack.push( hb );
	parent = hb;
    } else if ( qName == "vgroup" && parent ) {
	QVBox *vb = new QVBox( parent );
	mParentStack.push( vb );
	parent = vb;
    }

    if ( i )
	mConfigItemList.append( i );

    return true;
}

bool KXSXmlHandler::endElement( const QString&, const QString&, const QString &qName )
{
    if ( qName == "select" ) {
	selItem = 0;
    } else if ( qName == "_description" ) {
	inDesc = false;
    } else if ( (qName == "hgroup" || qName == "vgroup") && parent ) {
	if ( mParentStack.count() > 1 ) {
	    mParentStack.pop();
	    parent = mParentStack.top();
	}
    }
    return true;
}

bool KXSXmlHandler::characters( const QString &ch )
{
    if ( inDesc )
	desc += ch;
    return true;
}

