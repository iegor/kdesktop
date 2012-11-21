/*
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
 *  based on keyes (C) 1999 by Jerome Tollet <tollet@magic.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#ifndef __eyes_h__
#define __eyes_h__

#include <qpixmap.h>
#include <kpanelapplet.h>

class EyesApplet : public KPanelApplet
{
    Q_OBJECT

public:
    EyesApplet(const QString& configFile, Type t = Normal, int actions = 0,
               QWidget *parent = 0, const char *name = 0);

    int widthForHeight(int height) const;
    int heightForWidth(int width) const;

protected:
    void timerEvent(QTimerEvent*);
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent*);

private:
    void drawPupils(QPainter* p);

    QPoint oldleft, oldright, oldMouse;
    QPixmap _cache;
};

#endif // __eyes_h__
