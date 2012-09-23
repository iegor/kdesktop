/*
 * dvbstream.h
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

#ifndef DVBSTREAM_H
#define DVBSTREAM_H

#include <sys/poll.h>
#include <sys/stat.h>

#include <qthread.h>
#include <qptrlist.h>
#include <qfile.h>
#include <qobject.h>
#include <qtimer.h>
#include <qstringlist.h>

#include "channeldesc.h"
#include "dvbout.h"
#include "ts2rtp.h"
#include "dvbconfig.h"

#define MAX_CHANNELS 8

class DVBevents;
class DvbCam;
class KaffeineDvbPlugin;
class EventTable;



class DvbStream : public QObject, public QThread
{
	Q_OBJECT

public :

	enum { ErrDontSwitchAudio=-1, ErrCantTune=1, ErrCantSetPids=2, ErrIsRecording=3, ErrNoLive=4,
		ErrCantOpenFile=5, ErrIsBroadcasting=6, ErrCamUsed=7 };

	DvbStream( Device *d, const QString &charset, EventTable *et );
	~DvbStream();
	int getAdapter() { return dvbDevice->adapter; }
	int getTuner() { return dvbDevice->tuner; }
	fe_type_t getType() {return dvbDevice->type; }
	void setPlug( KaffeineDvbPlugin *p );
	QStringList getSources( bool all=false );
	bool canSource( ChannelDesc *chan );
	int getPriority();
	bool tuneDvb( ChannelDesc *chan, bool dvr=true );
	void stopFrontend();
	virtual void run();
	int goLive( ChannelDesc *chan, const QString &pipeName, int ringBufSize );
	void preStopLive();
	void stopLive( ChannelDesc *chan );
	void stop();
	void stopScan();
	void setScanning( bool b );
	bool hasVideo();
	bool doPause( const QString &name );
	bool timeShiftMode();
	bool running() const;
	ChannelDesc getLiveChannel();
	Transponder getCurrentTransponder();
	unsigned long getCurrentFreq();
	bool isTuned();
	bool startTimer( ChannelDesc *chan, QString path, int maxsize, RecTimer *t, QString filename );
	int canStartTimer( bool &live, ChannelDesc *chan );
	int canStartBroadcast( bool &live, ChannelDesc *chan );
	bool startBroadcast( QPtrList<ChannelDesc> *list, Ts2Rtp *rtp );
	void stopBroadcast();
	bool hasRec();
	bool hasBroadcast();
	bool hasLive();
	bool liveIsRecording();
	int getSNR();
	void probeCam();
	void showCamDialog();

	struct pollfd pfd;
	DVBevents *dvbEvents;

public slots:

	bool checkStatus();
	void receivePlayDvb();
	void recordEnded( DVBout *o, RecTimer *t, bool kill );
	void receiveShifting( bool b );
	void updateTimer( RecTimer *t, int ms );

protected:

	void timerEvent( QTimerEvent* ev );

private :

	int setDiseqc( int switchPos, ChannelDesc *chan, int hiband, int &rotor, bool dvr );
	void moveRotor( int switchPos, ChannelDesc *chan, int hiband, bool dvr );
	void rotorCommand( int cmd, int n1=0, int n2=0, int n3=0 );
	void gotoX( double azimuth );
	double getAzimuth( double angle );
	double getSourceAngle( QString source );
	int getSatPos( const QString &src );
	bool setPids( DVBout *o );
	void removePids( DVBout *o );
	void removeOut( DVBout *o );
	void recordingState();
	void startReading();
	bool openFe();
	bool closeFe();
	void connectStatus( bool con );

	QFile liveFile;
	bool timeShifting;
	QPtrList<DVBout> out;
	Device *dvbDevice;
	int waitPause;
	DVBout *delOut;
	Transponder currentTransponder;

	QString frontendName;
	QString dvrName;
	QString demuxName;
	int fdFrontend;
	int ndmx;
	int fdDvr, fdPipe;
	bool isRunning;
	QTimer statusTimer;
	QString timeShiftFileName;
	DvbCam *cam;
	bool camProbed;
	int diseqcTwice;
	KaffeineDvbPlugin *plug;

signals:

	void playDvb();
	void isRecording(bool);
	void isBroadcasting(bool);
	void timerEnded(RecTimer*);

	void errorMsg( QString );
	void snrStatus( int );
	void signalStatus( int );
	void lockStatus( bool );
	void shifting( bool );

};

#endif /* DVBSTREAM_H */
