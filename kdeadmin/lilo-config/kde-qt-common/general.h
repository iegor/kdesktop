/* general.h
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
#ifndef _GENERAL_H_
#define _GENERAL_H_ 1
#include <lilo.h>
#include <qwidget.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
class General:public QWidget
{
	Q_OBJECT
public:
	General(liloconf *l=0, QWidget *parent=0, const char *name=0);
    void makeReadOnly();
signals:
	void configChanged();
public slots:
	void saveChanges();
	void update();
private slots:
	void check_pw();
private:
	liloconf	*lilo;
	QComboBox	*drive;
	QSpinBox	*timeout;
	QCheckBox	*linear;
	QCheckBox	*compact;
	QCheckBox	*lock;
	QCheckBox	*restricted;
	QCheckBox	*use_password;
	QLineEdit	*password;
	QComboBox	*vga;
	QCheckBox	*prompt;
};
#endif
