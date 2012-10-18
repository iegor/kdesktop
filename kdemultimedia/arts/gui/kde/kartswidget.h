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

#ifndef ARTS_GUI_KARTSWIDGET_H
#define ARTS_GUI_KARTSWIDGET_H

#include <qwidget.h>
#include "artsgui.h"
#include <kdelibs_export.h>
class KArtsWidgetPrivate;

/**
 * KArtsWidget provides a simple way to treat Arts::Widget classes like
 * native Qt widgets. Suppose you use Qt, and want to put an Arts::Widget
 * type into a layout, you can do so using this code
 *
 * <pre>
 *    Arts::Widget widget = ...get widget from somewhere...;
 *    KArtsWidget *w = new KArtsWidget(widget, this);
 *    layout->addWidget(w);
 * </pre>
 *
 * In line 2 of the code, the "this" is the parent widget (which is usually
 * this in Qt code).
 *
 * The KArtsWidget class keeps a reference to the content widget, so the
 * content widget will not be freed until the KArtsWidget gets destroyed.
 */
class KDE_EXPORT KArtsWidget : public QWidget {
private:
	KArtsWidgetPrivate *d;

protected:
	Arts::Widget _content;

public:
	/**
	 * creates a new KArtsWidget
	 */
	KArtsWidget( QWidget* parent, const char* name );

	/**
	 * creates a new KArtsWidget and sets the content to an Arts::Widget
	 */
	KArtsWidget( Arts::Widget content, QWidget* parent, const char* name );
	
	/**
	 * creates a new KArtsWidget with WidgetFlags and content
	 *
	 * BCI: should replace the above in the next major release. ( akrille )
	 */
	KArtsWidget( Arts::Widget, QWidget* =0, const char* =0, WFlags =0 );
	// same without the content
	KArtsWidget( QWidget* =0, const char* =0, WFlags =0 );

	/**
	 * destructor
	 */
	~KArtsWidget();

	/**
	 * sets the content to a new Arts::Widget
	 */
	void setContent(Arts::Widget content);

	/**
	 * gets the content widget
	 */
	Arts::Widget content();
};

#endif /* ARTS_GUI_KARTSWIDGET_H*/
