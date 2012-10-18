/*
 * channeldesc.h
 *
 * Copyright (C) 2003-2006 Christophe Thommeret <hftom@free.fr>
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

#ifndef CHANNELDESC_H
#define CHANNELDESC_H

#include <qstring.h>
#include <qptrlist.h>
#include <qdatetime.h>
#include <qvaluelist.h>
#include <qpixmap.h>

#include <linux/dvb/frontend.h>

#include "kaffeinedvbevents.h"

#define MAXAPID 20
#define MAXSUBPID 10

class RecTimer
{

public:

	QString name;
	QString fullPath;
	QString channel;
	QDateTime begin;
	QTime duration;
	char running;
	int mode;
};

class AudioPid
{

public:

	AudioPid();
	AudioPid( unsigned short apid );
	~AudioPid();

	unsigned short pid;
	QString lang;
	char ac3;
};

class SubPid
{

public:

	unsigned short pid, page, id;
	unsigned char type;
	QString lang;
};

class Transponder
{

public:

	Transponder();
	Transponder( const Transponder &trans ); //copy
	~Transponder();
	bool sameAs( Transponder *trans );
	bool operator==( const Transponder &t );
	bool operator!=( const Transponder &t );

	QString source;
	fe_type_t type; //   S, C or T
	unsigned long freq;
	QValueList<unsigned long> freqlist;
	char pol;
	unsigned long sr;
	unsigned short nid;
	unsigned short tsid;
	fe_spectral_inversion_t inversion;
	fe_modulation_t modulation;
	fe_hierarchy_t hierarchy;
	fe_guard_interval_t guard;
	fe_transmit_mode_t transmission;
	fe_code_rate_t coderateL;
	fe_code_rate_t coderateH;
	fe_bandwidth_t bandwidth;
	int snr;
	fe_rolloff_t rolloff;
	char S2;
};

class ChannelDesc
{

public:

	ChannelDesc();
	ChannelDesc( const ChannelDesc &chan ); //copy
	~ChannelDesc();

	QString provider;
	QString name;
	QString category;
	unsigned int num;
	unsigned short sid;
	unsigned short vpid;
	unsigned char vType; // video stream type
	AudioPid apid[MAXAPID];
	char napid;
	char maxapid;
	unsigned short ttpid;
	SubPid subpid[MAXSUBPID];
	char nsubpid;
	char maxsubpid;
	unsigned short pmtpid;
	unsigned char fta; // 0 for free
	unsigned char type; // 1 for TV , 2 for RA
	int completed;
	Transponder tp;
	QPixmap pix;
};

#endif /* CHANNELDESC_H */
