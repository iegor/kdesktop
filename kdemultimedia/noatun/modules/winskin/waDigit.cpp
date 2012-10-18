/*
  The digit for the time
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2002  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "waDigit.h"
#include "waSkinModel.h"

#include <kconfig.h>
#include <kglobal.h>

WaDigit::WaDigit() : WaWidget(_WA_MAPPING_DIGITS)
{
    KConfig *config = KGlobal::config();
    config->setGroup("Winskin");

    reverse_time = config->readNumEntry("timeReversed", false);
}


WaDigit::~WaDigit()
{
    KConfig *config = KGlobal::config();
    config->setGroup("Winskin");
    config->writeEntry("timeReversed", reverse_time);
}

void WaDigit::paintEvent(QPaintEvent *)
{
    paintBackground();

    const char *time = timeString.latin1();
    int len = strlen(time);
    if (len == 0) 
	return;

    // Declare all these variables after we check for zero-length
    WaSkinModel *waSkinModel = WaSkinModel::instance();

    int x = waSkinModel->getMapGeometry(mapping).x();
    int y = waSkinModel->getMapGeometry(mapping).y();

    QRect mapRect;

    // We expect strings either in the form "xx:yy", or "-xx:yy"
    // If the string length is 6, we have it in the form of "-xx:yy"
    // Remove the -, move the string forward one character, and 
    // continue parsing
    mapRect = waSkinModel->getMapGeometry(_WA_MAPPING_MINUS);
    if (len == 6) {
        waSkinModel->getDigit('-', this, mapRect.x() - x, mapRect.y() - y);
        time++;
    }
    else {
        waSkinModel->getDigit(' ', this, mapRect.x() - x, mapRect.y() - y);
    }

    mapRect = waSkinModel->getMapGeometry(_WA_MAPPING_DIGIT_1);
    waSkinModel->getDigit(time[0], this, mapRect.x() - x, mapRect.y() - y);

    mapRect = waSkinModel->getMapGeometry(_WA_MAPPING_DIGIT_2);
    waSkinModel->getDigit(time[1], this, mapRect.x() - x, mapRect.y() - y);

    mapRect = waSkinModel->getMapGeometry(_WA_MAPPING_DIGIT_3);
    waSkinModel->getDigit(time[3], this, mapRect.x() - x, mapRect.y() - y);

    mapRect = waSkinModel->getMapGeometry(_WA_MAPPING_DIGIT_4);
    waSkinModel->getDigit(time[4], this, mapRect.x() - x, mapRect.y() - y);
}

void WaDigit::mousePressEvent(QMouseEvent* e) {
    if (e->button() == LeftButton) {
        reverse_time = !reverse_time;
        emit digitsClicked();
    }
    else
        WaWidget::mousePressEvent(e);
}

#include "waDigit.moc"
