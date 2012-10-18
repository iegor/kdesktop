/*
  Titlebar for winamp Skin
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2001  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <qwidget.h>
#include <qpixmap.h>

#include "waTitleBar.h"
#include "waSkinModel.h"

#include <stdlib.h>

#include <iostream>

WaTitleBar::WaTitleBar() : WaIndicator(_WA_MAPPING_TITLE, _WA_SKIN_TITLE_ACTIVE, _WA_SKIN_TITLE_INACTIVE)
{
    moving = false;
    setState(parentWidget()->isActiveWindow());
}

WaTitleBar::~WaTitleBar()
{
}

void WaTitleBar::mousePressEvent(QMouseEvent * e)
{
    if (e->button() != RightButton) {
	if (!moving) {
	    moving = true;
	    mDragStart = e->pos();
	    mLastPos = e->globalPos();
	}

        setState(true);
	update();
	return;
    }
    else
        WaWidget::mousePressEvent(e);
}

void WaTitleBar::mouseDoubleClickEvent(QMouseEvent *) {
    emit(shaded());
}

void WaTitleBar::mouseReleaseEvent(QMouseEvent * e)
{
    if (e->button() != RightButton) {
	moving = false;
	update();
	return;
    }
    else
        WaWidget::mouseReleaseEvent(e);
}

void WaTitleBar::mouseMoveEvent(QMouseEvent * e)
{
    QPoint diff = e->globalPos() - mLastPos;
    if (abs(diff.x()) > 10 || abs(diff.y()) > 10) {
	// Moving starts only, when passing a drag border
	moving = true;
    }

    if (moving) 
	parentWidget()->move(e->globalPos() - mDragStart);
}

#include <waTitleBar.moc>
