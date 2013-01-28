/***************************************************************************
                          mcslider.h  -  description
                             -------------------
    begin                : 20040410
    copyright            : (C) 2004 by Teemu Rytilahti
    email                : teemu.rytilahti@kde-fi.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MCSLIDER_H
#define MCSLIDER_H

#include <qslider.h>

class MCSlider : public QSlider
{
	Q_OBJECT

	public:
		MCSlider( Orientation orientation, QWidget *parent, const char *name = 0 );
		~MCSlider();
		void setBackground();
	private:
		virtual void wheelEvent(QWheelEvent *e);
	signals:
		void volumeUp();
		void volumeDown();
};
#endif
