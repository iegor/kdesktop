/*
 * dvbout.cpp
 *
 * Copyright (C) 2004-2007 Christophe Thommeret <hftom@free.fr>
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

#include <kstandarddirs.h>

#include "dvbout.h"
#include "kaffeinedvbplugin.h"

#define NTS 64

DVBout::DVBout( ChannelDesc chan, int anum, int tnum, KaffeineDvbPlugin *p )
{
	bool bok;
	bok = true;
	unsigned int i, j=0, k;

	for( i = 0 ; i < 256 ; i++ ) {
		k = 0;
		for (j = (i << 24) | 0x800000 ; j != 0x80000000 ; j <<= 1) {
			k = (k << 1) ^ (((k ^ j) & 0x80000000) ? 0x04c11db7 : 0);
		}
		CRC32[i] = k;
	}

	plug = 0;
	if ( p ) {
		plug = p;
		plugHandle = plug->init( chan.sid, anum, tnum, chan.fta );
	}

	fdPipe=0;
	channel = chan;
	thWrite = 0;
	rtp = 0;
	if ( channel.vpid )
		pids.append( channel.vpid );
	for ( i=0; i<channel.napid && i<MAX_AUDIO; i++ )
		pids.append( channel.apid[i].pid );
	for ( i=0; i<channel.nsubpid && i<MAX_DVBSUB; i++ )
		pids.append( channel.subpid[i].pid );
	wBuf = NULL;
	timeShifting = beginLive = false;
	haveRec = haveLive = instantRec = haveBroadcast = false;
	patpmt = wpatpmt = false;
	connect( &stopRecTimer, SIGNAL(timeout()), this, SLOT(stopRec()) );
	connect( &timerPatPmt, SIGNAL(timeout()), this, SLOT(setPatPmt()) );
	if ( !pids.contains(8192) )
		timerPatPmt.start(500);
}



void DVBout::calculateCRC( unsigned char *p_begin, unsigned char *p_end )
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



void DVBout::writePat()
{
	int i;

	tspat[0x00] = 0x47; // sync_byte
	tspat[0x01] = 0x40;
	tspat[0x02] = 0x00; // PID = 0x0000
	tspat[0x03] = 0x10; // | (ps->pat_counter & 0x0f);
	tspat[0x04] = 0x00; // pointer_field. CRC calculation begins here
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
	pmtpid = 0xff;
	while ( pids.contains( pmtpid ) ) pmtpid--;
	tspat[0x11] = 0x03; tspat[0x12] = 0xe8; tspat[0x13] = 0xe0; tspat[0x14] = pmtpid;
	// Put CRC in ts[0x15...0x18]
	calculateCRC( tspat + 0x05, tspat + 0x15 );
	// needed stuffing bytes
	for (i=0x19 ; i < 188 ; i++) tspat[i]=0xff;
}



void DVBout::writePmt()
{
	int i, off=0;

	tspmt[0x00] = 0x47; //sync_byte
	tspmt[0x01] = 0x40;
	tspmt[0x02] = pmtpid;
	tspmt[0x03] = 0x10;
	tspmt[0x04] = 0x00; // pointer_field. CRC calculation begins here
	tspmt[0x05] = 0x02; // 0x02: Program map section
	tspmt[0x06] = 0xb0;
	tspmt[0x07] = 0x20; // section_length
	tspmt[0x08] = 0x03;
	tspmt[0x09] = 0xe8; // prog number
	tspmt[0x0a] = 0xc1;
	// section # and last section #
	tspmt[0x0b] = tspmt[0x0c] = 0x00;
	if ( channel.vpid ) {
		// PCR PID
		tspmt[0x0d] = channel.vpid>>8; tspmt[0x0e] = channel.vpid&0xff;
	}
	else if ( channel.napid )
		tspmt[0x0d] = channel.apid[0].pid>>8; tspmt[0x0e] = channel.apid[0].pid&0xff;
	// program_info_length == 0
	tspmt[0x0f] = 0xf0; tspmt[0x10] = 0x00;
	if ( channel.vpid ) {
		// Program Map / Video PID
		tspmt[0x11] = channel.vType; // video stream type
		tspmt[0x12] = channel.vpid>>8; tspmt[0x13] = channel.vpid&0xff;
		tspmt[0x14] = 0xf0; tspmt[0x15] = 0x00; // es info length
		off = 0x15;
	}
	else
		off = 0x10;
	// audio pids
	i = 0;
	for ( i=0; i<channel.napid && i<MAX_AUDIO; i++ ) {
		if ( channel.apid[i].ac3 ) {
			tspmt[++off] = 0x81; // stream type = xine see this as ac3
			tspmt[++off] = channel.apid[i].pid>>8; tspmt[++off] = channel.apid[i].pid&0xff;
			tspmt[++off] = 0x00; tspmt[++off] = 0x0c; // es info length
			tspmt[++off] = 0x05; tspmt[++off] = 0x04; tspmt[++off] = 0x41;
			tspmt[++off] = 0x43; tspmt[++off] = 0x2d; tspmt[++off] = 0x33;
		}
		else {
			tspmt[++off] = 0x04; // stream type = audio
			tspmt[++off] = channel.apid[i].pid>>8; tspmt[++off] = channel.apid[i].pid&0xff;
			tspmt[++off] = 0xf0; tspmt[++off] = 0x06; // es info length
		}
		tspmt[++off] = 0x0a; // iso639 descriptor tag
		tspmt[++off] = 0x04; // descriptor length
		if ( !channel.apid[i].lang.isEmpty() ) {
			tspmt[++off] = channel.apid[i].lang.constref(0);
			tspmt[++off] = channel.apid[i].lang.constref(1);
			if ( channel.apid[i].ac3 )
				tspmt[++off] = '_';
			else
				tspmt[++off] = channel.apid[i].lang.constref(2);
		}
		else if ( channel.apid[i].ac3 ) {
			tspmt[++off] = 'd';
			tspmt[++off] = 'd';
			tspmt[++off] = 49+i;
		}
		else {
			tspmt[++off] = 'c';
			tspmt[++off] = 'h';
			tspmt[++off] = 49+i;
		}
		tspmt[++off] = 0x00; // audio type
	}
	// Subtitles
	for ( i=0; i<channel.nsubpid && i<MAX_DVBSUB; i++ ) {
		tspmt[++off] = 0x06; // stream type = ISO_13818_PES_PRIVATE
		tspmt[++off] = channel.subpid[i].pid>>8; tspmt[++off] = channel.subpid[i].pid&0xff;
		tspmt[++off] = 0xf0; tspmt[++off] = 0x0a; // es info length
		tspmt[++off] = 0x59; //DVB sub tag
		tspmt[++off] = 0x08; // descriptor length
		if ( !channel.subpid[i].lang.isEmpty() ) {
			tspmt[++off] = channel.subpid[i].lang.constref(0);
			tspmt[++off] = channel.subpid[i].lang.constref(1);
			tspmt[++off] = channel.subpid[i].lang.constref(2);
		}
		else {
			tspmt[++off] = 'c';
			tspmt[++off] = 'h';
			tspmt[++off] = 49+i;
		}
		tspmt[++off] = channel.subpid[i].type; //sub type
		tspmt[++off] = channel.subpid[i].page>>8; tspmt[++off] = channel.subpid[i].page&0xff; // comp_page_id
		tspmt[++off] = channel.subpid[i].id>>8; tspmt[++off] = channel.subpid[i].id&0xff; // anc_page_id
	}
	tspmt[0x07] = off-3; // update section_length
	// Put CRC in ts[0x29...0x2c]
	calculateCRC( tspmt+0x05, tspmt+off+1 );
	// needed stuffing bytes
	for (i=off+5 ; i < 188 ; i++) tspmt[i]=0xff;
}



bool DVBout::hasInstantRec() const
{
	return instantRec;
}



bool DVBout::hasRec() const
{
	return haveRec;
}



bool DVBout::hasLive() const
{
	if ( haveLive || fdPipe || timeShifting )
		return true;
	return false;
}



bool DVBout::hasBroadcast() const
{
	return haveBroadcast;
}



bool DVBout::doPause( const QString &name ) // called from dvbstream::run()
{
	if ( !haveLive )
		return false;

	if ( !timeShifting ) {
		liveFile.setName( name );
		liveFile.open( IO_WriteOnly|IO_Truncate );
		liveFile.writeBlock( (char*)tspat, TS_SIZE );
		liveFile.writeBlock( (char*)tspmt, TS_SIZE );
		mutex.lock();
		haveLive = false;
		if ( !wait(1000) ) {
			terminate();
			wait();
		}
		if ( close( fdPipe )<0 )
			perror("close out pipe: ");
		else
			fprintf(stderr,"out pipe closed\n");
		fdPipe = 0;
		if ( wDist>0 )
			liveFile.writeBlock( (char*)(wBuf+(wRead*TS_SIZE*NTS)), TS_SIZE*NTS*wDist );
		timeShifting = true;
		mutex.unlock();
		//emit shifting( timeShifting );
	}
	return true;
}



void DVBout::setPatPmt()
{
	patpmt = true;
}



bool DVBout::goLive( const QString &name, int ringBufSize )
{
	if ( fdPipe ) return false;

	haveLive = true;
	pipeName = name;
	beginLive = true;
	//activeApid = napid;

	writePat();
	writePmt();
	if ( !pids.contains(8192) )
		patpmt = wpatpmt = true;
	wbufSize = ringBufSize*1024*1024/(TS_SIZE*NTS);
	wBuf = new unsigned char[TS_SIZE*NTS*wbufSize];
	if ( !wBuf ) fprintf( stderr, "\nNO WBUF !!!\n\n" );
	wRead = wWrite = wDist = 0;
	start();
	return true;
}



void DVBout::preStopLive()
{
	mutex.lock();
	haveLive = false;
	mutex.unlock();
}



void DVBout::stopLive()
{
	mutex.lock();
	if ( timeShifting ) {
		liveFile.close();
		timeShifting = false;
		emit shifting( timeShifting );
	}
	mutex.unlock();
	if ( !wait(1000) ) {
		terminate();
		wait();
	}
	if ( fdPipe ) {
		close( fdPipe );
		fprintf( stderr, "pipe closed\n" );
		fdPipe = 0;
	}
	delete [] wBuf;
	wBuf = NULL;
}



bool DVBout::goBroadcast( Ts2Rtp *r )
{
	if ( haveBroadcast ) return false;
	fprintf(stderr,"Start Broadcast: %s\n", channel.name.ascii() );
	rtp = r;
	haveBroadcast = true;
	return true;
}



void DVBout::stopBroadcast()
{
	if ( !haveBroadcast ) return;
	fprintf(stderr,"Stop Broadcast: %s\n", channel.name.ascii() );
	mutex.lock();
	rtp->removeChannels();
	haveBroadcast = false;
	mutex.unlock();
}



bool DVBout::goRec( const QString &name, int maxsize, RecTimer *t )
{
	QString fname=name;

	if ( haveRec )
		return false;

	recTimer = t;
	fileName=name;
	fileNumber=0;

	if (maxsize>0) {
		fileMaxSize = (long long int)1048576*(long long int)maxsize;
		fname=fileName+"_"+QString().setNum(fileNumber);
	}
	else {
		fileMaxSize = 0;
		fname=fileName;
	}
	if ( QFile(fname+".m2t").exists() )
		renameFile( fname, ".m2t" );
	writePat();
	writePmt();
	if ( channel.apid[0].pid!=8192 )
		patpmt = true;
	outFile.setName( fname+".m2t" );
	if ( !outFile.open( IO_WriteOnly | IO_Truncate ) )
		return false;
	outFile.writeBlock( (char*)tspat, TS_SIZE );
	outFile.writeBlock( (char*)tspmt, TS_SIZE );
	recTimer->fullPath = fname+".m2t";

	haveRec = true;
	if ( recTimer ) {
		QTime t = recTimer->duration.addSecs( QDateTime::currentDateTime().secsTo(recTimer->begin) );
		stopRecTimer.start( QTime().msecsTo( t ), true );
	}
	else
		instantRec = true;
	fprintf( stderr, "Recording started: %s\n", channel.name.latin1() );
	return true;
}



void DVBout::renameFile( QString &name, const QString &ext )
{
	int index=1;

	while ( QFile(name+"-"+QString().setNum(index)+ext).exists() )
		index++;

	name = name+"-"+QString().setNum(index);
}



void DVBout::changeTimer( int ms )
{
	if ( stopRecTimer.isActive() )
		stopRecTimer.changeInterval( ms );
}



void DVBout::stopRec()
{
	if ( !haveRec )
		return;

	if ( stopRecTimer.isActive() )
		stopRecTimer.stop();
	mutex.lock();
	outFile.close();
	mutex.unlock();
	haveRec = instantRec = false;
	fprintf( stderr, "Recording stopped: %s\n", channel.name.latin1() );
	if ( !haveLive && !haveBroadcast )
		emit endRecord( this, recTimer, true );
	else
		emit endRecord( this, recTimer, false );
}



void DVBout::process( unsigned char *buf, int size )
{
	int i, pid;
	unsigned char *buffer=buf;
	QString fname;

	for ( i=0; i<size; i+=TS_SIZE ) {
		pid = (((buffer[1] & 0x1f) << 8) | buffer[2]);
		if ( pids.contains( pid ) || pids.contains( 8192) ) {
			memcpy( thBuf+thWrite, buffer, TS_SIZE );
			thWrite+=TS_SIZE;
			if ( thWrite==(TS_SIZE*NTS ) ) {
				if ( plug && plugHandle )
					plug->process( plugHandle, thBuf, TS_SIZE*NTS );
				mutex.lock();
				if ( haveLive && fdPipe ) {
					if ( beginLive ) {
						beginLive = !beginLive;
						start();
					}
					if ( wDist<wbufSize ) {
						memcpy( wBuf+(wWrite*TS_SIZE*NTS), thBuf, TS_SIZE*NTS );
						wpatpmt = patpmt;
						++wDist;
						++wWrite;
						if ( wWrite==wbufSize )
							wWrite = 0;
					}
					else {
						fprintf(stderr,"Live ringbuffer full!! (%d)\n",wDist);
					}
				}
				else if ( timeShifting ) {
					if ( patpmt ) {
						liveFile.writeBlock( (char*)tspat, TS_SIZE );
						liveFile.writeBlock( (char*)tspmt, TS_SIZE );
					}
					liveFile.writeBlock( (char*)thBuf, TS_SIZE*NTS );
				}
				if ( haveRec && fileNumber>=0 ) {
					if ((fileMaxSize>0)&&(outFile.size()>=fileMaxSize)) {
						outFile.close();
						fileNumber++;
						fname=fileName+"_"+QString().setNum(fileNumber);
						if (QFile(fname+"_"+QString().setNum(fileNumber)+".m2t").exists() )
							renameFile( fname, ".m2t" );

						outFile.setName( fname+".m2t" );
						if ( !outFile.open( IO_WriteOnly | IO_Truncate ) )
							fileNumber=-1;
						else
						{
							outFile.writeBlock( (char*)tspat, TS_SIZE );
							outFile.writeBlock( (char*)tspmt, TS_SIZE );
							recTimer->fullPath = fname+".m2t";
						}
					}
					if (fileNumber>=0)
					{
						if ( patpmt ) {
							outFile.writeBlock( (char*)tspat, TS_SIZE );
							outFile.writeBlock( (char*)tspmt, TS_SIZE );
						}
						outFile.writeBlock( (char*)thBuf, TS_SIZE*NTS );
					}

				}
				if ( haveBroadcast )
					rtp->process( thBuf, TS_SIZE*NTS );
				patpmt = false;
				mutex.unlock();
				thWrite = 0;
			}
		}
		buffer+=TS_SIZE;
	}
}



void DVBout::run()
{
	if ( haveLive && fdPipe ) {
		while ( haveLive && fdPipe ) {
			if ( wDist>5 ) {
				if ( wpatpmt ) {
					write( fdPipe, tspat, TS_SIZE );
					write( fdPipe, tspmt, TS_SIZE );
					wpatpmt = false;
				}
				write( fdPipe, wBuf+(wRead*TS_SIZE*NTS), TS_SIZE*NTS );
				--wDist;
				++wRead;
				if ( wRead==wbufSize )
					wRead = 0;
			}
			else {
				usleep( 100 );
			}
		}
		return;
	}

	if ( (fdPipe=open( pipeName.ascii(), O_WRONLY))<0 ) {
		perror("PIPE FILE: ");
		return;
	}
	fprintf(stderr,"pipe opened\n");
	emit playDvb();
}



DVBout::~DVBout()
{
	if ( plug )
		plug->close( plugHandle );
}

#include "dvbout.moc"
