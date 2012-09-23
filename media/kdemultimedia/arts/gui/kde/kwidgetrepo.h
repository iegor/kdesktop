    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#ifndef KWIDGETREPO_H
#define KWIDGETREPO_H

#include <map>
#include <qwidget.h>
#include <weakreference.h>
#include "artsgui.h"

namespace Arts {
	class KWidget_impl;
}

class KWidgetRepo {
protected:
	long nextID;
	std::map<long, QWidget *> qwidgets;
	std::map<long, Arts::KWidget_impl *> widgets;
	static KWidgetRepo *instance;

	KWidgetRepo();
	~KWidgetRepo();

public:

	long add(Arts::KWidget_impl *widget, QWidget *qwidget);
	Arts::Widget lookupWidget(long ID);
	QWidget *lookupQWidget(long ID);
	void remove(long ID);

	static KWidgetRepo *the();
	static void shutdown();
};

#endif /* KWIDGETREPO_H */
