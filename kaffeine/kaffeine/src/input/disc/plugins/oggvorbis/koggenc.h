/*
 * koggenc.h
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

#ifndef KOGGENC_H
#define KOGGENC_H

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vorbis/vorbisenc.h>
}

#include <kconfig.h>

#include "kaffeineaudioencoder.h"
#include "oggconfig.h"

class OggSettings : public OggConfig
{
	Q_OBJECT
public:
	OggSettings( QWidget *parent, KConfig *confile );
	~OggSettings();

	int getQuality();

public slots:
	virtual void accept();

private:
	KConfig *Conf;
};

class KOggEnc : public KaffeineAudioEncoder
{
	Q_OBJECT

public:

	KOggEnc( QWidget*, const char*, QObject*, const char*, const QStringList& );
	~KOggEnc();

	// Reimplemented from KaffeineAudioEncoder
	bool options( QWidget*, KConfig* );
	QString getExtension();
	void start( QString title=0, QString artist=0, QString album=0, QString tracknumber=0, QString genre=0 );
	char* getHeader( int &len );
	char* encode( char *data, int datalen, int &len );
	char* stop( int &len );
	//****************************

	static KAboutData* createAboutData();

private:

	ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
	ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */
	vorbis_info      vi; /* struct that stores all the static vorbis bitstream settings */
	vorbis_comment   vc; /* struct that stores all the user comments */
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */
	ogg_packet header;
	ogg_packet header_comm;
	ogg_packet header_code;
	char *bufEncode;
	char *tmpBuf;
	float encodingQuality;
};

#endif /* KOGGENC_H */
