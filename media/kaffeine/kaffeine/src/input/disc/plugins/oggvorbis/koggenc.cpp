/*
 * koggenc.cpp
 *
 * Copyright (C) 2002-2006 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <qslider.h>

#include <kparts/genericfactory.h>
#include <kiconloader.h>
#include <kpushbutton.h>

#include "koggenc.h"
#include "koggenc.moc"

OggSettings::OggSettings( QWidget *parent, KConfig *confile ) : OggConfig( parent )
{
	KIconLoader *icon = new KIconLoader();
	okBtn->setGuiItem( KGuiItem(i18n("OK"), icon->loadIconSet("ok", KIcon::Small) ) );
	cancelBtn->setGuiItem( KGuiItem(i18n("Cancel"), icon->loadIconSet("cancel", KIcon::Small) ) );
	delete icon;
	Conf = confile;
	Conf->setGroup("OggVorbis");
	oggSlid->setValue( Conf->readNumEntry( "Quality", 4 ) );
}

OggSettings::~OggSettings()
{
}

void OggSettings::accept()
{
	Conf->setGroup("OggVorbis");
	Conf->writeEntry( "Quality", oggSlid->value() );
	done( Accepted );
}

int OggSettings::getQuality()
{
	return oggSlid->value();
}

K_EXPORT_COMPONENT_FACTORY (libkaffeineoggvorbis, KParts::GenericFactory<KOggEnc>)

KOggEnc::KOggEnc( QWidget*, const char*, QObject* parent, const char* name, const QStringList& )
	: KaffeineAudioEncoder(parent,name)
{
	setInstance(KParts::GenericFactory<KOggEnc>::instance());
	encodingQuality = 0.4;
	bufEncode = new char[1];
	tmpBuf = new char[1];
}

KAboutData *KOggEnc::createAboutData()
{
	KAboutData* aboutData = new KAboutData( "kaffeineoggvorbis", I18N_NOOP("KaffeineOggVorbis"),
	                                        "0.1", I18N_NOOP("A Ogg Vorbis encoder plugin for Kaffeine."),
	                                        KAboutData::License_GPL,
	                                        "(c) 2006, Christophe Thommeret.", 0, "http://kaffeine.sourceforge.net");
	aboutData->addAuthor("Christophe Thommeret.",0, "hftom@free.fr");

	return aboutData;
}

QString KOggEnc::getExtension()
{
	return QString(".ogg");
}

bool KOggEnc::options( QWidget *parent, KConfig *conf )
{
	OggSettings dlg( parent, conf );
	int ret = dlg.exec();
	if ( ret!=QDialog::Accepted )
		return false;
	encodingQuality = dlg.getQuality()/10.0;
	return true;
}

void KOggEnc::start( QString title, QString artist, QString album, QString tracknumber, QString genre )
{
	char* tag;

	vorbis_info_init( &vi );
	vorbis_encode_init_vbr( &vi, 2, 44100, encodingQuality );
	/* add a comment */
	vorbis_comment_init( &vc );
	vorbis_comment_add_tag( &vc, "description", "Encoded by Kaffeine" );
	//vorbis_comment_add_tag( &vc, "vendor", "KOggEnc (Kilogram)" );
	/* set up the analysis state and auxiliary encoding storage */
	vorbis_analysis_init( &vd, &vi );
	vorbis_block_init( &vd, &vb );
	/* set up our packet->stream encoder */
	/* pick a random serial number; that way we can more likely build
	chained streams just by concatenation */
	srand( time(NULL) );
	ogg_stream_init( &os, rand() );
	if ( !title.isNull() ) {
		tag = qstrdup( title.utf8() );
		vorbis_comment_add_tag( &vc, "title", tag );
		delete [] tag;
	}
	if ( !artist.isNull() ) {
		tag = qstrdup( artist.utf8() );
		vorbis_comment_add_tag( &vc, "artist", tag );
		delete [] tag;
	}
	if ( !album.isNull() ) {
		tag = qstrdup( album.utf8() );
		vorbis_comment_add_tag( &vc, "album", tag );
		delete [] tag;
	}
	if ( !tracknumber.isNull() ) {
		tag = qstrdup( tracknumber.utf8() );
		vorbis_comment_add_tag( &vc, "tracknumber", tag );
		delete [] tag;
	}
	if ( !genre.isNull() ) {
		tag = qstrdup( genre.utf8() );
		vorbis_comment_add_tag( &vc, "genre", tag );
		delete [] tag;
	}
}

char* KOggEnc::getHeader( int &len )
{
	int buflen=0;

	vorbis_analysis_headerout( &vd, &vc, &header, &header_comm, &header_code );
	ogg_stream_packetin( &os, &header ); /* automatically placed in its own page */
	ogg_stream_packetin( &os, &header_comm );
	ogg_stream_packetin( &os, &header_code );

	while( ogg_stream_flush( &os, &og ) ){
		delete [] bufEncode;
		bufEncode = new char[ og.header_len+og.body_len+buflen ];
		//memcpy( mempcpy( mempcpy( bufEncode, tmpBuf, buflen ), og.header, og.header_len ), og.body, og.body_len );
		memcpy( (char*)memcpy( (char*)memcpy( bufEncode, tmpBuf, buflen )+buflen, og.header, og.header_len )+og.header_len, og.body, og.body_len );
		buflen+= og.header_len;
		buflen+= og.body_len;
		delete [] tmpBuf;
		tmpBuf = new char[ buflen ];
		memcpy( tmpBuf, bufEncode, buflen );
	}
	len = buflen;
	return bufEncode;
}

char* KOggEnc::encode( char *data, int datalen, int &len )
{
	int buflen=0;
	int i;
	float **buffer=vorbis_analysis_buffer( &vd, datalen/4 );

	for( i=0; i<datalen/4; i++ ){
		buffer[0][i] = ((data[i*4+1]<<8) | (0x00ff&(int)data[i*4]))/32768.f;
		buffer[1][i] = ((data[i*4+3]<<8) | (0x00ff&(int)data[i*4+2]))/32768.f;
	}
	vorbis_analysis_wrote( &vd, i );

	while( vorbis_analysis_blockout( &vd, &vb)==1 ) {
		vorbis_analysis( &vb, NULL );
		vorbis_bitrate_addblock( &vb );
		while( vorbis_bitrate_flushpacket( &vd, &op ) ) {
			ogg_stream_packetin( &os, &op );
			while( ogg_stream_pageout( &os, &og ) ) {
				delete [] bufEncode;
				bufEncode = new char[ og.header_len+og.body_len+buflen ];
				//memcpy( mempcpy( mempcpy( bufEncode, tmpBuf, buflen ), og.header, og.header_len ), og.body, og.body_len );
				memcpy( (char*)memcpy( (char*)memcpy( bufEncode, tmpBuf, buflen )+buflen, og.header, og.header_len )+og.header_len, og.body, og.body_len );
				buflen+= og.header_len;
				buflen+= og.body_len;
				delete [] tmpBuf;
				tmpBuf = new char[ buflen ];
				memcpy( tmpBuf, bufEncode, buflen );
			}
		}
	}
	len = buflen;
	return bufEncode;
}

char* KOggEnc::stop( int &len )
{
	int buflen=0;

	vorbis_analysis_wrote( &vd, 0 );
    	while( vorbis_analysis_blockout( &vd, &vb)==1 ) {
		vorbis_analysis( &vb, NULL );
		vorbis_bitrate_addblock( &vb );
		while( vorbis_bitrate_flushpacket( &vd, &op ) ) {
			ogg_stream_packetin( &os, &op );
			while( ogg_stream_pageout( &os, &og ) ) {
				delete [] bufEncode;
				bufEncode = new char[ og.header_len+og.body_len+buflen ];
				//memcpy( mempcpy( mempcpy( bufEncode, tmpBuf, buflen ), og.header, og.header_len ), og.body, og.body_len );
				memcpy( (char*)memcpy( (char*)memcpy( bufEncode, tmpBuf, buflen )+buflen, og.header, og.header_len )+og.header_len, og.body, og.body_len );
				buflen+= og.header_len;
				buflen+= og.body_len;
				delete [] tmpBuf;
				tmpBuf = new char[ buflen ];
				memcpy( tmpBuf, bufEncode, buflen );
			}
		}
	}
	ogg_stream_clear( &os );
	vorbis_block_clear( &vb );
	vorbis_dsp_clear( &vd );
	vorbis_comment_clear( &vc );
	vorbis_info_clear( &vi );
	len = buflen;
	if ( len>0 )
		return bufEncode;
	else
		return NULL;
}

KOggEnc::~KOggEnc()
{
	delete [] bufEncode;
	delete [] tmpBuf;
}
