/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002-2003 	by Stanislav Visnovsky
                        	    <visnovsky@kde.org>

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

#include "catalogfileplugin.h"
#include "importplugin_private.h"

#include "catalog.h"

#include <qstringlist.h>

#include <kdebug.h>
#include <ktrader.h>

using namespace KBabel;

CatalogImportPlugin::CatalogImportPlugin(QObject* parent, const char* name) : QObject(parent,name)
{
    d = new CatalogImportPluginPrivate;
    d->_catalog = 0;
    d->_started = false;
    d->_stopped = false;
}

CatalogImportPlugin::~CatalogImportPlugin()
{
    delete d;
}

void CatalogImportPlugin::appendCatalogItem( const CatalogItem& item, const bool obsolete )
{
    if( obsolete )
	d->_obsoleteEntries.append(item);
    else
	d->_entries.append(item);
}

void CatalogImportPlugin::setCatalogExtraData( const QStringList& data )
{
    d->_catalogExtraData=data;
    d->_updateCatalogExtraData=true;
}

void CatalogImportPlugin::setGeneratedFromDocbook( const bool generated )
{
    d->_generatedFromDocbook = generated;
    d->_updateGeneratedFromDocbook = true;
}

void CatalogImportPlugin::setErrorIndex(const QValueList<uint>& errors)
{
    d->_errorList = errors;
    d->_updateErrorList = true;
}

void CatalogImportPlugin::setFileCodec(QTextCodec* codec)
{
    d->_codec=codec;
    d->_updateCodec = true;
}

void CatalogImportPlugin::setHeader( const CatalogItem& item )
{
    d->_header=item;
    d->_updateHeader=true;
}

void CatalogImportPlugin::setMimeTypes( const QString& mimetypes )
{
    d->_mimeTypes=mimetypes;
}

ConversionStatus CatalogImportPlugin::open(const QString& file, const QString& mimetype, Catalog* catalog)
{
    d->_stopped=false;
    d->_catalog=catalog;
    startTransaction();
    
    ConversionStatus result = load(file, mimetype);
    if( d->_stopped ) 
    {
	d->_started=false;
	return STOPPED;
    }
    
    if( result == OK || result == RECOVERED_PARSE_ERROR || result == RECOVERED_HEADER_ERROR )
	commitTransaction();
	
    return result;
}

void CatalogImportPlugin::startTransaction()
{
    d->_started = (d->_catalog!=0);
    
    d->_updateCodec = false;
    d->_updateCatalogExtraData = false;
    d->_updateGeneratedFromDocbook = false;
    d->_updateErrorList = false;
    d->_updateHeader = false;
    d->_mimeTypes = "text/plain";
    d->_entries.clear();
}

void CatalogImportPlugin::commitTransaction()
{
    if( d->_started )
    {
	d->_catalog->clear();
	
	// fill in the entries
	QValueVector<CatalogItem> e;
	e.reserve( d->_entries.count() );
	for( QValueList<CatalogItem>::const_iterator it = d->_entries.begin(); it != d->_entries.end(); ++it )
	{
	    e.append( *it );
	}
	d->_catalog->setEntries( e );
	
	d->_catalog->setObsoleteEntries( d->_obsoleteEntries );
	
	if( d->_updateCodec ) d->_catalog->setFileCodec(d->_codec);
	if( d->_updateCatalogExtraData ) 
	    d->_catalog->setCatalogExtraData(d->_catalogExtraData);
	if( d->_updateGeneratedFromDocbook ) 
	    d->_catalog->setGeneratedFromDocbook(d->_generatedFromDocbook);
	if( d->_updateHeader ) 
	    d->_catalog->setHeader(d->_header);
	// generate before setting errors, since it clears the error list
	d->_catalog->generateIndexLists();
	if( d->_updateErrorList ) 
	    d->_catalog->setErrorIndex(d->_errorList);
	
	d->_catalog->setImportPluginID(id());
	d->_catalog->setMimeTypes( d->_mimeTypes );
    }
    
    d->_started = false;
}

QStringList CatalogImportPlugin::availableImportMimeTypes()
{
    QStringList result;
    
    KTrader::OfferList offers = KTrader::self()->query("KBabelFilter", "exist [X-KDE-Import]");
    
    for( KTrader::OfferListIterator ptr = offers.begin(); ptr!=offers.end() ; ++ptr )
    {
	result += (*ptr)->property("X-KDE-Import").toStringList();
    }
    
    return result;
}

bool CatalogImportPlugin::isStopped() const
{
    return d->_stopped;
}

void CatalogImportPlugin::stop()
{
    d->_stopped = true;
}

#include "catalogfileplugin.moc"
