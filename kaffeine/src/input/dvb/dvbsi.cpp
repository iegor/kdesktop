/*
 * dvbsi.cpp
 *
 * Copyright (C) 2003-2007 Christophe Thommeret <hftom@free.fr>
 * Copyright (C) 2008 Devin Heitmueller <devin.heitmueller@gmail.com>
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

#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>

#include <qdir.h>

#include <kapplication.h>

#include "dvbsi.h"

#define TIMER_EVENT_SCAN_END 100

// These values are taken from ATSC A/65C Sec 5
#define PSIP_BASE_PID 0x1ffb

// This shouldn't be necessary, but at least the HVR-950 doesn't seem to
// find them in the time specified in the spec (needs more investigation)
#define CYCLE_TIME_FUDGEFACTOR 1000

// These values are taken from ATSC A/65C Sec 7.1
// (all values in ms)
#define PSIP_MAX_CYCLE_TIME_MGT 150 + CYCLE_TIME_FUDGEFACTOR
#define PSIP_MAX_CYCLE_TIME_VCT 400 + CYCLE_TIME_FUDGEFACTOR

// These values are taken from ATSC A/65C Sec 4.1
#define PSIP_TABLE_TYPE_MGT     0xc7
#define PSIP_TABLE_TYPE_TVCT    0xc8
#define PSIP_TABLE_TYPE_CVCT    0xc9

NitSection::NitSection( QPtrList<Transponder> *tp, bool *end, bool *ok, int anum, int tnum ) : KaffeineDVBsection( anum, tnum )
{
	ended = end;
	transponders = tp;
	start();
}



NitSection::~NitSection()
{
	stop();
}



void NitSection::stop()
{
	if ( !wait(10000) ) {
		terminate();
		wait();
	}
}



void NitSection::run()
{
	getSection( 0x10, 0x40, 60000 );
	*ended = true;
}



bool NitSection::getSection( int pid, int tid, int timeout )
{
	unsigned char buf[4096];
	int i, n=0;
	int skip=0;
	int last=1, current=0, loop=0;
	QValueList<int> list;

	if ( !setFilter( pid, tid, timeout ) )
		return false;

	do {
		if ( poll(pf,1,timeout)>0 ){
			if ( pf[0].revents & POLLIN ){
				n = read( fdDemux, buf, 4096 );
				skip = 0;
			}
			else
				skip++;
		}
		else
			skip++;

		if ( skip || n<4 ) {
			fprintf(stderr,"\nInvalid section length or timeout: pid=%d\n\n", pid);
			stopFilter();
			return false;
		}

		last = getBits(buf,56,8);
		current = getBits(buf,48,8);
		for ( i=0; i<(int)list.count(); i++ ) {
			if ( current==list[i] ) {
				i = -1;
				break;
			}
		}
		if ( i>-1 )
			list.append( current );
		else {
			loop++; // missing section ?
			continue;
		}
		switch ( getBits(buf,0,8) ) {
			case 0x40 :
				fprintf(stderr,"Reading NIT: pid=%d\n", pid);
				tableNIT( buf );
				break;
			default:
				break;
		}
		loop = 0;
	} while ( (int)list.count()<=last && loop<=last );

	stopFilter();
	return true;
}



bool NitSection::tableNIT( unsigned char* buf )
{
	int length, loop, i, j;
	Transponder *trans, *curtrans;

	loop = getBits(buf,68,12);
	buf +=10+loop;
	length = getBits(buf,4,12);
	buf +=2;

	while ( length>0 ) {
		trans = new Transponder();
		trans->source = transponders->at(0)->source;
		trans->tsid = getBits(buf,0,16);
		loop = getBits(buf,36,12);
		buf +=6;
		length -=(6+loop);
		while ( loop>0 ) {
			switch ( getBits(buf,0,8) ) {
				case 0x43 :
					satelliteDesc( buf, trans );
					break;
				case 0x44 :
					cableDesc( buf, trans );
					break;
				case 0x5a :
					terrestrialDesc( buf, trans );
					break;
				case 0x62 :
					fprintf(stderr,"     Found frequency list.\n");
					freqListDesc( buf, trans );
					break;
				case 0x79 :
					S2satelliteDesc( buf, trans );
					break;
				default :
					break;
			}
			loop -=( getBits(buf,8,8)+2 );
			buf +=( getBits(buf,8,8)+2 );
		}
		if ( trans->freq==0 ) {
			delete trans;
			continue;
		}
		curtrans = 0;
		for ( i=0; i<(int)transponders->count(); i++ ) {
			if ( trans->tsid==transponders->at(i)->tsid || trans->sameAs( transponders->at(i) ) ) {
				curtrans = transponders->at(i);
				break;
			}
		}
		if ( !curtrans )
			transponders->append( trans );
		else {
			for ( i=0; i<(int)trans->freqlist.count(); i++ ) {
				for ( j=0; j<(int)curtrans->freqlist.count(); j++ ) {
					if ( curtrans->freqlist[j]==trans->freqlist[i] ) {
						j = -1;
						break;
					}
				}
				if ( j!=-1 ) {
					fprintf(stderr,"          Appending freq %lu to %lu\n", trans->freqlist[i], curtrans->freq );
					curtrans->freqlist.append( trans->freqlist[i] );
				}
			}
			delete trans;
		}
	}

	return true;
}



void NitSection::satelliteDesc( unsigned char* buf, Transponder *trans )
{
	QString s, t;

	trans->type = FE_QPSK;
	s = t.setNum( getBits(buf,16,32), 16 );
	trans->freq = s.toInt();
	trans->freq /=100;
	if ( getBits(buf,65,2) )
		trans->pol = 'v';
	else
		trans->pol = 'h';
	switch ( getBits(buf,70,2) ) {
		case 0 : trans->modulation = QAM_AUTO; break;
		case 1 : trans->modulation = QPSK; break;
		case 2 : trans->modulation = PSK_8; break;
		case 3 : trans->modulation = QAM_16; break;
	}
	s = t.setNum( getBits(buf,72,28), 16 );
	trans->sr = s.toInt();
	trans->sr /=10;
	switch ( getBits(buf,100,4) ) {
		case 0 : trans->coderateH = FEC_AUTO; break;
		case 1 : trans->coderateH = FEC_1_2; break;
		case 2 : trans->coderateH = FEC_2_3; break;
		case 3 : trans->coderateH = FEC_3_4; break;
		case 4 : trans->coderateH = FEC_5_6; break;
		case 5 : trans->coderateH = FEC_7_8; break;
		case 6 : trans->coderateH = FEC_8_9; break;
		case 7 : trans->coderateH = FEC_3_5; break;
		case 8 : trans->coderateH = FEC_4_5; break;
		case 9 : trans->coderateH = FEC_9_10; break;
		case 15 : trans->coderateH = FEC_NONE; break;
	}
	if ( getBits(buf,69,1) ) {
		fprintf(stderr,"!!!!!!!!!!!!!!!!!! Found S2 MODULATION SYSTEM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		trans->S2 = 1;
		switch ( getBits(buf,67,2) ) {
			case 0 : trans->rolloff = ROLLOFF_35; break;
			case 1 : trans->rolloff = ROLLOFF_25; break;
			case 2 : trans->rolloff = ROLLOFF_20; break;
		}
	}
}



void NitSection::S2satelliteDesc( unsigned char* buf, Transponder *trans )
{
	fprintf(stderr,"!!!!!!!!!!!!!!!!!! Found S2 DESCRIPTOR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	int scrambling_sequence_selector = getBits(buf,16,1);
	int multiple_input_stream_flag = getBits(buf,17,1);
	int backwards_compatibility_indicator = getBits(buf,18,1);
	int scrambling_sequence_index = 0;
	if ( scrambling_sequence_selector )
		scrambling_sequence_index = getBits(buf,30,18);
	int input_stream_identifier = 0;
	if ( multiple_input_stream_flag )
		input_stream_identifier = getBits(buf,48,8);
}



void NitSection::cableDesc( unsigned char* buf, Transponder *trans )
{
	QString s, t;

	trans->type = FE_QAM;
	s = t.setNum( getBits(buf,16,32), 16 );
	trans->freq = s.toInt();
	trans->freq /=10;
	switch ( getBits(buf,64,8) ) {
		case 1 : trans->modulation = QAM_16; break;
		case 2 : trans->modulation = QAM_32; break;
		case 3 : trans->modulation = QAM_64; break;
		case 4 : trans->modulation = QAM_128; break;
		case 5 : trans->modulation = QAM_256; break;
	}
	s = t.setNum( getBits(buf,72,28), 16 );
	trans->sr = s.toInt();
	trans->sr /=10;
	switch ( getBits(buf,100,4) ) {
		case 0 : trans->coderateH = FEC_AUTO; break;
		case 1 : trans->coderateH = FEC_1_2; break;
		case 2 : trans->coderateH = FEC_2_3; break;
		case 3 : trans->coderateH = FEC_3_4; break;
		case 4 : trans->coderateH = FEC_5_6; break;
		case 5 : trans->coderateH = FEC_7_8; break;
		case 6 : trans->coderateH = FEC_8_9; break;
		case 7 : trans->coderateH = FEC_3_5; break;
		case 8 : trans->coderateH = FEC_4_5; break;
		case 9 : trans->coderateH = FEC_9_10; break;
		case 15 : trans->coderateH = FEC_NONE; break;
	}
}



void NitSection::terrestrialDesc( unsigned char* buf, Transponder *trans )
{
	trans->type = FE_OFDM;
	trans->freq = getBits(buf,16,32)/100;
	trans->bandwidth = (fe_bandwidth_t)(BANDWIDTH_8_MHZ + getBits(buf,48,3));
	switch ( getBits(buf,56,2) ) {
		case 0 : trans->modulation = QPSK; break;
		case 1 : trans->modulation = QAM_16; break;
		case 2 : trans->modulation = QAM_64; break;
	}
	trans->hierarchy = (fe_hierarchy_t)(HIERARCHY_NONE + getBits(buf,58,3));
	switch ( getBits(buf,61,3) ) {
		case 0 : trans->coderateH = FEC_1_2; break;
		case 1 : trans->coderateH = FEC_2_3; break;
		case 2 : trans->coderateH = FEC_3_4; break;
		case 3 : trans->coderateH = FEC_5_6; break;
		case 4 : trans->coderateH = FEC_7_8; break;
	}
	switch ( getBits(buf,64,3) ) {
		case 0 : trans->coderateL = FEC_1_2; break;
		case 1 : trans->coderateL = FEC_2_3; break;
		case 2 : trans->coderateL = FEC_3_4; break;
		case 3 : trans->coderateL = FEC_5_6; break;
		case 4 : trans->coderateL = FEC_7_8; break;
	}
	trans->guard = (fe_guard_interval_t)(GUARD_INTERVAL_1_32 + getBits(buf,67,2));
	switch ( getBits(buf,69,2) ) {
		case 0 : trans->transmission = TRANSMISSION_MODE_2K; break;
		case 1 : trans->transmission = TRANSMISSION_MODE_8K; break;
	}
}



void NitSection::freqListDesc( unsigned char* buf, Transponder *trans )
{
	unsigned char len, type;
	unsigned char *b=buf;
	QString s, t;
	unsigned long freq;

	len = getBits(b,8,8);
	type = getBits(b,22,2);
	len-= 1;
	b+= 3;
	while ( len>0 ) {
		switch ( type ) {
			case 1: // satellite
				s = t.setNum( getBits(b,0,32), 16 );
				freq = s.toInt();
				freq /=100;
				fprintf( stderr, "          %lu\n", freq );
				trans->freqlist.append( freq );
				break;
			case 2: // cable
				s = t.setNum( getBits(b,0,32), 16 );
				freq = s.toInt();
				freq /=10;
				fprintf( stderr, "          %lu\n", freq );
				trans->freqlist.append( freq );
				break;
			case 3: // terrestrial
				freq = getBits(b,0,32)/100;
				fprintf( stderr, "          %lu\n", freq );
				trans->freqlist.append( freq );
				break;
		}
		len-= 4;
		b+= 4;
	}
}



DVBsi::DVBsi( bool *ok, int anum, int tnum, DvbStream *d, const QString &charset ) : KaffeineDVBsection( anum, tnum, charset )
{
	channels.setAutoDelete( true );
	transponders.setAutoDelete( true );
	isRunning = false;
	dvb = d;
	adapter = anum;
	tuner = tnum;
	ns = 0;
}



DVBsi::~DVBsi()
{
	isRunning = false;
	wait();
	channels.clear();
	transponders.clear();
}



void DVBsi::serviceDesc( unsigned char* buf, ChannelDesc *desc )
{
	unsigned int i, j;

	desc->type = getBits(buf,16,8);
	i = getBits(buf,24,8);
	desc->provider = getText( buf+4, i ).stripWhiteSpace();
	j = getBits(buf+i,32,8);
	desc->name = getText( buf+5+i, j ).stripWhiteSpace();
	if ( desc->name.isEmpty() )
		desc->name = "Unknown";
	fprintf(stderr,"%s: sid=%d\n", desc->name.latin1(), desc->sid );
}



bool DVBsi::tableSDT( unsigned char* buf )
{
	int length, loop;
	ChannelDesc *desc;
	QString s;
	unsigned short tsid;
	unsigned short nid;

	tsid = getBits(buf+3,0,16);
	nid = getBits(buf+8,0,16);
	length = getBits(buf,12,12);
	length -=8;
	buf +=11;

	while ( length>4 ) {
		desc = new ChannelDesc();
		channels.append( desc );
		desc->tp.tsid = tsid;
		desc->tp.nid = nid;
		desc->sid = getBits(buf,0,16);
		//desc->fta = getBits(buf,27,1 );
		loop = getBits(buf,28,12);
		buf +=5;
		length -=(5+loop);
		while ( loop>0 ) {
			switch ( getBits(buf,0,8) ) {
				case 0x48 :
					serviceDesc( buf, desc );
					break;
				default :
					break;
			}
			loop -=( getBits(buf,8,8)+2 );
			buf +=( getBits(buf,8,8)+2 );
		}
	}
	return true;
}



bool DVBsi::tablePMT( unsigned char* buf )
{
	int length, loop;
	int type;
	int sid;
	int i;
	int pid=0;
	ChannelDesc *desc=0;
	QString lang;
	int ns;
	unsigned char st;
	bool audio, ac3;


	sid = getBits(buf+3,0,16);
	for ( i=indexChannels; i<(int)channels.count(); i++ ) {
		if ( channels.at( i )->sid==sid ) {
			desc = channels.at( i );
			break;
		}
	}

	if ( !desc )
		return false;

	length = getBits(buf,12,12);
	loop = getBits(buf+10,4,12);
	length -=(9+loop);
	buf +=12;

	while ( loop>0 ) {
		switch ( getBits(buf,0,8) ) {
			case 0x09 :
				desc->fta = 1;
				break;
			default :
				break;
		}
		loop -=( getBits(buf,8,8)+2 );
		buf +=( getBits(buf,8,8)+2 );
	}

	while ( length>4 ) {
		audio=ac3=false;
		lang="";
		type = getBits(buf,0,8);
		pid = getBits(buf,11,13);
		if ( type==1/*mpeg1*/ || type==2/*mpeg2*/ || type==16/*mpeg4*/ || type==27/*h264*/ ) {
			desc->type=1;
			desc->vpid = pid;
			desc->vType = type;
		}
		if ( type==3 || type==4 ) {
			audio = true;
		}
		if (type == 0x81) {
		  // AC3 was added in ATSC A/52B (See A3.1 "Stream Type")
		  audio = true;
		  ac3 = true;
		}
		loop = getBits(buf,28,12);
		buf +=5;
		length -=(5+loop);
		while ( loop>0 ) {
			switch ( getBits(buf,0,8) ) {
				case 0x09 :
					desc->fta = 1;
					break;
				case 0x0A :
					lang = langDesc( buf );
					break;
				case 0x56 :
					if ( type==6 )
						desc->ttpid = pid;
					 break;
				case 0x59 :  // DVB subtitle descriptor
					ns = (int)desc->nsubpid;
					if ( type==6 && ns<desc->maxsubpid ) {
						st = getBits(buf+5,0,8);
						if ( st>=0x10 && st<=0x23 ) {
							desc->subpid[ns].type = st;
							desc->subpid[ns].pid = pid;
							desc->subpid[ns].page = getBits(buf+6,0,16);
							desc->subpid[ns].id = getBits(buf+8,0,16);
							desc->subpid[ns].lang = langDesc(buf);
							fprintf( stderr, "\nDVB SUB on %s page_id: %d anc_id: %d lang: %s\n\n",
								desc->name.latin1(), desc->subpid[ns].page, desc->subpid[ns].id,
								desc->subpid[ns].lang.latin1() );
							desc->nsubpid++;
						}
					}
					break;
				case 0x6a :
				case 0x81 :
					audio = true;
					ac3 = true;
					break;
				default :
					break;
			}
			loop -=( getBits(buf,8,8)+2 );
			buf +=( getBits(buf,8,8)+2 );
		}
		if ( audio && desc->napid<desc->maxapid ) {
			if ( !desc->vpid )
				desc->type=2;
			desc->apid[(int)desc->napid].pid = pid;
			if ( ac3 )
				desc->apid[(int)desc->napid].ac3 = 1;
			if ( !lang.isEmpty() )
				desc->apid[(int)desc->napid].lang = lang;
			desc->napid++;
		}
	}

	return true;
}



bool DVBsi::tablePAT( unsigned char *buf )
{
	int length, i, sid, tsid, pmt;
	bool got;
	ChannelDesc *desc;

	tsid = getBits(buf+3,0,16);
	length = getBits(buf,12,12);
	length -=5;
	buf +=8;

	while ( length>4 ) {
		sid = getBits(buf,0,16);
		pmt = getBits(buf,19,13);
		buf +=4;
		length -=4;
		got = false;
		for ( i=indexChannels; i<(int)channels.count(); i++ ) {
			if ( channels.at( i )->sid==sid ) {
				channels.at( i )->pmtpid = pmt;
				got = true;
				break;
			}
		}
		if ( !got  && sid!=0 ) {
			desc = new ChannelDesc();
			channels.append( desc );
			desc->tp.tsid = tsid;
			desc->sid = sid;
			desc->pmtpid = pmt;
			desc->name = QString("TSID:%1-SID:%2").arg(tsid).arg(sid);
		}
	}

	return true;
}



bool DVBsi::getSection( int pid, int tid, int timeout, int sid )
{
	unsigned char buf[4096];
	int i, n=0;
	int skip=0;
	int last=1, current=0, loop=0;
	QValueList<int> list, sidList;
	int cursid;

	if ( !setFilter( pid, tid, timeout ) )
		return false;

	do {
		if ( poll(pf,1,timeout)>0 ){
			if ( pf[0].revents & POLLIN ){
				n = read( fdDemux, buf, 4096 );
				skip = 0;
			}
			else
				skip++;
		}
		else
			skip++;

		if ( skip || n<4 ) {
			fprintf(stderr,"\nInvalid section length or timeout: pid=%d\n\n", pid);
			stopFilter();
			return false;
		}

		cursid = getBits(buf+3,0,16);
		if ( sid && cursid!=sid && !sidList.contains(cursid) ) {
			sidList.append( cursid );
			continue;
		}

		last = getBits(buf,56,8);
		current = getBits(buf,48,8);
		for ( i=0; i<(int)list.count(); i++ ) {
			if ( current==list[i] ) {
				i = -1;
				break;
			}
		}
		if ( i>-1 )
			list.append( current );
		else {
			loop++; // missing section ?
			continue;
		}
		switch ( getBits(buf,0,8) ) {
			case 0x42 :
				fprintf(stderr,"Reading SDT: pid=%d\n", pid);
				tableSDT( buf );
				break;
			case 0x00 :
				fprintf(stderr,"Reading PAT: pid=%d\n", pid);
				tablePAT( buf );
				break;
			case 0x02 :
				fprintf(stderr,"Reading PMT: pid=%d\n", pid);
				tablePMT( buf );
				break;
			default:
				break;
		}
		loop = 0;
	} while ( (int)list.count()<=last && loop<=last );

	stopFilter();
	return true;
}

bool DVBsi::parseMGT( int pid, int tid, int timeout, int sid )
{
	unsigned char buf[4096];
	int n=0;
	int skip=0;
	bool vct_found = false;

	fprintf(stderr, "parseMGT called for 0x%02x 0x%02x\n", pid, tid);
	if ( !setFilter( pid, tid, timeout ) )
		return false;

	if ( poll(pf,1,timeout)>0 ){
		if ( pf[0].revents & POLLIN ){
			n = read( fdDemux, buf, 4096 );
			skip = 0;
		}
		else
			skip++;
	}
	else
		skip++;

	if ( skip || n<4 ) {
		fprintf(stderr,"\nInvalid section length or timeout: pid=%d\n\n", pid);
		stopFilter();
		return false;
	}

	// Parse the Master Guide Table Section
	unsigned int protocol_version = getBits(buf,64,8);
	unsigned int tables_defined = getBits(buf,72,16);
	fprintf(stderr, "protocol_version = %d\n", protocol_version);
	fprintf(stderr, "tables_defined = %d\n", tables_defined);

	// Now let's go through the table entries....
	unsigned char *t_entry = &buf[88/8];
	for (unsigned int t = 0; t < tables_defined; t++) {
		unsigned int table_type;
		unsigned int table_pid;
		unsigned int table_version_number;
		unsigned int table_number_bytes;
		unsigned int table_number_des_length;
		table_type = getBits(t_entry, 0, 16);
		table_pid = getBits(t_entry, 19, 13);
		table_version_number = getBits(t_entry, 35, 5);
		table_number_bytes = getBits(t_entry, 40, 32);
		table_number_des_length = getBits(t_entry, 76, 12);
		fprintf(stderr,
			"MGT entry type=0x%04x pid=0x%04x ver=%d sz=%d\n",
			table_type, table_pid, table_version_number,
			table_number_bytes);

		if (table_type == 0x0000 || table_type == 0x0001) {
			// TVCT table found
			vct_table = PSIP_TABLE_TYPE_TVCT;
			vct_found = true;
		} else if (table_type == 0x0002 ||
			   table_type == 0x0003) {
			// CVCT table found
			vct_table = PSIP_TABLE_TYPE_CVCT;
			vct_found = true;
		}
		t_entry += (11 + table_number_des_length);
	}

	stopFilter();
	return vct_found;
}

bool DVBsi::parseVCT( int pid, int tid, int timeout, int sid )
{
	unsigned char buf[4096];
	int n=0;
	int skip=0;

	fprintf(stderr, "parseVCT called for 0x%02x 0x%02x\n", pid, tid);
	if ( !setFilter( pid, tid, timeout ) )
		return false;

	if ( poll(pf,1,timeout)>0 ){
		if ( pf[0].revents & POLLIN ){
			n = read( fdDemux, buf, 4096 );
			skip = 0;
		}
		else
			skip++;
	}
	else
		skip++;

	if ( skip || n<4 ) {
		fprintf(stderr,"\nInvalid section length or timeout: pid=%d\n\n", pid);
		stopFilter();
		return false;
	}

	// Parse the Virtual Channel Table Section
	unsigned int protocol_version = getBits(buf,64,8);
	unsigned int num_channels = getBits(buf,72,8);
	fprintf(stderr, "protocol_version = %d\n", protocol_version);
	fprintf(stderr, "num_channels = %d\n", num_channels);

	// Now let's go through the table entries....
	unsigned char *t_entry = &buf[80/8];
	for (unsigned int t = 0; t < num_channels; t++) {
		char short_name[8];
		unsigned int major_channel_num;
		unsigned int minor_channel_num;
		unsigned int modulation_mode;
		unsigned int channel_tsid;
		unsigned int program_number;
		unsigned int access_controlled;
		unsigned int hidden;
		unsigned int service_type;
		unsigned int source_id;
		unsigned int reserved;
		unsigned int descriptors_length;

		// Short name
		// Yes, I need a real UCS-2 to UTF-8 conversion here...
		memset(short_name, 0, sizeof(short_name));
		snprintf(short_name, sizeof(short_name), "%c%c%c%c%c%c%c",
			 t_entry[1],t_entry[3],t_entry[5],t_entry[7],
			 t_entry[9],t_entry[11],t_entry[13]);
		reserved = getBits(t_entry, 112, 4);
		major_channel_num = getBits(t_entry, 116, 10);
		minor_channel_num = getBits(t_entry, 126, 10);
		modulation_mode = getBits(t_entry, 136, 8);
		channel_tsid = getBits(t_entry, 176, 16);
		program_number = getBits(t_entry, 192, 16);
		access_controlled = getBits(t_entry, 210, 1);
		hidden = getBits(t_entry, 211, 1);
		service_type = getBits(t_entry, 218, 6);
		source_id = getBits(t_entry, 224, 16);
		reserved = getBits(t_entry, 240, 6);
		descriptors_length = getBits(t_entry, 246, 10);

		fprintf(stderr, "short name=%s\n", short_name);
		fprintf(stderr, "reserved=0x%04x\n", reserved);
		fprintf(stderr, "major=%d\n", major_channel_num);
		fprintf(stderr, "minor=%d\n", minor_channel_num);
		fprintf(stderr, "modulation mode=0x%02x\n", modulation_mode);
		fprintf(stderr, "channel_tsid=0x%04x\n", channel_tsid);
		fprintf(stderr, "program_number=0x%04x\n", program_number);
		fprintf(stderr, "access_controlled=0x%01x\n", access_controlled);
		fprintf(stderr, "hidden=0x%01x\n", hidden);
		fprintf(stderr, "service_type=0x%02x\n", service_type);
		fprintf(stderr, "source_id=0x%04x\n", source_id);
		fprintf(stderr, "des length=%d\n", descriptors_length);

		ChannelDesc *desc = new ChannelDesc();
		desc->tp.tsid = channel_tsid;
		desc->name = QString("%1-%2 %3").arg(major_channel_num).arg(minor_channel_num).arg(short_name);
		desc->sid = program_number;
		if (access_controlled == 1)
			desc->fta = 1;
		else
			desc->fta = 0; // 0 for free

		// Algorithm taken from ATSC A/65C Sec 4.2
		// However, the algorithm doesn't appear correct, as it
		// truncates out data.  For example, both 68-1 and 4-1 would
		// yield the same one_part_number
		desc->num = (major_channel_num << 10) + minor_channel_num;
		fprintf(stderr, "channel num=%d\n", desc->num);

		// ATSC A/65C Sec 6.3.1 Table 6.7
		if (service_type == 0x01) {
			// Analog television (not supported)
		} else if (service_type == 0x02) {
			// ATSC Video
			desc->type = 1;
		} else if (service_type == 0x03) {
			// ATSC Audio
			desc->type = 2;
		} else if (service_type == 0x02) {
			// ATSC Data only service (not supported)
		} else if (service_type > 0x05 && service_type < 0x3f) {
			// Reserved (not supported)
		} else {
			// Unknown
		}

		// Now add the new channel to the list (if supported)
		if ((desc->type == 1 || desc->type == 2) && hidden == 0) {
			channels.append( desc );
		} else {
			fprintf(stderr, "Not adding channel\n");
			delete desc;
		}

		// Advance to the next entry
		t_entry += (32 + descriptors_length);
	}

	stopFilter();
	return true;
}

void DVBsi::stop()
{
	if ( !isRunning )
		return;

	isRunning = false;
	if ( !wait(10000) ) {
		if ( ns ) {
			ns->stop();
			delete ns;
			ns = 0;
		}
		terminate();
		wait();
		if ( scanMode<2 )
			dvb->stopScan();
	}
}



void DVBsi::out( bool stopscan )
{
	if ( ns ) {
		ns->stop();
		delete ns;
		ns = 0;
	}
	if ( stopscan )
		dvb->stopScan();
	KApplication::kApplication()->postEvent( this, new QTimerEvent( TIMER_EVENT_SCAN_END ) );
}



void DVBsi::go( QPtrList<Transponder> trans, int mode )
{
	int i;

	if ( isRunning )
		return;

	scanMode = mode;
	transponders.clear();
	for ( i=0; i<(int)trans.count(); i++ )
		transponders.append( new Transponder( *trans.at(i) ) );
	channels.clear();
	isRunning = true;
	start();
}



void DVBsi::timerEvent( QTimerEvent *e )
{
	switch ( e->timerId() ) {
		case TIMER_EVENT_SCAN_END :
			emit end( true );
			break;
	}
}

// See ATSC standard A/65c for info on PSIP and what all these acronyms are...
bool DVBsi::handle_atsc_transponder() {
	// Loop through the MGT to get the list of PIDS for virtual channels
	if (parseMGT(PSIP_BASE_PID, PSIP_TABLE_TYPE_MGT,
		     PSIP_MAX_CYCLE_TIME_MGT) == false) {
		// We couldn't find the MGT
		fprintf(stderr, "Could not find MGT in stream.  Cannot continue\n");
		return false;
	};

	// Now look at the TVCT for info on the individual channels found
	if (parseVCT(PSIP_BASE_PID, vct_table,
		     PSIP_MAX_CYCLE_TIME_VCT) == false) {
		fprintf(stderr, "Could not parse VCT in stream.  Cannot continue\n");
		return false;
	}
	return true;
}

void DVBsi::run()
{
	int i, j=0, k=0;
	ChannelDesc chan;
	Transponder trans;
	unsigned short tsid, nid;
	bool nitEnded=true, ok;

	progressTransponder = 0;

	if ( scanMode<2 ) {
		dvb->stopScan();
		for ( i=0; i<(int)transponders.count(); i++ ) {
			if ( !isRunning ) {
				out();
				return;
			}
			chan.tp = *transponders.at(i);
			usleep( 100000 );
			if ( !dvb->tuneDvb( &chan, false ) ) {
				for ( k=0; k<(int)chan.tp.freqlist.count(); k++ ) {
					chan.tp.freq = chan.tp.freqlist[k];
					fprintf(stderr,"Trying alternate frequency\n");
					if ( dvb->tuneDvb( &chan, false ) ) {
						k = -1;
						transponders.at(i)->freq = chan.tp.freq;
						break;
					}
					else
						usleep( 400000 );
				}
				if ( k>-1 ) {
					fprintf(stderr,"dvbsi: Cant tune DVB\n");
					progressTransponder++;
					usleep( 400000 );
					continue;
				}
			}

			indexChannels = j;
			fprintf(stderr,"Transponders: %d/%d\n", i+1, transponders.count() );
			fprintf(stderr,"scanMode=%d\n", scanMode);
			if ( chan.tp.type == FE_ATSC ) {
				// Separate out the ATSC scanning so that we
				// don't interfere with existing DVB support
				if (!handle_atsc_transponder())
					continue;
			}
			else {
				printf("it's dvb %d!\n", chan.tp.type);
				if ( scanMode ) {
					nitEnded = false;
					ns = new NitSection( &transponders, &nitEnded, &ok, adapter, tuner ); //NIT
				}

				getSection( 0x11, 0x42 );       //SDT
			}
			if ( !isRunning ) {
				out();
				return;
			}
			getSection( 0x00, 0x00 );         //PAT
			for ( ; j<(int)channels.count(); j++ ) {
				if ( channels.at( j )->pmtpid==0 ) {
					channels.at(j)->completed = 1;
					continue;
				}
				if ( !isRunning ) {
					out();
					return;
				}
				getSection( channels.at( j )->pmtpid, 0x02, 5000, channels.at( j )->sid );    //PMTs
				tsid = channels.at(j)->tp.tsid;
				nid = channels.at(j)->tp.nid;
				channels.at(j)->tp = *transponders.at(i);
				if ( channels.at(j)->tp.tsid==0 )
					channels.at(j)->tp.tsid = tsid;
				channels.at(j)->tp.nid = nid;
				channels.at(j)->tp.snr = dvb->getSNR();
				channels.at(j)->completed = 1;
			}
			while( !nitEnded ) {
				if ( !ok ) {
					ns->stop();
					break;
				}
				else
					msleep( 50 );
			}
			if ( scanMode ) {
				delete ns;
				ns = 0;
			}
			dvb->stopScan();
			progressTransponder++;
		}
		fprintf(stderr,"Transponders: %d\n", transponders.count());
	}
	else if ( scanMode==2 ) {
		indexChannels = 0;
		trans = dvb->getCurrentTransponder();
		getSection( 0x11, 0x42 );         //SDT
		getSection( 0x00, 0x00 );         //PAT
		for ( j=0; j<(int)channels.count(); j++ ) {
			if ( channels.at( j )->pmtpid==0 ) {
				channels.at(j)->completed = 1;
				continue;
			}
			if ( !isRunning ) {
				out(false);
				return;
			}
			getSection( channels.at( j )->pmtpid, 0x02, 5000, channels.at( j )->sid );    //PMTs
			tsid = channels.at(j)->tp.tsid;
			nid = channels.at(j)->tp.nid;
			channels.at(j)->tp = trans;
			channels.at(j)->tp.tsid = tsid;
			channels.at(j)->tp.nid = nid;
			channels.at(j)->tp.snr = dvb->getSNR();
			channels.at(j)->completed = 1;
		}
	}
	fprintf(stderr,"dvbsi: The end :)\n");
	isRunning = false;
	listChannels();
	KApplication::kApplication()->postEvent( this, new QTimerEvent( TIMER_EVENT_SCAN_END ) );
}


// !!!! only there for debugging !!!!!!!!!!
bool DVBsi::listChannels()
{
	QString s,t;
	int i, valid=0;
	bool ac3;

	for ( i=0; i<(int)channels.count(); i++ ) {
		ac3 = false;
		if ( channels.at( i )->vType<16 )
			continue;
		/*if ( channels.at( i )->nsubpid==0 )
			continue;
		if ( channels.at( i )->pmtpid==0 )
			continue;
		if ( channels.at(i)->name.isEmpty() )
			continue;
		if ( channels.at( i )->type<1 )
			continue;
		if ( channels.at( i )->type>2 )
			continue;
		if ( channels.at( i )->type==1 && channels.at(i)->vpid==0 )
			continue;
		if ( channels.at( i )->type==1 && channels.at(i)->napid==0 )
			continue;
		if ( channels.at( i )->type==2 && channels.at(i)->napid==0 )
			continue;
		if ( channels.at( i )->type==2 && channels.at(i)->vpid!=0 )
			continue;*/

		s = "|"+channels.at(i)->name;
		s = s+"|"+t.setNum(channels.at(i)->tp.freq);
		s = s+"|"+channels.at(i)->tp.pol;
		s = s+"|"+t.setNum(channels.at(i)->tp.sr);
		fprintf(stderr, "%s\n", s.latin1() );
		valid++;
	}
	fprintf(stderr, "Channels found: %d\n", valid );
	return true;
}

#include "dvbsi.moc"
