/* InputBox.cpp
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

#include "InputBox.moc"
#include <qlayout.h>
#include <qhbox.h>
#include <qwhatsthis.h>
#include <ui.h>

InputBox::InputBox(entries e, QWidget *parent, const char *name, bool hasCancel, WFlags f):QDialog(parent, name, true, f)
{
	QVBoxLayout *layout=new QVBoxLayout(this);
	layout->setMargin(SPACE_MARGIN);
	layout->setSpacing(SPACE_INSIDE);
	for(entries::iterator it=e.begin(); it!=e.end(); it++) {
		EditWidget *ed=new EditWidget((*it).label, (*it).dflt, (*it).isFile, this);
		QWhatsThis::add(ed, (*it).help);
		layout->addWidget(ed);
		edit.insert(edit.end(), ed);
	}
	if(hasCancel) {
		QHBox *btns=new QHBox(this);
		ok=new QPushButton(_("&OK"), btns);
		cancel=new QPushButton(_("&Cancel"), btns);
		layout->addWidget(btns);
		connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	} else {
		ok=new QPushButton(_("&OK"), this);
		layout->addWidget(ok);
	}
	connect(ok, SIGNAL(clicked()), SLOT(accept()));
}
