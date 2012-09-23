#include "artsmodulesmixers.h"
#include "flowsystem.h"
#include "stdsynthmodule.h"
#include "connect.h"

namespace Arts {
class SimpleMixerChannel_impl : virtual public SimpleMixerChannel_skel,
								virtual public StdSynthModule
{
protected:
	Synth_STD_EQUALIZER _equalizerLeft, _equalizerRight;
	StereoEffectStack _insertEffects;
	Synth_MUL mulGainLeft, mulGainRight;
	Synth_MUL mulVolumeLeft, mulVolumeRight;
	float _gainLeft, _gainRight, _pan, _volumeLeft, _volumeRight, pLeft, pRight;
	std::string _name;
public:
	SimpleMixerChannel_impl()
		: _gainLeft(1.0), _gainRight(1.0), _pan(0), _volumeLeft(1.0), _volumeRight(1.0), pLeft(1), pRight(1)
	{
		setValue(mulVolumeLeft,"invalue2",_volumeLeft*pLeft);
		setValue(mulVolumeRight,"invalue2",_volumeRight*pRight);
		setValue(mulGainLeft,"invalue2",_gainLeft);
		setValue(mulGainRight,"invalue2",_gainRight);
	}

	Synth_STD_EQUALIZER equalizerLeft() { return _equalizerLeft; }
	Synth_STD_EQUALIZER equalizerRight() { return _equalizerRight; }
	StereoEffectStack insertEffects() { return _insertEffects; }

	float gainLeft() { return _gainLeft; }
	void gainLeft(float g)
	{
		if(g != _gainLeft) {
			_gainLeft = g;
			setValue(mulGainLeft,"invalue2",g);
			gainLeft_changed(g);
		}
	}

	float gainRight() { return _gainRight; }
	void gainRight(float g)
	{
		if(g != _gainRight) {
			_gainRight = g;
			setValue(mulGainRight,"invalue2",g);
			gainRight_changed(g);
		}
	}

	float volumeLeft() { return _volumeLeft; }
	void volumeLeft(float v)
	{
		if(v != _volumeLeft) {
			_volumeLeft = v;
			setValue(mulVolumeLeft,"invalue2",v*pLeft);
			volumeLeft_changed(v);
		}
	}

	float volumeRight() { return _volumeRight; }
	void volumeRight(float v)
	{
		if(v != _volumeRight) {
			_volumeRight = v;
			setValue(mulVolumeRight,"invalue2",v*pRight);
			volumeRight_changed(v);
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
			setValue(mulVolumeLeft,"invalue2",_volumeLeft*pLeft);
			setValue(mulVolumeRight,"invalue2",_volumeRight*pRight);
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
		_equalizerLeft.start();
		_equalizerRight.start();
		_insertEffects.start();
		mulVolumeLeft.start();
		mulVolumeRight.start();
		mulGainLeft.start();
		mulGainRight.start();

		_node()->virtualize("inleft",mulGainLeft._node(),"invalue1");
		_node()->virtualize("inright",mulGainRight._node(),"invalue1");
		connect(mulGainLeft,"outvalue",_equalizerLeft,"invalue");
		connect(mulGainRight,"outvalue",_equalizerRight,"invalue");
		connect(_equalizerLeft,"outvalue",_insertEffects,"inleft");
		connect(_equalizerRight,"outvalue",_insertEffects,"inright");
		connect(_insertEffects,"outleft",mulVolumeLeft,"invalue1");
		connect(_insertEffects,"outright",mulVolumeRight,"invalue1");
		_node()->virtualize("outleft",mulVolumeLeft._node(),"outvalue");
		_node()->virtualize("outright",mulVolumeRight._node(),"outvalue");
	}
	void streamEnd()
	{
		_equalizerLeft.stop();
		_equalizerRight.stop();
		_insertEffects.stop();
		mulVolumeLeft.stop();
		mulVolumeRight.stop();
		mulGainLeft.stop();
		mulGainRight.stop();
	}
};
REGISTER_IMPLEMENTATION(SimpleMixerChannel_impl);
}

