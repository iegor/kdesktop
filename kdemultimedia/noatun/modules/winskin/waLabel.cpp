/*
  Standard label for Winskin
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include <qpainter.h>
#include <qpixmap.h>

#include "waLabel.h"
#include "waSkinModel.h"

WaLabel::WaLabel(int mapping) : WaWidget(mapping)
{
    connect(WaSkinModel::instance(), SIGNAL(skinChanged()), 
	    this, SLOT(pixmapChange()));

    completePixmap = new QPixmap();

    QSize size = sizeHint();

    completePixmap->resize(size.width(), size.height());
}

WaLabel::~WaLabel()
{
    delete completePixmap;
}

void WaLabel::paintEvent(QPaintEvent *)
{
    bitBlt(this, 0, 0, completePixmap);
}

void WaLabel::setText(const QString &new_text)
{
    int width = WaSkinModel::instance()->getMapGeometry(mapping).width();

    // Fit the text to the widget
    // This should always be three characters, but we generate its value anyway
    _text = new_text.rightJustify(width / _WA_TEXT_WIDTH, ' ');

    pixmapChange();

    update();
}

void WaLabel::pixmapChange()
{
    const char *label_text = _text.latin1();
    int n = label_text ? strlen(label_text) : 0;

    for (int i = 0; i < n; i++) 
	WaSkinModel::instance()->getText(label_text[i], completePixmap, 
				      i * _WA_TEXT_WIDTH, 0);
}

#include "waLabel.moc"
