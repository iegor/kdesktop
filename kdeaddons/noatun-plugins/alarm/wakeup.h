/***************************************************************************
                 wakeup.h  -  alarm plugin for noatun
                     -------------------
     begin    : Wed Apr 11 CEST 2000
     copyright: (C) 2001 by Mickael Marchand <mikmak@freenux.org>
                                                       
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef WAKEUP_H
#define WAKEUP_H

#include <noatun/pref.h>
#include <noatun/plugin.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>

class QSpinBox;
class QCheckBox;
class KMinuteSpinBox;
class KPercentSpinBox;

class Wakeup : public QObject, public Plugin
{
	Q_OBJECT
	NOATUNPLUGIND

public:
	Wakeup();
	~Wakeup();
	static Wakeup *wakeme;
	void PlayerAct();
	void update();

public slots:
	void slotVolumeChange(); 
	void slotCheckTime();
	
private:
	QStringList *day_list;
//	QString songlist;
	int hour[7];
	int minute[7];
	bool days[7];
	QTimer *volTimer;
	int volEndVal;
	int modeAlarm;
};


class WakeupPrefs : public CModule
{
	Q_OBJECT

public:
	WakeupPrefs( QObject *parent );
	virtual void save();
	virtual void load();
public slots:
	void slotViewFrame(int);
	void slotApplyAll();

private:
	QCheckBox *monday;
	QCheckBox *tuesday;
	QCheckBox *wednesday;
	QCheckBox *thursday;
	QCheckBox *friday;
	QCheckBox *saturday;
	QCheckBox *sunday;
	KMinuteSpinBox *minute1;
	KMinuteSpinBox *minute2;
	KMinuteSpinBox *minute3;
	KMinuteSpinBox *minute4;
	KMinuteSpinBox *minute5;
	KMinuteSpinBox *minute6;
	KMinuteSpinBox *minute7;
	QSpinBox *hour1;
	QSpinBox *hour2;
	QSpinBox *hour3;
	QSpinBox *hour4;
	QSpinBox *hour5;
	QSpinBox *hour6;
	QSpinBox *hour7;

	KPercentSpinBox *volEndValue;
	QRadioButton *alarmmode;
	QRadioButton *morningmode;
	QRadioButton *nightmode;
	QFrame *volFrame;
	QPushButton *applyall;
};

#endif // WAKEUP_H
