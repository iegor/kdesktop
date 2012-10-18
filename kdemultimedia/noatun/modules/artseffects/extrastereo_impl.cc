#include "artsgui.h"
#include "artseffects.h"
#include "stdsynthmodule.h"
#include <dynamicrequest.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <convert.h>
#include "debug.h"
#include <connect.h>
#include <cstdlib>

using namespace Arts;

class ExtraStereo_impl : virtual public ExtraStereo_skel,
						 virtual public StdSynthModule
{
	float _intensity;
//	StereoEffectGUI mStereoEffectGUI;
public:
	float intensity() { return _intensity; }
	void intensity(float newValue) {
		_intensity = newValue;
//		cout << " **** INTENSITY = " << newValue << endl;
	}

	ExtraStereo_impl() : _intensity(2.0)
	{
//		mStereoEffectGUI = DynamicCast(server.createObject("Arts::ExtraStereo"));
//		DynamicRequest(mStereoEffectGUI).method("_set_effect").param(*this).invoke();

	}
	void calculateBlock(unsigned long samples)
	{
		unsigned long i;

		for(i = 0; i < samples; i++)
		{
			float average = (inleft[i] + inright[i]) / 2.0;

			float outleftnew = average + (inleft[i] - average) * _intensity;
			if(outleftnew > 1.0) outleft[i] = 1.0;
			else if(outleftnew < -1.0) outleft[i] = -1.0;
			else outleft[i] = outleftnew;

			float outrightnew = average + (inright[i] - average) * _intensity;
			if(outrightnew > 1.0) outright[i] = 1.0;
			else if(outrightnew < -1.0) outright[i] = -1.0;
			else outright[i] = outrightnew;
		}
	}
};


class VoiceRemoval_impl : virtual public VoiceRemoval_skel,
						 virtual public StdSynthModule
{
public:

	VoiceRemoval_impl()
	{

	}
	// This is based on the work of Anders Carlsson <anders.carlsson@tordata.se>
	void calculateBlock(unsigned long samples)
	{
		for (unsigned i = 0; i < samples; i++)
		{
			float outleftnew=inleft[i]-inright[i];
			float outrightnew=inright[i]-inleft[i];

			if (inleft[i] < -1.0) outleftnew = -1.0;
			else if (inleft[i] > 1.0) outleftnew = 1.0;

			if (inright[i] < -1.0) outrightnew = -1.0;
			else if (inright[i] > 1.0) outrightnew = 1.0;
			outleft[i] = outleftnew;
			outright[i] = outrightnew;
		}
		
	}
};

class RawWriter_impl : virtual public RawWriter_skel,
                          virtual public StdSynthModule
{
	int mFd;
public:
	RawWriter_impl()
	{
		std::string file=getenv("HOME");
		file.append("/arts-write.raw");
		mFd=::open(file.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	}
	~RawWriter_impl()
	{
		::close(mFd);
	}
	// This is based on the work of Anders Carlsson <anders.carlsson@tordata.se>
	void calculateBlock(unsigned long samples)
	{
		for (unsigned i = 0; i < samples; i++)
		{
			outleft[i]=inleft[i];
			outright[i]=inright[i];
		}
		unsigned char *buffer=new unsigned char[samples*4];
		convert_stereo_2float_i16le(samples,
									inleft, inright, buffer);
		::write(mFd, (const void*)buffer, samples*4);
		delete [] buffer;
	}
};

class ExtraStereoGuiFactory_impl : public ExtraStereoGuiFactory_skel
{
public:
	Widget createGui(Object object)
	{
		arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());

		ExtraStereo e = DynamicCast(object);
		arts_return_val_if_fail(!e.isNull(), Arts::Widget::null());

		HBox hbox;
		hbox.width(80);
		hbox.height(80);
		hbox.show();


		Poti intense;
		intense.caption("Intensity");
		intense.color("red");
		intense.min(0);
		intense.max(5);
		intense.value(e.intensity());
		intense.parent(hbox);
		intense.show();
		connect(intense,"value_changed", e, "intensity");
		
		hbox._addChild(intense,"intensityWidget");
		
		return hbox;
	}
};


REGISTER_IMPLEMENTATION(ExtraStereo_impl);
REGISTER_IMPLEMENTATION(VoiceRemoval_impl);
REGISTER_IMPLEMENTATION(RawWriter_impl);
REGISTER_IMPLEMENTATION(ExtraStereoGuiFactory_impl);


