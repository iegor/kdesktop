/*
  Scrolling song title for winamp Skin
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2001  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <qpainter.h>
#include <qpixmap.h>

#include <stdlib.h>

#include <kconfig.h>
#include <kglobal.h>

#include "waInfo.h"
#include "waSkinModel.h"

WaInfo::WaInfo() : WaWidget(_WA_MAPPING_INFO)
{
    connect(WaSkinModel::instance(), SIGNAL(skinChanged()),
	    this, SLOT(pixmapChange()));

    completePixmap = new QPixmap();

    QSize size = sizeHint();
    completePixmap->resize(size.width(), size.height());

    xGrabbedPos = -1;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeEvent()));
}

WaInfo::~WaInfo()
{
    delete completePixmap;
}


void WaInfo::timeEvent()
{
    if ((xGrabbedPos == -1) && (xScrollDirection)) {
        xScrollPos += xScrollDirection;

        if (abs(xScrollPos) > completePixmap->width()) {
            xScrollPos = 0;
        }

        if (isVisible())
            repaint(false);
    }
}


void WaInfo::scrollerSetup()
{
    xScrollPos = 0;
    xScrollDirection = 0;
    timer->stop();
    QSize size = sizeHint();
    if (completePixmap->width() > size.width()) {
		xScrollDirection = 1;
		
		KConfig *config=KGlobal::config();
		config->setGroup("Winskin");
		int s = config->readNumEntry("ScrollDelay", 15);
		if (s!=0)
			timer->start(50-s);
    }
}

void WaInfo::paintEvent(QPaintEvent *)
{
    QSize size = sizeHint();

    if (completePixmap->width() <= size.width()) {
	bitBlt(this, 0, 0, completePixmap);
	return;
    }

    // pixmap widther than window:
    int xDrawWidth;
    int xRestWidth;

    xDrawWidth = completePixmap->width() - xScrollPos;
    if (xDrawWidth > size.width()) {
	xDrawWidth = size.width();
    }


    bitBlt(this, 0, 0, completePixmap, xScrollPos, 0, xDrawWidth);

    if (xDrawWidth < size.width()) {
	xRestWidth = size.width() - xDrawWidth;
	bitBlt(this, xDrawWidth, 0, completePixmap, 0, 0, xRestWidth);
    }
}


void WaInfo::setText(QString song)
{
    if (_text != song) {
        _text = song;
        pixmapChange();
    }
}

QString WaInfo::text() const
{
    return _text;
}


void WaInfo::pixmapChange()
{
    int i;
    const char *infoString = _text.latin1();

    int x = 0;
    int n=infoString ? strlen(infoString) : 0;

    QSize size = sizeHint();

    completePixmap->resize(QMAX(n * _WA_TEXT_WIDTH, size.width()), _WA_TEXT_HEIGHT);

    for (i = 0; i < n; i++) {
	WaSkinModel::instance()->getText(infoString[i], completePixmap, x, 0);
	x += _WA_TEXT_WIDTH;
    }

    // if the size is now smaller than the with of this widget, we
    // fill the pixmap with spaces
    if (x < size.width()) {
	while (x < size.width()) {
	    WaSkinModel::instance()->getText(' ', completePixmap, x, 0);
	    x += _WA_TEXT_WIDTH;
	}
    }

    scrollerSetup();
    update();
}

void WaInfo::mousePressEvent (QMouseEvent *e) {
    if (e->button() == LeftButton)
        xGrabbedPos = (e->x() + xScrollPos) % completePixmap->width();
}

void WaInfo::mouseMoveEvent (QMouseEvent * e) {
    xScrollPos = -e->x() + xGrabbedPos;

    if (xScrollPos < 0)
        xScrollPos = completePixmap->width() - (-xScrollPos % completePixmap->width());
    else
        xScrollPos %= completePixmap->width(); 

    update();
}

void WaInfo::mouseReleaseEvent (QMouseEvent *) {
    xGrabbedPos = -1;
}


#include "waInfo.moc"
