/*
 * cddump.h
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

#ifndef CDDUMP_H
#define CDDUMP_H

#include <sys/poll.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <qthread.h>
#include <qobject.h>
#include <qstring.h>
#include <qfile.h>
#include <qtimer.h>

#include "cdchannel.h"



struct srtpbits {
  unsigned int v:2;           /* version: 2 */
  unsigned int p:1;           /* is there padding appended: 0 */
  unsigned int x:1;           /* number of extension headers: 0 */
  unsigned int cc:4;          /* number of CSRC identifiers: 0 */
  unsigned int m:1;           /* marker: 0 */
  unsigned int pt:7;          /* payload type: 33 for MPEG2 TS - RFC 1890 */
  unsigned int sequence:16;   /* sequence number: random */
};

struct srtpheader {	/* in network byte order */
  struct srtpbits b;
  int timestamp;	/* start: random */
  int ssrc;		/* random */
};



class CdDump : public QObject, public QThread
{
	Q_OBJECT

public :

	CdDump( const QString &pipe );
	~CdDump();
	virtual void run();
	bool go( const QString &ad, int port, CdChannel c );
	void stop();
	bool doPause( const QString &name );
	bool running() const;

private slots:

	void setPatPmt();

private :

	void writePmt();
	void writePat();
	void calculateCRC( unsigned char *p_begin, unsigned char *p_end );

	QFile liveFile;
	bool timeShifting;
	QString timeShiftFileName;
	int waitPause;
	int fdPipe;
	QString fifoName;
	bool isRunning;
	int sock;
	struct sockaddr_in addr;
	CdChannel chan;
	unsigned char tspat[188];
	unsigned char tspmt[188];
	unsigned int CRC32[256];
	int pmtpid;
	QTimer timerPatPmt;
	bool patpmt;

signals:

	void playDvb();
	void shifting( bool );

};

#endif /* CDDUMP_H */
