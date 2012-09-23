/*
 * ts2rtp.cpp
 *
 * Copyright (C) 2003-2007 Christophe Thommeret <hftom@free.fr>
 *
 * some code from dvbstream (GPL)
 * Copyright (C) 2001,2002 Dave Chapman
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

#include <sys/time.h>

#include <kmessagebox.h>
#include <klocale.h>

#include "ts2rtp.h"
#include "gdvb.h"
#include "channeldesc.h"

#define TS_SIZE 188



Ts2Rtp::Ts2Rtp()
{
	unsigned int i, j=0, k;

	for( i = 0 ; i < 256 ; i++ ) {
		k = 0;
		for (j = (i << 24) | 0x800000 ; j != 0x80000000 ; j <<= 1) {
			k = (k << 1) ^ (((k ^ j) & 0x80000000) ? 0x04c11db7 : 0);
		}
		CRC32[i] = k;
	}

	thWrite = 0;
	rtpSocket = 0;
	writePsi = false;
	psiPackets = 0;
	// fill in the MPEG-2 TS deefaults
	// Note: MPEG-2 TS defines a timestamping base frequency of 90000 Hz.
	hRtp.b.v=2;
	hRtp.b.p=0;
	hRtp.b.x=0;
	hRtp.b.cc=0;
	hRtp.b.m=0;
	hRtp.b.pt=33;
	hRtp.b.sequence=rand() & 65535;
	hRtp.timestamp=rand();
	hRtp.ssrc=rand();

	psiBufferSize = 20*TS_SIZE;
	psiBuffer = (unsigned char*)malloc(psiBufferSize);

	connect( &psiTimer, SIGNAL(timeout()), this, SLOT(setPSI()) );
}



Ts2Rtp::~Ts2Rtp()
{
	stop();
	if ( rtpSocket ) {
		close( rtpSocket );
	}
	free( psiBuffer );
}



void Ts2Rtp::setSocket( const QString &addr, int m_port, int m_senderPort )
{
	address = addr;
	port = m_port;
	senderPort = m_senderPort;
}



bool Ts2Rtp::addChannels( QPtrList<ChannelDesc> *channels )
{
	int i, j, k, pmtpid=8191;
	ChannelDesc *desc, *d;
	QValueList<int> pids;

	if ( !rtpSocket && !makeSocket() )
		return false;

	sendList = "";
	for ( i=0; i<(int)channels->count(); i++ ) {
		desc = channels->at( i );
		sendList = sendList+desc->name+"|"+QString().setNum(desc->vpid)+"|"+QString().setNum(desc->apid[0].pid)+"|";
		if ( desc->apid[0].ac3 ) sendList+= "y|";
		else sendList+= "n|";
		sendList+= QString().setNum(desc->subpid[0].pid);
		sendList+= "|";
		sendList+= QString().setNum(desc->subpid[0].page);
		sendList+= "|";
		sendList+= QString().setNum(desc->subpid[0].id);
		sendList+= "|";
		sendList+= QString().setNum(desc->subpid[0].type);
		sendList+= "|";
		sendList+= desc->subpid[0].lang+"|";
		for ( j=i; j<(int)channels->count(); j++ ) {
			pids.clear();
			d = channels->at( j );
			if ( d->vpid )
				pids.append( d->vpid );
			for ( k=0; k<d->napid && k<MAX_AUDIO; k++ )
				pids.append( d->apid[k].pid );
			for ( k=0; k<d->nsubpid && k<MAX_DVBSUB; k++ )
				pids.append( d->subpid[k].pid );
			while ( pmtpid==17 || pids.contains( pmtpid ) )
				--pmtpid;
		}
		desc->pmtpid = pmtpid--;
	}
	sendList+="\n";
	psiTables( channels );
	writePsi = true;
	psiTimer.start( 500 );
	return true;
}



void Ts2Rtp::removeChannels()
{
	if ( !rtpSocket )
		return;
	stop();
	close( rtpSocket );
	rtpSocket = 0;
	fprintf( stderr, "rtp socket closed\n" );
	thWrite = 0;
	psiPackets = 0;
	if ( psiTimer.isActive() )
		psiTimer.stop();
}



bool Ts2Rtp::makeSocket()
{
	int iRet, iLoop = 1;

	if ( !makeSenderSocket( address, senderPort ) ) return false;

	rtpSocket = socket( AF_INET, SOCK_DGRAM, 0 );

	if ( rtpSocket < 0) {
		KMessageBox::error( 0, i18n("Can't open DVB broadcast socket.") );
		rtpSocket = 0;
		closeSender();
		return false;
	}

	rtpAddr.sin_family = AF_INET;
	rtpAddr.sin_port = htons( port );
	rtpAddr.sin_addr.s_addr = inet_addr( address.ascii() );

	iRet = setsockopt( rtpSocket, SOL_SOCKET, SO_BROADCAST, &iLoop, sizeof(iLoop));
	if (iRet < 0) {
		KMessageBox::error( 0, i18n("Can't init DVB broadcast socket.") );
		close( rtpSocket );
		rtpSocket = 0;
		closeSender();
		return false;
	}
	go();
	fprintf( stderr, "rtp socket opened\n" );
	return true;
}



void Ts2Rtp::process( unsigned char *buf, int size )
{
	int i, n;
	unsigned char *buffer=buf;

	if ( writePsi ) {
		i = 0;
		while ( i<psiPackets ) {
			n = psiPackets-i;
			if ( n>8 )
				n = 8;
			sendrtp( (char*)(psiBuffer+(TS_SIZE*i)), TS_SIZE*n );
			i+= n;
		}
		writePsi = false;
	}

	for ( i=0; i<size; i+=TS_SIZE ) {
		memcpy( thBuf+thWrite, buffer, TS_SIZE );
		thWrite+=TS_SIZE;
		if ( thWrite==(TS_SIZE*8 ) ) {
			sendrtp( (char*)thBuf, TS_SIZE*8 );
			thWrite = 0;
		}
		buffer+=TS_SIZE;
	}
}



/* Send a single RTP packet, converting the RTP header to network byte order. */
void Ts2Rtp::sendrtp( char *data, int len )
{
	struct timeval tv;

	gettimeofday( &tv, (struct timezone*) NULL );
	hRtp.timestamp = ((tv.tv_sec%1000000)*1000 + tv.tv_usec/1000)*90;

	char *buf=(char*)alloca(len+72);
	unsigned int intP;
	char* charP = (char*) &intP;
	int headerSize;
	buf[0]  = 0x00;
	buf[0] |= ((((char) hRtp.b.v)<<6)&0xc0);
	buf[0] |= ((((char) hRtp.b.p)<<5)&0x20);
	buf[0] |= ((((char) hRtp.b.x)<<4)&0x10);
	buf[0] |= ((((char) hRtp.b.cc)<<0)&0x0f);
	buf[1]  = 0x00;
	buf[1] |= ((((char) hRtp.b.m)<<7)&0x80);
	buf[1] |= ((((char) hRtp.b.pt)<<0)&0x7f);
	intP = htonl(hRtp.b.sequence);
	memcpy(&buf[2],charP+2,2);
	intP = htonl(hRtp.timestamp);
	memcpy(&buf[4],&intP,4);
	/* SSRC: not implemented */
	buf[8]  = 0x0f;
	buf[9]  = 0x0f;
	buf[10] = 0x0f;
	buf[11] = 0x0f;
	headerSize = 12 + 4*hRtp.b.cc; /* in bytes */
	memcpy(buf+headerSize,data,len);

	hRtp.b.sequence++;
	if ( rtpSocket ) sendto( rtpSocket, buf, len+headerSize, 0, (struct sockaddr *)&rtpAddr, sizeof(rtpAddr) );
}



void Ts2Rtp::setPSI()
{
	writePsi = true;
}


void Ts2Rtp::psiTables( QPtrList<ChannelDesc> *channels )
{
	unsigned char buf[15000];
	int off, i, j, sectionOff, loopOff, descOff, max;
	int npack=0;
	unsigned short tsid = channels->at(0)->tp.tsid;
	ChannelDesc *desc;

	psiPackets = 0;

	// SDT
	off = 0;
	// CRC calculation begins here
	buf[off++] = 0x42; // service description section
	buf[off++] = 0x80;
	sectionOff = off;
	buf[off++] = 0x00; // section_length (12bits)
	buf[off++] = tsid>>8; buf[off++] = tsid&0xff;
	buf[off++] = 0x01; // current_next_indicator
	buf[off++] = 0x00; // section_number
	buf[off++] = 0x00; // last_section_number
	buf[off++] = 0x00; buf[off++] = 0x00; // network_id
	buf[off++] = 0x00; // reserved

	for ( i=0; i<(int)channels->count(); i++ ) {
		desc = channels->at( i );
		buf[off++] = desc->sid>>8; buf[off++] = desc->sid&0xff; // service_id
		buf[off++] = 0x00; // reserved
		buf[off++] = 0x80; // running_status(3bits) + free_ca(1bit) + descriptors_loop_length
		loopOff = off;
		buf[off++] = 0x00; // descriptors_loop_length

		buf[off++] = 0x48; // descriptor_tag
		descOff = off;
		buf[off++] = 0x00; //descriptor_length
		buf[off++] = desc->type; //service_type
		buf[off++] = 0x03; // provider_name_length
		buf[off++] = 0x4c; buf[off++] = 0x41; buf[off++] = 0x4e; // provider_name
		buf[off++] = desc->name.length(); // service_name_length
		memcpy( buf+off, desc->name.latin1(), desc->name.length() );
		off+= desc->name.length();
		buf[descOff] = off-descOff-1;
		buf[loopOff] = off-loopOff-1;
	}
	buf[sectionOff-1] |= (off+1)>>8;
	buf[sectionOff] = (off+1)&0xff;
	calculateCRC( buf, buf+off );
	fillPackets( 17, buf, off+4, npack );

	// PAT
	off = 0;
	// CRC calculation begins here
	buf[off++] = 0x00; // program association section
	buf[off++] = 0x80;
	sectionOff = off;
	buf[off++] = 0x00; // section_length (12bits)
	buf[off++] = tsid>>8; buf[off++] = tsid&0xff;
	buf[off++] = 0x01; // current_next_indicator
	buf[off++] = 0x00; // section_number
	buf[off++] = 0x00; // last_section_number

	for ( i=0; i<(int)channels->count(); i++ ) {
		desc = channels->at( i );
		buf[off++] = desc->sid>>8; buf[off++] = desc->sid&0xff; // service_id
		buf[off++] = desc->pmtpid>>8; buf[off++] = desc->pmtpid&0xff; // program_map_PID(13bits)
	}
	buf[sectionOff-1] |= (off+1)>>8;
	buf[sectionOff] = (off+1)&0xff;
	calculateCRC( buf, buf+off );
	fillPackets( 0, buf, off+4, npack );

	// PMT
	for ( i=0; i<(int)channels->count(); i++ ) {
		desc = channels->at( i );
		off = 0;
		// CRC calculation begins here
		buf[off++] = 0x02; // program map section
		buf[off++] = 0x80;
		sectionOff = off;
		buf[off++] = 0x00; // section_length (12bits)
		buf[off++] = desc->sid>>8; buf[off++] = desc->sid&0xff;
		buf[off++] = 0x01; // current_next_indicator
		buf[off++] = 0x00; // section_number
		buf[off++] = 0x00; // last_section_number
		if ( desc->vpid ) {
			buf[off++] = desc->vpid>>8; buf[off++] = desc->vpid&0xff; // PCR pid
		}
		else if ( desc->napid ) {
			buf[off++] = desc->apid[0].pid>>8; buf[off++] = desc->apid[0].pid&0xff; // PCR pid
		}
		buf[off++] = 0x00; buf[off++] = 0x00; // infos_length
		if ( desc->vpid ) {
			buf[off++] = desc->vType; // stream_type
			buf[off++] = desc->vpid>>8; buf[off++] = desc->vpid&0xff; // pid
			buf[off++] = 0x00; buf[off++] = 0x00; // infos_length
		}

		for ( j=0; j<desc->napid && j<MAX_AUDIO; j++ ) {
			if ( desc->apid[j].ac3 ) {
				buf[off++] = 0x06; // stream type
				buf[off++] = desc->apid[j].pid>>8; buf[off++] = desc->apid[j].pid&0xff;
				buf[off++] = 0x00; buf[off++] = 0x09; // es info length
				buf[off++] = 0x6a; // descriptor tag
				buf[off++] = 0x01; // descriptor length
				buf[off++] = 0x00;
			}
			else {
				buf[off++] = 0x04; // stream type = audio
				buf[off++] = desc->apid[j].pid>>8; buf[off++] = desc->apid[j].pid&0xff;
				buf[off++] = 0x00; buf[off++] = 0x06; // es info length
			}
			buf[off++] = 0x0a; // iso639 descriptor tag
			buf[off++] = 0x04; // descriptor length
			if ( !desc->apid[j].lang.isEmpty() ) {
				buf[off++] = desc->apid[j].lang.constref(0);
				buf[off++] = desc->apid[j].lang.constref(1);
				if ( desc->apid[j].ac3 )
					buf[off++] = '_';
				else
					buf[off++] = desc->apid[j].lang.constref(2);
			}
			else if ( desc->apid[j].ac3 ) {
				buf[off++] = 'd';
				buf[off++] = 'd';
				buf[off++] = 49+j;
			}
			else {
				buf[off++] = 'c';
				buf[off++] = 'h';
				buf[off++] = 49+j;
			}
			buf[off++] = 0x00; // audio type
		}

		for ( j=0; j<desc->nsubpid && j<MAX_DVBSUB; j++ ) {
			buf[off++] = 0x06; // stream type = ISO_13818_PES_PRIVATE
			buf[off++] = desc->subpid[j].pid>>8; buf[off++] = desc->subpid[j].pid&0xff;
			buf[off++] = 0x00; buf[off++] = 0x0a; // es info length
			buf[off++] = 0x59; //DVB sub tag
			buf[off++] = 0x08; // descriptor length
			if ( !desc->subpid[j].lang.isEmpty() ) {
				buf[off++] = desc->subpid[j].lang.constref(0);
				buf[off++] = desc->subpid[j].lang.constref(1);
				buf[off++] = desc->subpid[j].lang.constref(2);
			}
			else {
				buf[off++] = 'c';
				buf[off++] = 'h';
				buf[off++] = 49+j;
			}
			buf[off++] = desc->subpid[j].type; //sub type
			buf[off++] = desc->subpid[j].page>>8; buf[off++] = desc->subpid[j].page&0xff; // comp_page_id
			buf[off++] = desc->subpid[j].id>>8; buf[off++] = desc->subpid[j].id&0xff; // anc_page_id
		}

		buf[sectionOff-1] |= (off+1)>>8;
		buf[sectionOff] = (off+1)&0xff;
		calculateCRC( buf, buf+off );
		fillPackets( desc->pmtpid, buf, off+4, npack );
	}

	psiPackets = npack;
}



void Ts2Rtp::fillPackets( unsigned short pid, unsigned char *buf, int len, int &npack )
{
	int i, off=npack*TS_SIZE, offbuf=0, inc;
	bool pus=true;
	int continuity=0;

	while ( (off-(npack*TS_SIZE))<len ) {
		if ( (off+TS_SIZE)>(psiBufferSize-(10*TS_SIZE)) ) {
			psiBufferSize+= 10*TS_SIZE;
			psiBuffer = (unsigned char*)realloc( psiBuffer, psiBufferSize );
			fprintf(stderr,"psiBufferSize = %d\n",psiBufferSize);
		}
		psiBuffer[off++] = 0x47; // sync_byte
		if ( pus )
			psiBuffer[off++] = 0x40|(pid>>8);
		else
			psiBuffer[off++] = pid>>8;
		psiBuffer[off++] = pid&0xff; // PID
		psiBuffer[off++] = 0x10|continuity;
		if ( pus ) {
			psiBuffer[off++] = 0x00; // pointer_field
			pus = false;
		}
		inc = TS_SIZE-(off%TS_SIZE);
		if ( (len-offbuf)<inc )
			inc = len-offbuf;
		memcpy( psiBuffer+off, buf+offbuf, inc );
		off+= inc;
		offbuf+= inc;
		++continuity;
	}
	// needed stuffing bytes
	for ( i=0 ; i<(off%TS_SIZE) ; i++)
		psiBuffer[off++] = 0xff;
	npack+= (off-(npack*TS_SIZE))/TS_SIZE;
}



void Ts2Rtp::calculateCRC( unsigned char *p_begin, unsigned char *p_end )
{
	unsigned int i_crc = 0xffffffff;

	// Calculate the CRC
	while( p_begin < p_end ) {
		i_crc = (i_crc<<8) ^ CRC32[ (i_crc>>24) ^ ((unsigned int)*p_begin) ];
		p_begin++;
	}
	// Store it after the data
	p_end[0] = (i_crc >> 24) & 0xff;
	p_end[1] = (i_crc >> 16) & 0xff;
	p_end[2] = (i_crc >>  8) & 0xff;
	p_end[3] = (i_crc >>  0) & 0xff;
}
