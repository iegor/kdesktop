    /*

    Copyright (C) 2001 Stefan Westerfeld
                       stefan@space.twc.de
                  2003 Arnold Krille <arnold@arnoldarts.de>

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

#include "kartswidget.h"
#include "kwidgetrepo.h"
#include "debug.h"
#include "qlayout.h"

class KArtsWidgetPrivate {
public:
	QHBoxLayout *layout;
};

KArtsWidget::KArtsWidget( QWidget* parent, const char* name )
	:QWidget( parent, name ), _content( Arts::Widget::null() )
{
	d = new KArtsWidgetPrivate;
	d->layout = new QHBoxLayout(this);
}

KArtsWidget::KArtsWidget( Arts::Widget content, QWidget* parent, const char* name )
	:QWidget( parent, name ), _content( Arts::Widget::null())
{
	d = new KArtsWidgetPrivate;
	d->layout = new QHBoxLayout(this);
	setContent(content);
}

KArtsWidget::KArtsWidget( Arts::Widget content, QWidget* parent, const char* name, WFlags wflags )
	:QWidget( parent, name, wflags ), _content( Arts::Widget::null() )
{
	d = new KArtsWidgetPrivate;
	d->layout = new QHBoxLayout( this );
	setContent( content );
}

KArtsWidget::KArtsWidget(QWidget* parent, const char* name, WFlags wflags )
	:QWidget(parent, name, wflags ), _content(Arts::Widget::null())
{
	d = new KArtsWidgetPrivate;
	d->layout = new QHBoxLayout(this);
}

KArtsWidget::~KArtsWidget()
{
	QWidget *contentAsWidget
		= KWidgetRepo::the()->lookupQWidget(_content.widgetID());
	contentAsWidget->reparent(0,QPoint(0,0),_content.visible());
	delete d;
	d = 0;
}

/* TODO: change content (reparent old widget away, reparent new widget here) */
void KArtsWidget::setContent(Arts::Widget content)
{
	arts_return_if_fail(!content.isNull());
	QWidget *contentAsWidget
		= KWidgetRepo::the()->lookupQWidget(content.widgetID());
	arts_return_if_fail(contentAsWidget != 0);

	_content = content;
	contentAsWidget->reparent(this,QPoint(0,0),content.visible());
	d->layout->addWidget(contentAsWidget);
}

Arts::Widget KArtsWidget::content()
{
	return _content;
}
