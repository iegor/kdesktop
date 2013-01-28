/***************************************************************************
                          mcslider.cpp  -  description
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

#include <qpixmap.h>

#include "mcslider.h"

MCSlider::MCSlider( Orientation orientation, QWidget *parent, const char *name )
	 : QSlider( orientation, parent, name )
{
    setBackgroundOrigin(WidgetOrigin);
    setBackground();
}

MCSlider::~MCSlider()
{
}

// This is needed because KStyle draws slider background incorrectly.

void MCSlider::setBackground()
{
    unsetPalette();
    
    if (parentWidget()->paletteBackgroundPixmap())
    {
        QPixmap pm(width(), height());
        pm.fill(parentWidget(), pos());
        setPaletteBackgroundPixmap(pm);
    }
}

void MCSlider::wheelEvent(QWheelEvent *e)
{
	if (e->orientation() == Horizontal)
		return;

	if (e->state() == ShiftButton)
	{
		if (e->delta() > 0)
			emit volumeUp();
		else
			emit volumeDown();
		e->accept();
	}
	else
	{
		QSlider::wheelEvent(e);
	}
}

#include "mcslider.moc"
