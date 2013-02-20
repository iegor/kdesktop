#ifndef PITCHABLESPEED_H
#define PITCHABLESPEED_H

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2001 hans_meine@gmx.net

#include <noatun/plugin.h>
#include <arts/artsmodules.h>
#include "speedtuner.h"

class QSlider;

class PitchableSpeed : public SpeedTuner, public Plugin
{
Q_OBJECT

public:
	PitchableSpeed();
	virtual ~PitchableSpeed();

	void init();

public slots:
	void toggle(void);

	void newSong();
	
protected:
	virtual void closeEvent(QCloseEvent *);

	void setupSliderRange();
	void setSpeed();
	float sliderSpeed();

protected slots:
	void speedChanged(int);
	void offsetChanged(int);
	void rangeChanged(int);
	
	void preserveFrequenciesChanged(bool);
	
private:
	static const int DEFAULT_OFFSET;
	static const int DEFAULT_RANGE;

	int offset, range; // in percent, defaults 5% offset +-10% range
	
	bool frequenciesPreserved;
	Arts::Synth_STEREO_PITCH_SHIFT pitchShift;
	long pitchShiftEffectID;

	int pluginMenuItem;
};

#endif // PITCHABLESPEED_H
