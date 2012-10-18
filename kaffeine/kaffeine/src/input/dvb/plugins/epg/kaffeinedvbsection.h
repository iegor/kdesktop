/*
 * kaffeinedvbsection.h
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

#ifndef KAFFEINEDVBSECTION_H
#define KAFFEINEDVBSECTION_H

#include <sys/poll.h>
#include <linux/dvb/dmx.h>

#include <qthread.h>
#include <qdatetime.h>



class KaffeineDVBsection : public QThread
{

public:

	KaffeineDVBsection();
	KaffeineDVBsection( int anum, int tnum, const QString &charset="ISO8859-1" );
	~KaffeineDVBsection();
	void initSection( int anum, int tnum, const QString &charset="ISO8859-1" );
	static unsigned int getBits( unsigned char *b, int offbits, int nbits );

protected:

	virtual void run() {}
	QString langDesc( unsigned char* buf );
	bool setFilter( int pid, int tid, int timeout=5000, bool checkcrc=true );
	void stopFilter();
	QString getText( unsigned char *buf, int length );
	bool doIconv( QCString &s, QCString table, char *buffer, int buflen );
	QTime getTime( unsigned char *buf );
	QDate getDate( unsigned char *buf );
	QDateTime getDateTime( unsigned char *buf );

	int fdDemux;
	int indexChannels;
	bool isRunning;
	int adapter;
	int tuner;
	QCString defaultCharset;
	struct pollfd pf[1];
};

#endif /* KAFFEINEDVBSECTION_H */
