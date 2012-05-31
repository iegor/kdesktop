#include "flowsystem.h"
#include "artsmoduleseffects.h"
#include "stdsynthmodule.h"

#include <artsmodulessynth.h>

using namespace Arts;

class Synth_STEREO_PITCH_SHIFT_impl : virtual public Synth_STEREO_PITCH_SHIFT_skel,
                                      virtual public StdSynthModule
{
protected:
	Synth_PITCH_SHIFT leftPitchShift, rightPitchShift;
	
public:
	float speed() { return leftPitchShift.speed(); }
	void speed(float newSpeed)
	{
		leftPitchShift.speed(newSpeed);
		rightPitchShift.speed(newSpeed);
	}

	float frequency() { return leftPitchShift.frequency(); }
	void frequency(float newFrequency)
	{
		leftPitchShift.frequency(newFrequency);
		rightPitchShift.frequency(newFrequency);
	}

	void streamStart()
	{
		leftPitchShift.start();
		rightPitchShift.start();
		_node()->virtualize("inleft",leftPitchShift._node(),"invalue");
		_node()->virtualize("outleft",leftPitchShift._node(),"outvalue");
		_node()->virtualize("inright",rightPitchShift._node(),"invalue");
		_node()->virtualize("outright",rightPitchShift._node(),"outvalue");
	}

	void streamEnd()
	{
		_node()->devirtualize("inleft",leftPitchShift._node(),"invalue");
		_node()->devirtualize("outleft",leftPitchShift._node(),"outvalue");
		_node()->devirtualize("inright",rightPitchShift._node(),"invalue");
		_node()->devirtualize("outright",rightPitchShift._node(),"outvalue");
		leftPitchShift.stop();
		rightPitchShift.stop();
	}
};

REGISTER_IMPLEMENTATION(Synth_STEREO_PITCH_SHIFT_impl);
