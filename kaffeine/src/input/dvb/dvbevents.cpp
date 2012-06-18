/*
 * dvbevents.cpp
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
#include <ktrader.h>
#include <kparts/componentfactory.h>

#include "dvbevents.h"



DVBevents::DVBevents( QString devType, int anum, int tnum, const QString &charset, EventTable *table )
	: KaffeineDVBsection( anum, tnum, charset )
{
	events = table;
	KaffeineEpgPlugin *plug;
	QString plugName;
	int error;

	KTrader::OfferList offers = KTrader::self()->query("KaffeineEpgPlugin");
	KTrader::OfferList::Iterator end( offers.end() );
	for ( KTrader::OfferList::Iterator it=offers.begin(); it!=end; ++it ) {
		error = 0;
		KService::Ptr ptr = (*it);
		if ( !ptr->name().contains(devType) )
			continue;
		plug = KParts::ComponentFactory::createPartInstanceFromService<KaffeineEpgPlugin>(ptr, 0, ptr->name().ascii(), 0, 0, 0, &error );
		plugName = ptr->desktopEntryName();
		if (error > 0) {
			fprintf( stderr, "Loading of EPG plugin %s failed: %s\n", plugName.ascii(), KLibLoader::self()->lastErrorMessage().ascii() );
			plug = NULL;
		}
		else {
			plugs.append( plug );
			plug->setTable( table );
			plug->initSection( anum, tnum, charset );
			plugNames.append( plugName );
		}
	}
	fprintf( stderr, "%d EPG plugins loaded for device %d:%d.\n", plugs.count(), anum, tnum );
}



DVBevents::~DVBevents()
{
	int i;

	isRunning = false;
	if ( !wait(2000) ) {
		terminate();
		wait();
	}
	for ( i=0; i<(int)plugs.count(); ++i ) {
		plugs.at(i)->stop();
		KService::Ptr service = KService::serviceByDesktopName( plugNames[i] );
		KLibLoader::self()->unloadLibrary( service->library().ascii() );
	}
}



bool DVBevents::shortEventDesc( unsigned char *buf, EventDesc *desc )
{
	QString name, text;
	int len, len2;
	ShortEvent *ev;

	if ( !safeLen( buf+6 ) )
		return false;
	len = getBits(buf,40,8);
	if ( !safeLen( buf+6+len ) )
		return false;
	name = getText( buf+6, len );
	if ( !safeLen( buf+6+len+1 ) )
		return false;
	len2 = getBits(buf+6+len,0,8);
	if ( !safeLen( buf+7+len+len2 ) )
		return false;
	text = getText( buf+7+len, len2);
	if ( desc->title.isEmpty() ) {
		desc->title=name;
		desc->subtitle=text;
		return true;
	}
	desc->shortEvents.append( new ShortEvent() );
	ev = desc->shortEvents.getLast();
	ev->name = name;
	ev->text = text;
	return true;
}



bool DVBevents::extEventDesc( unsigned char *buf, EventDesc *desc )
{
	int loop, len1, len2;
	unsigned char *b = buf;
	QString s;

	if ( !safeLen( b+7 ) )
		return false;
	loop = getBits(b+6,0,8);
	b +=7;

	while ( loop>0 ) {
		if ( !safeLen( b+1 ) )
			return false;
		len1 = getBits(b,0,8);
		if ( !safeLen( b+1+len1 ) )
			return false;
		s = getText(b+1,len1);
		if ( !safeLen( b+1+len1+1 ) )
			return false;
		len2 = getBits(b+1+len1,0,8);
		if ( !safeLen( buf+2+len1+len2 ) )
			return false;
		if ( !s.isEmpty() )
			s = s+" : ";
		s = s+getText(b+2+len1,len2);
		desc->extEvents.append( new QString( s ) );
		b +=(2+len1+len2);
		loop -=(2+len1+len2);
	}
	if ( !safeLen( b+1 ) )
		return false;
	len1 = getBits(b,0,8);
	if ( !safeLen( b+1+len1 ) )
		return false;
	s = getText(b+1,len1);
	desc->extEvents.append( new QString( s ) );
	return true;
}



bool DVBevents::tableEIT( unsigned char* buffer )
{
	unsigned char* buf = buffer;
	unsigned int length, loop, sid, tid, eid, tsid, sn, lsn, nid;
	int i, sec;
	EventDesc *desc=0, *itdesc=0;
	EventSid *slist;
	QPtrList<EventDesc> *currentEvents;
	bool nodesc, parse;
	QDateTime start, cur, dt;
	unsigned int cdt = QDateTime::currentDateTime().toTime_t();

	tid = getBits(buf,0,8);
	length = getBits(buf,12,12);
	sid = getBits(buf,24,16);
	sn = getBits(buf,48,8);
	lsn = getBits(buf,56,8);
	tsid = getBits(buf,64,16);
	nid = getBits(buf,80,16);
	length -=11;
	buf +=14;

	slist = currentSrc->getEventSid( nid, tsid, sid );
	if ( !slist )
		return false;
	slist->lock();
	currentEvents = slist->getEvents();
	QPtrListIterator<EventDesc> it( *currentEvents );

	while ( length>4 ) {
		nodesc=parse=false;
		if ( !safeLen( buf+2 ) )
			goto stop;
		eid = getBits(buf,0,16);
		if ( !safeLen( buf+2+5 ) )
			goto stop;
		start = getDateTime( buf+2 );
		nodesc=parse=true;

		it.toFirst();
		while ( (desc=it.current())!=0 ) {
			if ( desc->sid==sid ) {
				if ( desc->startDateTime==start || desc->eid==eid ) {
					if ( desc->tid==0x4e && tid!=0x4e ) {
						parse = false;
						nodesc = false;
						break;
					}
					else {
						nodesc = false;
						if ( (cdt-desc->loop)<300 ) { // only reparse events every 300 seconds
							parse = false;
						}
						else {
							desc->extEvents.clear();
							desc->shortEvents.clear();
							desc->title=desc->subtitle="";
						}
						break;
					}
				}
			}
			++it;
		}

		if ( nodesc )
			desc = new EventDesc();
		if ( parse ) {
			if ( !safeLen( buf+10 ) )
				goto stop;
			desc->duration = getTime( buf+7 );
			if ( !safeLen( buf+11 ) )
				goto stop;
			desc->running = getBits(buf,80,3);
			desc->sid = sid;
			desc->tid = tid;
			desc->tsid = tsid;
			desc->nid = nid;
			desc->lsn = lsn;
			desc->sn = sn;
			desc->eid = eid;
			desc->loop = cdt;
		}

		if ( desc->sn != sn ) {
			slist->unlock();
			return false;
		}
		if ( !safeLen( buf+12 ) )
			goto stop;
		loop = getBits(buf,84,12);
		buf +=12;
		length -=(12+loop);
		while ( loop>0 ) {
			if ( parse ) {
				if ( !safeLen( buf+1 ) )
					goto stop;
				switch ( getBits(buf,0,8) ) {
					case 0x4D :
						if ( !shortEventDesc( buf, desc ) )
							goto stop;
						break;
					case 0x4E :
						if ( !extEventDesc( buf, desc ) )
							goto stop;
						break;
					default :
						break;
				}
			}
			if ( !safeLen( buf+2 ) )
				goto stop;
			loop -=( getBits(buf,8,8)+2 );
			buf +=( getBits(buf,8,8)+2 );
		}
//out:
		if ( parse ) {
			if ( !nodesc ) {
				if ( start==desc->startDateTime )
					goto ifend;
				currentEvents->take( currentEvents->find( desc ) );
			}
			desc->startDateTime = start;
			for ( i=0; i<(int)currentEvents->count(); i++ ) {
				itdesc = currentEvents->at(i);
				if ( desc->startDateTime<itdesc->startDateTime ) {
					currentEvents->insert( i, desc );
					break;
				}
				itdesc = 0;
			}
			if ( !itdesc )
				currentEvents->append( desc );
		}
ifend:
		if ( parse )
			++(desc->sn);
		if ( nodesc ) {
			cur = QDateTime::currentDateTime();
			dt = desc->startDateTime;
			sec = desc->duration.hour()*3600+desc->duration.minute()*60+desc->duration.second();
			if ( dt.addSecs( sec )<cur || desc->title.length()<3 ) {
				currentEvents->remove( desc );
			}
			else
				desc->source = currentSrc->getSource();
		}

	}
	slist->unlock();
	return true;
stop:
	slist->unlock();
	fprintf( stderr, "Stop parsing EIT (%d:%d)\n", adapter, tuner );
	if ( nodesc )
		delete desc;
	return false;
}




bool DVBevents::safeLen( unsigned char* buf )
{
	if ( buf<(secbuf+readSize) )
		return true;
	fprintf( stderr, "EIT (%d:%d) : buffer overflow! Rejected\n", adapter, tuner );
	return false;
}



bool DVBevents::go( QString src, int freqKHz, bool all )
{
	int tid, i;

	if ( isRunning )
		return true;

	if ( all )
		tid = 0;
	else
		tid = 0x4e;

	currentSrc = events->getEventSource( src );

	if ( !setFilter( 0x12, tid, 1000 ) )
		return false;
	isRunning = true;
	start();
	fprintf(stderr,"dvbEvents %d:%d started\n", adapter, tuner);

	for ( i=0; i<(int)plugs.count(); ++i ) {
		plugs.at(i)->go( src, freqKHz );
	}

	return true;
}



void DVBevents::stop()
{
	int i;

	if ( !isRunning )
		return;

	isRunning = false;
	if ( !wait(2000) ) {
		terminate();
		wait();
		fprintf(stderr,"dvbEvents %d:%d terminated\n", adapter, tuner);
	}
	else
		fprintf(stderr,"dvbEvents %d:%d ended\n", adapter, tuner);
	stopFilter();

	for ( i=0; i<(int)plugs.count(); ++i ) {
		plugs.at(i)->stop();
	}
}



void DVBevents::run()
{
	int n=0, tid;
	int skip=0;

	setpriority(PRIO_PROCESS, 0, 19); // eit parsing is cpu eater on some astra multiplex.
	while ( isRunning ) {
		if ( !isRunning )
			break;

		if ( poll(pf,1,1000)>0 ){
			if ( pf[0].revents & POLLIN ){
				n = read( fdDemux, secbuf, 4096 );
				skip = 0;
			}
			else
				skip++;
		}
		else
			skip++;

		if (skip)
			continue;
		if ( n<16 )
			continue;
		else
			readSize = n;

		if ( !isRunning )
			break;

		tid = getBits(secbuf,0,8);
		if ( tid>0x4D && tid<0x70 ) {
			tableEIT( secbuf );
		}
	}
}
