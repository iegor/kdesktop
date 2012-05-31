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

// $Id: kfloatspinbox.h 465369 2005-09-29 14:33:08Z mueller $

#ifndef __kfloatspinbox_h_
#define __kfloatspinbox_h_

#include <qspinbox.h>

class KFloatSpinBox : public QSpinBox
{
	Q_OBJECT
	public:
		KFloatSpinBox( float minValue, float maxValue, float step, unsigned int precision, QWidget * parent = 0, const char * name = 0 );
		virtual ~KFloatSpinBox();

		void setRange( float minValue, float maxValue, float step, unsigned int precision );
		void setRangeBlocking( float minValue, float maxValue, float step, unsigned int precision );

		float value() const;

	public slots:
		virtual void setValue( float value );
		/**
		 * differs from the above in that it will block all signals
		 */
		virtual void setValueBlocking( float value );

	protected:
		virtual QString mapValueToText( int value );
		virtual int mapTextToValue( bool * ok );
		virtual void valueChange();

	signals:
		void valueChanged( float value );

	private slots:
		void slotValueChanged( int value );

	private:
		int m_factor;
		int m_min;
		int m_max;
		int m_step;
		bool m_doselection;
};

#endif
