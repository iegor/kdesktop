/***************************************************************************
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#include "krecglobal.h"
#include "krecglobal.moc"

#include "krecexport_template.h"

#include <kconfig.h>
#include <qwidget.h>
#include <kapplication.h>
#include <kstatusbar.h>

#include <ktrader.h>
#include <kparts/componentfactory.h>
#include <qvaluelist.h>
#include <kservice.h>

#include <kdebug.h>

KRecGlobal::KRecGlobal( QObject* p, const char* n )
  : QObject( p,n )
  , _qwidget( 0 )
  , _statusbar( 0 )
  , _timeformatcache( -1 )
  , _framebasecache( -1 )
{
	//kdDebug( 60005 ) << k_funcinfo << endl;
	_exports = new QDict<KRecExportItem>;
}
KRecGlobal::~KRecGlobal() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
}

KRecGlobal* KRecGlobal::the() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	static KRecGlobal* object = new KRecGlobal();
	return object;
}

void KRecGlobal::setMainWidget( QWidget* n ) { _qwidget = n; }
QWidget* KRecGlobal::mainWidget() { return _qwidget; }

KConfig* KRecGlobal::kconfig() { return kapp->config(); }

void KRecGlobal::setStatusBar( KStatusBar *bar ) { _statusbar = bar; }
void KRecGlobal::message( const QString &text ) { if ( _statusbar ) _statusbar->message( text, 2000 ); }

bool KRecGlobal::registerExport( KRecExportItem* item ) {
	//kdDebug( 60005 ) << k_funcinfo << "About to register ExportItem for \"" << item->exportFormat() << "\" extensions:\"" << item->extensions() << "\"" << endl;
	if ( !the()->_exports->find( item->exportFormat() ) ) {
		the()->_exports->insert( item->exportFormat(), item );
		the()->_exportformats << item->exportFormat();
	}
	return true;
}

KRecExportItem* KRecGlobal::getExportItem( const QString &exportFormat ) {
	KRecExportItem *tmp = _exports->find( exportFormat );
	return tmp->newItem();
}

QStringList KRecGlobal::exportFormats() const {
	return _exportformats;
}

QString KRecGlobal::exportFormatEndings() const {
	QString out;

	KTrader::OfferList offers = KTrader::self()->query( "KRec/exportplugin" );
	KTrader::OfferList::iterator it = offers.begin();
	while (  it!=offers.end() ) {
		out += " *.";
		out += ( *it )->property( "X-KDE-ExportSuffix" ).toStringList().join( " *." );
		++it;
	}

	return out;
}

KRecExportItem* KRecGlobal::getExportItemForEnding( const QString &ending ) {
	KTrader::OfferList offers = KTrader::self()->query( "KRec/exportplugin" );
	KTrader::OfferList::iterator it = offers.begin();
	while (  it!=offers.end() ) {
		kdDebug(60005) << ( *it )->property( "X-KDE-ExportSuffix" ).toStringList() << endl;
		if ( ( *it )->property( "X-KDE-ExportSuffix" ).toStringList().grep( ending ).count() ) {
			kdDebug(60005) << "Ending(" << ending << ") found" << endl;
			return KParts::ComponentFactory::createInstanceFromService<KRecExportItem>( ( *it ), this, "exportplugin" );
		} else kdDebug(60005) << "Doesn't contain " << ending << endl;
		++it;
	}
	return 0;
}

int KRecGlobal::timeFormatMode() {
	if ( _timeformatcache < 0 ) {
		kapp->config()->setGroup( "General" );
		_timeformatcache = kapp->config()->readNumEntry( "TimeFormat", 0 );
	}
	return _timeformatcache;
}
void KRecGlobal::setTimeFormatMode( int n ) {
	kapp->config()->setGroup( "General" );
	kapp->config()->writeEntry( "TimeFormat", n );
	_timeformatcache = n;
}

int KRecGlobal::frameBase() {
	if ( _framebasecache < 0 ) {
		kapp->config()->setGroup( "General" );
		_framebasecache = kapp->config()->readNumEntry( "FrameBase", 25 );
	}
	return _framebasecache;
}
void KRecGlobal::setFrameBase( int n ) {
	kapp->config()->setGroup( "General" );
	kapp->config()->writeEntry( "FrameBase", n );
	_framebasecache = n;
}

