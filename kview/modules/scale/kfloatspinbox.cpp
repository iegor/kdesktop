/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

// $Id: kfloatspinbox.cpp 465369 2005-09-29 14:33:08Z mueller $

#include "kfloatspinbox.h"

#if defined(QT_ACCESSIBILITY_SUPPORT)
#include <qaccessible.h>
#endif

#include <knumvalidator.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>

int pow( int a, int b )
{
	int ret = 1;
	for( int i = 0; i < b; ++i )
		ret *= a;
	return ret;
}

KFloatSpinBox::KFloatSpinBox( float minValue, float maxValue, float step, unsigned int precision, QWidget * parent, const char * name )
	: QSpinBox( parent, name )
	, m_doselection( true )
{
	setRange( minValue, maxValue, step, precision );
	connect( this, SIGNAL( valueChanged( int ) ), this, SLOT( slotValueChanged( int ) ) );
}

KFloatSpinBox::~KFloatSpinBox()
{
}

void KFloatSpinBox::setRange( float minValue, float maxValue, float step, unsigned int precision )
{
	m_factor = pow( 10, precision );
	m_min = (int)( minValue * m_factor );
	m_max = (int)( maxValue * m_factor );
	m_step = (int)( step * m_factor );
	QSpinBox::setRange( m_min, m_max );
	setSteps( m_step, m_step * 10 );
	if( precision == 0 )
		setValidator( new KIntValidator( m_min, m_max, this, 10, "KFloatValidator::KIntValidator" ) );
	else
		setValidator( new KFloatValidator( minValue, maxValue, true, this, "KFloatSpinBox::KFloatValidator" ) );
}

float KFloatSpinBox::value() const
{
	float ret = (float)QSpinBox::value() / m_factor;
	kdDebug( 4630 ) << ret << endl;
	return ret;
}

void KFloatSpinBox::setValue( float value )
{
	QSpinBox::setValue( (int)( value * m_factor ) );
}

void KFloatSpinBox::setValueBlocking( float value )
{
	m_doselection = false;
	blockSignals( true );
	KFloatSpinBox::setValue( value );
	blockSignals( false );
	m_doselection = true;
}

QString KFloatSpinBox::mapValueToText( int value )
{
	return KGlobal::locale()->formatNumber( (float)value / (float)m_factor, 4 );
}

int KFloatSpinBox::mapTextToValue( bool * ok )
{
	return (int)( m_factor * KGlobal::locale()->readNumber( text(), ok ) );
}

void KFloatSpinBox::valueChange()
{
	if( m_doselection )
		QSpinBox::valueChange();
	else
	{
		updateDisplay();
		emit valueChanged( value() );
#if defined(QT_ACCESSIBILITY_SUPPORT)
		QAccessible::updateAccessibility( this, 0, QAccessible::ValueChanged );
#endif
	}
}

void KFloatSpinBox::slotValueChanged( int )
{
	emit valueChanged( value() );
}

#include "kfloatspinbox.moc"
