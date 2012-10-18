/*
 * kaffeinedvbsection.cpp
 *
 * Copyright (C) 2003-2007 Christophe Thommeret <hftom@free.fr>
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <iconv.h>

#include <klocale.h>

#include "kaffeinedvbsection.h"



KaffeineDVBsection::KaffeineDVBsection()
{
}



KaffeineDVBsection::KaffeineDVBsection( int anum, int tnum, const QString &charset )
{
	initSection( anum, tnum, charset );
}



void KaffeineDVBsection::initSection( int anum, int tnum, const QString &charset )
{
	defaultCharset = charset.ascii();
	adapter = anum;
	tuner = tnum;
	isRunning = false;
	fdDemux = -1;
}



KaffeineDVBsection::~KaffeineDVBsection()
{
	if ( fdDemux>-1 )
		close( fdDemux );
}



bool KaffeineDVBsection::setFilter( int pid, int tid, int timeout, bool checkcrc )
{
	struct dmx_sct_filter_params sctfilter;
	QString demuxer = QString("/dev/dvb/adapter%1/demux%2").arg( adapter ).arg( tuner );

	if ((fdDemux = open( demuxer.ascii(), O_RDWR | O_NONBLOCK )) < 0) {
		perror ("open failed");
		return false;
	}

	pf[0].fd = fdDemux;
	pf[0].events = POLLIN;

	memset( &sctfilter, 0, sizeof( sctfilter ) );

	sctfilter.pid = pid;
	if ( tid<256 && tid>0 ) {
		sctfilter.filter.filter[0] = tid;
		sctfilter.filter.mask[0] = 0xff;
	}
	sctfilter.flags = DMX_IMMEDIATE_START;
	if ( checkcrc )
		sctfilter.flags|= DMX_CHECK_CRC;
	sctfilter.timeout = timeout;

	if ( ioctl( fdDemux, DMX_SET_FILTER, &sctfilter ) < 0 ) {
		perror ( "ioctl DMX_SET_FILTER failed" );
		return false;
	}
	return true;
}



void KaffeineDVBsection::stopFilter()
{
	ioctl( fdDemux, DMX_STOP );
	close( fdDemux );
	fdDemux = -1;
}



unsigned int KaffeineDVBsection::getBits( unsigned char *b, int offbits, int nbits )
{
	int i, nbytes;
	unsigned int ret = 0;
	unsigned char *buf;

	buf = b+(offbits/8);
	offbits %=8;
	nbytes = (offbits+nbits)/8;
	if ( ((offbits+nbits)%8)>0 )
		nbytes++;
	for ( i=0; i<nbytes; i++ )
		ret += buf[i]<<((nbytes-i-1)*8);
	i = (4-nbytes)*8+offbits;
	ret = ((ret<<i)>>i)>>((nbytes*8)-nbits-offbits);

	return ret;
}



bool KaffeineDVBsection::doIconv( QCString &s, QCString table, char *buffer, int buflen )
{
	size_t inSize, outSize=buflen;
	char *inBuf, *outBuf;
	iconv_t cd;

	inSize = s.length();
	if ( inSize<1 )
		return false;
	cd = iconv_open( "UTF8", table );
	//check if charset unknown
	if( cd == (iconv_t)(-1) )
		return false;
	inBuf = s.data();
	outBuf = buffer;
	outBuf[0] = 0;
	iconv( cd, &inBuf, &inSize, &outBuf, &outSize );
	*outBuf = 0;
	iconv_close( cd );
	return true;
}



QString KaffeineDVBsection::getText( unsigned char *buf, int length )
{
	QCString s;
	QString ret="";
	int i=0;
	char buffer[1000];
	QCString table=defaultCharset;

	if ( length==0 )
		return "";

	while ( i<length ) {
		if ( buf[i]<0x20 && (i+2)<length ) {
			if ( !s.isEmpty() ) {
				if ( doIconv( s, table, buffer, sizeof(buffer) ) ) {
					ret += QString::fromUtf8( buffer );
					s = "";
				}
			}
			switch ( buf[i] ) {
				case 0x01: table = "ISO8859-5"; ++i; break;
				case 0x02: table = "ISO8859-6"; ++i; break;
				case 0x03: table = "ISO8859-7"; ++i; break;
				case 0x04: table = "ISO8859-8"; ++i; break;
				case 0x05: table = "ISO8859-9"; ++i; break;
				case 0x06: table = "ISO8859-10"; ++i; break;
				case 0x07: table = "ISO8859-11"; ++i; break;
				case 0x09: table = "ISO8859-13"; ++i; break;
				case 0x0A: table = "ISO8859-14"; ++i; break;
				case 0x0B: table = "ISO8859-15"; ++i; break;
				case 0x13: table = "GB2312"; ++i; break;
				case 0x14: table = "BIG5"; ++i; break;
				case 0x10: {
					switch ( buf[i+2] ) {
						case 0x01: table = "ISO8859-1"; break;
						case 0x02: table = "ISO8859-2"; break;
						case 0x03: table = "ISO8859-3"; break;
						case 0x04: table = "ISO8859-4"; break;
						case 0x05: table = "ISO8859-5"; break;
						case 0x06: table = "ISO8859-6"; break;
						case 0x07: table = "ISO8859-7"; break;
						case 0x08: table = "ISO8859-8"; break;
						case 0x09: table = "ISO8859-9"; break;
						case 0x0A: table = "ISO8859-10"; break;
						case 0x0B: table = "ISO8859-11"; break;
						case 0x0D: table = "ISO8859-13"; break;
						case 0x0E: table = "ISO8859-14"; break;
						case 0x0F: table = "ISO8859-15"; break;
					}
					i+= 3;
					break;
				}
				default: ++i;
			}
		}

		if ( buf[i]>=0x80 && buf[i]<=0x9f ) {
			++i;
			continue; // control codes
		}
		s += buf[i++];
	}

	if ( !s.isEmpty() ) {
		if ( doIconv( s, table, buffer, sizeof(buffer) ) ) {
			ret += QString::fromUtf8( buffer );
		}
	}
	return ret;
}



QString KaffeineDVBsection::langDesc( unsigned char* buf )
{
	char c[4];
	QString s;

	memset( mempcpy( c, buf+2, 3 ), 0, 1 );
	s = c;
	return s;
}



QTime KaffeineDVBsection::getTime( unsigned char *buf )
{
	int h, m, s;

	h = ((getBits(buf,0,4)*10)+getBits(buf,4,4))%24;
	m = ((getBits(buf,8,4)*10)+getBits(buf,12,4))%60;
	s = ((getBits(buf,16,4)*10)+getBits(buf,20,4))%60;
	return QTime( h, m, s );
}



QDate KaffeineDVBsection::getDate( unsigned char *buf )
{
	int i, j, m, D, Y, M, k, mjd;

	mjd = getBits(buf,0,16);
	i = (int)((mjd-15078.2)/365.25);
	j = (int)(i*365.25);
	m = (int)((mjd-14956.1-j)/30.6001);
	D = mjd-14956-j-(int)(m*30.6001);
	if ( m==14 || m==15 )
		k = 1;
	else
		k = 0;
	Y = i+k+1900;
	M = m-1-k*12;

	return QDate( (Y>=1970)?Y:1970, (M>0 && M<13)?M:1, (D>0 && D<32)?D:1 );
}



QDateTime KaffeineDVBsection::getDateTime( unsigned char *buf )
{
	/*int hh, mm, ss;
	int i, j, m, D, Y, M, k, mjd;
	int sec;

	mjd = getBits(buf,0,16);
	i = (int)((mjd-15078.2)/365.25);
	j = (int)(i*365.25);
	m = (int)((mjd-14956.1-j)/30.6001);
	D = mjd-14956-j-(int)(m*30.6001);
	if ( m==14 || m==15 ) k = 1;
	else k = 0;
	Y = i+k+1900;
	M = m-1-k*12;

	hh = ((getBits(buf+2,0,4)*10)+getBits(buf+2,4,4))%24;
	mm = ((getBits(buf+2,8,4)*10)+getBits(buf+2,12,4))%60;
	ss = ((getBits(buf+2,16,4)*10)+getBits(buf+2,20,4))%60;

	QDateTime dt( QDate( (Y>=1970)?Y:1970, (M>0 && M<13)?M:1, (D>0 && D<32)?D:1 ), QTime( hh, mm, ss ) );
	QDateTime u( QDate( 1970, 1, 1 ), QTime( 0, 0, 0 ) );
	sec = u.secsTo( dt );
	u.setTime_t( sec ); // UTC to local
	return u;*/

	int i, j, m, k, mjd;
	struct tm tt;
	struct tm *t=&tt;

	mjd = getBits(buf,0,16);
	i = (int)((mjd-15078.2)/365.25);
	j = (int)(i*365.25);
	m = (int)((mjd-14956.1-j)/30.6001);
	t->tm_mday = mjd-14956-j-(int)(m*30.6001);
	if ( m==14 || m==15 )
		k = 1;
	else
		k = 0;
	t->tm_year = i+k;
	t->tm_mon = m-1-k*12-1;
	t->tm_sec = ((getBits(buf+2,16,4)*10)+getBits(buf+2,20,4))%60;
	t->tm_min = ((getBits(buf+2,8,4)*10)+getBits(buf+2,12,4))%60;
	t->tm_hour = ((getBits(buf+2,0,4)*10)+getBits(buf+2,4,4))%24;
	t->tm_isdst = -1;
	t->tm_gmtoff = 0;

	time_t p=timegm(t);
	if ( p>0 ) {
		t = localtime(&p);
		return QDateTime( QDate( t->tm_year+1900, t->tm_mon+1, t->tm_mday ), QTime( t->tm_hour, t->tm_min, t->tm_sec ) );
	}
	else
		return QDateTime( QDate( 1970, 1, 1 ), QTime( 0, 0, 0 ) );
}
