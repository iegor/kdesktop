    /*

    Copyright (C) 2000 Stefan Westerfeld
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

#ifndef ARTS_GUI_KFADER_IMPL_H
#define ARTS_GUI_KFADER_IMPL_H
#include "kwidget_impl.h"
#include "kfader.h"

#include <qobject.h>
#include <qstring.h>


namespace Arts {

class KFader_impl;

class FaderIntMapper :public QObject {
	Q_OBJECT
	KFader_impl *impl;
public:
	FaderIntMapper(KFader_impl *impl, KFader *kp);
public slots:
	void valueChanged(int x);
};

class KFader_impl : virtual public Arts::Fader_skel,
                    public Arts::KWidget_impl
{
protected:
	KFader * _kfader;
	QString _caption;
	std::string _color;
	float _min, _max, _value;
	float _factor;
	float _logarithmic;
	long _range;

	float convertToLog(float);
	float convertFromLog(float);
	void applyValue();

public:
	KFader_impl( KFader * w = 0 );
	void constructor( Widget p ) { parent( p ); }

	std::string caption();
	void caption(const std::string& newText);
	std::string color();
	void color(const std::string& newColor);

	float logarithmic();
	void logarithmic(float);

	float min();
	void min(float newMin);
	float max();
	void max(float newMax);
	float value();
	void value(float newValue);

	long range();
	void range(long newRange);

	/* from qt */
	void valueChanged(int newValue);
};

}
#endif /* ARTS_GUI_KFADER_IMPL_H */

// vim: sw=4 ts=4
