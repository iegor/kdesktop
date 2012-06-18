/*
 * channeldesc.cpp
 *
 * Copyright (C) 2003-2006 Christophe Thommeret <hftom@free.fr>
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

#include <math.h>

#include "channeldesc.h"

AudioPid::AudioPid()
{
	pid=ac3=0;
	lang="";
}

AudioPid::AudioPid( unsigned short apid )
{
	pid = apid;
	ac3=0;
	lang="";
}

AudioPid::~AudioPid()
{
}

ChannelDesc::ChannelDesc()
{
	fta=sid=ttpid=vpid=pmtpid=type=0;
	vType=0;
	num=0;
	name=provider=category="";
	completed = 0;
	tp.freq=tp.sr=0;
	tp.pol='v';
	maxapid=MAXAPID;
	for ( int i=0; i<maxapid; i++ ) {
		apid[i].pid=apid[i].ac3=0;
		apid[i].lang="";
	}
	napid=0;
	maxsubpid=MAXSUBPID;
	for ( int i=0; i<maxsubpid; i++ ) {
		subpid[i].pid=subpid[i].page=subpid[i].id=0;
		subpid[i].type=0;
		subpid[i].lang="???";
	}
	nsubpid=0;
}

ChannelDesc::ChannelDesc( const ChannelDesc &chan )
{
	num = chan.num;
	fta = chan.fta;
	sid = chan.sid;
	vpid = chan.vpid;
	vType = chan.vType;
	pmtpid = chan.pmtpid;
	type = chan.type;
	ttpid = chan.ttpid;
	name = chan.name;
	provider = chan.provider;
	category = chan.category;
	completed = chan.completed;
	tp = chan.tp;
	maxapid = chan.maxapid;
	for ( int i=0; i<maxapid; i++ ) {
		apid[i] = chan.apid[i];
		apid[i].ac3 = chan.apid[i].ac3;
		apid[i].lang = chan.apid[i].lang;
	}
	napid = chan.napid;
	maxsubpid = chan.maxsubpid;
	for ( int i=0; i<maxsubpid; i++ ) {
		subpid[i] = chan.subpid[i];
		subpid[i].type = chan.subpid[i].type;
		subpid[i].lang = chan.subpid[i].lang;
	}
	nsubpid = chan.nsubpid;
}

ChannelDesc::~ChannelDesc()
{
}

Transponder::Transponder()
{
	source = "";
	type = FE_QPSK;
	freq = 0;
	sr = 0;
	pol = 'v';
	nid=tsid = 0;
	inversion=INVERSION_AUTO;
	modulation=QAM_AUTO;
	hierarchy=HIERARCHY_AUTO;
	guard=GUARD_INTERVAL_AUTO;
	transmission=TRANSMISSION_MODE_AUTO;
	coderateL=FEC_AUTO;
	coderateH=FEC_AUTO;
	bandwidth=BANDWIDTH_AUTO;
	snr = 0;
	rolloff = ROLLOFF_AUTO;
	S2 = 0;
}

Transponder::Transponder( const Transponder &trans )
{
	source = trans.source;
	type = trans.type;
	freq = trans.freq;
	sr = trans.sr;
	pol = trans.pol;
	tsid = trans.tsid;
	nid = trans.nid;
	inversion=trans.inversion;
	modulation=trans.modulation;
	hierarchy=trans.hierarchy;
	guard=trans.guard;
	transmission=trans.transmission;
	coderateL=trans.coderateL;
	coderateH=trans.coderateH;
	bandwidth=trans.bandwidth;
	rolloff = trans.rolloff;
	S2 = trans.S2;
}

bool Transponder::sameAs( Transponder *trans )
{
	int f1 = this->freq*1000;
	int f2 = trans->freq*1000;

	if ( fabs(f1-f2) < 2000 ) return true;
	return false;
}

bool Transponder::operator==( const Transponder &t )
{
	if ( this->bandwidth==t.bandwidth
		&& this->source==t.source
		&& this->coderateH==t.coderateH
		&& this->coderateL==t.coderateL
		&& this->freq==t.freq
		&& this->guard==t.guard
		&& this->hierarchy==t.hierarchy
		&& this->inversion==t.inversion
		&& this->modulation==t.modulation
		&& this->pol==t.pol
		&& this->sr==t.sr
		&& this->transmission==t.transmission ) return true;
	return false;
}

bool Transponder::operator!=( const Transponder &t )
{
	if ( this->bandwidth!=t.bandwidth
		|| this->source!=t.source
		|| this->coderateH!=t.coderateH
		|| this->coderateL!=t.coderateL
		|| this->freq!=t.freq
		|| this->guard!=t.guard
		|| this->hierarchy!=t.hierarchy
		|| this->inversion!=t.inversion
		|| this->modulation!=t.modulation
		|| this->pol!=t.pol
		|| this->sr!=t.sr
		|| this->transmission!=t.transmission ) return true;
	return false;
}

Transponder::~Transponder()
{
}
