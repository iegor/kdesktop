/*
 * dvbout.h
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

#ifndef DVBOUT_H
#define DVBOUT_H

#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>
#include <qstring.h>
#include <qfile.h>
#include <qtimer.h>
#include <qvaluelist.h>

#include "channeldesc.h"
#include "ts2rtp.h"
#include "gdvb.h"

class KaffeineDvbPlugin;

class DVBout : public QObject, public QThread
{
	Q_OBJECT

public:

	DVBout( ChannelDesc chan, int anum, int tnum, KaffeineDvbPlugin *p );
	~DVBout();
	void process( unsigned char *buf, int size );
	bool goLive( const QString &name, int ringBufSize );
	void preStopLive();
	void stopLive();
	bool goBroadcast( Ts2Rtp *r );
	void stopBroadcast();
	bool goRec( const QString &name, int maxsize, RecTimer *t );
	bool hasRec() const;
	bool hasLive() const;
	bool hasBroadcast() const;
	bool hasInstantRec() const;
	bool timeShiftMode() const;
	bool doPause( const QString &name );
	void changeTimer( int ms );
	QValueList<int> pidsList() { return pids; }

	ChannelDesc channel;
	QValueList<int> dmx, pids;
	RecTimer *recTimer;

public slots:

	void stopRec();

private slots:

	void setPatPmt();

private:

	void writePmt();
	void writePat();
	void calculateCRC( unsigned char *p_begin, unsigned char *p_end );
	void renameFile( QString &name, const QString &ext );

	int pmtpid;
	bool patpmt, wpatpmt;
	bool timeShifting;
	QString pipeName;
	QFile outFile, liveFile;
	int fdPipe;
	Ts2Rtp *rtp;
	unsigned char thBuf[188*100];
	unsigned char *wBuf;
	int wRead, wWrite, wDist;
	unsigned char tspat[188];
	unsigned char tspmt[188];
	unsigned int CRC32[256];
	int thWrite;
	bool beginLive;
	bool haveRec, haveLive, instantRec, haveBroadcast;
	QTimer stopRecTimer, timerPatPmt;
	QMutex mutex;
	KaffeineDvbPlugin *plug;
	void *plugHandle;

	int fileNumber;
	QString fileName;
	long long int fileMaxSize;
	int wbufSize;

signals:

	void playDvb();
	void endRecord(DVBout*, RecTimer*, bool);
	void shifting(bool);

protected:

	virtual void run();

};

#endif /* DVBOUT_H */
