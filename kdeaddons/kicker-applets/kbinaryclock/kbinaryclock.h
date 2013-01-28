/*
 * Copyright (C) 2003 Benjamin C Meyer (ben+kbinaryclock@meyerhome.net)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KBINARYCLOCK_H
#define KBINARYCLOCK_H

#include <kpanelapplet.h>
#include <qevent.h>
#include <kglobal.h>
#include <kaboutdata.h>
#include <qdatetime.h>
#include <qtooltip.h>
#include <kconfigdialog.h>

#include "settings.h"
#include "prefs.h"

class KLed;
class QGridLayout;
class DatePicker;
class QGridLayout;
class KBinaryClock;

class SettingsImp : public Settings {
	Q_OBJECT
public:
	SettingsImp(QWidget* parent=0,
				const char* name=0,
				WFlags fl=0);
public slots:
	void updatePreview();

};

class KConfigDialogImp : public KConfigDialog {
public:
	KConfigDialogImp(QWidget *parent, const char *name,
		KConfigSkeleton *prefs,
	KDialogBase::DialogType dialogType = KDialogBase::IconList,
	KDialogBase::ButtonCode defaultButton = Ok,
	bool modal=false);
	SettingsImp *settings;
};

class ClockAppletToolTip : public QToolTip
{
		public:
				ClockAppletToolTip( KBinaryClock* clock );

		protected:
				virtual void maybeTip( const QPoint & );

		private:
				KBinaryClock *m_clock;
};

class KBinaryClock : public KPanelApplet {
	Q_OBJECT
public:
	KBinaryClock(const QString& configFile, Type t = Normal, int actions = 0, QWidget *parent = 0, const char *name = 0);
	~KBinaryClock();

	virtual int	widthForHeight (int height) const;
	virtual int	heightForWidth (int width) const;

protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);

protected slots:
	void preferences();
	void updateClock();
	void loadSettings();
	void slotCopyMenuActivated(int);
	void slotCalendarDeleted();
	void slotEnableCalendar();

private:
	void openContextMenu();
	void toggleCalendar();
	KLed *ledMatrix[6][4];
	int ledWidth;
	DatePicker *_calendar;
	bool _disableCalendar;
	Prefs *prefs;
	ClockAppletToolTip m_tooltip;
};

#endif // KBINARYCLOCK_H

