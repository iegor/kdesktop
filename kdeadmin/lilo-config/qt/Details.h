/* Details.h
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
#ifndef _DETAILS_H_
#define _DETAILS_H_ 1
#include <lilo.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
class Details:public QDialog
{
	Q_OBJECT
public:
	Details(liloimage *lilo, QWidget *parent=0, const char *name=0, WFlags f=0);
	bool isReadOnly() const { return readonly->isChecked(); };
	QString vgaMode() const;
	bool isUnsafe() const { return unsafe->isChecked(); };
	bool isLocked() const { return lock->isChecked(); };
	bool isRestricted() const { return restricted->isChecked(); };
	bool usePassword() const { return use_password->isChecked(); };
	QString Password() const { return password->text(); };
private slots:
	void check_pw();
private:
	liloimage	*l;
	QCheckBox	*readonly;
	QComboBox	*vga;
	QCheckBox	*unsafe;
	QCheckBox	*lock;
	QCheckBox	*restricted;
	QCheckBox	*use_password;
	QLineEdit	*password;
	QPushButton	*ok;
	QPushButton	*cancel;
};
#endif
