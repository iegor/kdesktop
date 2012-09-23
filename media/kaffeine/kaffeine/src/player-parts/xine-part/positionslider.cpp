/*
 * positionslider.cpp
 *
 * Copyright (C) 2004-2005 Giorgos Gousios
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2004-2005 Miguel Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <math.h>

#include "positionslider.h"
#include "positionslider.moc"


PositionSlider::PositionSlider(Qt::Orientation o, QWidget *parent, const char* name) :
           QSlider(o, parent, name), m_userChange(false)
{
	connect(this, SIGNAL(sliderPressed()), this ,SLOT(slotSliderPressed()));
	connect(this, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
	installEventFilter(this);
}



PositionSlider::~PositionSlider()
{
}



void PositionSlider::setPosition(int val, bool changePosition)
{
	if(!m_userChange)
		setValue(val);
	if(changePosition) {
		setValue(val);
		emit sliderMoved(val);
	}
}



void PositionSlider::slotSliderPressed()
{
	m_userChange = true;
	emit signalStartSeeking();
}



void PositionSlider::slotSliderReleased()
{
	emit sliderLastMove(this->value());
	emit signalStopSeeking();
	m_userChange = false;
}



void PositionSlider::wheelEvent(QWheelEvent* e)
{
	float offset = log10( QABS(e->delta()) ) / 0.002;
	int newVal = 0;
	if (e->delta()>0)
		newVal = value() - int(offset);
	else
		newVal = value() + int(offset);
	if (newVal < 0) newVal = 0;
		//setPosition(newVal, true);
		emit sliderLastMove( newVal );
	e->accept();
}



bool PositionSlider::eventFilter(QObject *obj, QEvent *ev)
{
	if( obj == this && (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick) ) {
		QMouseEvent *e = (QMouseEvent *)ev;
		QRect r = sliderRect();

		if( r.contains( e->pos() ) || e->button() != LeftButton )
			return false;

		int range = maxValue() - minValue();
		int pos = (orientation() == Horizontal) ? e->pos().x() : e->pos().y();
		int maxpos = (orientation() == Horizontal) ? width() : height();
		int value = pos * range / maxpos + minValue();

		if (QApplication::reverseLayout())
			value = maxValue() - (value - minValue());

		setPosition(value, true);
		return true;
	}
	else {
		return false;
	}
}
