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

#include "kwidget_impl.h"
#include "kwidgetrepo.h"
#include "debug.h"
#include <stdio.h>

using namespace Arts;

KWidget_impl::KWidget_impl( QWidget * widget )
	: _qwidget( widget ? widget : new QWidget )
{
	_widgetID = KWidgetRepo::the()->add( this, _qwidget );

	/*
	 * KWidgetGuard will protect us against deleting the widget if Qt already
	 * has done so (for instance if our widget was inside a panel, and
	 * the panel got deleted, our widget will be gone, too)
	 */
	_guard = new KWidgetGuard(this);
	QObject::connect(_qwidget, SIGNAL(destroyed()),
			_guard, SLOT(widgetDestroyed()));
}

KWidget_impl::~KWidget_impl()
{
	if(_qwidget)
	{
		delete _qwidget;
		arts_assert(_qwidget == 0);	// should be true due to KWidgetGuard
	}
	delete _guard;
}

void KWidget_impl::widgetDestroyed()
{
	KWidgetRepo::the()->remove(_widgetID);
	_widgetID = 0;
	_qwidget = 0;
}

long KWidget_impl::widgetID()
{
	return _widgetID;
}

Widget KWidget_impl::parent()
{
	return KWidgetRepo::the()->lookupWidget(_parentID);
}

void KWidget_impl::parent(Arts::Widget newParent)
{
	if(!newParent.isNull())
	{
		_parentID = newParent.widgetID();

		QWidget *qparent;
		qparent = KWidgetRepo::the()->lookupQWidget(newParent.widgetID());
		if( qparent != 0 )
		{
			QPoint pos(x(),y());
			bool showIt = visible();
			_qwidget->reparent(qparent, pos, showIt);
		}
	}
	else
	{
		_parentID = 0;
	}
}

long KWidget_impl::x()
{
	return _qwidget->x();
}

void KWidget_impl::x(long newX)
{
	_qwidget->move(newX,y());
}

long KWidget_impl::y()
{
	return _qwidget->y();
}

void KWidget_impl::y(long newY)
{
	_qwidget->move(x(),newY);
}

long KWidget_impl::width()
{
	return _qwidget->width();
}

void KWidget_impl::width(long newWidth)
{
	_qwidget->resize(newWidth,height());
}

long KWidget_impl::height()
{
	return _qwidget->height();
}

void KWidget_impl::height(long newHeight)
{
	_qwidget->resize(width(),newHeight);
}

bool KWidget_impl::visible()
{
	return _qwidget->isVisible();
}

void KWidget_impl::visible(bool newVisible)
{
	if(newVisible) show(); else hide();
}

SizePolicy KWidget_impl::hSizePolicy()
{
	return ( SizePolicy )_qwidget->sizePolicy().horData();
}

void KWidget_impl::hSizePolicy( SizePolicy p )
{
	QSizePolicy sp = _qwidget->sizePolicy();
	sp.setHorData( ( QSizePolicy::SizeType )p );
	_qwidget->setSizePolicy( sp );
}

SizePolicy KWidget_impl::vSizePolicy()
{
	return ( SizePolicy )_qwidget->sizePolicy().verData();
}

void KWidget_impl::vSizePolicy( SizePolicy p )
{
	QSizePolicy sp = _qwidget->sizePolicy();
	sp.setVerData( ( QSizePolicy::SizeType )p );
	_qwidget->setSizePolicy( sp );
}

void KWidget_impl::show()
{
	_qwidget->show();
}

void KWidget_impl::hide()
{
	_qwidget->hide();
}

REGISTER_IMPLEMENTATION(KWidget_impl);
#include "kwidget_impl.moc"

// vim: sw=4 ts=4
