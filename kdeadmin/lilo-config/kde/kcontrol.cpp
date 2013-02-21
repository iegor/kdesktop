/* kcontrol.cpp
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
#include "kcontrol.moc"
#include <mainwidget.h>
#include <ui.h>
#include <kglobal.h>
#include <klocale.h>
#include <qlayout.h>
#include <kaboutdata.h>
#include <unistd.h>

KControl::KControl(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
	QVBoxLayout *layout=new QVBoxLayout(this);
	m=new MainWidget(this, name);
	layout->addWidget(m);
	connect(m, SIGNAL(configChanged()), SLOT(configChanged()));
        if (getuid() != 0) {
            m->makeReadOnly();
        }
	KAboutData *about = new KAboutData(I18N_NOOP("kcmlilo"), I18N_NOOP("LILO Configuration"),
	0, 0, KAboutData::License_GPL,
	I18N_NOOP("(c) 2000, Bernhard Rosenkraenzer"));
	about->addAuthor("Bernhard \"Bero\" Rosenkraenzer", 0, "bero@redhat.com");
	setAboutData(about);
}

void KControl::load()
{
	m->load();
}

void KControl::save()
{
	m->save();
}

void KControl::defaults()
{
	m->defaults();
}

void KControl::reset()
{
	m->reset();
}

void KControl::configChanged() // SLOT
{
	emit changed(true);
}

extern "C"
{
  KDE_EXPORT KCModule *create_lilo(QWidget *parent, const char *name)
  {
    return new KControl(parent, "kcmlilo");
  }
}

