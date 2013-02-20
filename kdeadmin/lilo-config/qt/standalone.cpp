/* standalone.cpp
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

#include <qapplication.h>
#include <qobject.h>
#include <qwhatsthis.h>
#include <qwindowsstyle.h>
#include <mainwidget.h>
#include <ui.h>
#include "standalone.moc"

Standalone::Standalone(QWidget *parent, const char *name):QWidget(parent,name)
{
	m=new MainWidget(this);
	connect(m, SIGNAL(configChanged()), SLOT(configChanged()));
	actions=new QHButtonGroup(this);
	_whatsthis=new QPushButton(_("&What's This?"), actions);
	_whatsthis->setAccel(SHIFT+Key_F1);
	QWhatsThis::add(_whatsthis, _("The <i>What's This?</i> button is part of this program's help system. Click on the What's This? button then on any widget in the window to get information (like this) on it."));
	connect(_whatsthis, SIGNAL(clicked()), SLOT(whatsthis()));
	_help=new QPushButton(_("&Help"), actions);
	_help->setAccel(Key_F1);
	QWhatsThis::add(_help, _("This button calls up the program's online help system. If it does nothing, no help file has been written (yet); in that case, use the <i>What's This</i> button on the left."));
	connect(_help, SIGNAL(clicked()), this, SLOT(help()));
	_deflt=new QPushButton(_("&Default"), actions);
	QWhatsThis::add(_deflt, _("This button resets all parameters to some (hopefully sane) default values."));	
	connect(_deflt, SIGNAL(clicked()), this, SLOT(defaults()));
	_reset=new QPushButton(_("&Reset"), actions);
	QWhatsThis::add(_reset, _("This button resets all parameters to what they were before you started the program."));
	connect(_reset, SIGNAL(clicked()), this, SLOT(reset()));
	_apply=new QPushButton(_("&Apply"), actions);
	QWhatsThis::add(_apply, _("This button saves all your changes without exiting."));
	connect(_apply, SIGNAL(clicked()), this, SLOT(apply()));
	_ok=new QPushButton(_("&OK"), actions);
	QWhatsThis::add(_ok, _("This button saves all your changes and exits the program."));
	connect(_ok, SIGNAL(clicked()), this, SLOT(ok()));
	_cancel=new QPushButton(_("&Cancel"), actions);
	QWhatsThis::add(_cancel, _("This button exits the program without saving your changes."));
	connect(_cancel, SIGNAL(clicked()), this, SLOT(cancel()));
	_apply->setEnabled(false);
	setMinimumWidth(actions->sizeHint().width()+10);
	arrangeWidgets();
}

void Standalone::arrangeWidgets()
{
	m->setGeometry(SPACE_MARGIN, SPACE_MARGIN, width()-2*SPACE_MARGIN, height()-actions->sizeHint().height()-SPACE_MARGIN-SPACE_INSIDE);
	actions->setGeometry(SPACE_MARGIN, height()-actions->sizeHint().height()-SPACE_MARGIN, width()-2*SPACE_MARGIN, actions->sizeHint().height());
}

void Standalone::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	arrangeWidgets();
}

void Standalone::whatsthis()
{
	QWhatsThis::enterWhatsThisMode();
}
void Standalone::help()
{
}
void Standalone::defaults()
{
	m->defaults();
}
void Standalone::reset()
{
	m->reset();
}
void Standalone::apply()
{
	m->save();
}
void Standalone::ok()
{
	m->save();
	emit done();
}
void Standalone::cancel()
{
	emit done();
}

void Standalone::configChanged() // SLOT
{
	_apply->setEnabled(true);
}

int main(int argc, char **argv) {
	QApplication a(argc, argv);
	Standalone *s=new Standalone(0);
	int ret;
	a.setStyle(new QWindowsStyle());
	a.setMainWidget(s);
	QObject::connect(s, SIGNAL(done()), &a, SLOT(quit()));
	s->show();
	ret=a.exec();
	delete s;
	return ret;
}
