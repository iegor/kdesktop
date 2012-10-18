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

#include <config.h>

#ifdef HAVE_VORBIS

#include "krecexport_ogg.h"
#include "krecexport_ogg.moc"

#include "krecglobal.h"

#include <kdebug.h>
#include <ktempfile.h>
#include <qfile.h>
#include <qtimer.h>
#include <kconfig.h>
#include <stdlib.h>
#include <time.h>
#include <qdatetime.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kmessagebox.h>

K_EXPORT_COMPONENT_FACTORY( libkrecexport_ogg, KGenericFactory<KRecExport_OGG> )

KRecExport_OGG krecExportOGG( 0 );

KRecExport_OGG::KRecExport_OGG( QObject* p, const char* n, const QStringList& )
  : KRecExportItem( p,n )
  , _file( 0 )
  , init_done( false )
{
kdDebug( 60005 ) << k_funcinfo << endl;
	registerAtGlobal( this );
kdDebug( 60005 ) << "Registered Exports: " << KRecGlobal::the()->exportFormats() << endl;
}
KRecExport_OGG::~KRecExport_OGG() {
kdDebug( 60005 ) << k_funcinfo << endl;
}

KRecExport_OGG* KRecExport_OGG::newItem() {
kdDebug( 60005 ) << k_funcinfo << endl;
	return new KRecExport_OGG( 0 );
}

QStringList KRecExport_OGG::extensions() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	QStringList tmp;
	tmp << "*.ogg" << "*.OGG";
	return tmp;
}

bool KRecExport_OGG::initialize( const QString &filename ) {
kdDebug( 60005 ) << k_funcinfo << endl;
	if ( !_file &&
	     !( samplingRate()!=44100 && bits()!=16 && channels()!=2 &&
	        KMessageBox::warningContinueCancel( KRecGlobal::the()->mainWidget(),
	          i18n( "At this time OGG-export only supports files in 44kHz " \
	                "samplingrate, 16bit and 2 channels." )
	        ) == KMessageBox::Cancel
	     )
	) {
		KMessageBox::information( KRecGlobal::the()->mainWidget(),
		  i18n( "Please note that this plugin takes its qualitysettings from" \
		        " the corresponding section of the audiocd:/ configuration. Make use" \
		        " of the Control Center to configure these settings." ),
		  i18n( "Quality Configuration" ), "qualityinfo_ogg" );
		_file = new QFile( filename );
		if ( _file->open( IO_Raw|IO_WriteOnly ) ) {
			if ( ! init_done ) {
				setOggParameters();
				vorbis_analysis_init( &vd, &vi );
				vorbis_block_init( &vd, &vb );

				srand( time( NULL ) );
				ogg_stream_init( &os, rand() );
			}
			if ( _file->size() == 0 ) {
				ogg_packet header;
				ogg_packet header_comm;
				ogg_packet header_code;

				vorbis_comment_init( &vc );
				vorbis_comment_add_tag ( &vc, const_cast<char *>( "kde-encoder" ),
				                         const_cast<char *>( "KRec" ) );
				if ( write_vorbis_comments ) {
					QDateTime dt = QDateTime::currentDateTime();
					vorbis_comment_add_tag ( &vc, const_cast<char *>( "title" ),
					                         const_cast<char *>( "" ) );
					vorbis_comment_add_tag ( &vc, const_cast<char *>( "artist" ),
					                         const_cast<char *>( "" ) );
					vorbis_comment_add_tag ( &vc, const_cast<char *>( "album" ),
					                         const_cast<char *>( "" ) );
					vorbis_comment_add_tag ( &vc, const_cast<char *>( "genre" ),
					                         const_cast<char *>( "" ) );
					vorbis_comment_add_tag ( &vc, const_cast<char *>( "tracknumber" ),
					                         const_cast<char *>( "" ) );
					vorbis_comment_add_tag ( &vc, const_cast<char *>( "date" ),
					                         const_cast<char *>( dt.toString( "yyyy-MM-dd hh:mm:ss" ).utf8().data() ) );
				}
				vorbis_analysis_headerout( &vd, &vc, &header, &header_comm, &header_code );

				ogg_stream_packetin( &os, &header );
				ogg_stream_packetin( &os, &header_comm );
				ogg_stream_packetin( &os, &header_code );

				while ( ogg_stream_flush( &os, &og ) ) {
					//kdDebug( 60005 ) << "Writing Ogg/Vorbis Header." << endl;
					_file->writeBlock( reinterpret_cast<char *>( og.header ), og.header_len );
					_file->writeBlock( reinterpret_cast<char *>( og.body ), og.body_len );
				}
			} else
				_file->at( _file->size() );
			init_done = true;
		} else return false;
		return true;
	}
	return false;
}
bool KRecExport_OGG::process() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _file ) {
		if ( running() ) {
			QByteArray bytearray( 4096 );
			emit getData( bytearray );
			float **buffer = vorbis_analysis_buffer( &vd, bytearray.size() >> 2 );

			// uninterleave samples
			unsigned int i;
			int index = 0;
			int16_t sample;
			unsigned char *packet = reinterpret_cast<unsigned char *>( bytearray.data() );
			for ( i = 0; i < ( bytearray.size() >> 2 ); i++ ) {
				sample = packet[ index ] | ( packet[ index + 1 ] << 8 );
				index += 2;
				buffer[ 0 ][ i ] = sample / 32768.0;
				sample = packet[ index ] | ( packet[ index + 1 ] << 8 );
				index += 2;
				buffer[ 1 ][ i ] = sample / 32768.0;
			}

			vorbis_analysis_wrote( &vd, i );

			while ( vorbis_analysis_blockout( &vd, &vb ) == 1 ) {
#if HAVE_VORBIS >= 2
				vorbis_analysis( &vb, NULL );
				vorbis_bitrate_addblock( &vb );
				while ( vorbis_bitrate_flushpacket( &vd, &op ) ) {
#else
				// Support for very old libvorbis
				vorbis_analysis( &vb, &op );
				{
#endif
					ogg_stream_packetin( &os, &op );
					while ( ogg_stream_pageout( &os, &og ) ) {
						_file->writeBlock( reinterpret_cast<char *>( og.header ), og.header_len );
						_file->writeBlock( reinterpret_cast<char *>( og.body ), og.body_len );
					}
				}
			}
			QTimer::singleShot( 10, this, SLOT( process() ) );
		}
		return true;
	} else return false;
}
bool KRecExport_OGG::finalize() {
kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _file ) {
		ogg_stream_clear( &os );
		vorbis_block_clear( &vb );
		vorbis_dsp_clear( &vd );
		vorbis_info_clear( &vi );

		_file->close();
		delete _file;
		_file = 0;

		return true;
	} else return false;
}


// Derived from kdemultimedia/kioslave/audiocd/audiocd.cpp.
// We use the encoding settings from kcmaudiocd.
void KRecExport_OGG::setOggParameters() {
kdDebug( 60005 ) << k_funcinfo << endl;
	KConfig *config;
	config = new KConfig( "kcmaudiocdrc" );

	config->setGroup( "Vorbis" );

	// 0 for quality, 1 for managed bitrate
	int vorbis_encode_method = config->readNumEntry( "encmethod", 0 );
	// default quality level of 3, to match oggenc
	double vorbis_quality = config->readDoubleNumEntry( "quality",3.0 );

	int vorbis_bitrate_lower = -1;
	if (  config->readBoolEntry( "set_vorbis_min_bitrate",false ) )
		vorbis_bitrate_lower = config->readNumEntry( "vorbis_min_bitrate",40 ) * 1000;

	int vorbis_bitrate_upper = -1;
	if (  config->readBoolEntry( "set_vorbis_max_bitrate",false ) )
		vorbis_bitrate_upper = config->readNumEntry( "vorbis_max_bitrate",350 ) * 1000;

	// this is such a hack!
	int vorbis_bitrate;
	if (  vorbis_bitrate_upper != -1 && vorbis_bitrate_lower != -1 )
		vorbis_bitrate = 104000; // empirically determined ...?!
	else
		vorbis_bitrate = 160 * 1000;

	int vorbis_bitrate_nominal = -1;
	if (  config->readBoolEntry( "set_vorbis_nominal_bitrate",true ) ) {
		vorbis_bitrate_nominal = config->readNumEntry( "vorbis_nominal_bitrate",160 ) * 1000;
		vorbis_bitrate = vorbis_bitrate_nominal;
	}

	write_vorbis_comments = config->readBoolEntry( "vorbis_comments",true );

	vorbis_info_init(  &vi );
	switch (  vorbis_encode_method ) {
		case 0:
			// Support very old libvorbis by simply falling through
#if HAVE_VORBIS >= 2
			vorbis_encode_init_vbr(  &vi, 2/*this->channels()*/,
			                         44100/*this->samplingRate()*/,
			                         vorbis_quality/10.0 );
			break;
#endif
		case 1:
			vorbis_encode_init(  &vi, 2/*this->channels()*/,
			                     44100/*this->samplingRate()*/,
			                     vorbis_bitrate_upper, vorbis_bitrate_nominal,
			                     vorbis_bitrate_lower );
			break;
	}

	delete config;
}

#endif // vorbis

// vim:sw=4:ts=4
