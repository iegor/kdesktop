/* mainwidget.cpp
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
#include "mainwidget.moc"
#include "ui.h"

MainWidget::MainWidget(QWidget *parent, const char *name)
  : QTabWidget(parent, name)
{
	l=new liloconf();
	general=new General(l, this);
	connect(general, SIGNAL(configChanged()), SIGNAL(configChanged()));
	images=new Images(l, this);
	connect(images, SIGNAL(configChanged()), SIGNAL(configChanged()));
	expert=new Expert(l, this);
	connect(expert, SIGNAL(configChanged()), SIGNAL(configChanged()));
	connect(expert, SIGNAL(configChanged()), images, SLOT(update()));
	addTab(general, _("&General Options"));
	addTab(images, _("&Operating Systems"));
	addTab(expert, _("&Expert"));
	connect(this, SIGNAL(selected(const QString &)), SLOT(tabChanged(const QString &)));
	load();
	arrangeWidgets();
}

void MainWidget::makeReadOnly()
{
    general->makeReadOnly();
    images->makeReadOnly();
    expert->makeReadOnly();
}

void MainWidget::load()
{
	defaults();
}

void MainWidget::save()
{
	if(previous==_("&Expert"))
		expert->saveChanges();
	else {
		general->saveChanges();
		images->saveChanges();
	}
	l->checked=false;
	if(!l->isOk()) {
		if(WarningYesNo(this, "About to write a faulty lilo.conf", "WARNING: the config file is currently NOT ok.\nDo you really want to override /etc/lilo.conf?\n\nIf you aren't sure, select \"no\" and click the \"Check configuration\" button to see the details.\nIf you don't know what's wrong, try clicking the \"Probe\" button to auto-generate a working lilo.conf.\nIf you're getting this message after using the \"Probe\" button, please send a full bug report,\nincluding the output of \"Check configuration\" and the generated lilo.conf (displayed in the \"Expert\" tab),\nto bero@kde.org.")!=IDYES)
			return;
	}
	l->writeFile("/etc/lilo.conf");
	l->install();
}

void MainWidget::defaults()
{
}

void MainWidget::reset()
{
	load();
}

void MainWidget::arrangeWidgets()
{
}
void MainWidget::tabChanged(QString const &lbl) // SLOT
{
	if(previous==_("&Expert"))
		expert->saveChanges();
	else if(previous==_("&General options"))
		general->saveChanges();
	else if(previous==_("&Operating systems"))
		images->saveChanges();
	if(lbl==_("&Expert"))
		expert->update();
	else if(lbl==_("&Operating systems"))
		images->update();
	else if(lbl==_("&General options"))
		general->update();
	previous=lbl;
}
