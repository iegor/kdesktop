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

#include "kspinbox_impl.h"
#include "kspinbox_impl.moc"
#include "anyref.h"
#include "stdio.h"

using namespace Arts;
using namespace std;

SpinBoxIntMapper::SpinBoxIntMapper(KSpinBox_impl *impl, QSpinBox *sp)
	:impl(impl)
{
	connect(sp, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}

void SpinBoxIntMapper::valueChanged(int pos)
{
	impl->valueChanged(pos);
}

KSpinBox_impl::KSpinBox_impl( QSpinBox * widget )
	: KWidget_impl( widget ? widget : new QSpinBox )
{
	_min = 0; _max = 100; _value = 0;
	_qspinbox = static_cast<QSpinBox*>( _qwidget );
	_qspinbox->setRange( 0, 100 );
	( void )new SpinBoxIntMapper( this, _qspinbox );
}

string KSpinBox_impl::caption()
{
	return _caption.utf8().data();
}

void KSpinBox_impl::caption(const string& newCaption)
{
	_caption = QString::fromUtf8(newCaption.c_str());
	// FIXME: do something with the caption here
}

long KSpinBox_impl::min()
{
	return _min;
}

void KSpinBox_impl::min(long newMin)
{
	_min = newMin;
	_qspinbox->setMinValue(newMin);
}

long KSpinBox_impl::max()
{
	return _max;
}

void KSpinBox_impl::max(long newMax)
{
	_max = newMax;
	_qspinbox->setMaxValue(newMax);
}

long KSpinBox_impl::value()
{
	return _value;
}

void KSpinBox_impl::value(long newValue)
{
	if(newValue != _value)
	{
		_value = newValue;
		_qspinbox->setValue(newValue);
		if(visible())
			value_changed(newValue);
	}
}

void KSpinBox_impl::valueChanged(int newvalue)
{
	value(newvalue);
}

REGISTER_IMPLEMENTATION(KSpinBox_impl);

// vim: sw=4 ts=4
