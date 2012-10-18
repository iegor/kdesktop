    /*

    Copyright (C) 2000       Stefan Westerfeld
                             stefan@space.twc.de
                  2001, 2002 Matthias Kretz
                             kretz@kde.org

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

#ifndef ARTS_GUI_KBUTTON_IMPL_H
#define ARTS_GUI_KBUTTON_IMPL_H
#include "kwidget_impl.h"
#include <qpushbutton.h>


namespace Arts {
class KButton_impl;
class KButtonMapper : public QObject {
	Q_OBJECT
	KButton_impl *impl;
	QPushButton * button;
public:
	KButtonMapper(KButton_impl *impl, QPushButton *but);
protected slots:
	void pressed();
	void released();
	void toggled( bool );
	void clicked();
};

class KButton_impl : virtual public Arts::Button_skel,
                     public Arts::KWidget_impl
{
private:
	bool _clicked;

protected:
	QPushButton * _qpushbutton;

public:
	KButton_impl( QPushButton * w = 0 );
	void constructor( Widget parent );
	void constructor( const std::string &, Widget );

	void emitClicked();

	std::string text();
	void text(const std::string& newCaption);

	bool toggle();
	void toggle(bool);

	bool pressed();
	bool clicked();
	void changeState(bool);
};

}
#endif //ARTS_GUI_KBUTTON_IMPL_H

// vim:sw=4:ts=4
