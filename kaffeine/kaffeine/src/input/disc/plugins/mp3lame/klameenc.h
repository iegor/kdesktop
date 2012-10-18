/*
 * klameenc.h
 *
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
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

#ifndef KLAMEENC_H
#define KLAMEENC_H

#include <lame/lame.h>

#include <kconfig.h>

#include "kaffeineaudioencoder.h"
#include "lameconfig.h"

class LameSettings : public LameConfig
{
	Q_OBJECT
public:
	LameSettings( QWidget *parent, KConfig *confile );
	~LameSettings();

	int getBitrate();
	bool isVBR();

public slots:
	virtual void accept();

private:
	KConfig *Conf;
};

class KLameEnc : public KaffeineAudioEncoder
{
	Q_OBJECT

public:

	KLameEnc( QWidget*, const char*, QObject*, const char*, const QStringList& );
	~KLameEnc();

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

	char bufEncode[8000];
	lame_global_flags *flags;
	int bitrate;
	bool vbr;
};

#endif /* KLAMEENC_H */
