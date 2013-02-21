/* standalone.h
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/

#ifndef _STANDALONE_H_
#define _STANDALONE_H_ 1
#include <qwidget.h>
#include <qpushbutton.h>
#include <qhbuttongroup.h>
#include <qevent.h>
#include "mainwidget.h"
class Standalone: public QWidget
{
	Q_OBJECT
public:
	Standalone(QWidget *parent=0, const char *name=0);
	void resizeEvent(QResizeEvent *e);
public slots:
	void whatsthis();
	void help();
	void defaults();
	void reset();
	void apply();
	void ok();
	void cancel();
	void configChanged();
signals:
	void done();
private:
	void arrangeWidgets();
	MainWidget *m;
	QHButtonGroup *actions;
	QPushButton *_whatsthis;
	QPushButton *_help;
	QPushButton *_deflt;
	QPushButton *_reset;
	QPushButton *_apply;
	QPushButton *_ok;
	QPushButton *_cancel;
};
#endif
