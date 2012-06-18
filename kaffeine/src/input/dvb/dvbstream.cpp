/*
 * dvbstream.cpp
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
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <resolv.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <values.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <math.h>

#include <linux/dvb/dmx.h>

#include <qdir.h>
#include <qprogressdialog.h>

#include <klocale.h>
#include <kapplication.h>
#include <kmessagebox.h>

#include "dvbstream.h"
#include "dvbevents.h"
#include "gdvb.h"
#include "dvbcam.h"
#include "kaffeinedvbplugin.h"

#define MAXTUNETRY 1



DvbStream::DvbStream( Device *d, const QString &charset, EventTable *et )
{
	dvbDevice = d;
	isRunning = false;
	timeShifting = false;
	waitPause = 0;
	delOut = 0;
	fdFrontend = fdDvr = 0;
	ndmx = 0;
	currentTransponder = Transponder();
	frontendName = QString("/dev/dvb/adapter%1/frontend%2").arg( dvbDevice->adapter ).arg( dvbDevice->tuner );
	dvrName = QString("/dev/dvb/adapter%1/dvr%2").arg( dvbDevice->adapter ).arg( dvbDevice->tuner );
	demuxName = QString("/dev/dvb/adapter%1/demux%2").arg( dvbDevice->adapter ).arg( dvbDevice->tuner );

	out.setAutoDelete( true );

	QString deviceType="NONE";
	switch ( dvbDevice->type ) {
		case FE_QPSK: deviceType = "DVBS"; break;
		case FE_OFDM: deviceType = "DVBT"; break;
		case FE_QAM: deviceType = "DVBC"; break;
		case FE_ATSC: deviceType = "DVBA"; break;
	}
	dvbEvents = new DVBevents( deviceType, dvbDevice->adapter, dvbDevice->tuner, charset, et );

	camProbed = false;
	cam = NULL;
	plug = NULL;

	connect( &statusTimer, SIGNAL(timeout()), this, SLOT(checkStatus()) );
}



void DvbStream::setPlug( KaffeineDvbPlugin *p )
{
	plug = p;
}



void DvbStream::probeCam()
{
	int ci_type=0;
	if ( camProbed )
		return;
	if ( (ci_type=DvbCam::probe( dvbDevice->adapter, 0 ))>0 ) {
		cam = new DvbCam( dvbDevice->adapter, 0, dvbDevice->tuner, ci_type, dvbDevice->camMaxService );
		dvbDevice->hasCAM = true;
	}
	camProbed = true;
}



void DvbStream::showCamDialog()
{
	if ( cam )
		dvbDevice->camMaxService = cam->showCamDialog();
}



QStringList DvbStream::getSources( bool all )
{
	if ( !all ) {
		if ( dvbDevice->type==FE_OFDM )
			return QStringList( "Terrestrial" );
		if ( dvbDevice->type==FE_QAM )
			return QStringList( "Cable" );
		if ( dvbDevice->type==FE_ATSC )
			return QStringList( "Atsc" );
	}
	QStringList source;
	int i;
	if ( dvbDevice->type==FE_QPSK ) {
		for ( i=0; i<dvbDevice->numLnb; i++ )
			source+= dvbDevice->lnb[i].source;
	}
	else {
		QString s = dvbDevice->source;
		source+= s.remove(0,2);
	}
	return source;
}



bool DvbStream::canSource( ChannelDesc *chan )
{
	if ( chan->tp.type!=dvbDevice->type ) {
		return false;
	}
	if ( dvbDevice->type!=FE_QPSK ) {
		if ( chan->tp.type==dvbDevice->type )
			return true;
		else
			return false;
	}
	if ( chan->tp.S2 && !dvbDevice->doS2 )
		return false;
	int i;
	for ( i=0; i<dvbDevice->numLnb; i++ ) {
		if ( dvbDevice->lnb[i].source.contains(chan->tp.source) )
			return true;
	}
	return false;
}



int DvbStream::getPriority()
{
	return dvbDevice->priority;
}



int DvbStream::getSatPos( const QString &src )
{
	int i;

	if ( dvbDevice->type!=FE_QPSK )
		return -1;

	for ( i=0; i<dvbDevice->numLnb; i++ )
		if ( dvbDevice->lnb[i].source.contains(src) )
			return i;
	return -1;
}



bool DvbStream::openFe()
{
	if ( fdFrontend ) {
		fprintf(stderr,"openFe: fdFrontend != 0\n");
		return false;
	}
	fdFrontend = open( frontendName.ascii(), O_RDWR /*| O_NONBLOCK*/ );
	if ( fdFrontend<0 ) {
		perror("openFe:");
		fdFrontend = 0;
		return false;
	}
	return true;
}



bool DvbStream::closeFe()
{
	if ( !fdFrontend )
		return true;
	if ( close( fdFrontend )<0 ) {
		perror("closeFe: ");
		return false;
	}
	fprintf(stderr,"Frontend closed\n");
	fdFrontend = 0;
	currentTransponder = Transponder();
	return true;
}



void DvbStream::connectStatus( bool con )
{
	if ( con )
		statusTimer.start( 1000 );
	else
		statusTimer.stop();
}



ChannelDesc DvbStream::getLiveChannel()
{
	int i;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( out.at(i)->hasLive() )
			return out.at(i)->channel;
	}
	return ChannelDesc();
}



bool DvbStream::hasLive()
{
	int i;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( out.at(i)->hasLive() )
			return true;
	}
	return false;
}



bool DvbStream::hasRec()
{
	int i;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( out.at(i)->hasRec() )
			return true;
	}
	return false;
}



bool DvbStream::hasBroadcast()
{
	int i;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( out.at(i)->hasBroadcast() )
			return true;
	}
	return false;
}



bool DvbStream::liveIsRecording()
{
	int i;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( out.at(i)->hasLive() ) {
			if ( out.at(i)->hasRec() )
				return true;
			else
				return false;
		}
	}
	return false;
}



Transponder DvbStream::getCurrentTransponder()
{
	return currentTransponder;
}



unsigned long DvbStream::getCurrentFreq()
{
	return currentTransponder.freq;
}



bool DvbStream::isTuned()
{
	if ( fdFrontend )
		return true;
	else
		return false;
}



bool DvbStream::tuneDvb( ChannelDesc *chan, bool dvr )
{
	unsigned long lof=0;
	int res, hiband=0;
	struct dvb_frontend_info fe_info;
	fe_status_t status;
	unsigned long freq=chan->tp.freq;
	unsigned long srate=chan->tp.sr;
	int lnbPos = getSatPos( chan->tp.source );
	int rotorMove = 0;
	int loop=0, i;

	struct dtv_property cmdargs[20];
	struct dtv_properties cmdseq;
	int inversion;
	int bandwidth;
	
	if ( chan->tp.S2 && !dvbDevice->doS2 )
		return false;

	closeFe();
	if ( !openFe() )
		return false;

	if ( (res = ioctl( fdFrontend, FE_GET_INFO, &fe_info ) < 0) ) {
		perror("FE_GET_INFO: ");
		return false;
	}

	probeCam();

	fprintf(stderr, "Using DVB device %d:%d \"%s\"\n", dvbDevice->adapter, dvbDevice->tuner, fe_info.name);

	freq*=1000;
	srate*=1000;

	QTime t1 = QTime::currentTime();

	if ( chan->tp.inversion==INVERSION_AUTO ) {
		if ( fe_info.caps & FE_CAN_INVERSION_AUTO )
			inversion = chan->tp.inversion;
		else {
			fprintf(stderr,"Can NOT inversion_auto\n");
			inversion = INVERSION_OFF;
		}
	}
	else
		inversion=chan->tp.inversion;

	switch( fe_info.type ) {
		case FE_OFDM : {
			QString s = fe_info.name;
			//if ( s.contains("TerraTec/qanu USB2.0 Highspeed DVB-T Receiver") ) // cinergyT2 hack
			//	freq+=167000;
			if (freq < 1000000)
				freq*=1000UL;
			cmdargs[0].cmd = DTV_DELIVERY_SYSTEM; cmdargs[0].u.data = SYS_DVBT;
			cmdargs[1].cmd = DTV_FREQUENCY; cmdargs[1].u.data = freq;
			cmdargs[2].cmd = DTV_MODULATION; cmdargs[2].u.data = chan->tp.modulation;
			cmdargs[3].cmd = DTV_CODE_RATE_HP; cmdargs[3].u.data = chan->tp.coderateH;
			cmdargs[4].cmd = DTV_CODE_RATE_LP; cmdargs[4].u.data = chan->tp.coderateL;
			cmdargs[5].cmd = DTV_GUARD_INTERVAL; cmdargs[5].u.data = chan->tp.guard;
			cmdargs[6].cmd = DTV_TRANSMISSION_MODE; cmdargs[6].u.data = chan->tp.transmission;
			cmdargs[7].cmd = DTV_HIERARCHY; cmdargs[7].u.data = chan->tp.hierarchy;
			if ( chan->tp.bandwidth==BANDWIDTH_8_MHZ )
				bandwidth = 8000000;
			else if ( chan->tp.bandwidth==BANDWIDTH_7_MHZ )
				bandwidth = 7000000;
			else if ( chan->tp.bandwidth==BANDWIDTH_6_MHZ )
				bandwidth = 6000000;
			cmdargs[8].cmd = DTV_BANDWIDTH_HZ; cmdargs[8].u.data = bandwidth;
			cmdargs[9].cmd = DTV_INVERSION; cmdargs[9].u.data = inversion;
			cmdargs[10].cmd = DTV_TUNE;
			cmdseq.num = 11;
			cmdseq.props = cmdargs;
			fprintf(stderr,"tuning DVB-T to %lu Hz\n", freq);
			fprintf(stderr,"inv:%d bw:%d fecH:%d fecL:%d mod:%d tm:%d gi:%d hier:%d\n", chan->tp.inversion,
				chan->tp.bandwidth, chan->tp.coderateH, chan->tp.coderateL, chan->tp.modulation,
				chan->tp.transmission, chan->tp.guard, chan->tp.hierarchy );
			break;
		}
		case FE_QAM : {
			cmdargs[0].cmd = DTV_DELIVERY_SYSTEM; cmdargs[0].u.data = SYS_DVBC_ANNEX_AC;
			cmdargs[1].cmd = DTV_FREQUENCY; cmdargs[1].u.data = freq;
			cmdargs[2].cmd = DTV_MODULATION; cmdargs[2].u.data = chan->tp.modulation;
			cmdargs[3].cmd = DTV_SYMBOL_RATE; cmdargs[3].u.data = srate;
			cmdargs[4].cmd = DTV_INNER_FEC; cmdargs[4].u.data = chan->tp.coderateH;
			cmdargs[5].cmd = DTV_INVERSION; cmdargs[5].u.data = inversion;
			cmdargs[6].cmd = DTV_TUNE;
			cmdseq.num = 7;
			cmdseq.props = cmdargs;
			fprintf(stderr,"tuning DVB-C to %lu\n", freq);
			fprintf(stderr,"inv:%d sr:%lu fecH:%d mod:%d\n", chan->tp.inversion,
				srate, chan->tp.coderateH, chan->tp.modulation );
			break;
		}
		case FE_QPSK : {
			fprintf(stderr,"tuning DVB-S to %lu %c %lu\n", freq, chan->tp.pol, srate);
			if (freq > 2200000) {
				if ( dvbDevice->lnb[lnbPos].switchFreq ) { // Dual LO
					if ( freq<(dvbDevice->lnb[lnbPos].switchFreq*1000) ) {
						lof = dvbDevice->lnb[lnbPos].loFreq*1000;
					}
					else {
						lof = dvbDevice->lnb[lnbPos].hiFreq*1000;
						hiband = 1;
					}
				}
				else {
					if ( dvbDevice->lnb[lnbPos].hiFreq ) { // C Band Multipoint
						if ( (chan->tp.pol=='h') || (chan->tp.pol=='H') )
							lof = (dvbDevice->lnb[lnbPos].hiFreq*1000);
						else
							lof = (dvbDevice->lnb[lnbPos].loFreq*1000);
					}
					else // Single LO
						lof = (dvbDevice->lnb[lnbPos].loFreq*1000);
				}
				if ( freq<lof )
					freq = ( lof-freq );
				else
					freq = ( freq-lof );
			}
			fprintf(stderr,"inv:%d fecH:%d mod:%d\n", chan->tp.inversion, chan->tp.coderateH, chan->tp.modulation );
			if ( setDiseqc( lnbPos, chan, hiband, rotorMove, dvr )!=0 ) {
				closeFe();
				return false;
			}
			fprintf( stderr, "Diseqc settings time = %d ms\n", t1.msecsTo( QTime::currentTime() ) );
			t1 = QTime::currentTime();
			if ( chan->tp.S2 ) {
				fprintf(stderr,"\nTHIS IS DVB-S2 >>>>>>>>>>>>>>>>>>>\n");
				cmdargs[0].cmd = DTV_DELIVERY_SYSTEM; cmdargs[0].u.data = SYS_DVBS2;
				cmdargs[1].cmd = DTV_FREQUENCY; cmdargs[1].u.data = freq;
				cmdargs[2].cmd = DTV_MODULATION; cmdargs[2].u.data = chan->tp.modulation;
				cmdargs[3].cmd = DTV_SYMBOL_RATE; cmdargs[3].u.data = srate;
				cmdargs[4].cmd = DTV_INNER_FEC; cmdargs[4].u.data = chan->tp.coderateH;
				cmdargs[5].cmd = DTV_PILOT; cmdargs[5].u.data = PILOT_AUTO;
				cmdargs[6].cmd = DTV_ROLLOFF; cmdargs[6].u.data = chan->tp.rolloff;
				cmdargs[7].cmd = DTV_INVERSION; cmdargs[7].u.data = inversion;
				cmdargs[8].cmd = DTV_TUNE;
				cmdseq.num = 9;
				cmdseq.props = cmdargs;
			}
			else {
				cmdargs[0].cmd = DTV_DELIVERY_SYSTEM; cmdargs[0].u.data = SYS_DVBS;
				cmdargs[1].cmd = DTV_FREQUENCY; cmdargs[1].u.data = freq;
				cmdargs[2].cmd = DTV_MODULATION; cmdargs[2].u.data = chan->tp.modulation;
				if ( chan->tp.modulation==QAM_AUTO )
					cmdargs[2].u.data = QPSK;
				cmdargs[3].cmd = DTV_SYMBOL_RATE; cmdargs[3].u.data = srate;
				cmdargs[4].cmd = DTV_INNER_FEC; cmdargs[4].u.data = chan->tp.coderateH;
				cmdargs[5].cmd = DTV_INVERSION; cmdargs[5].u.data = inversion;
				cmdargs[6].cmd = DTV_TUNE;
				cmdseq.num = 7;
				cmdseq.props = cmdargs;
			}
			break;
		}
		case FE_ATSC : {
			cmdargs[0].cmd = DTV_DELIVERY_SYSTEM; cmdargs[0].u.data = SYS_ATSC;
			cmdargs[1].cmd = DTV_FREQUENCY; cmdargs[1].u.data = freq;
			cmdargs[2].cmd = DTV_MODULATION; cmdargs[2].u.data = chan->tp.modulation;
			cmdargs[3].cmd = DTV_INVERSION; cmdargs[3].u.data = inversion;
			cmdargs[4].cmd = DTV_TUNE;
			cmdseq.num = 5;
			cmdseq.props = cmdargs;
			fprintf(stderr,"tuning ATSC to %lu\n", freq);
			fprintf(stderr,"inv:%d mod:%d\n", chan->tp.inversion, chan->tp.modulation );
			break;
		}
	}

	if ( rotorMove ) {
		if ( ioctl( fdFrontend, FE_SET_PROPERTY, &cmdseq )<0 ) {
			perror("ERROR tuning\n");
			closeFe();
			return false;
		}
		moveRotor( lnbPos, chan, hiband, dvr );
		loop = 2;
	}

	while ( loop>-1 ) {
		if ( ioctl( fdFrontend, FE_SET_PROPERTY, &cmdseq )<0 ) {
			perror("ERROR tuning\n");
			closeFe();
			return false;
		}
		QTime lockTime = QTime::currentTime();
		do {
			usleep( 100000 );
			fprintf( stderr, "." );
			if ( ioctl( fdFrontend, FE_READ_STATUS, &status )==-1 ) {
				perror( "FE_READ_STATUS" );
				break;
			}
			if ( status & FE_HAS_LOCK ) {
				fprintf(stderr," LOCKED.");
				loop = 0;
				break;
			}
		} while ( lockTime.msecsTo( QTime::currentTime() )<=dvbDevice->tuningTimeout );
		fprintf(stderr,"\n");
		--loop;
	}

	if ( !( status & FE_HAS_LOCK ) ) {
		fprintf( stderr, "\nNot able to lock to the signal on the given frequency\n" );
		closeFe();
		return false;
	}

	fprintf( stderr, "Tuning time = %d ms\n", t1.msecsTo( QTime::currentTime() ) );

	if ( rotorMove )
		dvbDevice->lnb[lnbPos].currentSource = chan->tp.source;

	if ( !dvr )
		return true;

	if ( ( fdDvr = open( dvrName.ascii(), O_RDONLY|O_NONBLOCK)) < 0 ) {
		perror("DVR DEVICE: ");
		closeFe();
		fdDvr = 0;
		return false;
	}
	pfd.fd=fdDvr;
	pfd.events=POLLIN|POLLPRI;

	currentTransponder = chan->tp;
	return true;
}



int DvbStream::setDiseqc( int switchPos, ChannelDesc *chan, int hiband, int &rotor, bool dvr )
{
	struct dvb_diseqc_master_cmd switchCmd[] = {
		{ { 0xe0, 0x10, 0x38, 0xf0, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf2, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf1, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf3, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf4, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf6, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf5, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf7, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf8, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xfa, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xf9, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xfb, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xfc, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xfe, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xfd, 0x00, 0x00 }, 4 },
		{ { 0xe0, 0x10, 0x38, 0xff, 0x00, 0x00 }, 4 },
	};

	int i;
	int voltage18 = ( (chan->tp.pol=='H')||(chan->tp.pol=='h') );
	int ci = 4 * switchPos + 2 * hiband + (voltage18 ? 1 : 0);
	bool secMini = false;
	bool hasRotor = false;
	bool hasSwitch = true;
	
	if ( dvbDevice->numLnb<2 )
		hasSwitch = false;
		
	if ( dvbDevice->lnb[switchPos].rotorType!=0 && dvbDevice->lnb[switchPos].rotorType!=3 )
		hasRotor = true;
		
	if ( dvbDevice->numLnb==2 && dvbDevice->secMini )
		secMini = true;
		
	if ( dvbDevice->secTwice )
		diseqcTwice = 2;
	else
		diseqcTwice = 1;

	fprintf( stderr, "DiSEqC: switch pos %i, %sV, %sband (index %d)\n", switchPos, voltage18 ? "18" : "13", hiband ? "hi" : "lo", ci );
	if ( ci < 0 || ci >= (int)(sizeof(switchCmd)/sizeof(struct dvb_diseqc_master_cmd)) )
		return -EINVAL;

	if ( ioctl(fdFrontend, FE_SET_TONE, SEC_TONE_OFF) )
		perror("FE_SET_TONE failed");
	usleep(15*1000);
	if ( ioctl(fdFrontend, FE_SET_VOLTAGE, ci%2 ? SEC_VOLTAGE_18 : SEC_VOLTAGE_13) )
		perror("FE_SET_VOLTAGE failed");

	if ( hasSwitch ) {
		if ( !secMini ) {
			fprintf( stderr, "DiSEqC: %02x %02x %02x %02x %02x %02x\n", switchCmd[ci].msg[0], switchCmd[ci].msg[1], switchCmd[ci].msg[2], switchCmd[ci].msg[3], switchCmd[ci].msg[4], 	switchCmd[ci].msg[5] );
			for ( i=0; i<diseqcTwice; ++i ) {
				usleep(15*1000);
				if ( ioctl(fdFrontend, FE_DISEQC_SEND_MASTER_CMD, &switchCmd[ci]) )
					perror("FE_DISEQC_SEND_MASTER_CMD failed");
			}
		}
		else {
			fprintf( stderr, "DiSEqC: mini_diseqc\n" );
			for ( i=0; i<diseqcTwice; ++i ) {
				usleep(15*1000);
				if ( ioctl(fdFrontend, FE_DISEQC_SEND_BURST, (ci/4)%2 ? SEC_MINI_B : SEC_MINI_A) )
					perror("FE_DISEQC_SEND_BURST failed");
			}
		}
	}

	if ( hasRotor && chan->tp.source!=dvbDevice->lnb[switchPos].currentSource ) {
		rotor = 1;
		return 0;
	}

	if ( (ci/2)%2 ) {
		usleep(15*1000);
		if ( ioctl(fdFrontend, FE_SET_TONE, SEC_TONE_ON) )
			perror("FE_SET_TONE failed");
	}

	return 0;
}



void DvbStream::moveRotor( int switchPos, ChannelDesc *chan, int hiband, bool dvr )
{
	int i, j, index=-1;
	double angle=0.0, oldAngle=0.0;
	int rotor=0;
	int voltage18 = ( (chan->tp.pol=='H')||(chan->tp.pol=='h') );
	int ci = 4 * switchPos + 2 * hiband + (voltage18 ? 1 : 0);
	QString msg;

	fprintf( stderr, "Driving rotor to %s\n", chan->tp.source.ascii() );
	for ( i=0; i<(int)dvbDevice->lnb[switchPos].source.count(); i++ ) {
		if ( dvbDevice->lnb[switchPos].source[i]==chan->tp.source ) {
			index = i;
			break;
		}
	}
	angle = getSourceAngle( chan->tp.source );
	if ( dvbDevice->lnb[switchPos].rotorType==1 ) {
		fprintf( stderr, "Rotor: gotoX=%f\n", angle );
		gotoX( angle );
	}
	else {
		int pos = dvbDevice->lnb[switchPos].position[index];
		fprintf( stderr, "Rotor: gotoN=%d\n", pos );
		rotorCommand( 9, pos );
	}
	if ( dvbDevice->lnb[switchPos].currentSource.isEmpty() ) {
		rotor = 10;
		msg =  i18n("Moving rotor from unknown position...");
	}
	else {
		oldAngle = getSourceAngle( dvbDevice->lnb[switchPos].currentSource );
		fprintf( stderr, "old rotor pos: %f °\n", oldAngle );
		fprintf( stderr, "new rotor pos: %f °\n", angle );
		angle = fabs(angle-oldAngle);
		fprintf( stderr, "Rotation angle: %f °\n", angle );
		if ( voltage18 )
			rotor = (int)(angle*dvbDevice->lnb[switchPos].speed18v)+1;
		else
			rotor = (int)(angle*dvbDevice->lnb[switchPos].speed13v)+1;
		 msg = i18n("Moving rotor...");
	}
	fprintf( stderr, "Rotation time: %d sec.\n", rotor );
	
	if ( !dvr ) {
		for ( j=0; j<(rotor*2); j++ ) {
			usleep( 500000 );
		}
	}
	else {
		QProgressDialog progress( msg, i18n("Cancel"), rotor*2, 0, "progress", true );
		for ( j=0; j<(rotor*2); j++ ) {
			progress.setProgress( j );
			qApp->processEvents();
			if ( progress.wasCanceled() )
				break;
			usleep( 500000 );
		}
		progress.setProgress( rotor*2 );
		qApp->processEvents();
	}

	if ( (ci/2)%2 ) {
		usleep(15*1000);
		if ( ioctl(fdFrontend, FE_SET_TONE, SEC_TONE_ON) )
			perror("FE_SET_TONE failed");
	}
}



double DvbStream::getSourceAngle( QString source )
{
	double angle=1.0;
	int pos = source.findRev("-");
	source.remove(0,pos+1);
	source = source.upper();
	if ( source.endsWith("W") )
		angle = -1.0;
	source.truncate( source.length()-1 );
	angle*= source.toDouble();
	return getAzimuth( angle );
}



#define TO_RADS (M_PI / 180.0)
#define TO_DEC (180.0 / M_PI)

double DvbStream::getAzimuth( double angle )
{
	double latitude = dvbDevice->usalsLatitude;
	double longitude = dvbDevice->usalsLongitude;
	double P, Ue, Us, az, x, el, Azimuth;

	P = latitude*TO_RADS;           // Earth Station Latitude
	Ue = longitude*TO_RADS;           // Earth Station Longitude
	Us = angle*TO_RADS;          // Satellite Longitude

	az = M_PI+atan( tan( Us-Ue )/sin( P ) );
	x = acos( cos(Us-Ue)*cos(P) );
	el = atan( ( cos( x )-0.1513 )/sin( x ) );
	Azimuth = atan( ( -cos(el)*sin(az) )/( sin(el)*cos(P)-cos(el)*sin(P)*cos(az)) )* TO_DEC;

	return Azimuth;
}



void DvbStream::gotoX( double azimuth )
{
	double USALS=0.0;
	int CMD1=0x00, CMD2=0x00;
	int DecimalLookup[10] = { 0x00, 0x02, 0x03, 0x05, 0x06, 0x08, 0x0A, 0x0B, 0x0D, 0x0E };

	if ( azimuth>0.0 )
		CMD1 = 0xE0;    // East
	else
		CMD1 = 0xD0;      // West

	USALS = fabs( azimuth );

	while (USALS > 16) {
		CMD1++;
		USALS-= 16;
	}
	while (USALS >= 1.0) {
		CMD2+=0x10;
		USALS--;
	}
	USALS*= 10.0;
	int rd = (int)USALS;
	USALS-= rd;
	if ( USALS>0.5 )
		++rd;
	CMD2+= DecimalLookup[rd];

	rotorCommand( 12, CMD1, CMD2 );
}



void DvbStream::rotorCommand( int cmd, int n1, int n2, int n3 )
{
	struct dvb_diseqc_master_cmd cmds[] = {
		{ { 0xe0, 0x31, 0x60, 0x00, 0x00, 0x00 }, 3 },  //0 Stop Positioner movement
		{ { 0xe0, 0x31, 0x63, 0x00, 0x00, 0x00 }, 3 },  //1 Disable Limits
		{ { 0xe0, 0x31, 0x66, 0x00, 0x00, 0x00 }, 3 },  //2 Set East Limit
		{ { 0xe0, 0x31, 0x67, 0x00, 0x00, 0x00 }, 3 },  //3 Set West Limit
		{ { 0xe0, 0x31, 0x68, 0x00, 0x00, 0x00 }, 4 },  //4 Drive Motor East continously
		{ { 0xe0, 0x31, 0x68,256-n1,0x00, 0x00 }, 4 },  //5 Drive Motor East nn steps
		{ { 0xe0, 0x31, 0x69,256-n1,0x00, 0x00 }, 4 },  //6 Drive Motor West nn steps
		{ { 0xe0, 0x31, 0x69, 0x00, 0x00, 0x00 }, 4 },  //7 Drive Motor West continously
		{ { 0xe0, 0x31, 0x6a, n1, 0x00, 0x00 }, 4 },  //8 Store nn
		{ { 0xe0, 0x31, 0x6b, n1, 0x00, 0x00 }, 4 },   //9 Goto nn
		{ { 0xe0, 0x31, 0x6f, n1, n2, n3 }, 4}, //10 Recalculate Position
		{ { 0xe0, 0x31, 0x6a, 0x00, 0x00, 0x00 }, 4 },  //11 Enable Limits
		{ { 0xe0, 0x31, 0x6e, n1, n2, 0x00 }, 5 },   //12 Gotoxx
		{ { 0xe0, 0x10, 0x38, 0xF4, 0x00, 0x00 }, 4 }    //13 User
	};

	int i;
	for ( i=0; i<diseqcTwice; ++i ) {
		usleep(15*1000);
		if ( ioctl( fdFrontend, FE_DISEQC_SEND_MASTER_CMD, &cmds[cmd] ) )
			perror("Rotor : FE_DISEQC_SEND_MASTER_CMD failed");
	}
}



bool DvbStream::setPids( DVBout *o )
{
	int i, dmx;
	struct dmx_pes_filter_params pesFilterParams;
	dmx_pes_type_t pestype = DMX_PES_OTHER;
	QValueList<int> pidList;

	pidList = o->pidsList();

	for ( i=0; i<(int)pidList.count(); i++ ) {
		if ( ( dmx = open( demuxName.ascii(), O_RDWR)) < 0 ) {
			fprintf(stderr,"FD %i: ",i);
			perror("DEMUX DEVICE: ");
			return false;
		}
		else
			o->dmx.append(dmx);
	}

	for ( i=0; i<(int)pidList.count(); i++ ) {
		pesFilterParams.pid = pidList[i];
		pesFilterParams.input = DMX_IN_FRONTEND;
		pesFilterParams.output = DMX_OUT_TS_TAP;
		pesFilterParams.pes_type = pestype;
		pesFilterParams.flags = DMX_IMMEDIATE_START;
		if ( ioctl( o->dmx[i], DMX_SET_PES_FILTER, &pesFilterParams) < 0)  {
			fprintf( stderr, "FILTER %i: ", pidList[i] );
			perror("DMX SET PES FILTER");
		}
	}

	ndmx +=pidList.count();

	return true;
}



void DvbStream::removePids( DVBout *o )
{
	int i;

	for ( i=0; i<(int)o->dmx.count(); i++ )
		close( o->dmx[i] );
	ndmx -=o->dmx.count();
}



void DvbStream::removeOut( DVBout *o )
{
	disconnect( o, SIGNAL(endRecord(DVBout*,RecTimer*,bool)), this, SLOT(recordEnded(DVBout*,RecTimer*,bool)) );
	disconnect( o, SIGNAL(playDvb()), this, SLOT(receivePlayDvb()) );
	disconnect( o, SIGNAL(shifting(bool)), this, SLOT(receiveShifting(bool)) );
	delOut = o;
	while ( delOut )
		usleep(100);
}



bool DvbStream::checkStatus()
{
	int32_t strength;
	fe_status_t festatus;
	bool ret=true;

       strength=0;
	ioctl(fdFrontend,FE_READ_SIGNAL_STRENGTH,&strength);
	emit signalStatus(strength*100/65535);

	strength=0;
	ioctl(fdFrontend,FE_READ_SNR,&strength);
	emit snrStatus(strength*100/65535);

	memset( &festatus, 0, sizeof(festatus) );
	ioctl(fdFrontend,FE_READ_STATUS,&festatus);

	if (festatus & FE_HAS_LOCK)
		emit lockStatus( true );
	else {
		emit lockStatus( false );
		ret = false;
	}

	return ret;
}



int DvbStream::getSNR()
{
	int32_t snr=0;
	if ( fdFrontend ) {
		ioctl( fdFrontend, FE_READ_SNR, &snr );
		snr = snr*100/65535;
	}
	return snr;
}



bool DvbStream::hasVideo()
{
	if ( getLiveChannel().vpid )
		return true;
	return false;
}



void DvbStream::run()
{
	int READSIZE = 188*20;
	int BUFSIZE = 188*100;
	int WSIZE = 188*64;
	unsigned char buf[READSIZE];
	unsigned char thBuf[BUFSIZE];
	int n, i, thWrite=0;
	DVBout *o=0;

	signal( SIGPIPE, SIG_IGN );

	while ( isRunning ) {
		if ( poll( &pfd, 1, 100 ) ) {
			n = read( fdDvr, buf, READSIZE );
			if ( n && !(n%188) ) {
				//fprintf( stderr, "DVR0: read : %d\n", n );
				memcpy( thBuf+thWrite, buf, n );
				thWrite+=n;
				if ( thWrite>=WSIZE ) {
					for ( i=0; i<(int)out.count(); i++ )
						out.at(i)->process( thBuf, thWrite );
					thWrite = 0;
				}
			}
			else
				fprintf( stderr, "DVR0: read failed : %d\n", n );
			    
		}

		if ( waitPause>0 ) {
			o = 0;
			for ( i=0; i<(int)out.count(); i++ )
				if ( out.at(i)->hasLive() )
					o = out.at(i);
			if ( o ) {
				if ( o->doPause( timeShiftFileName ) )
					waitPause = 0;
				else
					waitPause = -1;
			}
			else
				waitPause = -1;
		}

		if ( delOut ) {
			out.remove( delOut );
			delOut = 0;
		}
	}

	fprintf(stderr,"dvbstream::run() end\n");
}



bool DvbStream::doPause( const QString &name )
{
	//if ( !hasVideo() ) return false;

	if ( !timeShifting ) {
		timeShiftFileName = name;
		waitPause = 1;
		while ( waitPause>0 )
			usleep(100);
		if ( waitPause<0 )
			return false;
		else
			receiveShifting( true );
		return true;
	}
	return false;
}



bool DvbStream::timeShiftMode()
{
	return timeShifting;
}



void DvbStream::receiveShifting( bool b )
{
	timeShifting = b;
	emit shifting( b );
}



bool DvbStream::running() const
{
	return isRunning;
}



void DvbStream::receivePlayDvb()
{
	KApplication::kApplication()->postEvent( this, new QTimerEvent( 500 ) );
}



void DvbStream::timerEvent( QTimerEvent* ev )
{
	switch ( ev->timerId() ) {
		case 500:
			emit playDvb();
	}
}



void DvbStream::recordEnded( DVBout *o, RecTimer* t, bool kill )
{
	int i;

	if ( kill ) {
		removePids( o );
		if ( cam )
			cam->stopService( &(o->channel) );
		removeOut( o );
		if ( out.count()==0 )
			stop();
	}
	recordingState();
	if ( t )
		emit timerEnded( t );
}



void DvbStream::recordingState()
{
	int i;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( out.at(i)->hasRec() ) {
			emit isRecording( true );
			return;
		}
	}
	emit isRecording( false );
}



void DvbStream::updateTimer( RecTimer *t, int ms )
{
	int i;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( out.at(i)->recTimer==t ) {
			if ( ms )
				out.at(i)->changeTimer( ms );
			else
				out.at(i)->stopRec();
			return;
		}
	}
	if ( !ms )
		emit timerEnded( t ); // not running
}



void DvbStream::stopBroadcast()
{
	int i;
	DVBout *o=0;
	QPtrList<DVBout> p;

	for ( i=0; i<(int)out.count(); i++ ) {
		o = out.at(i);
		o->stopBroadcast();
		if ( !o->hasLive() && !o->hasRec() )
			p.append( o );
	}
	for ( i=0; i<(int)p.count(); i++ ) {
		removePids( p.at(i) );
		if ( cam )
			cam->stopService( &(p.at(i)->channel) );
		removeOut( p.at(i) );
	}
	if ( out.count()==0 )
		stop();
	emit isBroadcasting( false );
}



int DvbStream::canStartBroadcast( bool &live, ChannelDesc *chan )
{
	int i, ret=0;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( (chan->tp!=out.at(i)->channel.tp) && out.at(i)->hasRec() )
			return ErrIsRecording;
		if ( chan->fta && cam && !cam->canPlay( chan ) )
			return ErrCamUsed;
		if ( out.at(i)->hasLive() && chan->tp!=out.at(i)->channel.tp )
			live = true;
	}
	return ret;
}



bool DvbStream::startBroadcast( QPtrList<ChannelDesc> *list, Ts2Rtp *rtp )
{
	int i, j;
	bool stop=false, newout;
	DVBout *o=0;
	int no=0;
	ChannelDesc *chan=list->at(0);
	QPtrList<ChannelDesc> broadcastList;
	broadcastList.setAutoDelete( true );

	if ( !isTuned() ) {
		for ( i=0; i<MAXTUNETRY; i++ ) {
			if ( tuneDvb( chan ) ) {
				i = -1;
				break;
			}
			else
				usleep(100000);
		}
		if ( i<0 )
			stop = true;
		else
			return false;
	}

	for ( i=0; i<(int)list->count(); i++ ) {
		newout = false;
		o = 0;
		for ( j=0; j<(int)out.count(); j++ ) {
			if ( out.at(j)->channel.name==list->at(i)->name )
				o = out.at(j);
		}

		if ( !o ) {
			o = new DVBout( *list->at(i), dvbDevice->adapter, dvbDevice->tuner, plug );
			connect( o, SIGNAL(endRecord(DVBout*,RecTimer*,bool)), this, SLOT(recordEnded(DVBout*,RecTimer*,bool)) );
			connect( o, SIGNAL(playDvb()), this, SLOT(receivePlayDvb()) );
			connect( o, SIGNAL(shifting(bool)), this, SLOT(receiveShifting(bool)) );
			out.append( o );
			if ( !setPids( o ) ) {
				removePids( o );
				removeOut( o );
				o = 0;
			}
			else
				newout = true;
		}

		if ( o ) {
			if ( !o->goBroadcast( rtp ) ) {
				if ( newout ) {
					removePids( o );
					removeOut( o );
				}
			}
			else {
				broadcastList.append( new ChannelDesc( *list->at(i) ) );
				no++;
				if ( list->at(i)->fta && cam )
					cam->startService( list->at(i) );
			}
		}
	}

	if ( !no && stop )
		stopFrontend();
	else
		rtp->addChannels( &broadcastList );

	fprintf(stderr,"NOUT: %d\n", out.count() );

	if ( !no )
		return false;
	else
		startReading();
	emit isBroadcasting( true );
	return true;
}



int DvbStream::canStartTimer( bool &live, ChannelDesc *chan )
{
	int i, ret=0;
	DVBout *o;

	for ( i=0; i<(int)out.count(); i++ ) {
		o = out.at(i);
		if ( (chan->tp!=o->channel.tp) && o->hasRec() )
			return ErrIsRecording;
		if ( (o->channel.name==chan->name) && o->hasRec() )
			return ErrIsRecording;
		if ( chan->fta && cam && !cam->canPlay( chan ) )
			return ErrCamUsed;
		if ( o->hasLive() ) {
			if ( chan->tp!=o->channel.tp )
				live = true;
		}
	}
	return ret;
}



bool DvbStream::startTimer( ChannelDesc *chan, QString path, int maxsize, RecTimer *t, QString name )
{
	int i;
	bool stop=false, newout=false;
	DVBout *o=0;
	QPtrList<DVBout> p;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( out.at(i)->channel.name==chan->name )
			o = out.at(i);
		else {
			if ( out.at(i)->hasBroadcast() && out.at(i)->channel.tp!=chan->tp ) {
				p.append( out.at(i) );
			}
		}
	}

	for ( i=0; i<(int)p.count(); i++ ) {
		p.at(i)->stopBroadcast();
		removePids( p.at(i) );
		removeOut( p.at(i) );
	}

	if ( p.count() ) {
		emit isBroadcasting( false );
		this->stop();
	}

	if ( !isTuned() ) {
		for ( i=0; i<MAXTUNETRY; i++ ) {
			if ( tuneDvb( chan ) ) {
				i = -1;
				break;
			}
			else
				usleep(100000);
		}
		if ( i<0 )
			stop = true;
		else
			return false;
	}

	if ( !o ) {
		o = new DVBout( *chan, dvbDevice->adapter, dvbDevice->tuner, plug );
		connect( o, SIGNAL(endRecord(DVBout*,RecTimer*,bool)), this, SLOT(recordEnded(DVBout*,RecTimer*,bool)) );
		connect( o, SIGNAL(playDvb()), this, SLOT(receivePlayDvb()) );
		connect( o, SIGNAL(shifting(bool)), this, SLOT(receiveShifting(bool)) );
		out.append( o );
		if ( !setPids( o ) ) {
			if ( stop )
				stopFrontend();
			removePids( o );
			removeOut( o );
			return false;
		}
		newout = true;
	}

	if ( t->mode && !name.contains("%date") )
		name+= "-%date";
	name = name.replace( "%chan", chan->name ).replace("%date", t->begin.toString( "yyyyMMdd-hhmmss" ) ).replace("%name", t->name );
	name = name.replace( "/", "_" ).replace( ">", "_" ).replace("<","_").replace(":","_").replace('"',"_").replace("\\","_").replace("|","_");
	name = path+name;

	if ( !o->goRec( name, maxsize, t ) ) {
		if ( stop )
			stopFrontend();
		if ( newout ) {
			removePids( o );
			removeOut( o );
		}
		return false;
	}

	fprintf(stderr,"NOUT: %d\n", out.count() );
	if ( chan->fta && cam )
		cam->startService( chan );

	startReading();

	recordingState();
	return true;
}



int DvbStream::goLive( ChannelDesc *chan, const QString &pipeName, int ringBufSize )
{
	int i;
	bool stop=false;
	DVBout *o=0;

	for ( i=0; i<(int)out.count(); i++ ) {
		if ( (chan->tp!=out.at(i)->channel.tp) && out.at(i)->hasRec() )
			return ErrIsRecording;
		if ( (chan->tp!=out.at(i)->channel.tp) && out.at(i)->hasBroadcast() )
			return ErrIsBroadcasting;
		if ( chan->fta && cam && !cam->canPlay( chan ) )
			return ErrCamUsed;
		if ( out.at(i)->channel.name==chan->name )
			o = out.at(i);
	}

	if ( !isTuned() ) {
		for ( i=0; i<MAXTUNETRY; i++ ) {
			if ( tuneDvb( chan ) ) {
				i = -1;
				break;
			}
			else
				usleep(100000);
		}
		if ( i<0 )
			stop = true;
		else
			return ErrCantTune;
	}

	if ( !o ) {
		o = new DVBout( *chan, dvbDevice->adapter, dvbDevice->tuner, plug );
		connect( o, SIGNAL(endRecord(DVBout*,RecTimer*,bool)), this, SLOT(recordEnded(DVBout*,RecTimer*,bool)) );
		connect( o, SIGNAL(playDvb()), this, SLOT(receivePlayDvb()) );
		connect( o, SIGNAL(shifting(bool)), this, SLOT(receiveShifting(bool)) );
		out.append( o );
		if ( !setPids( o ) ) {
			if ( stop )
				stopFrontend();
			removePids( o );
			removeOut( o );
			return ErrCantSetPids;
		}
	}

	if ( o->hasRec() )
		i = ErrDontSwitchAudio;
	else
		i = 0;

	o->goLive( pipeName, ringBufSize );
	fprintf(stderr,"NOUT: %d\n", out.count() );

	if ( chan->fta && cam  )
		cam->startService( chan );
	startReading();

	return i;
}



void DvbStream::preStopLive()
{
	int i;
	DVBout *o;

	for ( i=0; i<(int)out.count(); i++ ) {
		o = out.at(i);
		if ( o->hasLive() ) {
			o->preStopLive();
			break;
		}
	}
}



void DvbStream::stopLive( ChannelDesc *chan )
{
	int i;
	DVBout *o;
	QPtrList<DVBout> p;
	bool camUsed = false;

	fprintf(stderr,"Asked to stop\n");
	for ( i=0; i<(int)out.count(); i++ ) {
		o = out.at(i);
		if ( o->hasLive() ) {
			o->stopLive();
			if ( !o->hasRec() && !o->hasBroadcast() ) {
				p.append( o );
			}
			break;
		}
	}
	for ( i=0; i<(int)p.count(); i++ ) {
		removePids( p.at(i) );
		if ( cam )
			cam->stopService( &(p.at(i)->channel) );
		removeOut( p.at(i) );
	}
	fprintf(stderr,"Live stopped\n");

	if ( out.count()==0 && chan->tp!=currentTransponder )
		stop();
}



void DvbStream::startReading()
{
	if ( !isRunning ) {
		isRunning = true;
		start();
		dvbEvents->go( currentTransponder.source, currentTransponder.freq, true );
	}
}



void DvbStream::stop()
{
	isRunning = false;
	if ( !wait(10000) ) {
		terminate();
		wait();
		fprintf(stderr,"dvbstream::run() terminated\n");
	}
	dvbEvents->stop();
	stopFrontend();
}



void DvbStream::stopFrontend()
{
	if ( fdDvr ) {
		close( fdDvr );
		fprintf(stderr,"fdDvr closed\n");
		fdDvr = 0;
	}
	closeFe();
}



void DvbStream::setScanning( bool b )
{
	connectStatus( b );
}



void DvbStream::stopScan()
{
	closeFe();
}



DvbStream::~DvbStream()
{
	stop();
	if ( cam ) {
		delete cam;
	}
	delete dvbEvents;
}

#include "dvbstream.moc"
