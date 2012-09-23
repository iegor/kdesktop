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

#ifndef ARTS_ALSAMIDIPORT_IMPL_H
#define ARTS_ALSAMIDIPORT_IMPL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
 * compile real version if we have ALSA support, dummy version otherwise
 */
#if defined(HAVE_ARTS_LIBASOUND2) || defined(HAVE_ARTS_LIBASOUND)

#ifdef HAVE_ALSA_ASOUNDLIB_H
#include <alsa/asoundlib.h>
#elif defined(HAVE_SYS_ASOUNDLIB_H)
#include <sys/asoundlib.h>
#endif

#include "artsmidi.h"

namespace Arts {

class AlsaMidiPort_impl : virtual public AlsaMidiPort_skel {
protected:
	long _client, _port;
	bool opened;

	snd_seq_t *alsaSeq;

	int alsaQueue;
	int alsaClientId;
	int alsaPort;

	snd_seq_addr_t alsaSourceAddr;
	snd_seq_addr_t alsaDestAddr;

	void fillAlsaEvent(snd_seq_event_t *ev, const MidiCommand& command);
	void sendAlsaEvent(snd_seq_event_t *ev);
	void flushAlsa();

public:
	AlsaMidiPort_impl(snd_seq_t *seq, long client, long port);
	void close();

	/* interface MidiPort */
	Arts::TimeStamp time();
	Arts::TimeStamp playTime();
	void processCommand(const MidiCommand& command);
	void processEvent(const MidiEvent& event);

	/* interface AlsaMidiPort */
	void client(long newClient);
	long client();

	void port(long newPort);
	long port();
	
	bool open();
};

}
#endif /* HAVE_ARTS_LIBASOUND2 */
#endif /* ARTS_ALSAMIDIPORT_IMPL_H */
