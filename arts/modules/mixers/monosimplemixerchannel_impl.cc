#include "artsmodulesmixers.h"
#include "flowsystem.h"
#include "stdsynthmodule.h"
#include "connect.h"

namespace Arts {
class MonoSimpleMixerChannel_impl : virtual public MonoSimpleMixerChannel_skel,
                                    virtual public StdSynthModule
{
protected:
	Synth_STD_EQUALIZER _equalizer;
	StereoEffectStack _insertEffects;
	Synth_MUL mulGain;
	Synth_MUL mulVolumeLeft, mulVolumeRight;
	float _gain, _pan, _volume, pLeft, pRight;
	std::string _name;
public:
	MonoSimpleMixerChannel_impl()
		: _gain(1.0), _pan(0), _volume(1.0), pLeft(1), pRight(1)
	{
		setValue(mulVolumeLeft,"invalue2",_volume*pLeft);
		setValue(mulVolumeRight,"invalue2",_volume*pRight);
		setValue(mulGain,"invalue2",_gain);
	}

	Synth_STD_EQUALIZER equalizer() { return _equalizer; }
	StereoEffectStack insertEffects() { return _insertEffects; }

	float gain() { return _gain; }
	void gain(float g)
	{
		if(g != _gain) {
			_gain = g;
			setValue(mulGain,"invalue2",g);
			gain_changed(g);
		}
	}

	float volume() { return _volume; }
	void volume(float v)
	{
		if(v != _volume) {
			_volume = v;
			setValue(mulVolumeLeft,"invalue2",v*pLeft);
			setValue(mulVolumeRight,"invalue2",v*pRight);
			volume_changed(v);
		}
	}

	float pan() { return _pan; }
	void pan(float p)
	{
		if(p != _pan)
		{
			_pan = p;
			pLeft = 1.0;
			pRight = 1.0;
			if(p > 0)
				pLeft = 1-p;
			else
				pRight = 1+p;
			setValue(mulVolumeLeft,"invalue2",_volume*pLeft);
			setValue(mulVolumeRight,"invalue2",_volume*pRight);
			pan_changed(p);
		}
	}

	std::string name() { return _name; }
	void name(const std::string& newName)
	{
		if(_name != newName) {
			_name = newName;
			name_changed(newName);
		}
	}

	void streamInit()
	{
		_equalizer.start();
		mulVolumeLeft.start();
		mulVolumeRight.start();
		mulGain.start();
		//_insertEffects.start();

		_node()->virtualize("inleft",mulGain._node(),"invalue1");
		connect(mulGain,"outvalue",_equalizer,"invalue");
		//connect(_equalizer,"outvalue",_insertEffects,"inleft");
		//connect(_insertEffects,"outleft",mulVolume,"invalue1");
		connect(_equalizer,"outvalue",mulVolumeLeft,"invalue1");
		connect(_equalizer,"outvalue",mulVolumeRight,"invalue1");
		_node()->virtualize("outleft",mulVolumeLeft._node(),"outvalue");
		_node()->virtualize("outright",mulVolumeRight._node(),"outvalue");

	}
	void streamEnd()
	{
		_equalizer.stop();
		//_insertEffects.stop();
		mulVolumeLeft.stop();
		mulVolumeRight.stop();
		mulGain.stop();
	}
};
REGISTER_IMPLEMENTATION(MonoSimpleMixerChannel_impl);
}

