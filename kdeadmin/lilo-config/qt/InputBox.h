/* InputBox.h
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

#ifndef _INPUTBOX_H_
#define _INPUTBOX_H_ 1
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include "EditWidget.h"
#include <list>
class InputBox:public QDialog
{
	Q_OBJECT
public:
	typedef struct { QString label; QString dflt; bool isFile; QString help; } entry;
	typedef std::list<entry> entries;
	InputBox(entries e, QWidget *parent=0, const char *name=0, bool hasCancel=true, WFlags f=0);
	QStringList const text() const { QStringList s; for(std::list<EditWidget*>::const_iterator it=edit.begin(); it!=edit.end(); it++) s << (*it)->text(); return s; };
private:
	std::list<EditWidget*> edit;
	QPushButton *ok;
	QPushButton *cancel;
};
#endif
