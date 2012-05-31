    /*

    Copyright (C) 2000 Stefan Westerfeld
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
#include <iomanager.h>
#include <debug.h>
#include <fcntl.h>

using namespace std;

namespace Arts {

class RawMidiPort_impl : virtual public RawMidiPort_skel,
						 virtual public IONotify
{
protected:
	int fd;

	string _device;
	bool _input, _output;
	bool _running;
	mcopbyte laststatus;
	queue<mcopbyte> inq;
	MidiClient clientRecord, clientPlay;
	MidiPort outputPort;
	MidiManager manager;

	RawMidiPort self() {
		return RawMidiPort::_from_base(_copy());
	}
	SystemMidiTimer timer;

public:
	RawMidiPort_impl()
		:_device("/dev/midi"), _input(true), _output(true), _running(false),
		clientRecord(MidiClient::null()), clientPlay(MidiClient::null()),
		outputPort(MidiPort::null()),
		manager(Reference("global:Arts_MidiManager"))
	{
	}
	Arts::TimeStamp time()
	{
		return timer.time();
	}
	Arts::TimeStamp playTime()
	{
		return timer.time();
	}

	// attribute string running;
	bool running() { return _running; }
	void running(bool newrunning) {
		if(_running == newrunning) return;

		if(newrunning)
			open();
		else
			close();

		running_changed(_running);
	}

	// attribute string device;
	void device(const string& newdevice)
	{
		if(newdevice == _device) return;

		if(_running)
		{
			close();
			_device = newdevice;
			open();
		}
		else _device = newdevice;

		device_changed(newdevice);
	}
	string device() { return _device; }

	// attribute boolean input;
	void input(bool newinput)
	{
		if(newinput == _input) return;

		if(_running)
		{
			close();
			_input = newinput;
			open();
		}
		else _input = newinput;

		input_changed(newinput);
	}
	bool input() { return _input; }

	// attribute boolean output;
	void output(bool newoutput)
	{
		if(newoutput == _output) return;

		if(_running)
		{
			close();
			_output = newoutput;
			open();
		}
		else _output = newoutput;

		output_changed(newoutput);
	}
	bool output() { return _output; }

	bool open() {
		arts_return_val_if_fail(_running == false, true);
		arts_return_val_if_fail(_output || _input, false);
		arts_return_val_if_fail(manager.isNull() == false, false);
		laststatus = 0;

		int mode = O_NDELAY;
		if(_input)
		{
			if(_output)
				mode |= O_RDWR;
			else
				mode |= O_RDONLY;
		}
		else mode |= O_WRONLY;

   		fd = ::open(_device.c_str(),mode);
		if(fd != -1)
		{
			IOManager *iom = Dispatcher::the()->ioManager();
			if(_output)
				iom->watchFD(fd,IOType::read,this);

			string name = "OSS Midi Port ("+_device+")";
			if(_input)
			{
				clientRecord =
					manager.addClient(mcdRecord,mctDestination,name,name);
				clientRecord.addInputPort(self());
			}
			if(_output)
			{
				clientPlay =
					manager.addClient(mcdPlay,mctDestination,name,name);
				outputPort = clientPlay.addOutputPort();
			}

			_running = true;
			running_changed(true);
		}
		return _running;
	}

	void close()
	{
		arts_return_if_fail(_running == true);

		if(_input)
		{
			clientRecord.removePort(self());
			clientRecord = MidiClient::null();
		}
		if(_output)
		{
			clientPlay.removePort(outputPort);
			clientPlay = MidiClient::null();
		}

		Dispatcher::the()->ioManager()->remove(this,IOType::all);
		::close(fd);
	}

	int midiMsgLen(mcopbyte status)
	{
		switch(status & mcsCommandMask)
		{
			case mcsNoteOn:
      		case mcsNoteOff:
      		case mcsKeyPressure:
      		case mcsParameter:
      		case mcsPitchWheel:
         		return 3;
         		break;
      		case mcsProgram:
      		case mcsChannelPressure:
         		return 2;
         		break;
		}
		return 0;
	}
	void notifyIO(int fd, int type)
	{
		arts_return_if_fail(_running);
		assert(fd == this->fd);

		// convert iomanager notification types to audiosubsys notification
		if(type & IOType::read)
		{
			mcopbyte buffer[1024];
			int count = read(fd,buffer,1024);
			for(int i=0; i<count; i++)
			{
		   		/*
   				 * for now ignore all realtime-messages (0xF8 .. 0xFF),
				 * which may get * embedded *inside* every other message
   				 */
				if(buffer[i] < 0xf8)
					inq.push(buffer[i]);
			}
		}
		processMidi();
	}

	void processMidi()
	{
		for(;;)
		{
			// if we get a status byte, this is our new status
			if(!inq.empty())
			{
				if(inq.front() & 0x80)
				{
					laststatus = inq.front();
					inq.pop();
				}
			}

			// try to read a midi message with our current status
			// (this supports running status as well as normal messages)
			int len = midiMsgLen(laststatus);
			if(len)
			{
				if(len == 2)
				{
					if(inq.empty()) return; // need more input

					MidiCommand command;
					command.status = laststatus;
					command.data1 = inq.front(); inq.pop();
					command.data2 = 0;
					outputPort.processCommand(command);
				}
				else if(len == 3)
				{
					if(inq.size() < 2) return; // need more input

					MidiCommand command;
					command.status = laststatus;
					command.data1 = inq.front(); inq.pop();
					command.data2 = inq.front(); inq.pop();
					outputPort.processCommand(command);
				}
				else
				{
					arts_assert(false);
				}
			}
			else
			{
				if(inq.empty()) return; // need more input

				/* we are somewhat out of sync it seems -> read something
				 * away and hope we'll find a status byte */
				inq.pop();
			}
		}
	}
	void processCommand(const MidiCommand& command)
	{
		char message[3] = { command.status, command.data1, command.data2 };

		int len = midiMsgLen(command.status);
		if(midiMsgLen(command.status))
         	write(fd, message, len);
	}
	void processEvent(const MidiEvent& event)
	{
		timer.queueEvent(self(), event);
	}
};

REGISTER_IMPLEMENTATION(RawMidiPort_impl);
}

