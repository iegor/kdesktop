    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de
                  2001, 2002 Matthias Kretz <kretz@kde.org>

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

#include "kpoti_impl.h"
#include "kpoti_impl.moc"
#include "anyref.h"
#include "stdio.h"

#include <math.h>

using namespace Arts;
using namespace std;

PotiIntMapper::PotiIntMapper(KPoti_impl *impl, KPoti *kp)
	: QObject( kp )
	, impl( impl )
{
	connect(kp, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}

void PotiIntMapper::valueChanged(int pos)
{
	impl->valueChanged(pos);
}

KPoti_impl::KPoti_impl( KPoti * widget )
	: KFrame_impl( widget ? widget : new KPoti( 0, 100, 1, 0 ) )
{
	_min = 0; _max = 1; _value = 0;
	_factor = 1;
	_logarithmic = 0;
	_range = 100;

	_kpoti = static_cast<KPoti*>( _qwidget );
	( void )new PotiIntMapper( this, _kpoti );
}

string KPoti_impl::caption()
{
	return _kpoti->text().utf8().data();
}

void KPoti_impl::caption(const string& newText)
{
	_caption = QString::fromUtf8( newText.c_str() );
	_kpoti->setText( _caption );
}

string KPoti_impl::color()
{
	return _color;
}

void KPoti_impl::color(const string& newColor)
{
	_color = newColor;
    if(strlen(_color.c_str()))
    {
        QColor qc(_color.c_str());
        _kpoti->setColor(qc);
    }
}

float KPoti_impl::min()
{
	return _min;
}

void KPoti_impl::min(float newMin)
{
	if(_min != newMin)
	{
		_min = newMin;
		applyValue();
	}
}

float KPoti_impl::max()
{
	return _max;
}

void KPoti_impl::max(float newMax)
{
	if(_max != newMax)
	{
		_max = newMax;
		applyValue();
	}
}

float KPoti_impl::value()
{
	float ret = float(_kpoti->value()) / _factor;
	if(_logarithmic > 0)
		ret = convertFromLog(ret);
	if(ret < _min)
		ret = _min;
	else if(ret > _max)
		ret = _max;
	return ret;
}

void KPoti_impl::value(float newValue)
{
	if(newValue != _value)
	{
		_value = newValue;
		applyValue();
		if(visible())
			value_changed(value());
	}
}

long KPoti_impl::range()
{
	return _range;
}

void KPoti_impl::range(long newRange)
{
	if(_range != newRange)
	{
		_range = newRange;
		applyValue();
	}
}

void KPoti_impl::valueChanged(int newvalue)
{
	_value = (float)newvalue / _factor;
	if(_logarithmic > 0)
		_value = convertFromLog(_value);
	if(visible())
		value_changed(value());
}

float KPoti_impl::convertToLog(float val)
{
	return log(val) / log(_logarithmic);
}

float KPoti_impl::convertFromLog(float val)
{
	return pow(_logarithmic, val);
}

void KPoti_impl::applyValue()
{
	double dmin = _min;
	double dmax = _max;
	double dvalue = _value;
	if(_logarithmic > 0)
	{
		dmin = convertToLog(_min);
		dmax = convertToLog(_max);
		dvalue = convertToLog(_value);
	}
	_factor = _range / (dmax - dmin);
	int imin = int(_factor * dmin);
	int imax = int(_factor * dmax);
	int ivalue = int(_factor * dvalue);
	_kpoti->setRange(imin, imax);
	_kpoti->setValue(ivalue);
}

void KPoti_impl::logarithmic(float newLogarithmic)
{
	if(_logarithmic != newLogarithmic)
	{
		_logarithmic = newLogarithmic;
		applyValue();
	}
}

float KPoti_impl::logarithmic()
{
	return _logarithmic;
}

REGISTER_IMPLEMENTATION(KPoti_impl);

// vim: sw=4 ts=4
