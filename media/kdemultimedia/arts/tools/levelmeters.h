    /*

    Copyright (C) 2000 Hans Meine
	                   <hans_meine@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#ifndef ARTS_TOOLS_LEVELMETERS_H
#define ARTS_TOOLS_LEVELMETERS_H

#include <qobject.h>
#include <qframe.h>
#include <qdatetime.h>
#include <qptrlist.h>
#include <math.h>

// helper functions
const float LEVEL_MIN= 1.f/(1<<20); // minimal positive sample for 20 bit resolution
inline float levelToDB(float level) {
	if (level<LEVEL_MIN) level=LEVEL_MIN; // prevent from div by 0
	return (6.f/log(2.f))*log(level);
}

inline float DBToLevel(float db) {
	return exp(db/(log(2.f)/6.f));
}

/**
 * Base class for a single volume / value bar.
 */
class ACLevelMeter : public QFrame {
	Q_OBJECT
public:
	ACLevelMeter(QWidget *parent): QFrame(parent) {}
public slots:
	virtual void setValue(float f) = 0;
};

/**
 * Base class for a pair of volume / value bars.
 */
class StereoLevelMeter : public QFrame {
	Q_OBJECT
public:
	StereoLevelMeter(QWidget *parent): QFrame(parent) {}
public slots:
	virtual void setValues(float left, float right) = 0;
};

/**
 * Special LevelMeter which remembers min and max of the last [peakMillis]
 * milliseconds and displays a full bar with optional max/min markers.
 */
class PeakBar : public ACLevelMeter {
	Q_OBJECT
	bool clipped;

protected:
	static const int peakMillis; // how long do the peaks stay at their max?
	
	class Observation {
	public:
		QTime time;
		float value;
		Observation(float aValue): value(aValue) { time.start(); }
	};
	QPtrList<Observation> lastValues;
	
	float currentValue, maxValue, minValue;
	void checkMinMax();

	bool displayMinPeak;
	bool horizontalMode;

	void frameChanged();
	
public:
	PeakBar(QWidget *parent);
	
	QSize sizeHint() const;

	void drawContents(QPainter *p);
	virtual void setValue(float f);
};

/**
 * Special class which draws the Db scale with ticks, numbers and so on.
 */
class ScaleView : public QFrame {
	Q_OBJECT
protected:
	QFont font;
	int dbRange;
	int upperMargin, lowerMargin;
public:
	ScaleView(QWidget *parent);
	void setDbRange(int db);
	void setScaleMargins(int margins) { upperMargin= margins; lowerMargin=margins; }
	QSize sizeHint() const;
	void drawContents(QPainter *p);
};

/**
 * Reusable class which displays two volume bars (left/right) with a Db scale
 * and clip indicators. Supports / will have a context menu with some display
 * options like Db range, whether minimal values are also shown and others.
 */
class PeakLevelMeters : public StereoLevelMeter {
	Q_OBJECT
protected:
	int dbRange;
	PeakBar left, right;
	ScaleView scaleView;

public:
	PeakLevelMeters(QWidget *parent);

public slots:
	void setValues(float leftVal, float rightVal);
	void setDbRange(int db);
};

class KLed;

/**
 * Simple LevelMeter implementation with 12 KLeds.
 * Shows them in green/yellow/red combi if not blueState set, in which
 * case it's all blue. (Original artscontrol widget by stw.)
 */
class LedMeter : public ACLevelMeter {
	Q_OBJECT
protected:
	KLed *leds[12];

public:
	LedMeter(QWidget *parent, bool blueState = false);
	void setValue(float f);
};

/**
 * A simple pair of LedMeters.
 */
class StereoLedMeters : public StereoLevelMeter {
	Q_OBJECT
protected:
	LedMeter left, right;
	
public:
	StereoLedMeters(QWidget *parent);
public slots:
	void setValues(float left, float right);
};

#endif /* ARTS_TOOLS_LEVELMETERS_H */
