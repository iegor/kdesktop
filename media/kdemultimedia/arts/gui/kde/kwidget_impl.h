    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de
                  2002 Matthias Kretz <kretz@kde.org>

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

#ifndef ARTS_GUI_KWIDGET_IMPL_H
#define ARTS_GUI_KWIDGET_IMPL_H
#include "artsgui.h"
#include <qwidget.h>
#include <kwidgetrepo.h>
#include <kdelibs_export.h>

namespace Arts {

class KWidgetGuard;
class KDE_EXPORT KWidget_impl : virtual public Arts::Widget_skel {
protected:
	QWidget * _qwidget;
	KWidgetGuard * _guard;
	long _parentID;
	long _widgetID;
public:
	KWidget_impl( QWidget * w = 0 );

	~KWidget_impl();

	long widgetID();
	Widget parent();
	void parent(Arts::Widget);
	long x();
	void x(long newX);
	long y();
	void y(long newY);
	long width();
	void width(long newWidth);
	long height();
	void height(long newHeight);

	bool visible();
	void visible(bool newVisible);

	SizePolicy hSizePolicy();
	void hSizePolicy( SizePolicy );
	SizePolicy vSizePolicy();
	void vSizePolicy( SizePolicy );

	void show();
	void hide();

	void widgetDestroyed();
};

class KWidgetGuard : public QObject {
	Q_OBJECT
protected:
	KWidget_impl *impl;

public:
	KWidgetGuard(KWidget_impl *impl) : impl(impl) { }

public slots:
	void widgetDestroyed() {
		impl->widgetDestroyed();
	}
};

}
#endif /* ARTS_GUI_KWIDGET_IMPL_H */

// vim: sw=4 ts=4
