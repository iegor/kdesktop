#include "flowsystem.h"
#include "artsmoduleseffects.h"
#include "stdsynthmodule.h"

using namespace Arts;

class Synth_STEREO_PITCH_SHIFT_FFT_impl : virtual public Synth_STEREO_PITCH_SHIFT_FFT_skel,
									  virtual public StdSynthModule
{
protected:
	Synth_PITCH_SHIFT_FFT leftPitchShift, rightPitchShift;
	
public:
	float speed() { return leftPitchShift.speed(); }
	void speed(float newSpeed)
	{
		leftPitchShift.speed(newSpeed);
		rightPitchShift.speed(newSpeed);
	}

	float scaleFactor() { return leftPitchShift.scaleFactor(); }
	void scaleFactor(float newScaleFactor)
	{
		leftPitchShift.scaleFactor(newScaleFactor);
		rightPitchShift.scaleFactor(newScaleFactor);
	}

	long frameSize() { return leftPitchShift.frameSize(); }
	void frameSize(long newFrameSize)
	{
		leftPitchShift.frameSize(newFrameSize);
		rightPitchShift.frameSize(newFrameSize);
	}

	long oversample() { return leftPitchShift.oversample(); }
	void oversample(long newOversample)
	{
		leftPitchShift.oversample(newOversample);
		rightPitchShift.oversample(newOversample);
	}

	void streamStart()
	{
		leftPitchShift.start();
		rightPitchShift.start();
		_node()->virtualize("inleft",leftPitchShift._node(),"inStream");
		_node()->virtualize("outleft",leftPitchShift._node(),"outStream");
		_node()->virtualize("inright",rightPitchShift._node(),"inStream");
		_node()->virtualize("outright",rightPitchShift._node(),"outStream");
	}

	void streamEnd()
	{
		_node()->devirtualize("inleft",leftPitchShift._node(),"inStream");
		_node()->devirtualize("outleft",leftPitchShift._node(),"outStream");
		_node()->devirtualize("inright",rightPitchShift._node(),"inStream");
		_node()->devirtualize("outright",rightPitchShift._node(),"outStream");
		leftPitchShift.stop();
		rightPitchShift.stop();
	}
};

REGISTER_IMPLEMENTATION(Synth_STEREO_PITCH_SHIFT_FFT_impl);
