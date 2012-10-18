#include "artsmodulessynth.h"
#include "artsbuilder.h"
#include "stdsynthmodule.h"
#include "objectmanager.h"
#include "connect.h"
#include "flowsystem.h"
#include "debug.h"
#include "dynamicrequest.h"
#include "audiosubsys.h"
#include <fstream>
#include <math.h>
#include <stdlib.h>

using namespace Arts;
using namespace std;

/*-------- instrument mapping ---------*/

class InstrumentMap {
protected:
	struct InstrumentData;
	class Tokenizer;
	list<InstrumentData> instruments;
	string directory;
	void loadLine(const string& line);

public:
	struct InstrumentParam;

	void loadFromList(const string& filename, const vector<string>& list);
	StructureDesc getInstrument(mcopbyte channel, mcopbyte note,
										mcopbyte velocity, mcopbyte program,
										vector<InstrumentParam>*& params);
};

struct InstrumentMap::InstrumentParam
{
	string param;
	Any value;

	InstrumentParam()
	{
	}

	InstrumentParam(const InstrumentParam& src)
		: param(src.param), value(src.value)
	{
	}

	InstrumentParam(const string& param, const string& strValue)
		: param(param)
	{
		/* put the string into the any */
		value.type = "string";

		Buffer b;
		b.writeString(strValue);
		b.read(value.value, b.size());
	}
};

struct InstrumentMap::InstrumentData 
{
	struct Range
	{
		int minValue, maxValue;
		Range() : minValue(0), maxValue(0)
		{
		}
		Range(int minValue, int maxValue)
			: minValue(minValue), maxValue(maxValue)
		{
		}
		bool match(int value)
		{
			return (value >= minValue) && (value <= maxValue);
		}
	};
	Range channel, pitch, program, velocity;
	vector<InstrumentParam> params;
	StructureDesc instrument;
};

class InstrumentMap::Tokenizer {
protected:
	bool haveToken, haveNextToken;
	string token, nextToken, input;
	string::iterator ii;
public:
	Tokenizer(const string& line)
		: haveToken(false), haveNextToken(false),
		  input(line+"\n"), ii(input.begin())
	{
		/* adding a \n ensures that we will definitely find the last token */
	}
	string getToken()
	{
		if(!haveMore())
			return "";

		if(haveNextToken)
		{
			string t = token;
			haveNextToken = false;
			token = nextToken;
			return t;
		}
		else
		{
			haveToken = false;
			return token;
		}
	}
	bool haveMore()
	{
		if(haveToken)
			return true;

		token = "";
		while(ii != input.end() && !haveToken)
		{
			const char& c = *ii++;
		
			if(c == ' ' || c == '\t' || c == '\n')
			{
				if(!token.empty()) haveToken = true;
			}
			else if(c == '=') /* || c == '-' || c == '+')*/
			{
				if(!token.empty())
				{
					haveNextToken = true;
					nextToken = c;
				}
				else
				{
					token = c;
				}
				haveToken = true;
			}
			else
			{
				token += c;
			}
		}
		return haveToken;
	}
};

void InstrumentMap::loadLine(const string& line)
{
	Tokenizer t(line);
	InstrumentData id;
	/* default: no filtering */
	id.channel = InstrumentData::Range(0,15);
	id.pitch = id.program = id.velocity = InstrumentData::Range(0,127);

	string s[3];
	int i = 0;
	bool seenDo = false;
	bool loadOk = false;

	if(t.getToken() != "ON")
	{
		arts_warning("error in arts-map: lines must start with ON (did start with %s)\n", t.getToken().c_str());
		return;
	}

	while(t.haveMore())
	{
		const string& token = t.getToken();

		if(token == "DO")
			seenDo = true;
		else
		{
			s[i] = token;
			if(i == 2)	/* evaluate */
			{
				if(s[1] != "=")
				{
					arts_warning("error in arts-map: no = operator\n");
					return;
				}

				if(seenDo)
				{
					if(s[0] == "structure")
					{
						string filename = s[2];

						/* if it's no absolute path, its relative to the map */
						if(!filename.empty() && filename[0] != '/')
							filename = directory + "/" + s[2];

						ifstream infile(filename.c_str());
						string line;
						vector<string> strseq;

						while(getline(infile,line))
							strseq.push_back(line);

						id.instrument.loadFromList(strseq);
						if(id.instrument.name() != "unknown")
						{
							loadOk = true;
						}
						else
						{
							arts_warning("mapped instrument: "
									"can't load structure %s",s[2].c_str());
						}
					}
					else
					{
						/* TODO: handle different datatypes */
						id.params.push_back(InstrumentParam(s[0], s[2]));
					}
				}
				else
				{
					InstrumentData::Range range;
					range.minValue = atoi(s[2].c_str());
					range.maxValue = range.minValue;
					int i = s[2].find("-",0);
					if(i != 0)
					{
						range.minValue = atoi(s[2].substr(0,i).c_str());
						range.maxValue =
							atoi(s[2].substr(i+1,s[2].size()-(i+1)).c_str());
					}
					if(s[0] == "pitch")		id.pitch = range;
					if(s[0] == "channel")	id.channel = range;
					if(s[0] == "program")	id.program = range;
					if(s[0] == "velocity")	id.velocity = range;
				}
				i = 0;
			}
			else i++;
		}
	}
	if(loadOk) instruments.push_back(id);
}

void InstrumentMap::loadFromList(const string& filename,
									const vector<string>& list)
{
	int r = filename.rfind('/');
	if(r > 0)
		directory = filename.substr(0,r);
	else
		directory = "";

	vector<string>::const_iterator i;
	instruments.clear();
	for(i = list.begin(); i != list.end(); i++) loadLine(*i);
}

StructureDesc InstrumentMap::getInstrument(mcopbyte channel, mcopbyte note,
											mcopbyte velocity, mcopbyte program,
											vector<InstrumentParam>*& params)
{
	list<InstrumentData>::iterator i;
	for(i = instruments.begin(); i != instruments.end(); i++)
	{
		InstrumentData &id = *i;

		if(id.channel.match(channel) && id.pitch.match(note) &&
		   id.velocity.match(velocity) && id.program.match(program))
		{
			params = &id.params;
			return id.instrument;
		}
	}

	return StructureDesc::null();
}


/*-------instrument mapping end -------*/

static SynthModule get_AMAN_PLAY(Object structure)
{
	Object resultObj = structure._getChild("play");
	assert(!resultObj.isNull());

	SynthModule result = DynamicCast(resultObj);
	assert(!result.isNull());

	return result;
}

struct TSNote {
	MidiPort port;
	MidiEvent event;
	TSNote(MidiPort port, const MidiEvent& event) :
		port(port), event(event)
	{
	}
};

class AutoMidiRelease : public TimeNotify {
public:
	vector<MidiReleaseHelper> impls;
	AutoMidiRelease()
	{
		Dispatcher::the()->ioManager()->addTimer(10, this);
	}
	virtual ~AutoMidiRelease()
	{
		Dispatcher::the()->ioManager()->removeTimer(this);
	}
	void notifyTime()
	{
		vector<MidiReleaseHelper>::iterator i = impls.begin();
		while(i != impls.end())
		{
			if(i->terminate())
			{
				MidiReleaseHelper& helper = *i;

				arts_debug("one voice terminated");
				// put the MidiReleaseHelper and the voice into the ObjectCache
				// (instead of simply freeing it)
				ObjectCache cache = helper.cache();
				SynthModule voice = helper.voice();
				get_AMAN_PLAY(voice).stop();
				voice.stop();
				cache.put(voice,helper.name());
				impls.erase(i);
				return;
			} else i++;
		}
	}
}	*autoMidiRelease;

// cache startup & shutdown
static class AutoMidiReleaseStart :public StartupClass
{
public:
	void startup() { autoMidiRelease = new AutoMidiRelease(); }
	void shutdown() { delete autoMidiRelease; }
} autoMidiReleaseStart;

class MidiReleaseHelper_impl : virtual public MidiReleaseHelper_skel,
							   virtual public StdSynthModule
{
protected:
	bool _terminate;
	SynthModule _voice;
	ObjectCache _cache;
	string _name;

public:
	MidiReleaseHelper_impl()
	{
		autoMidiRelease->impls.push_back(MidiReleaseHelper::_from_base(_copy()));
	}
	~MidiReleaseHelper_impl() {
		artsdebug("MidiReleaseHelper: one voice is gone now\n");
	}


	SynthModule voice() { return _voice; }
	void voice(SynthModule voice) { _voice = voice; }

	ObjectCache cache() { return _cache; }
	void cache(ObjectCache cache) { _cache = cache; }

	string name() { return _name; }
	void name(const string& name) { _name = name; }

	bool terminate() { return _terminate; }
	void streamStart() { _terminate = false; }

	void calculateBlock(unsigned long /*samples*/)
	{
		if(done[0] > 0.5)
			_terminate = true;
	}
};
REGISTER_IMPLEMENTATION(MidiReleaseHelper_impl);

class Synth_MIDI_TEST_impl : virtual public Synth_MIDI_TEST_skel,
							 virtual public StdSynthModule {
protected:
	struct ChannelData {
		SynthModule voice[128];
		string name[128];
		float pitchShiftValue;
		mcopbyte program;
		ChannelData() {
			// initialize all voices with NULL objects (no lazy create)
			for(int i = 0; i < 128; i++) voice[i] = SynthModule::null();

			pitchShiftValue = 0.0;
			program = 0;
		}
	}	*channelData; /* data for all 16 midi channels */

	bool useMap;
	InstrumentMap map;
	StructureDesc instrument;
	StructureBuilder builder;
	AudioManagerClient amClient;
	ObjectCache cache;
	MidiClient client;
	MidiTimer timer;

	string _filename;
	string _busname;
	string _title;
public:
	Synth_MIDI_TEST self() { return Synth_MIDI_TEST::_from_base(_copy()); }

	Synth_MIDI_TEST_impl();
	~Synth_MIDI_TEST_impl();

	void filename(const string& newname);
	string filename()
	{
		return _filename;
	}
	void busname(const string& newname);
	string busname()
	{
		return _busname;
	}
	string title()
	{
		return _title;
	}
	void noteOn(mcopbyte channel, mcopbyte note, mcopbyte velocity);
	void noteOff(mcopbyte channel, mcopbyte note);
	void pitchWheel(mcopbyte channel, mcopbyte lsb, mcopbyte msb);

	float getFrequency(mcopbyte note,mcopbyte channel);

	void streamStart();
	void streamEnd();

	TimeStamp time()
	{
		return timer.time();
	}
	TimeStamp playTime()
	{
		/*
		 * what the user currently hears is exactly latencySec before our
		 * port timeStamp (as this is the size of the audio buffer)
		 */
		double latencySec = AudioSubSystem::the()->outputDelay();
		TimeStamp t = time();

		int sec = int(latencySec);
		t.sec -= sec;
		latencySec -= double(sec);
		t.usec -= int(latencySec * 1000000.0);

		if (t.usec < 0)
		{
			t.usec += 1000000;
			t.sec -= 1;
		}

		arts_assert(t.usec >= 0 && t.usec < 1000000);
		return t;
	}
	void processEvent(const MidiEvent& event)
	{
		timer.queueEvent(self(),event);
	}
	void processCommand(const MidiCommand& command)
	{
		mcopbyte channel = command.status & mcsChannelMask;

		switch(command.status & mcsCommandMask)
		{
			case mcsNoteOn: noteOn(channel,command.data1,command.data2);
				return;
			case mcsNoteOff: noteOff(channel,command.data1);
				return;
			case mcsPitchWheel: pitchWheel(channel,command.data1,command.data2);
				return;
			case mcsProgram: channelData[channel].program = command.data1;
				return;
			case mcsParameter:
					if(command.data1 == mcpAllNotesOff && command.data2 == 0)
						for(mcopbyte note=0; note<128; note++)
							noteOff(channel,note);
				return;
		}
	}
};
REGISTER_IMPLEMENTATION(Synth_MIDI_TEST_impl);


void Synth_MIDI_TEST_impl::busname(const string& newname)
{
	// TODO:
	_busname = newname;
}

void Synth_MIDI_TEST_impl::filename(const string& newname)
{
	ifstream infile(newname.c_str());
	string line;
	vector<string> strseq;

	while(getline(infile,line))
		strseq.push_back(line);

	_filename = newname;

/* search extension */
    string::const_reverse_iterator i;
	string extension;
	bool extensionok = false;

    for(i = newname.rbegin(); i != newname.rend() && !extensionok; i++)
    {
        if(*i == '.')
            extensionok = true;
        else
            extension.insert(extension.begin(), (char)tolower(*i));
    }

	if(extensionok && extension == "arts")
	{
		instrument.loadFromList(strseq);
		_title = "aRts Instrument ("+instrument.name()+")";
		useMap = false;
	}
	else if(extensionok && extension == "arts-map")
	{
		map.loadFromList(newname, strseq);
		_title = "aRts Instrument (mapped)";
		useMap = true;
	}

	if(!client.isNull())
		client.title(title());
	amClient.title(title());
}

Synth_MIDI_TEST_impl::Synth_MIDI_TEST_impl()
	: amClient(amPlay, "aRts Instrument","Synth_MIDI_TEST")
{
	useMap = false;
	client = MidiClient::null();
	timer = SubClass("Arts::AudioMidiTimer");
	channelData = new ChannelData[16];
}

Synth_MIDI_TEST_impl::~Synth_MIDI_TEST_impl()
{
	delete[] channelData;
}

void Synth_MIDI_TEST_impl::streamStart()
{
	// register with the midi manager
	MidiManager manager = Reference("global:Arts_MidiManager");
	if(!manager.isNull())
	{
		client = manager.addClient(mcdRecord,mctDestination,title(),
										"Arts::Synth_MIDI_TEST");
		client.addInputPort(self());
	}
	else
		arts_warning("Synth_MIDI_TEST: no midi manager found - not registered");
}

void Synth_MIDI_TEST_impl::streamEnd()
{
	client = MidiClient::null();
}

void Synth_MIDI_TEST_impl::noteOn(mcopbyte channel, mcopbyte note,
											mcopbyte velocity)
{
	if(velocity == 0)
	{
		noteOff(channel,note);
		return;
	}
	if(!channelData[channel].voice[note].isNull())
	{
		noteOff(channel,note);
		arts_info("Synth_MIDI_TEST: duplicate noteOn (mixed channels?)");
	}

	vector<InstrumentMap::InstrumentParam> *params = 0;
	if(useMap)
	{
		mcopbyte program = channelData[channel].program;
		StructureDesc sd = map.getInstrument(channel,note,velocity,program,params);
		if(sd.isNull()) return;
		instrument = sd;
	}

	Object structureObject = cache.get(instrument.name());
	if(structureObject.isNull())
	{
		arts_debug("creating new structure");
		structureObject = builder.createObject(instrument);

		SynthModule play;
		// TODO: allow changing busname!
		if(!_busname.empty())
		{
			Synth_BUS_UPLINK b;
			b.busname(_busname);
			play = b;
		}
		else
		{
			Synth_AMAN_PLAY a(amClient);
			play = a;
		}
		structureObject._addChild(play,"play");
		connect(structureObject,"left",play,"left");
		connect(structureObject,"right",play,"right");
	}
	else
	{
		arts_debug("used cached structure");
	}

	SynthModule structure = DynamicCast(structureObject);
	assert(!structure.isNull());

	if(params)
	{
		vector<InstrumentMap::InstrumentParam>::iterator pi;
		for(pi = params->begin(); pi != params->end(); pi++)
		{
			DynamicRequest req(structure);

			req.method("_set_"+pi->param).param(pi->value).invoke();
		}
	}
	setValue(structure,"frequency",getFrequency(note,channel));
	setValue(structure,"velocity",(float)velocity/127.0);
	setValue(structure,"pressed",1.0);

	get_AMAN_PLAY(structure).start();
	structure.start();

	channelData[channel].voice[note] = structure;
	channelData[channel].name[note] = instrument.name();
}

void Synth_MIDI_TEST_impl::noteOff(mcopbyte channel, mcopbyte note)
{
	if(!channelData[channel].voice[note].isNull())
	{
		setValue(channelData[channel].voice[note],"pressed",0.0);

		MidiReleaseHelper h;

		h.voice(channelData[channel].voice[note]);
		h.cache(cache);
		h.name(channelData[channel].name[note]);

		connect(channelData[channel].voice[note],"done",h,"done");
		h.start();
		assert(!h.terminate());
		channelData[channel].voice[note] = SynthModule::null();
	}
}

float Synth_MIDI_TEST_impl::getFrequency(mcopbyte note, mcopbyte channel)
{
	/* 2 semitones pitchshift */
	return 261.63 * pow(2,((float)(note)+(channelData[channel].pitchShiftValue*2.0))/12.0)/32.0;
}

void Synth_MIDI_TEST_impl::pitchWheel(mcopbyte channel,
										mcopbyte lsb, mcopbyte msb)
{
	mcopbyte note;

	channelData[channel].pitchShiftValue =
		(float)((lsb + msb*128) - (0x40*128))/8192.0;

	for(note = 0; note < 128; note++)
	{
		if(!channelData[channel].voice[note].isNull())
			setValue(channelData[channel].voice[note],"frequency",getFrequency(note,channel));
	}
}
