/* InputBox.cpp
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
#include "InputBox.moc"
#include <qvbox.h>
#include <qwhatsthis.h>
#include <ui.h>

InputBox::InputBox(entries e, QWidget *parent, const char *name, bool hasCancel, WFlags f)
  : KDialogBase(parent, name, true, QString::null, hasCancel ? Ok | Cancel : Ok, Ok, true )
{
	QVBox *page = makeVBoxMainWidget();
	for(entries::iterator it=e.begin(); it!=e.end(); it++) {
		EditWidget *ed=new EditWidget((*it).label, (*it).dflt, (*it).isFile, page);
		QWhatsThis::add(ed, (*it).help);
		edit.insert(edit.end(), ed);
	}
}
