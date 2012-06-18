/*
 * dvbevents.h
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

#ifndef DVBEVENTS_H
#define DVBEVENTS_H

#include <qptrlist.h>

#include "kaffeinedvbsection.h"
#include "kaffeinedvbevents.h"



class DVBevents : public KaffeineDVBsection
{

public:

	DVBevents( QString devType, int anum, int tnum, const QString &charset, EventTable *table );
	~DVBevents();
	bool go( QString src, int freqKHz, bool all=false );
	void stop();

private:

	virtual void run();
	bool tableEIT( unsigned char* buffer );
	bool shortEventDesc( unsigned char *buf, EventDesc *desc );
	bool extEventDesc( unsigned char *buf, EventDesc *desc );
	bool safeLen( unsigned char* buf );

	unsigned char secbuf[4096];
	int readSize;

	EventTable *events;
	EventSource *currentSrc;

	QPtrList<KaffeineEpgPlugin> plugs;
	QStringList plugNames;
};

#endif /* DVBEVENTS_H */
