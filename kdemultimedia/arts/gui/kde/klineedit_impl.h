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

#ifndef klineedit_impl_h
#define klineedit_impl_h

#include "kwidget_impl.h"
#include <klineedit.h>


namespace Arts {

class KLineEdit_impl : virtual public Arts::LineEdit_skel,
                       public Arts::KWidget_impl
{
protected:
	KLineEdit * _klineedit;

public:
	KLineEdit_impl( KLineEdit * w = 0 );
	void constructor( Widget p ) { parent( p ); }

	std::string caption();
	void caption(const std::string& newCaption);
	std::string text();
	void text(const std::string& newText);
	void textChanged(const std::string& newText);
};

class KLineEditStringMapper :public QObject {
	Q_OBJECT
	KLineEdit_impl *impl;
public:
	KLineEditStringMapper(KLineEdit_impl *impl, KLineEdit *ed);
public slots:
	void textChanged(const QString& newText);
};

}

#endif

// vim: sw=4 ts=4
