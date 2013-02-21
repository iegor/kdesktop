/* mainwidget.h
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
** Contributions by M. Laurent and W. Bastian.
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
#ifndef _MAINWIDGET_H_
#define _MAINWIDGET_H_ 1
#include <qwidget.h>
#include <qlistbox.h>
#include <qtabwidget.h>
#include <qevent.h>
#include "general.h"
#include "images.h"
#include "expert.h"

class MainWidget: public QTabWidget {
	Q_OBJECT
public:
	MainWidget(QWidget *parent, const char *name=0);
	void arrangeWidgets();
    void makeReadOnly();
public slots:
	void load();
	void save();
	void reset();
	void defaults();
	void tabChanged(const QString &lbl);
signals:
	void configChanged();
private:
	QString previous;
	General *general;
	Images	*images;
	Expert	*expert;
	liloconf *l;
};
#endif
