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

#include "kfader_impl.h"
#include "kfader_impl.moc"
#include "anyref.h"
#include "stdio.h"

#include <math.h>

using namespace Arts;
using namespace std;

FaderIntMapper::FaderIntMapper(KFader_impl *impl, KFader *kp) :impl(impl)
{
	connect(kp, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}

void FaderIntMapper::valueChanged(int pos)
{
	impl->valueChanged(pos);
}

KFader_impl::KFader_impl( KFader * widget )
	: KWidget_impl( widget ? widget : new KFader( 0, 100, 1, 0 ) )
{
	_min = 0; _max = 1; _value = 0;
	_factor = 1;
	_logarithmic = 0;
	_range = 100;

	_kfader = static_cast<KFader*>( _qwidget );
	_kfader->setMinimumWidth( 40 );
	_kfader->setMinimumHeight( 100 );
	new FaderIntMapper( this, _kfader );
}

string KFader_impl::caption()
{
	return _caption.utf8().data();
}

void KFader_impl::caption(const string& newText)
{
	_caption = QString::fromUtf8(newText.c_str());
	_kfader->setName(_caption.utf8().data());
}

string KFader_impl::color()
{
	return _color;
}

void KFader_impl::color(const string& newColor)
{
	_color = newColor;
    if(strlen(_color.c_str()))
    {
        QColor qc(_color.c_str());
        _kfader->setColor(qc);
    }
}

float KFader_impl::min()
{
	return _min;
}

void KFader_impl::min(float newMin)
{
	if(_min != newMin)
	{
		_min = newMin;
		applyValue();
	}
}

float KFader_impl::max()
{
	return _max;
}

void KFader_impl::max(float newMax)
{
	if(_max != newMax)
	{
		_max = newMax;
		applyValue();
	}
}

float KFader_impl::value()
{
	//float ret = ( _max + _min - float(_kfader->value()) ) / _factor;
	//if(_logarithmic > 0)
		//ret = convertFromLog(ret);
	float ret = _value;
	if(ret < _min)
		ret = _min;
	else if(ret > _max)
		ret = _max;
	return ret;
}

void KFader_impl::value(float newValue)
{
	if(newValue != _value)
	{
		_value = newValue;
		applyValue();
		if(visible())
			value_changed(value());
	}
}

long KFader_impl::range()
{
	return _range;
}

void KFader_impl::range(long newRange)
{
	if(_range != newRange)
	{
		_range = newRange;
		applyValue();
	}
}

void KFader_impl::valueChanged(int newvalue)
{
	_value = float(newvalue) / _factor;
	if(_logarithmic > 0)
		_value = convertFromLog(_value);
	_value = _max + _min - _value;
	if(visible())
		value_changed(value());
}

float KFader_impl::convertToLog(float val)
{
	return log(val) / log(_logarithmic);
}

float KFader_impl::convertFromLog(float val)
{
	return pow(_logarithmic, val);
}

void KFader_impl::applyValue()
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
	int ivalue = int(_factor * (dmax + dmin - dvalue));
	_kfader->setRange(imin, imax);
	_kfader->setValue(ivalue);
}

void KFader_impl::logarithmic(float newLogarithmic)
{
	if(_logarithmic != newLogarithmic)
	{
		_logarithmic = newLogarithmic;
		applyValue();
	}
}

float KFader_impl::logarithmic()
{
	return _logarithmic;
}

REGISTER_IMPLEMENTATION(KFader_impl);

// vim: sw=4 ts=4
