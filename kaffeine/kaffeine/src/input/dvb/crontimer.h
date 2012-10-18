/*
 * crontimer.h
 *
 * Copyright (C) 2005 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef CRONTIMER_H
#define CRONTIMER_H

#include <qwidget.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

#include "crontimerui.h"

class CronTimer : public CronTimerUI
{

	Q_OBJECT

public:

	CronTimer( int m, QWidget *parent );
	~CronTimer();
	int getMode() const;

	enum Type { Noone=0, Daily=1, Weekly=2, Monthly=4, Custom=8, Monday=16, Tuesday=32, Wednesday=64, Thursday=128, Friday=256, Saturday=512, Sunday=1024 };

protected:

	virtual void accept();

private slots:

	void modeSelected( int id );

private:

	int mode;

};

#endif /* CRONTIMER_H */
