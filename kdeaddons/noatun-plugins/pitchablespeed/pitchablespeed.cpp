/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2001 hans_meine@gmx.net

#include "pitchablespeed.h"
#include <arts/reference.h>
#include <arts/soundserver.h>
#include <arts/kmedia2.h>
#include <noatun/app.h>
#include <noatun/player.h>
#include <noatun/engine.h>
#include <noatun/noatunarts.h>
#include <qslider.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <klocale.h>
#include <knuminput.h>

extern "C"
{
	Plugin *create_plugin()
	{
		KGlobal::locale()->insertCatalogue("pitchablespeed");
		return new PitchableSpeed();
	}
}

const int PitchableSpeed::DEFAULT_OFFSET = 5;
const int PitchableSpeed::DEFAULT_RANGE = 10;

PitchableSpeed::PitchableSpeed() : Plugin(),
	offset(DEFAULT_OFFSET),
	range(DEFAULT_RANGE)
,	frequenciesPreserved(false)
{
	
	rangeInput->setRange(2,150,1,false);
	rangeInput->setValue(range);
	offsetInput->setRange(-5,15,1,false);
	offsetInput->setValue(offset);
	setupSliderRange();
	connect(napp->player(), SIGNAL(changed()),
			this, SLOT(newSong()));
	connect(preserveFrequencies, SIGNAL(toggled(bool)),
			this, SLOT(preserveFrequenciesChanged(bool)));
	//show();
}

PitchableSpeed::~PitchableSpeed()
{
	napp->pluginMenuRemove(pluginMenuItem);

	Arts::PlayObject playobject= napp->player()->engine()->playObject();
	Arts::PitchablePlayObject pitchable= Arts::DynamicCast(playobject);
	if (!pitchable.isNull())
		pitchable.speed(1.0);

	if (frequenciesPreserved)
		napp->player()->engine()->effectStack()->remove(pitchShiftEffectID);
}

void PitchableSpeed::newSong()
{
	setSpeed();

	Arts::PlayObject playobject= napp->player()->engine()->playObject();
	Arts::PitchablePlayObject pitchable= Arts::DynamicCast(playobject);
	slider->setEnabled(!pitchable.isNull());
}

float PitchableSpeed::sliderSpeed()
{
	return 1.0f - slider->value()/1000.0f;
}

void PitchableSpeed::setSpeed()
{
	Arts::PlayObject playobject= napp->player()->engine()->playObject();
	Arts::PitchablePlayObject pitchable= Arts::DynamicCast(playobject);
	float newSpeed= 1.0f;
	if (!pitchable.isNull())
	{
		newSpeed= sliderSpeed();
		pitchable.speed(newSpeed);
		if (frequenciesPreserved)
			pitchShift.speed(1.f/newSpeed);
	}

	QString percentStr;
	percentStr.setNum((newSpeed-1.f)*100.f,'f',1);
	if (newSpeed<=0) percentStr.prepend('+').append(' ').append('%');
	currentValEdit->setText(percentStr);
}

void PitchableSpeed::setupSliderRange()
{
	int min= (-offset+range)<99 ? (-offset+range)*10 : 990;
	slider->setRange((-offset-range)*10,min);
	slider->setTickInterval(10);
}

void PitchableSpeed::speedChanged(int )
{
	setSpeed();
}

void PitchableSpeed::offsetChanged(int newOffset)
{
	offset= newOffset;
	setupSliderRange();
}

void PitchableSpeed::rangeChanged(int newRange)
{
	range= newRange;
	setupSliderRange();
}

void PitchableSpeed::preserveFrequenciesChanged(bool preserveNow)
{
	if (preserveNow==frequenciesPreserved)
		return;
	if (preserveNow)
	{
		pitchShift = Arts::DynamicCast(napp->player()->engine()->server()->
				 createObject("Arts::Synth_STEREO_PITCH_SHIFT"));
		pitchShift.frequency(10.f); // TODO: might have to experiment a bit with this
		pitchShift.speed(1.f/sliderSpeed());
		pitchShift.start();
		pitchShiftEffectID= napp->player()->engine()->effectStack()->
							insertTop(pitchShift, "correcting pitch shift");
	}
	else
	{
		napp->player()->engine()->effectStack()->remove(pitchShiftEffectID);
		pitchShift.stop();
		pitchShift = Arts::Synth_STEREO_PITCH_SHIFT::null();
	}
	frequenciesPreserved= preserveNow;
}

void PitchableSpeed::init()
{
	pluginMenuItem = napp->pluginMenuAdd(i18n("Toggle Speed Slider"),
										 this, SLOT(toggle(void)));
}

void PitchableSpeed::toggle(void)
{
	show();
	raise();
}

void PitchableSpeed::closeEvent(QCloseEvent *)
{
	hide();
}

#include "pitchablespeed.moc"

