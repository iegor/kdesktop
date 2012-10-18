#include <qlayout.h>
#include <qslider.h>
#include "extrastereogui_impl.h"

namespace Arts {

ExtraStereoGUI_impl::ExtraStereoGUI_impl() : QWidget(0)
{
		(new QHBoxLayout(this))->setAutoAdd(true);
		mSlider=new QSlider(0,100,10, 0, Horizontal, this);
		mSlider->show();
		show();
}

void ExtraStereoGUI_impl::changeSlider(int v)
{
	effect.intensity((float)v/100.0);
}

void ExtraStereoGUI_impl::setEffect(StereoEffect newEffect)
{
	effect = DynamicCast(newEffect);
}

REGISTER_IMPLEMENTATION(ExtraStereoGUI_impl);

};

