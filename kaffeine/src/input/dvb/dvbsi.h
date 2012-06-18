/*
 * dvbsi.h
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

#ifndef DVBSI_H
#define DVBSI_H

#include "dvbsection.h"
#include "dvbstream.h"



class NitSection : public KaffeineDVBsection
{

public:

	NitSection( QPtrList<Transponder> *tp, bool *end, bool *ok, int anum, int tnum );
	~NitSection();
	bool getSection( int pid, int tid, int timeout=5000 );
	bool tableNIT( unsigned char* buf );
	void satelliteDesc( unsigned char* buf, Transponder *trans );
	void S2satelliteDesc( unsigned char* buf, Transponder *trans );
	void cableDesc( unsigned char* buf, Transponder *trans );
	void terrestrialDesc( unsigned char* buf, Transponder *trans );
	void freqListDesc( unsigned char* buf, Transponder *trans );
	void stop();

	bool *ended;
	QPtrList<Transponder> *transponders;

protected:

	virtual void run();
};



class DVBsi : public QObject, public KaffeineDVBsection
{
	Q_OBJECT

public:

	DVBsi( bool *ok, int anum, int tnum, DvbStream *d, const QString &charset );
	~DVBsi();
	bool getSection( int pid, int tid, int timeout=5000, int sid=0 );
	bool listChannels();
	bool tableSDT( unsigned char* buf );
	bool tablePAT( unsigned char *buf );
	bool tablePMT( unsigned char* buf );
	void serviceDesc( unsigned char* buf, ChannelDesc *desc );

	// ATSC related methods
	virtual bool handle_atsc_transponder();
	virtual bool parseMGT( int pid, int tid, int timeout=5000, int sid=0 );
	virtual bool parseVCT( int pid, int tid, int timeout=5000, int sid=0 );

	QPtrList<ChannelDesc> channels;
	QPtrList<Transponder> transponders;
	DvbStream *dvb;
	int indexChannels;
	int progressTransponder;
	int adapter, tuner;

public slots:

	void go( QPtrList<Transponder> trans, int mode=1 );
	void stop();

protected:

	virtual void run();
	virtual void timerEvent( QTimerEvent *e );

private:

	void out( bool stopscan=true );

	int scanMode;
	NitSection *ns;

	/* ATSC related */
	int vct_table;

signals:

	void end( bool );
};

#endif /* DVBSI_H */
