/*
 * kaffeinedvbevents.h
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

#ifndef KAFFEINEDVBEVENTS_H
#define KAFFEINEDVBEVENTS_H

#include <qtimer.h>
#include <qmutex.h>
#include <qfile.h>
#include <qobject.h>

#include <kparts/part.h>

#include "kaffeinedvbsection.h"



class ShortEvent
{

public:

	ShortEvent();
	~ShortEvent();

	QString name;
	QString text;
};

class EventDesc
{

public:

	EventDesc();
	~EventDesc();

	QString source;
	unsigned char tid;
	unsigned short sid;
	unsigned short tsid;
	unsigned short nid;
	unsigned char sn;
	unsigned char lsn;
	unsigned short eid;
	unsigned char running;
	QDateTime startDateTime;
	QTime duration;
	QPtrList<ShortEvent> shortEvents;
	QPtrList<QString> extEvents;
	QString title;
	QString subtitle;
	unsigned int loop;
};



class EventSid
{
public:
	EventSid( int s );
	~EventSid();
	int getSid() { return sid; }
	void lock() { mutex.lock(); }
	void unlock() { mutex.unlock(); }
	QPtrList<EventDesc> *getEvents() { return &events; }
	EventDesc *getEventDesc( int n );
	int getNDesc() { return events.count(); }
	void remove( EventDesc *d );
private:
	QMutex mutex;
	int sid;
	QPtrList<EventDesc> events;
};



class EventTsid
{
public:
	EventTsid( int n, int t );
	~EventTsid();
	int getTsid() { return tsid; }
	int getNid() { return nid; }
	EventSid *getEventSid( int sid );
	EventSid *getNEventSid( int n );
	int getNSid() { return sidList.count(); }
	EventDesc *getEventDesc( int sid, int n );
private:
	QMutex mutex;
	int tsid, nid;
	QPtrList<EventSid> sidList;
};



class EventSource
{
public:
	EventSource( QString src );
	~EventSource();
	EventSid *getEventSid( int nid, int tsid, int sid );
	EventTsid *getNEventTsid( int n );
	int getNTsid() { return tsidList.count(); }
	EventDesc *getEventDesc( int nid, int tsid, int sid, int n );
	QString getSource() { return source; }
private:
	QMutex mutex;
	QString source;
	QPtrList<EventTsid> tsidList;
};



class EventTable : public QObject, public QThread
{
	Q_OBJECT
public:
	EventTable();
	~EventTable();
	EventSource *getEventSource( QString src );
	EventDesc *getEventDesc( QString src, int nid, int tsid, int sid, int n );
	EventSource *getNEventSource( int n );
	int getNSource() { return srcList.count(); }
	void doClean( bool b );
	void saveEpg();
	void loadEpg();
protected:
	virtual void run();
private:
	bool validString( QFile &f, EventDesc *d, int len, int buflen, int nev );
	QMutex mutex;
	QPtrList<EventSource> srcList;
	QTimer cleanTimer;
	bool epgLoaded;
private slots:
	void setClean();
};



class KDE_EXPORT KaffeineEpgPlugin : public KParts::Part,  public KaffeineDVBsection
{
	Q_OBJECT
public:
	KaffeineEpgPlugin( QObject* parent, const char* name );
	virtual ~KaffeineEpgPlugin();
	virtual bool go( QString /*src*/, int /*freqKhz*/ ) { return false; }
	virtual void stop() {}
	void setTable( EventTable *table ) { events = table; }

protected:
	bool safeLen( unsigned char* buf );

	unsigned char secbuf[4096];
	int readSize;
	EventTable *events;
};

#endif /* KAFFEINEDVBEVENTS_H */
