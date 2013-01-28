/**************************************************************************
			wakeup.cpp  -  alarm plugin for noatun
				------------------
		begin    : Wed Apr 11 CEST 2001
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


#include <noatun/engine.h>
#include <noatun/player.h>
#include <noatun/app.h>

#include <klocale.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>
#include <kiconloader.h>

#include <kconfig.h>
#include <kglobal.h>
#include "wakeup.h"
#include "kminutespinbox.h"
#include "kpercentspinbox.h"

extern "C" Plugin *create_plugin()
{
  KGlobal::locale()->insertCatalogue("wakeup");
  return new Wakeup();
}

Wakeup *Wakeup::wakeme = 0;

Wakeup::Wakeup() : QObject(), Plugin()
{
	wakeme = this;

	NOATUNPLUGINC(Wakeup);
	new WakeupPrefs(this);

	update();

	QTimer *timer=new QTimer(this);
	connect (timer,SIGNAL(timeout()),SLOT(slotCheckTime()));
	timer->start(60000,false);
}

Wakeup::~Wakeup()
{
}

void Wakeup::update()
{
	KConfig *config=KGlobal::config();
	config->setGroup("Noatun Alarm");

	hour[0]=config->readNumEntry("Hour1",7);
	minute[0]=config->readNumEntry("Minute1",0);
	hour[1]=config->readNumEntry("Hour2",7);
	minute[1]=config->readNumEntry("Minute2",0);
	hour[2]=config->readNumEntry("Hour3",7);
	minute[2]=config->readNumEntry("Minute3",0);
	hour[3]=config->readNumEntry("Hour4",7);
	minute[3]=config->readNumEntry("Minute4",0);
	hour[4]=config->readNumEntry("Hour5",7);
	minute[4]=config->readNumEntry("Minute5",0);
	hour[5]=config->readNumEntry("Hour6",7);
	minute[5]=config->readNumEntry("Minute6",0);
	hour[6]=config->readNumEntry("Hour7",7);
	minute[6]=config->readNumEntry("Minute7",0);

	days[0]=config->readBoolEntry("Monday",false);
	days[1]=config->readBoolEntry("Tuesday",false);
	days[2]=config->readBoolEntry("Wednesday",false);
	days[3]=config->readBoolEntry("Thursday",false);
	days[4]=config->readBoolEntry("Friday",false);
	days[5]=config->readBoolEntry("Saturday",false);
	days[6]=config->readBoolEntry("Sunday",false);
	
	volEndVal=config->readNumEntry("VolumeEnd",80);
	modeAlarm=config->readNumEntry("ModeAlarm",0);
//	songlist=config->readEntry("List","");
}

void Wakeup::slotCheckTime()
{
	QTime t;
	QDate d;

	t=QTime::currentTime();
	d=QDate::currentDate();

	if (t.minute()!=minute[d.dayOfWeek()-1] || t.hour()!=hour[d.dayOfWeek()-1]) return;

	if (days[d.dayOfWeek()-1])
	{
		//napp->player()->play();
		PlayerAct();
	}
}

void Wakeup::PlayerAct()
{
    if (modeAlarm==1 && !napp->player()->isPlaying())
    {
	    napp->player()->setVolume(0);
    }
        
    if (!napp->player()->isPlaying() && modeAlarm!=2) napp->player()->play();
        
    // timer for volume (up/down) according to prefs :-)
    volTimer = new QTimer (this);
    connect (volTimer,SIGNAL(timeout()), SLOT(slotVolumeChange()));
    
    volTimer->start(1000,false); // updates volume every seconds
        
}

void Wakeup::slotVolumeChange()
{
	int vol = napp->player()->volume();
	if ( vol >= volEndVal && modeAlarm==1)
	{
		volTimer->stop();
		return;
	}
	else if (vol <= 0 && modeAlarm==2)
	{
		volTimer->stop();
		napp->player()->stop();
		return;
	}
	else if (modeAlarm==0)
	{
		volTimer->stop();
		return;
	}
	
	if (modeAlarm==1)
	{
		napp->player()->setVolume( vol + 1 );
	}
	else if (modeAlarm==2)
	{
		napp->player()->setVolume( vol - 1 );
	}
}

///////////////////////////////////////////////////////////////////
// Prefs CModule 
///////////////////////////////////////////////////////////////////
WakeupPrefs::WakeupPrefs( QObject *parent ) :
	CModule( i18n("Wakeup"), i18n("Alarm Configuration"), "date", parent )
{
	QVBoxLayout *layout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	QGridLayout *grid = new QGridLayout (layout,8,4);
	
	monday=new QCheckBox(i18n("Monday"),this);
	tuesday=new QCheckBox(i18n("Tuesday"),this);
	wednesday=new QCheckBox(i18n("Wednesday"),this);
	thursday=new QCheckBox(i18n("Thursday"),this);
	friday=new QCheckBox(i18n("Friday"),this);
	saturday=new QCheckBox(i18n("Saturday"),this);
	sunday=new QCheckBox(i18n("Sunday"),this);
	hour1 = new QSpinBox(0, 23, 1, this , "Hour1");
	hour1->setButtonSymbols(QSpinBox::PlusMinus);
	minute1 = new KMinuteSpinBox(1, this , "Minute1");
	hour2 = new QSpinBox(0, 23, 1, this, "Hour2");
	hour2->setButtonSymbols(QSpinBox::PlusMinus);
	minute2 = new KMinuteSpinBox(1, this, "Minute2");
	hour3 = new QSpinBox(0, 23, 1, this, "Hour3");
	hour3->setButtonSymbols(QSpinBox::PlusMinus);
	minute3 = new KMinuteSpinBox(1, this, "Minute3");
	hour4 = new QSpinBox(0, 23, 1, this, "Hour4");
	hour4->setButtonSymbols(QSpinBox::PlusMinus);
	minute4 = new KMinuteSpinBox(1, this, "Minute4");
	hour5 = new QSpinBox(0, 23, 1, this, "Hour5");
	hour5->setButtonSymbols(QSpinBox::PlusMinus);
	minute5 = new KMinuteSpinBox(1, this, "Minute5");
	hour6 = new QSpinBox(0, 23, 1, this, "Hour6");
	hour6->setButtonSymbols(QSpinBox::PlusMinus);
	minute6 = new KMinuteSpinBox(1, this, "Minute6");
	hour7 = new QSpinBox(0, 23, 1, this, "Hour7");
	hour7->setButtonSymbols(QSpinBox::PlusMinus);
	minute7 = new KMinuteSpinBox(1, this, "Minute7");
	
	QLabel *selectday = new QLabel (i18n("Select days:"),this);
	QLabel *choosehour = new QLabel (i18n("Hour"),this);
	QLabel *choosemin = new QLabel (i18n("Minute"),this);
	grid->addWidget (selectday,0,0,Qt::AlignLeft); 
	grid->addWidget (choosehour,0,1,Qt::AlignLeft); 
	grid->addWidget (choosemin,0,2,Qt::AlignLeft); 

	applyall = new QPushButton (this,"applyall");
	applyall->setPixmap( BarIcon("down", KIcon::SizeSmall) ); 
	applyall->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, 
						QSizePolicy::Fixed, 
						applyall->sizePolicy().hasHeightForWidth()) );
	grid->addWidget (applyall,1,3,Qt::AlignLeft);						

	grid->addWidget (monday,1,0,Qt::AlignLeft);
	grid->addWidget (tuesday,2,0,Qt::AlignLeft);
	grid->addWidget (wednesday,3,0,Qt::AlignLeft);
	grid->addWidget (thursday,4,0,Qt::AlignLeft);
	grid->addWidget (friday,5,0,Qt::AlignLeft);
	grid->addWidget (saturday,6,0,Qt::AlignLeft);
	grid->addWidget (sunday,7,0,Qt::AlignLeft);
	grid->addWidget (hour1,1,1,Qt::AlignLeft);
	grid->addWidget (hour2,2,1,Qt::AlignLeft);
	grid->addWidget (hour3,3,1,Qt::AlignLeft);
	grid->addWidget (hour4,4,1,Qt::AlignLeft);
	grid->addWidget (hour5,5,1,Qt::AlignLeft);
	grid->addWidget (hour6,6,1,Qt::AlignLeft);
	grid->addWidget (hour7,7,1,Qt::AlignLeft);
	grid->addWidget (minute1,1,2,Qt::AlignLeft);
	grid->addWidget (minute2,2,2,Qt::AlignLeft);
	grid->addWidget (minute3,3,2,Qt::AlignLeft);
	grid->addWidget (minute4,4,2,Qt::AlignLeft);
	grid->addWidget (minute5,5,2,Qt::AlignLeft);
	grid->addWidget (minute6,6,2,Qt::AlignLeft);
	grid->addWidget (minute7,7,2,Qt::AlignLeft);
	grid->setColStretch (0,3);
	
	QButtonGroup *modeGroup = new QButtonGroup (3, Qt::Vertical, i18n ("Volume Control"),this);
	alarmmode = new QRadioButton (i18n("No change to volume, just starts the player"),modeGroup);
	nightmode = new QRadioButton (i18n("The volume decreases slowly and finally stops the player"),modeGroup);
	morningmode = new QRadioButton (i18n("The player starts and the volume increases to the chosen value"),modeGroup);
	modeGroup->setExclusive(true);
	morningmode->setChecked(true);

	volFrame = new QFrame (this);
	QLabel *volEndLabel = new QLabel (i18n("Volume increases to:"), volFrame);
	volEndValue = new KPercentSpinBox (1 ,volFrame,"volendvalue");

	layout->addWidget(modeGroup);
	layout->addWidget(volFrame);
	layout->addStretch();

	QHBoxLayout *volLayout = new QHBoxLayout(volFrame, KDialog::marginHint(), KDialog::spacingHint()); 
	volLayout->addWidget(volEndLabel);
	volLayout->addWidget(volEndValue);
	volLayout->addStretch();
	
	connect (morningmode,SIGNAL(stateChanged(int)),SLOT(slotViewFrame(int)));
	connect (applyall,SIGNAL(clicked()),SLOT(slotApplyAll()));
	
	load();
}

void WakeupPrefs::slotApplyAll()
{
	hour2->setValue(hour1->text().toInt());
	hour3->setValue(hour1->text().toInt());
	hour4->setValue(hour1->text().toInt());
	hour5->setValue(hour1->text().toInt());
	hour6->setValue(hour1->text().toInt());
	hour7->setValue(hour1->text().toInt());
	minute2->setValue(minute1->text().toInt());
	minute3->setValue(minute1->text().toInt());
	minute4->setValue(minute1->text().toInt());
	minute5->setValue(minute1->text().toInt());
	minute6->setValue(minute1->text().toInt());
	minute7->setValue(minute1->text().toInt());
}

void WakeupPrefs::slotViewFrame(int state)
{
	if (state==2) volFrame->show();
	else volFrame->hide();
}

void WakeupPrefs::save()
{
	KConfig *config=KGlobal::config();
	config->setGroup("Noatun Alarm");
	config->writeEntry("Monday",monday->isChecked());
	config->writeEntry("Tuesday",tuesday->isChecked());
	config->writeEntry("Wednesday",wednesday->isChecked());
	config->writeEntry("Thursday",thursday->isChecked());
	config->writeEntry("Friday",friday->isChecked());
	config->writeEntry("Saturday",saturday->isChecked());
	config->writeEntry("Sunday",sunday->isChecked());
	config->writeEntry("Hour1",hour1->text());
	config->writeEntry("Minute1",minute1->text());
	config->writeEntry("Hour2",hour2->text());
	config->writeEntry("Minute2",minute2->text());
	config->writeEntry("Hour3",hour3->text());
	config->writeEntry("Minute3",minute3->text());
	config->writeEntry("Hour4",hour4->text());
	config->writeEntry("Minute4",minute4->text());
	config->writeEntry("Hour5",hour5->text());
	config->writeEntry("Minute5",minute5->text());
	config->writeEntry("Hour6",hour6->text());
	config->writeEntry("Minute6",minute6->text());
	config->writeEntry("Hour7",hour7->text());
	config->writeEntry("Minute7",minute7->text());
	QString val(volEndValue->text());
	config->writeEntry("VolumeEnd",val);
	
	if (nightmode->isChecked()) config->writeEntry("ModeAlarm",2);
	else if (morningmode->isChecked()) config->writeEntry("ModeAlarm",1);
	else config->writeEntry("ModeAlarm", 0);
		
	config->sync();

	Wakeup *wake = Wakeup::wakeme;
	if (wake) wake->update();
}

void WakeupPrefs::load()
{
	KGlobal::config()->setGroup("Noatun Alarm");
	monday->setChecked(KGlobal::config()->readBoolEntry("Monday",false));
	tuesday->setChecked(KGlobal::config()->readBoolEntry("Tuesday",false));
	wednesday->setChecked(KGlobal::config()->readBoolEntry("Wednesday",false));
	thursday->setChecked(KGlobal::config()->readBoolEntry("Thursday",false));
	friday->setChecked(KGlobal::config()->readBoolEntry("Friday",false));
	saturday->setChecked(KGlobal::config()->readBoolEntry("Saturday",false));
	sunday->setChecked(KGlobal::config()->readBoolEntry("Sunday",false));

	hour1->setValue(KGlobal::config()->readNumEntry("Hour1",7));
	minute1->setValue(KGlobal::config()->readNumEntry("Minute1",0));
	hour2->setValue(KGlobal::config()->readNumEntry("Hour2",7));
	minute2->setValue(KGlobal::config()->readNumEntry("Minute2",0));
	hour3->setValue(KGlobal::config()->readNumEntry("Hour3",7));
	minute3->setValue(KGlobal::config()->readNumEntry("Minute3",0));
	hour4->setValue(KGlobal::config()->readNumEntry("Hour4",7));
	minute4->setValue(KGlobal::config()->readNumEntry("Minute4",0));
	hour5->setValue(KGlobal::config()->readNumEntry("Hour5",7));
	minute5->setValue(KGlobal::config()->readNumEntry("Minute5",0));
	hour6->setValue(KGlobal::config()->readNumEntry("Hour6",7));
	minute6->setValue(KGlobal::config()->readNumEntry("Minute6",0));
	hour7->setValue(KGlobal::config()->readNumEntry("Hour7",7));
	minute7->setValue(KGlobal::config()->readNumEntry("Minute7",0));
	
	QString volend(KGlobal::config()->readEntry("VolumeEnd","80%"));
	volend.truncate(volend.length()-1);
	
	volEndValue->setValue(volend.toInt());
	
	switch (KGlobal::config()->readNumEntry("ModeAlarm",0))
	{
		case 0:
			alarmmode->setChecked(true);
			break;
		case 1:
			morningmode->setChecked(true);
			break;
		case 2:
			nightmode->setChecked(true);
			break;
		default:
			alarmmode->setChecked(true);
	}
}

#include "wakeup.moc"
