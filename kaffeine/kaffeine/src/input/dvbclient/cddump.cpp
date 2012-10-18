/*
 * cddump.cpp
 *
 * Copyright (C) 2005 Christophe Thommeret <hftom@free.fr>
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

#include <fcntl.h>
#include <unistd.h>

#include <klocale.h>
#include <kapplication.h>
#include <kmessagebox.h>

#include "cddump.h"



CdDump::CdDump( const QString &pipe )
{
	unsigned int i, j, k;

	for( i = 0 ; i < 256 ; i++ ) {
		k = 0;
		for (j = (i << 24) | 0x800000 ; j != 0x80000000 ; j <<= 1) {
			k = (k << 1) ^ (((k ^ j) & 0x80000000) ? 0x04c11db7 : 0);
		}
		CRC32[i] = k;
	}

	isRunning = false;
	fifoName = pipe;
	connect( &timerPatPmt, SIGNAL(timeout()), this, SLOT(setPatPmt()) );
	timerPatPmt.start( 500 );
}



void CdDump::calculateCRC( unsigned char *p_begin, unsigned char *p_end )
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



void CdDump::writePat()
{
	int i;

	tspat[0x00] = 0x47; // sync_byte
	tspat[0x01] = 0x40;
	tspat[0x02] = 0x00; // PID = 0x0000
	tspat[0x03] = 0x10; // | (ps->pat_counter & 0x0f);
	tspat[0x04] = 0x00; // CRC calculation begins here
	tspat[0x05] = 0x00; // 0x00: Program association section
	tspat[0x06] = 0xb0;
	tspat[0x07] = 0x11; // section_length = 0x011
	tspat[0x08] = 0x00;
	tspat[0x09] = 0xbb; // TS id = 0x00b0 (what the vlc calls "Stream ID")
	tspat[0x0a] = 0xc1;
	// section # and last section #
	tspat[0x0b] = tspat[0x0c] = 0x00;
	// Network PID (useless)
	tspat[0x0d] = tspat[0x0e] = 0x00; tspat[0x0f] = 0xe0; tspat[0x10] = 0x10;
	// Program Map PID
	pmtpid = 0x64;
	while ( pmtpid==chan.vpid || pmtpid==chan.apid ) pmtpid--;
	tspat[0x11] = 0x03; tspat[0x12] = 0xe8; tspat[0x13] = 0xe0; tspat[0x14] = pmtpid;
	// Put CRC in ts[0x15...0x18]
	calculateCRC( tspat + 0x05, tspat + 0x15 );
	// needed stuffing bytes
	for (i=0x19 ; i < 188 ; i++) tspat[i]=0xff;
}



void CdDump::writePmt()
{
	int i, off=0;

	tspmt[0x00] = 0x47; //sync_byte
	tspmt[0x01] = 0x40;
	tspmt[0x02] = pmtpid;
	tspmt[0x03] = 0x10;
	tspmt[0x04] = 0x00; // CRC calculation begins here
	tspmt[0x05] = 0x02; // 0x02: Program map section
	tspmt[0x06] = 0xb0;
	tspmt[0x07] = 0x20; // section_length
	tspmt[0x08] = 0x03;
	tspmt[0x09] = 0xe8; // prog number
	tspmt[0x0a] = 0xc1;
	// section # and last section #
	tspmt[0x0b] = tspmt[0x0c] = 0x00;
	// PCR PID
	tspmt[0x0d] = chan.vpid>>8; tspmt[0x0e] = chan.vpid&0xff;
	// program_info_length == 0
	tspmt[0x0f] = 0xf0; tspmt[0x10] = 0x00;
	// Program Map / Video PID
	tspmt[0x11] = 0x02; // stream type = video
	tspmt[0x12] = chan.vpid>>8; tspmt[0x13] = chan.vpid&0xff;
	tspmt[0x14] = 0xf0; tspmt[0x15] = 0x09; // es info length
	// useless info
	tspmt[0x16] = 0x07; tspmt[0x17] = 0x04; tspmt[0x18] = 0x08; tspmt[0x19] = 0x80;
	tspmt[0x1a] = 0x24; tspmt[0x1b] = 0x02; tspmt[0x1c] = 0x11; tspmt[0x1d] = 0x01;
	tspmt[0x1e] = 0xfe;
	off = 0x1e;
	// audio pid
	if ( chan.ac3 ) {
		tspmt[++off] = 0x81; // stream type = xine see this as ac3
		tspmt[++off] = chan.apid>>8; tspmt[++off] = chan.apid&0xff;
		tspmt[++off] = 0xf0; tspmt[++off] = 0x0c; // es info length
		tspmt[++off] = 0x05; tspmt[++off] = 0x04; tspmt[++off] = 0x41;
		tspmt[++off] = 0x43; tspmt[++off] = 0x2d; tspmt[++off] = 0x33;
	}
	else {
		tspmt[++off] = 0x04; // stream type = audio
		tspmt[++off] = chan.apid>>8; tspmt[++off] = chan.apid&0xff;
		tspmt[++off] = 0xf0; tspmt[++off] = 0x06; // es info length
	}
	tspmt[++off] = 0x0a; // iso639 descriptor tag
	tspmt[++off] = 0x04; // descriptor length
	tspmt[++off] = '?';
	tspmt[++off] = '?';
	tspmt[++off] = '?';
	tspmt[++off] = 0x00; // audio type
	// subtitles
	if ( chan.subpid ) {
		tspmt[++off] = 0x06; // stream type = ISO_13818_PES_PRIVATE
		tspmt[++off] = chan.subpid>>8; tspmt[++off] = chan.subpid&0xff;
		tspmt[++off] = 0xf0; tspmt[++off] = 0x0a; // es info length
		tspmt[++off] = 0x59; //DVB sub tag
		tspmt[++off] = 0x08; // descriptor length
		if ( !chan.lang.isEmpty() ) {
			tspmt[++off] = chan.lang.constref(0);
			tspmt[++off] = chan.lang.constref(1);
			tspmt[++off] = chan.lang.constref(2);
		}
		else {
			tspmt[++off] = '?';
			tspmt[++off] = '?';
			tspmt[++off] = '?';
		}
		tspmt[++off] = chan.type; //sub type
		tspmt[++off] = chan.page>>8; tspmt[++off] = chan.page&0xff; // comp_page_id
		tspmt[++off] = chan.id>>8; tspmt[++off] = chan.id&0xff; // anc_page_id
	}
	tspmt[0x07] = off-3; // update section_length
	// Put CRC in ts[0x29...0x2c]
	calculateCRC( tspmt+0x05, tspmt+off+1 );
	// needed stuffing bytes
	for (i=off+5 ; i < 188 ; i++) tspmt[i]=0xff;
}



void CdDump::setPatPmt()
{
	patpmt = true;
}



bool CdDump::go( const QString &ad, int port, CdChannel c )
{
	int sockopt=1;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		KMessageBox::information( 0, i18n("Can't open socket."), i18n("DVB Client") );
		sock = 0;
		return false;
	}

	addr.sin_family = AF_INET;         // host byte order
	addr.sin_port = htons( port );     // short, network byte order
	addr.sin_addr.s_addr = inet_addr( ad.ascii() );
	memset( &( addr.sin_zero ), '\0', 8 ); // zero the rest of the struct

	if( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt) ) < 0 ){
        	KMessageBox::information( 0, i18n("Can't set socket option!!!"), i18n("DVB Client") );
		close( sock );
		sock = 0;
		return false;
	}

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		KMessageBox::information( 0, i18n("Can't bind socket!!!"), i18n("DVB Client") );
		close( sock );
		sock = 0;
		return false;
        }

	chan = c;
	writePat();
	writePmt();
	patpmt = true;
	timeShiftFileName = "";
	timeShifting = false;
	isRunning = true;
	start();
	return true;
}



void CdDump::stop()
{
	isRunning = false;
	if ( !wait(1000) ) {
		terminate();
		wait();
	}
	if ( fdPipe ) {
		close( fdPipe );
		fdPipe = 0;
	}
	if ( timeShifting )
		liveFile.close();
	if ( sock ) {
		close( sock );
		sock = 0;
	}
}



bool CdDump::running() const
{
	return isRunning;
}



CdDump::~CdDump()
{
	stop();
}



bool CdDump::doPause( const QString &name )
{
	if ( !timeShifting ) {
		timeShiftFileName = name;
		liveFile.setName( timeShiftFileName );
		liveFile.open( IO_WriteOnly|IO_Truncate );
		liveFile.writeBlock( (char*)tspat, 188 );
		liveFile.writeBlock( (char*)tspmt, 188 );
		timeShifting = true;
		return true;
	}
	return false;
}



void CdDump::run()
{
	struct srtpheader rh;
	int lengthData;
	char buf[1600];
	unsigned char *b;
#define NTS 64
	unsigned char tbuf[NTS*188];
	int twrite;
	unsigned int intP;
	char* charP = (char*) &intP;
	int headerSize;
	int lengthPacket;
	int len;
	int pid;

	if ( (fdPipe=open( fifoName.ascii(), O_WRONLY))<0 ) {
		perror("DUMP PIPE FILE: ");
		return;
	}
	fprintf(stderr,"Dump pipe opened\n");

	twrite = 0;

	while( isRunning ) {
		lengthPacket = recv( sock, buf, 1590, 0 );
		rh.b.v  = (unsigned int) ((buf[0]>>6)&0x03);
		rh.b.p  = (unsigned int) ((buf[0]>>5)&0x01);
		rh.b.x  = (unsigned int) ((buf[0]>>4)&0x01);
		rh.b.cc = (unsigned int) ((buf[0]>>0)&0x0f);
		rh.b.m  = (unsigned int) ((buf[1]>>7)&0x01);
		rh.b.pt = (unsigned int) ((buf[1]>>0)&0x7f);
		intP = 0;
		memcpy( charP+2, &buf[2], 2 );
		rh.b.sequence = ntohl( intP );
		intP = 0;
		memcpy( charP, &buf[4], 4 );
		rh.timestamp = ntohl( intP );
		headerSize = 12 + 4*rh.b.cc; /* in bytes */
		lengthData = lengthPacket - headerSize;

		b = (unsigned char*)buf+headerSize;
		len = lengthData;
		while ( len>0 ) {
			pid = (((b[1] & 0x1f) << 8) | b[2]);
			if ( pid==chan.vpid || pid==chan.apid || pid==chan.subpid ) {
				memcpy( tbuf+twrite, b, 188 );
				twrite+= 188;
				if ( twrite==NTS*188 ) {
					if ( fdPipe ) {
						if ( patpmt ) {
							write( fdPipe, tspat, 188 );
							write( fdPipe, tspmt, 188 );
						}
						write( fdPipe, tbuf, NTS*188 );
						if ( timeShifting ) {
							if ( close( fdPipe )<0 )
								perror("close out pipe: ");
							else {
								fprintf(stderr,"out pipe closed\n");
								fdPipe = 0;
							}
						}
					}
					else {
						if ( patpmt ) {
							liveFile.writeBlock( (char*)tspat, 188 );
							liveFile.writeBlock( (char*)tspmt, 188 );
						}
						liveFile.writeBlock( (char*)tbuf, 188*NTS );
					}
					patpmt = false;
					twrite = 0;
				}
			}
			b+=188;
			len-=188;
		}
	}
}

#include "cddump.moc"
