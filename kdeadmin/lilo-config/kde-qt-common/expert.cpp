/* expert.cpp
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
#include "expert.moc"
#include <ui.h>
#include <qwhatsthis.h>
#include <String.h>
#include <strstream>
Expert::Expert(liloconf *l, QWidget *parent, const char *name):QWidget(parent, name)
{
	lilo=l;
	layout=new QHBoxLayout(this);
	edit=new QMultiLineEdit(this);
	layout->addWidget(edit);
	connect(edit, SIGNAL(textChanged()), SIGNAL(configChanged()));
	QWhatsThis::add(edit, _("You can edit the lilo.conf file directly here. All changes you make here are automatically transferred to the graphical interface."));
	update();
}
Expert::~Expert()
{
	delete edit;
}
void Expert::update()
{
	bool blocked = signalsBlocked();
	blockSignals(true);
	edit->setText(((String)*lilo).cstr());
	blockSignals(blocked);
}
void Expert::saveChanges()
{
	lilo->set(edit->text().latin1());
}

void Expert::makeReadOnly()
{
    edit->setEnabled( false );
}
