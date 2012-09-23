/*
 * kaffeinedvbevents.cpp
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
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <qdatetime.h>
#include <qfile.h>

#include <klocale.h>
#include <kstandarddirs.h>

#include "kaffeinedvbevents.h"



ShortEvent::ShortEvent()
{
	name=text="";
}

ShortEvent::~ShortEvent()
{
}

EventDesc::EventDesc()
{
	loop = 0;
	nid=sid=tsid=eid=0;
	tid=sn=lsn=running=0;
	title=subtitle="";
	source="";
	shortEvents.setAutoDelete( true );
	extEvents.setAutoDelete( true );
}

EventDesc::~EventDesc()
{
	shortEvents.clear();
	extEvents.clear();
}



EventSid::EventSid( int s )
{
	sid = s;
	events.setAutoDelete( true );
}

EventSid::~EventSid()
{
	QMutexLocker locker( &mutex );
	events.clear();
}

void EventSid::remove( EventDesc *d )
{
	QMutexLocker locker( &mutex );
	events.remove( d );
}

EventDesc *EventSid::getEventDesc( int n )
{
	QMutexLocker locker( &mutex );
	return events.at(n);
}



EventTsid::EventTsid( int n, int t )
{
	tsid = t;
	nid = n;
	sidList.setAutoDelete( true );
}

EventTsid::~EventTsid()
{
	QMutexLocker locker( &mutex );
	sidList.clear();
}

EventSid *EventTsid::getNEventSid( int n )
{
	QMutexLocker locker( &mutex );
	return sidList.at( n );
}

EventSid *EventTsid::getEventSid( int sid )
{
	int i;

	QMutexLocker locker( &mutex );
	for ( i=0; i<(int)sidList.count(); i++ ) {
		if ( sidList.at(i)->getSid()==sid )
			return sidList.at(i);
	}
	EventSid *es = new EventSid( sid );
	sidList.append( es );
	return es;
}

EventDesc *EventTsid::getEventDesc( int sid, int n )
{
	int i;
	EventSid *es=0;

	mutex.lock();
	for ( i=0; i<(int)sidList.count(); i++ ) {
		if ( sidList.at(i)->getSid()==sid ) {
			es = sidList.at(i);
			break;
		}
	}
	mutex.unlock();
	if ( !es )
		return 0;
	return es->getEventDesc( n );
}



EventSource::EventSource( QString src )
{
	source = src;
	tsidList.setAutoDelete( true );
}

EventSource::~EventSource()
{
	QMutexLocker locker( &mutex );
	tsidList.clear();
}

EventTsid *EventSource::getNEventTsid( int n )
{
	QMutexLocker locker( &mutex );
	return tsidList.at( n );
}

EventSid *EventSource::getEventSid( int nid, int tsid, int sid )
{
	int i;
	EventTsid *et=0;

	mutex.lock();
	for ( i=0; i<(int)tsidList.count(); i++ ) {
		if ( tsidList.at(i)->getTsid()==tsid && (nid==0 || tsidList.at(i)->getNid()==nid) ) {
			et = tsidList.at(i);
			break;
		}
	}
	if ( !et ) {
		if ( nid==0) {
			mutex.unlock();
			return 0;
		}
		et = new EventTsid( nid, tsid );
		tsidList.append( et );
	}
	mutex.unlock();
	return et->getEventSid( sid );
}

EventDesc *EventSource::getEventDesc( int nid, int tsid, int sid, int n )
{
	int i;
	EventTsid *et=0;

	mutex.lock();
	for ( i=0; i<(int)tsidList.count(); i++ ) {
		if ( tsidList.at(i)->getTsid()==tsid && (nid==0 || tsidList.at(i)->getNid()==nid) ) {
			et = tsidList.at(i);
			break;
		}
	}
	mutex.unlock();
	if ( !et )
		return 0;
	return et->getEventDesc( sid, n );
}




EventTable::EventTable()
{
	srcList.setAutoDelete( true );
	connect ( &cleanTimer, SIGNAL(timeout()), this, SLOT(setClean()) );
	cleanTimer.start( 10000 );
	epgLoaded = false;
}

EventTable::~EventTable()
{
	QMutexLocker locker( &mutex );
	srcList.clear();
}

void EventTable::saveEpg()
{
	EventSource *esrc;
	EventTsid *et;
	EventSid *es;
	QPtrList<EventDesc> *esEvents;
	EventDesc *desc;
	int i, k, j, m, n;
	QCString c;
	int count=0;
	unsigned char sync=0xff;
	QTime t1=QTime::currentTime();

	QFile f( locateLocal("appdata", "dvbepg.data" ) );
	if ( f.open(IO_WriteOnly|IO_Truncate) ) {
		QDataStream dt( &f );
		for( i=0; i<getNSource(); i++ ) {
			if ( !(esrc=getNEventSource( i )) )
				continue;
			for ( m=0; m<esrc->getNTsid(); m++ ) {
				if ( !(et=esrc->getNEventTsid( m )) )
					continue;
				for ( n=0; n<et->getNSid(); n++ ) {
					if ( !(es=et->getNEventSid( n )) )
						continue;
					esEvents = es->getEvents();
					es->lock();
					for ( j=0; j<(int)esEvents->count(); j++ ) {
						if ( !(desc=esEvents->at( j )) )
							continue;
						dt << sync; // sync byte
						c = desc->source.utf8();
						dt << c.data();
						dt << desc->tid;
						dt << desc->sid;
						dt << desc->tsid;
						dt << desc->nid;
						dt << desc->sn;
						dt << desc->lsn;
						dt << desc->eid;
						dt << desc->running;
						dt << desc->startDateTime.toTime_t();
						dt << (uint)((desc->duration.hour()*3600)+(desc->duration.minute()*60)+desc->duration.second());
						dt << desc->shortEvents.count();
						for ( k=0; k<(int)desc->shortEvents.count(); k++ ) {
							c = desc->shortEvents.at(k)->name.utf8();
							dt << c.data();
							c = desc->shortEvents.at(k)->text.utf8();
							dt << c.data();
						}
						dt << (uint)desc->extEvents.count();
						for ( k=0; k<(int)desc->extEvents.count(); k++ ) {
							c = desc->extEvents.at(k)->utf8();
							dt << c.data();
						}
						c = desc->title.utf8();
						dt << c.data();
						c = desc->subtitle.utf8();
						dt << c.data();
						++count;
					}
					es->unlock();
				}
			}
		}
		f.close();
		fprintf( stderr, "Saved epg data : %d events (%d msecs)\n", count, t1.msecsTo(QTime::currentTime()) );
	}
}

void EventTable::loadEpg()
{
	EventDesc *desc;
	ShortEvent *sev;
	EventSid *slist;
	uint count, tmp, i;
#define EPGBUFSIZE 500
	char buf[EPGBUFSIZE];
	int num=0;
	unsigned char sync;
	QDateTime cur=QDateTime::currentDateTime();
	QTime t1=QTime::currentTime();

	if ( epgLoaded )
		return;

	epgLoaded = true;
	QFile f( locateLocal("appdata", "dvbepg.data" ) );
	if ( f.open(IO_ReadOnly) ) {
		QDataStream dt( &f );
		while (!dt.eof() ) {
			dt >> sync;
			if ( sync!=0xff ) {
				f.close();
				fprintf( stderr, "Sync error while loading epg data : %d events loaded\n", num );
				return;
			}
			desc = new EventDesc();
			dt >> tmp;
			if ( !validString( f, desc, tmp, EPGBUFSIZE, num ) ) return;
			dt.readRawBytes( buf, tmp );
			desc->source = QString::fromUtf8( buf );
			dt >> desc->tid;
			dt >> desc->sid;
			dt >> desc->tsid;
			dt >> desc->nid;
			dt >> desc->sn;
			dt >> desc->lsn;
			dt >> desc->eid;
			dt >> desc->running;
			dt >> tmp;
			desc->startDateTime.setTime_t( tmp );
			dt >> tmp;
			desc->duration = QTime().addSecs( tmp );
			dt >> count;
			for ( i=0; i<count; i++ ) {
				sev = new ShortEvent();
				dt >> tmp;
				if ( !validString( f, desc, tmp, EPGBUFSIZE, num ) ) return;
				dt.readRawBytes( buf, tmp );
				sev->name = QString::fromUtf8( buf );
				dt >> tmp;
				if ( !validString( f, desc, tmp, EPGBUFSIZE, num ) ) return;
				dt.readRawBytes( buf, tmp );
				sev->text = QString::fromUtf8( buf );
				desc->shortEvents.append( sev );
			}
			dt >> count;
			for ( i=0; i<count; i++ ) {
				dt >> tmp;
				if ( !validString( f, desc, tmp, EPGBUFSIZE, num ) ) return;
				dt.readRawBytes( buf, tmp );
				desc->extEvents.append( new QString( QString::fromUtf8( buf ) ) );
			}
			dt >> tmp;
			if ( !validString( f, desc, tmp, EPGBUFSIZE, num ) ) return;
			dt.readRawBytes( buf, tmp );
			desc->title = QString::fromUtf8( buf );
			dt >> tmp;
			if ( !validString( f, desc, tmp, EPGBUFSIZE, num ) ) return;
			dt.readRawBytes( buf, tmp );
			desc->subtitle = QString::fromUtf8( buf );
			if ( desc->startDateTime.addSecs( (desc->duration.hour()*3600)+(desc->duration.minute()*60)+desc->duration.second() )<cur )
				delete desc;
			else {
				slist = getEventSource( desc->source )->getEventSid( desc->nid, desc->tsid, desc->sid );
				if ( !slist )
					continue;
				slist->lock();
				slist->getEvents()->append( desc );
				slist->unlock();
				++num;
			}
		}
		f.close();
		fprintf( stderr, "Loaded epg data : %d events (%d msecs)\n", num, t1.msecsTo(QTime::currentTime()) );
	}
}

bool EventTable::validString( QFile &f, EventDesc *d, int len, int buflen, int nev )
{
	if ( len<1 || len>buflen ) {
		f.close();
		fprintf( stderr, "Error while loading epg data : %d events loaded\n", nev );
		delete d;
		return false;
	}
	return true;
}

void EventTable::setClean()
{
	start();
}

void EventTable::doClean( bool b )
{
	if ( b ) {
		if ( cleanTimer.isActive() )
			return;
		cleanTimer.start( 10000 );
	}
	else {
		cleanTimer.stop();
		wait();
	}
}

void EventTable::run()
{
	int k, m, n, sec;
	EventSource *esrc;
	EventTsid *et;
	EventSid *es;
	EventDesc *desc;
	QDateTime dt, cur;

	setpriority(PRIO_PROCESS, 0, 19);

	cur = QDateTime::currentDateTime();
	for( k=0; k<getNSource(); k++ ) {
		if ( !(esrc=getNEventSource( k )) )
			continue;
		for ( m=0; m<esrc->getNTsid(); m++ ) {
			if ( !(et=esrc->getNEventTsid( m )) )
				continue;
			for ( n=0; n<et->getNSid(); n++ ) {
				if ( !(es=et->getNEventSid( n )) )
					continue;
				if ( !(desc=es->getEventDesc( 0 )) )
					continue;
				dt = desc->startDateTime;
				sec = desc->duration.hour()*3600+desc->duration.minute()*60+desc->duration.second();
				if ( dt.addSecs( sec )<cur )
					es->remove( desc );
			}
		}
	}
}

EventSource *EventTable::getNEventSource( int n )
{
	QMutexLocker locker( &mutex );
	return srcList.at( n );
}

EventSource *EventTable::getEventSource( QString src )
{
	int i;

	QMutexLocker locker( &mutex );
	for ( i=0; i<(int)srcList.count(); i++ ) {
		if ( srcList.at(i)->getSource()==src )
			return srcList.at(i);
	}
	EventSource *es = new EventSource( src );
	srcList.append( es );
	return es;
}

EventDesc *EventTable::getEventDesc( QString src, int nid, int tsid, int sid, int n )
{
	int i;
	EventSource *es=0;

	mutex.lock();
	for ( i=0; i<(int)srcList.count(); i++ ) {
		if ( srcList.at(i)->getSource()==src ) {
			es = srcList.at(i);
			break;
		}
	}
	mutex.unlock();
	if ( !es )
		return 0;
	return es->getEventDesc( nid, tsid, sid, n );
}



KaffeineEpgPlugin::KaffeineEpgPlugin( QObject* parent, const char* name ) : KParts::Part( parent, name )
{
}

KaffeineEpgPlugin::~KaffeineEpgPlugin()
{
}

bool KaffeineEpgPlugin::safeLen( unsigned char* buf )
{
	if ( buf<(secbuf+readSize) )
		return true;
	fprintf( stderr, "EIT (%d:%d) : buffer overflow! Rejected\n", adapter, tuner );
	return false;
}

#include "kaffeinedvbevents.moc"
