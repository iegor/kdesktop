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

#include "krecexport_mp3.h"
#include "krecexport_mp3.moc"

#include "krecglobal.h"

#include <kdebug.h>
#include <ktempfile.h>
#include <qfile.h>
#include <qtimer.h>
#include <kgenericfactory.h>
#include <stdlib.h>
#include <time.h>
#include <qdatetime.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kmessagebox.h>

// Error code definitions from lame.h
static const char *lame_error[] =
{
	I18N_NOOP( "Unknown encoding error." ),
	I18N_NOOP( "Buffer was too small." ),
	I18N_NOOP( "Memory allocation problem." ),
	I18N_NOOP( "Parameter initialisation not performed." ),
	I18N_NOOP( "Psycho acoustic problems." ),
	I18N_NOOP( "OGG cleanup encoding error." ),
	I18N_NOOP( "OGG frame encoding error" )
};

K_EXPORT_COMPONENT_FACTORY( libkrecexport_mp3, KGenericFactory<KRecExport_MP3> )

KRecExport_MP3 krecExportMP3( 0 );

KRecExport_MP3::KRecExport_MP3( QObject* p, const char* n, const QStringList& )
  : KRecExportItem( p,n )
  , _file( 0 )
  , error_occurred( false )
  , init_done( false )
{
kdDebug( 60005 ) << k_funcinfo << endl;
	registerAtGlobal( this );
kdDebug( 60005 ) << "Registered Exports: " << KRecGlobal::the()->exportFormats() << endl;
}
KRecExport_MP3::~KRecExport_MP3() {
kdDebug( 60005 ) << k_funcinfo << endl;
}

KRecExport_MP3* KRecExport_MP3::newItem() {
kdDebug( 60005 ) << k_funcinfo << endl;
	return new KRecExport_MP3( 0 );
}

QStringList KRecExport_MP3::extensions() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	QStringList tmp;
	tmp << "*.wav" << "*.WAV";
	return tmp;
}

bool KRecExport_MP3::initialize( const QString &filename ) {
kdDebug( 60005 ) << k_funcinfo << endl;
	if ( !_file &&
	     !( bits()!=16 && channels()!=2 &&
	        KMessageBox::warningContinueCancel( KRecGlobal::the()->mainWidget(),
	          i18n( "At this time MP3-Export only supports files in stereo and 16bit." )
	        ) == KMessageBox::Cancel
	     )
	) {
		KMessageBox::information( KRecGlobal::the()->mainWidget(),
		  i18n( "Please note that this plugin takes its qualitysettings from" \
		        " the corresponding section of the Audio CDs Control Center module" \
			" configuration. Make use" \
		        " of the Control Center to configure these settings." ),
		  i18n( "Quality Configuration" ), "qualityinfo_mp3" );
		_file = new QFile( filename );
		if ( _file->open( IO_Raw|IO_WriteOnly ) ) {
			if ( ! init_done ) {
				gfp = lame_init();
				setLameParameters();
				if ( write_id3 ) {
					id3tag_init( gfp );
					id3tag_v1_only ( gfp );
					id3tag_set_album  ( gfp, "" );
					id3tag_set_artist ( gfp, "" );
					id3tag_set_title  ( gfp, "" );
					id3tag_set_comment( gfp, "krec" );
				}
				/// Set input parameters right
				lame_set_in_samplerate( gfp, this->samplingRate() );
				lame_set_num_channels( gfp, this->channels() );
				lame_init_params( gfp );
			}
			if ( _file->size() >= 128 )
				_file->at( _file->size() - 128 );
			else
				_file->at( _file->size() );
		} else return false;
		return true;
	} else return false;
}
bool KRecExport_MP3::process() {
	//kdDebug( 60005 ) << k_funcinfo << running << endl;
	if ( _file ) {
		if ( running() ) {
			QByteArray bytearray( 4096 );
			emit getData( bytearray );
			int mp3bytes = lame_encode_buffer_interleaved( gfp,
			                 reinterpret_cast<short int *>( bytearray.data() ),
			                 bytearray.size() >> 2, mp3buf, sizeof( mp3buf ) );
			if ( mp3bytes > 0 )
			_file->writeBlock( reinterpret_cast<char *>( mp3buf ), mp3bytes );
			if ( mp3bytes < 0 && ! error_occurred ) {
				int code = ( mp3bytes < -6 ) ? 0 : -mp3bytes;
				KMessageBox::detailedError( 0, i18n( "MP3 encoding error." ),
				                               i18n( lame_error[ code ] ) );
				error_occurred = true;
			}
			QTimer::singleShot( 10, this, SLOT( process() ) );
		}
		return true;
	} else return false;
}
bool KRecExport_MP3::finalize() {
kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _file ) {
		int mp3bytes = lame_encode_flush( gfp, mp3buf, sizeof( mp3buf ) );
		if ( mp3bytes > 0 )
			_file->writeBlock( reinterpret_cast<char *>( mp3buf ), mp3bytes );
		if ( mp3bytes < 0 && ! error_occurred ) {
			int code = ( mp3bytes < -6 ) ? 0 : -mp3bytes;
			KMessageBox::detailedError( 0, i18n( "MP3 encoding error." ),
			                               i18n( lame_error[ code ] ) );
			error_occurred = true;
		}
		lame_close( gfp );

		_file->close();
		delete _file;
		_file = 0;

		return true;
	} else return false;
}

// Derived from kdemultimedia/kioslave/audiocd/audiocd.cpp.
// We use the encoding settings from kcmaudiocd.
void KRecExport_MP3::setLameParameters() {
	KConfig *config;
	config = new KConfig( "kcmaudiocdrc" );

	config->setGroup( "MP3" );

	int quality = config->readNumEntry( "quality",2 );

	if ( quality < 0 ) quality = 0;
	if ( quality > 9 ) quality = 9;

	int method = config->readNumEntry( "encmethod",0 );
	if ( method == 0 ) {
		// Constant Bitrate Encoding
		lame_set_VBR( gfp, vbr_off );
		lame_set_brate( gfp,config->readNumEntry( "cbrbitrate",160 ) );
		lame_set_quality( gfp, quality );
	} else {
		// Variable Bitrate Encoding
		if ( config->readBoolEntry( "set_vbr_avr",true ) ) {
		lame_set_VBR( gfp,vbr_abr );
		lame_set_VBR_mean_bitrate_kbps( gfp,
		        config->readNumEntry( "vbr_average_bitrate",0 ) );
		} else {
			if ( lame_get_VBR( gfp ) == vbr_off ) lame_set_VBR( gfp, vbr_default );
			if ( config->readBoolEntry( "set_vbr_min",true ) )
				lame_set_VBR_min_bitrate_kbps( gfp,
				        config->readNumEntry( "vbr_min_bitrate",0 ) );
			if ( config->readBoolEntry( "vbr_min_hard",true ) )
				lame_set_VBR_hard_min( gfp, 1 );
			if ( config->readBoolEntry( "set_vbr_max",true ) )
				lame_set_VBR_max_bitrate_kbps( gfp,
				        config->readNumEntry( "vbr_max_bitrate",0 ) );
			lame_set_VBR_q( gfp, quality );
		}

		if ( config->readBoolEntry( "write_xing_tag",true ) )
			 lame_set_bWriteVbrTag( gfp, 1 );
	}

	switch ( config->readNumEntry( "mode",0 ) ) {
		case 0: lame_set_mode( gfp, STEREO );       break;
		case 1: lame_set_mode( gfp, JOINT_STEREO ); break;
		case 2: lame_set_mode( gfp, DUAL_CHANNEL ); break;
		case 3: lame_set_mode( gfp, MONO );         break;
		default: lame_set_mode( gfp,STEREO );       break;
	}

	lame_set_copyright( gfp, config->readBoolEntry( "copyright",false ) );
	lame_set_original( gfp, config->readBoolEntry( "original",true ) );
	lame_set_strict_ISO( gfp, config->readBoolEntry( "iso",false ) );
	lame_set_error_protection( gfp, config->readBoolEntry( "crc",false ) );

	write_id3 = config->readBoolEntry( "id3",true );

	if ( config->readBoolEntry( "enable_lowpassfilter",false ) ) {
		lame_set_lowpassfreq( gfp,
		                      config->readNumEntry( "lowpassfilter_freq",0 ) );
		if ( config->readBoolEntry( "set_lowpassfilter_width",false ) )
			lame_set_lowpasswidth( gfp,
			                       config->readNumEntry( "lowpassfilter_width",0 ) );
	}

	if ( config->readBoolEntry( "enable_highpassfilter",false ) ) {
		lame_set_highpassfreq( gfp,
		                       config->readNumEntry( "highpassfilter_freq",0 ) );
		if ( config->readBoolEntry( "set_highpassfilter_width",false ) )
			lame_set_highpasswidth( gfp,
			                        config->readNumEntry( "highpassfilter_width",0 ) );
	}

	delete config;
}

// vim:sw=4:ts=4
