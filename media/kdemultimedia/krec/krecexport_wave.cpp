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

#include "krecexport_wave.h"
#include "krecexport_wave.moc"

#include "krecglobal.h"

#include <kdebug.h>
#include <ktempfile.h>
#include <qfile.h>
#include <qtimer.h>
#include <kgenericfactory.h>

K_EXPORT_COMPONENT_FACTORY( libkrecexport_wave, KGenericFactory<KRecExport_Wave> )

KRecExport_Wave krecExportWave( 0 );

KRecExport_Wave::KRecExport_Wave( QObject* p, const char* n, const QStringList& )
  : KRecExportItem( p,n )
  , _file( 0 )
{
kdDebug( 60005 ) << k_funcinfo << endl;
	registerAtGlobal( this );
kdDebug( 60005 ) << "Registered Exports: " << KRecGlobal::the()->exportFormats() << endl;
}
KRecExport_Wave::~KRecExport_Wave() {
kdDebug( 60005 ) << k_funcinfo << endl;
}

KRecExport_Wave* KRecExport_Wave::newItem() {
kdDebug( 60005 ) << k_funcinfo << endl;
	return new KRecExport_Wave( 0 );
}

QStringList KRecExport_Wave::extensions() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	QStringList tmp;
	tmp << "*.wav" << "*.WAV";
	return tmp;
}

bool KRecExport_Wave::initialize( const QString &filename ) {
kdDebug( 60005 ) << k_funcinfo << endl;
	if ( !_file ) {
		_file = new QFile( filename );
		if ( _file->open( IO_Raw|IO_WriteOnly ) ) {
			/// Write an empty Wave-header...
			for ( int i=0; i<44; i++ )
				_file->putch( 0 );
		} else return false;
		return true;
	} else return false;
}
bool KRecExport_Wave::process() {
	//kdDebug( 60005 ) << k_funcinfo << running << endl;
	if ( _file ) {
		if ( running() ) {
			QByteArray bytearray( 4096 );
			emit getData( bytearray );
			_file->writeBlock( bytearray );
			QTimer::singleShot( 10, this, SLOT( process() ) );
		}
		return true;
	} else return false;
}
bool KRecExport_Wave::finalize() {
kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _file ) {
		// PCM-Compatible WAVE Header
		// bytes		variable			description
		// 0  - 3		'RIFF'
		// 4  - 7		wRiffLength			length of file minus the 8 byte riff header
		// 8  - 11		'WAVE'
		// 12 - 15		'fmt '
		// 16 - 19		wFmtSize			length of format chunk minus 8 byte header
		// 20 - 21		wFormatTag			identifies PCM, ULAW etc
		// 22 - 23		wChannels			number of channels
		// 24 - 27		dwSamplesPerSecond	samples per second per channel
		// 28 - 31		dwAvgBytesPerSec	non-trivial for compressed formats
		// 32 - 33		wBlockAlign			basic block size
		// 34 - 35		wBitsPerSample		non-trivial for compressed formats
		// 36 - 39		'data'
		// 40 - 43		dwDataLength		length of data chunk minus 8 byte header
		// 44 - (dwDataLength + 43)			the data

		// Basic 16b,2c,44kHz Wave-Header
		char riffHeader[] =
		{
		  0x52, 0x49, 0x46, 0x46, // 0  "AIFF"
		  0x00, 0x00, 0x00, 0x00, // 4  wavSize
		  0x57, 0x41, 0x56, 0x45, // 8  "WAVE"
		  0x66, 0x6d, 0x74, 0x20, // 12 "fmt "
		  0x10, 0x00, 0x00, 0x00, // 16
		  0x01, 0x00, 0x02, 0x00, // 20
		  0x44, 0xac, 0x00, 0x00, // 24
		  0x10, 0xb1, 0x02, 0x00, // 28
		  0x04, 0x00, 0x10, 0x00, // 32
		  0x64, 0x61, 0x74, 0x61, // 36 "data"
		  0x00, 0x00, 0x00, 0x00  // 40 byteCount
		};

		long wavSize = _file->size() - 8;
		write32( riffHeader, wavSize, 4 );

		write16( riffHeader, channels(), 22 );
		write32( riffHeader, samplingRate(), 24 );
		write16( riffHeader, bits(), 34 );

		long byteCount = wavSize - 44;
		write32( riffHeader, byteCount, 40 );

		_file->at( 0 );
		_file->writeBlock( riffHeader, 44 );
		_file->at( _file->size() );

		_file->close();
		delete _file;
		_file = 0;

		return true;
	} else return false;
}

// vim:sw=4:ts=4
