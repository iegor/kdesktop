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

#include "krecexport_template.h"
#include "krecexport_template.moc"

#include "krecglobal.h"

#include <qtimer.h>
#include <kdebug.h>

KRecExportItem::KRecExportItem( QObject* p, const char* n, const QStringList& ) : QObject( p,n ), _running( false ) {
	//kdDebug( 60005 ) << k_funcinfo << endl;
}
KRecExportItem::~KRecExportItem() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
}
int KRecExportItem::samplingRate() const {
	//kdDebug( 60005 ) << k_funcinfo << _samplingRate << endl;
	return _samplingRate;
}
int KRecExportItem::bits() const {
	//kdDebug( 60005 ) << k_funcinfo << _bits << endl;
	return _bits;
}
int KRecExportItem::channels() const {
	//kdDebug( 60005 ) << k_funcinfo << _channels << endl;
	return _channels;
}

void KRecExportItem::registerAtGlobal( KRecExportItem* item ) {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	bool registered = false;
	if ( !registered ) registered = KRecGlobal::the()->registerExport( item );
	//if ( registered ) kdDebug( 60005 ) << "Register successful!" << endl;
	//	else kdDebug( 60005 ) << "Register NOT successful!" << endl;
}

void KRecExportItem::initialize( int samplingRate, int bits, int channels ) {
kdDebug( 60005 ) << k_funcinfo << "samplingRate:" << samplingRate << " bits:" << bits << " channels:" << channels << endl;
	_samplingRate = samplingRate;
	_bits = bits;
	_channels = channels;
}
bool KRecExportItem::start() {
kdDebug( 60005 ) << k_funcinfo << endl;
	if ( !running() ) {
		if ( process() ) {
			_running = true;
			QTimer::singleShot( 0, this, SLOT( process() ) );
			emit running( running() );
		}
		return true;
	} else return false;
}

void KRecExportItem::stop() {
kdDebug( 60005 ) << k_funcinfo << endl;
	_running = false;
	emit running( running() );
}

Q_INT16 KRecExportItem::read16( char* array, int index ) {
	Q_INT16 tmp;
	tmp = array[ index ] + ( array[ index + 1 ] << 8 ) & 0xff;
	return tmp;
}
/// Helper: writes an integer into an char* formated for wave-files
void KRecExportItem::write16( char* array, Q_INT16 value, int index ) {
	array[ index     ] = ( value >> 0 ) & 0xff;
	array[ index + 1 ] = ( value >> 8 ) & 0xff;
}
void KRecExportItem::write32( char* array, Q_INT32 value, int index ) {
	write16( array, value, index );
	array[ index + 2 ] = ( value >> 16 ) & 0xff;
	array[ index + 3 ] = ( value >> 24 ) & 0xff;
}

// vim:sw=4:ts=4
