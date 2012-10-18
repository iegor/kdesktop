/*
 * ts2rtp.h
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

#ifndef TS2RTP_H
#define TS2RTP_H

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <qptrlist.h>
#include <qtimer.h>

#include "sender.h"

class ChannelDesc;



struct rtpbits {
  unsigned int v:2;           /* version: 2 */
  unsigned int p:1;           /* is there padding appended: 0 */
  unsigned int x:1;           /* number of extension headers: 0 */
  unsigned int cc:4;          /* number of CSRC identifiers: 0 */
  unsigned int m:1;           /* marker: 0 */
  unsigned int pt:7;          /* payload type: 33 for MPEG2 TS - RFC 1890 */
  unsigned int sequence:16;   /* sequence number: random */
};

struct rtpheader {	/* in network byte order */
  struct rtpbits b;
  int timestamp;	/* start: random */
  int ssrc;		/* random */
};



class Ts2Rtp : public Sender
{
	Q_OBJECT
public:

	Ts2Rtp();
	~Ts2Rtp();
	void setSocket( const QString &addr, int m_port, int m_senderPort );
	bool addChannels( QPtrList<ChannelDesc> *channels );
	void removeChannels();
	void process( unsigned char *buf, int size );

private:

	bool makeSocket();
	void sendrtp( char *data, int len );
	void psiTables( QPtrList<ChannelDesc> *channels );
	void fillPackets( unsigned short pid, unsigned char *buf, int len, int &npack );
	void calculateCRC( unsigned char *p_begin, unsigned char *p_end );

	int rtpSocket;
	struct sockaddr_in rtpAddr;
	struct rtpheader hRtp;
	QString address;
	int port, senderPort;
	unsigned char thBuf[188*10];
	unsigned int CRC32[256];
	int thWrite;
	unsigned char *psiBuffer;
	int psiBufferSize;
	int psiPackets;
	QTimer psiTimer;
	bool writePsi;

private slots:
	void setPSI();
};

#endif /* TS2RTP_H */
