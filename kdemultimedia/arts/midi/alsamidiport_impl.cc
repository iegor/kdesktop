    /*

    Copyright (C) 2001-2002 Stefan Westerfeld
                            stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "alsamidiport_impl.h"

#if defined(HAVE_ARTS_LIBASOUND2) || defined(HAVE_ARTS_LIBASOUND)
#include <arts/debug.h>

#ifdef HAVE_ARTS_LIBASOUND
#define snd_seq_queue_status_alloca(x) \
	*x = (snd_seq_queue_status_t *)alloca(sizeof(snd_seq_queue_status_t))
#define snd_seq_queue_status_get_tick_time(x) x->tick
#define snd_seq_queue_status_get_real_time(x) (&(x->time))
#endif

using namespace std;
using namespace Arts;

AlsaMidiPort_impl::AlsaMidiPort_impl(snd_seq_t *seq, long client, long port)
	: _client(client), _port(port), alsaSeq(seq)
{
	opened = false;
}

/* interface MidiPort */
Arts::TimeStamp AlsaMidiPort_impl::time()
{
	snd_seq_queue_status_t *status;
	snd_seq_queue_status_alloca(&status);

	snd_seq_get_queue_status(alsaSeq, alsaQueue, status);
	snd_seq_tick_time_t ttime = snd_seq_queue_status_get_tick_time(status);
	const snd_seq_real_time_t *rtime =
		snd_seq_queue_status_get_real_time(status);

	return Arts::TimeStamp(rtime->tv_sec, rtime->tv_nsec / 1000);
}

Arts::TimeStamp AlsaMidiPort_impl::playTime()
{
	return time();
}

void AlsaMidiPort_impl::fillAlsaEvent(snd_seq_event_t *ev,
		                              const MidiCommand& command)
{
	ev->source = alsaSourceAddr;
	ev->dest = alsaDestAddr;

	mcopbyte channel = command.status & mcsChannelMask;

	switch(command.status & mcsCommandMask)
	{
		case mcsNoteOn:
			snd_seq_ev_set_noteon(ev, channel, command.data1, command.data2);
			break;
		case mcsNoteOff:
			snd_seq_ev_set_noteoff(ev, channel, command.data1, command.data2);
			break;
		case mcsProgram:
			snd_seq_ev_set_pgmchange(ev, channel, command.data1);
			break;
		case mcsParameter:
			snd_seq_ev_set_controller(ev, channel, command.data1, command.data2);
			break;
		default:
			/* unhandled */
			return;
	}
}

void AlsaMidiPort_impl::sendAlsaEvent(snd_seq_event_t *ev)
{
	int ret = snd_seq_event_output(alsaSeq, ev);
	if (ret < 0) {
		arts_warning("AlsaMidiPort: error writing note %s\n",
				     snd_strerror(ret));
		return;
	}
	flushAlsa();
}

void AlsaMidiPort_impl::processCommand(const MidiCommand& command)
{
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);

	fillAlsaEvent(&ev, command);
	sendAlsaEvent(&ev);
}

void AlsaMidiPort_impl::processEvent(const MidiEvent& event)
{
	snd_seq_event_t ev;
	snd_seq_real_time_t time;

	time.tv_sec = event.time.sec;
	time.tv_nsec = event.time.usec * 1000;

	snd_seq_ev_clear(&ev);
	snd_seq_ev_schedule_real(&ev, alsaQueue, 0, &time);

	fillAlsaEvent(&ev, event.command);
	sendAlsaEvent(&ev);
}

/* interface AlsaMidiPort */
void AlsaMidiPort_impl::client(long newClient)
{
	if(newClient != _client)
	{
		_client = newClient;

		if(opened)
		{
			close();
			open();
		}

		client_changed(newClient);
	}
}

long AlsaMidiPort_impl::client()
{
	return _client;
}

void AlsaMidiPort_impl::port(long newPort)
{
	if(newPort != _port)
	{
		_port = newPort;

		if(opened)
		{
			close();
			open();
		}

		port_changed(newPort);
	}
}

long AlsaMidiPort_impl::port()
{
	return _port;
}

bool AlsaMidiPort_impl::open()
{
	arts_return_val_if_fail(opened == false, false);

	alsaQueue = snd_seq_alloc_queue(alsaSeq);
	alsaClientId = snd_seq_client_id(alsaSeq);

	alsaPort = snd_seq_create_simple_port(alsaSeq, "aRts",
									      SND_SEQ_PORT_CAP_WRITE |
									      SND_SEQ_PORT_CAP_SUBS_WRITE |
									      SND_SEQ_PORT_CAP_READ,
									      SND_SEQ_PORT_TYPE_MIDI_GENERIC);
	if (alsaPort < 0) {
		arts_warning("AlsaMidiPort: can't creating port %s\n",
				     snd_strerror(alsaPort));
		return false;
	}

	alsaSourceAddr.client = alsaClientId;
	alsaSourceAddr.port = alsaPort;

	alsaDestAddr.client = _client;
	alsaDestAddr.port = _port;

	int ret;
	ret = snd_seq_connect_to(alsaSeq, alsaPort,
			                 alsaDestAddr.client,
							 alsaDestAddr.port);
	if (ret < 0) {
		arts_warning("AlsaMidiPort: error connecting port %s\n",
				     snd_strerror(ret));
		/* FIXME: destroy port here */
		return false;
	}

	snd_seq_start_queue(alsaSeq, alsaQueue, 0);
	flushAlsa();

	opened = true;
	return true;
}

void AlsaMidiPort_impl::close()
{
	if(!opened)
		return;

	opened = false;
}

void AlsaMidiPort_impl::flushAlsa()
{
#ifdef HAVE_ARTS_LIBASOUND2
	snd_seq_drain_output(alsaSeq);
#else
	int err;
	while((err = snd_seq_flush_output(alsaSeq)) > 0)
	{
		arts_debug("alsa flush error %d\n",snd_strerror(err));
		usleep(2000);
	}
#endif
}
#endif
