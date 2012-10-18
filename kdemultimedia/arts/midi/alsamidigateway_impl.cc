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

#include "artsmidi.h"

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

#include "alsamidiport_impl.h"
#include <arts/debug.h>
#include <stdio.h>

using namespace Arts;
using namespace std;

class AlsaMidiGateway_impl : virtual public AlsaMidiGateway_skel {
protected:
	snd_seq_t *seq;

	struct PortEntry {
		int alsaClient, alsaPort;
		bool keep;

		AlsaMidiPort port;
		MidiClient client;
	};
	list<PortEntry> ports;

#ifdef HAVE_ARTS_LIBASOUND2
/* ALSA-0.9 specific code */
	int alsaOpen() {
		return snd_seq_open(&seq, "hw", SND_SEQ_OPEN_DUPLEX, 0);
	}
	bool alsaScan(MidiManager midiManager) {
		snd_seq_client_info_t *cinfo;
		snd_seq_port_info_t *pinfo;

		snd_seq_client_info_alloca(&cinfo);
		snd_seq_client_info_set_client(cinfo, -1);

		while (snd_seq_query_next_client(seq, cinfo) >= 0) {
			int client = snd_seq_client_info_get_client(cinfo);

			snd_seq_port_info_alloca(&pinfo);
			snd_seq_port_info_set_client(pinfo, client);

			snd_seq_port_info_set_port(pinfo, -1);
			while (snd_seq_query_next_port(seq, pinfo) >= 0) {
				unsigned int cap;

				cap = (SND_SEQ_PORT_CAP_SUBS_WRITE|SND_SEQ_PORT_CAP_WRITE);
				if ((snd_seq_port_info_get_capability(pinfo) & cap) == cap) {
					string name = snd_seq_port_info_get_name(pinfo);
					int client = snd_seq_port_info_get_client(pinfo);
					int port = snd_seq_port_info_get_port(pinfo);

					createPort(midiManager, name, client, port);
				}
			}
		}

		return true;
	}
#else
/* ALSA-0.5 specific code */
	int alsaOpen() {
		return snd_seq_open(&seq, SND_SEQ_OPEN);
	}

	bool alsaScan(MidiManager midiManager) {
		snd_seq_system_info_t sysinfo;

    	int err = snd_seq_system_info(seq, &sysinfo);
    	if (err < 0)
		{
        	arts_warning("snd_seq_systeminfo failed: %s", snd_strerror(err));
			return false;
		}

		for(int client = 0; client < sysinfo.clients; client++)
		{
			snd_seq_client_info_t cinfo;
			if (snd_seq_get_any_client_info(seq, client, &cinfo) == 0)
			{
				for(int port = 0; port < sysinfo.ports; port++)
				{
					snd_seq_port_info_t pinfo;
					if(snd_seq_get_any_port_info(seq, client, port, &pinfo) == 0)
					{
						unsigned int cap;
						cap = (SND_SEQ_PORT_CAP_SUBS_WRITE|SND_SEQ_PORT_CAP_WRITE);

						if ((pinfo.capability & cap) == cap)
							createPort(midiManager, pinfo.name, client, port);
					}
				}
			}
		}

		return true;
	}
#endif

public:
	AlsaMidiGateway_impl() : seq(0)
	{
	}

	~AlsaMidiGateway_impl()
	{
		if(seq)
			snd_seq_close(seq);
	}

	void createPort(MidiManager midiManager, string name, int client, int port)
	{
		if(name != "aRts")
		{
			char nr[1024];

			sprintf(nr, " (%3d:%-3d)", client, port);
			name += nr;

			list<PortEntry>::iterator pi = ports.begin();
			while(pi != ports.end() && (pi->alsaClient != client || pi->alsaPort != port))
				pi++;

			if(pi != ports.end()) /* we already have this port */
				pi->keep = true;
			else				  /* we need to create it */
			{
				PortEntry pe;
				pe.port = AlsaMidiPort::_from_base(
						new AlsaMidiPort_impl(seq, client, port));

				if(pe.port.open())
				{
					pe.client = midiManager.addClient(mcdRecord,
							mctDestination,
							name, name);
					pe.client.addInputPort(pe.port);
					pe.alsaClient = client;
					pe.alsaPort = port;
					pe.keep = true;

					ports.push_back(pe);
				}
			}
		}
	}

	bool rescan()
	{
		MidiManager midiManager = DynamicCast(Reference("global:Arts_MidiManager"));
		if(midiManager.isNull())
		{
			arts_warning("AlsaMidiGateway: can't find MidiManager");
			return false;
		}

		if(!seq)
		{
			int err = alsaOpen();
			if (err < 0)
			{
				arts_warning("AlsaMidiGateway: could not open sequencer %s",
							 snd_strerror(err));
				seq = 0;
				return false;
			}
		}

		list<PortEntry>::iterator pi;
		for(pi = ports.begin(); pi != ports.end(); pi++)
			pi->keep = false;

		if(!alsaScan(midiManager))
			return false;

		/* erase those ports that are no longer needed */
		pi = ports.begin();
		while(pi != ports.end())
		{
			if(!pi->keep)
				pi = ports.erase(pi);
			else
				pi++;
		}

		return true;
	}
};

#else

using namespace Arts;
using namespace std;

class AlsaMidiGateway_impl : virtual public AlsaMidiGateway_skel {
public:
	bool rescan()
	{
		/* dummy version: no ALSA support compiled in */
		return false; 
	}
};

#endif

namespace Arts {
	REGISTER_IMPLEMENTATION(AlsaMidiGateway_impl);
}
