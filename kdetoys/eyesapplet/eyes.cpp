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

#include <math.h>

#include <qpainter.h>
#include <qcursor.h>
#include <qimage.h>

#include <klocale.h>
#include <kglobal.h>
#include <kapplication.h>

#include "eyes.h"
#include "eyes.moc"

#define AAFACTOR 4

extern "C"
{
    KDE_EXPORT KPanelApplet* init(QWidget *parent, const QString& configFile)
    {
        KGlobal::locale()->insertCatalogue("keyesapplet");
        EyesApplet *applet = new EyesApplet(configFile, KPanelApplet::Normal, 0, parent, "keyesapplet");
        return applet;
    }
}

EyesApplet::EyesApplet(const QString& configFile, Type t, int actions,
                       QWidget *parent, const char *name)
  : KPanelApplet( configFile, t, actions, parent, name )
{
    setWFlags(WNoAutoErase);
    setBackgroundOrigin(AncestorOrigin);
    startTimer(50);
    oldleft = QPoint(-1, -1);
    oldright = QPoint(-1, -1);
    oldMouse = QPoint(-1, -1);
}

int EyesApplet::widthForHeight(int h) const
{
    return static_cast<int>(1.4 * h); // rectangular shape.
}
int EyesApplet::heightForWidth(int w) const
{
    return static_cast<int>(w / 1.4); // rectangular shape.
}

void EyesApplet::resizeEvent( QResizeEvent*e )
{
    QWidget::resizeEvent(e);
}

void EyesApplet::timerEvent(QTimerEvent*)
{
    QPoint mouse = mapFromGlobal(QCursor::pos());
    if (mouse != oldMouse)
        update();
}

void EyesApplet::paintEvent(QPaintEvent*)
{
    int spWidth = width() * AAFACTOR;
    int spHeight = height() * AAFACTOR;
    
    if (spWidth != _cache.width() || spHeight != _cache.height())
        _cache.resize(spWidth, spHeight);
    
    QPainter paint(&_cache);
    
    if (paletteBackgroundPixmap())
    {
        QPixmap bg(width(), height());
        QPainter p(&bg);
        QPoint offset = backgroundOffset();
        p.drawTiledPixmap(0, 0, width(), height(), *paletteBackgroundPixmap(), offset.x(), offset.y());
        p.end();
        QImage bgImage = bg.convertToImage().scale(spWidth, spHeight);
        paint.drawImage(0, 0, bgImage);
    }
    else
    {
        _cache.fill(paletteBackgroundColor());
    }
    
    // draw eyes, no pupils
    paint.setPen(QPen(black, 2 * AAFACTOR));
    paint.setBrush(QBrush(white));
    
    int w = spWidth; // - AAFACTOR * 2;
    int h = spHeight; // - AAFACTOR * 2;
    
    // left eye
    paint.drawEllipse(AAFACTOR, AAFACTOR, w/2 - AAFACTOR, h - AAFACTOR * 2);
    
    // right eye
    paint.drawEllipse(w/2, AAFACTOR, w/2 - AAFACTOR, h - AAFACTOR * 2);
    
    // draw pupils
    drawPupils(&paint);
    paint.end();
    
    QPainter paintFinal(this);
    QImage spImage = _cache.convertToImage();
    QImage displayImage = spImage.smoothScale(size());
    paintFinal.drawImage(0, 0, displayImage);
    paintFinal.end();
}

void EyesApplet::drawPupils(QPainter* p)
{
    QPoint pos, mouse, vect;
    double cos_alpha,sin_alpha;
    
    int w = width() * AAFACTOR;
    int h = height() * AAFACTOR;

    oldMouse = mapFromGlobal(QCursor::pos());
    mouse =  oldMouse * AAFACTOR;
    int tmp = QMIN(h, w)/6;

    // left pupil
    vect.setX(mouse.x() - h / 4);
    vect.setY(mouse.y() - h / 2);

    cos_alpha = vect.x() / sqrt(double(vect.x() * vect.x() + vect.y() * vect.y()));
    sin_alpha = vect.y() / sqrt(double(vect.x() * vect.x() + vect.y() * vect.y()));

    if(vect.x() * vect.x() + vect.y() * vect.y() > (w/4 - tmp) * (w/4 - tmp)*
       cos_alpha * cos_alpha+ (h/2-tmp) * (h/2-tmp) * sin_alpha * sin_alpha) {
        pos.setX(int((w/4-tmp) * cos_alpha+w/4));
        pos.setY(int((h/2-tmp) * sin_alpha+h/2));
    }
    else
    	pos = mouse;

    if(pos != oldleft) {

        int sizeEye=QMIN(h,w)/6;

//         // draw over old pos
// 	p->setPen(QPen(NoPen));
// 	p->setBrush(QBrush(white));
// 	p->drawEllipse(oldleft.x() - sizeEye/2, oldleft.y() - sizeEye/2, sizeEye, sizeEye);

        // draw left pupil
        p->setPen(QPen(NoPen));
        p->setBrush(QBrush(black));
        p->drawEllipse(pos.x() - sizeEye/2, pos.y() - sizeEye/2, sizeEye, sizeEye);

    //oldleft = pos;
    }

    // right pupil
    vect.setX(mouse.x() - 3*w/4);
    vect.setY(mouse.y() - h/2);

    cos_alpha = vect.x()/sqrt(double(vect.x()*vect.x()+vect.y()*vect.y()));
    sin_alpha = vect.y()/sqrt(double(vect.x()*vect.x()+vect.y()*vect.y()));

    if(vect.x()*vect.x() + vect.y()*vect.y() > (w/4-tmp)*(w/4-tmp)
       *cos_alpha*cos_alpha+(h/2-tmp)*(h/2-tmp)*sin_alpha*sin_alpha)
    {
        pos.setX(int((w/4-tmp)*cos_alpha+3*w/4));
        pos.setY(int((h/2-tmp)*sin_alpha+h/2));
    }
    else
        pos = mouse;

    if(pos != oldright) {

        int sizeEye=QMIN(h,w)/6;

//         // draw over old pos
// 	p->setPen(QPen(NoPen));
// 	p->setBrush(QBrush(white));
// 	p->drawEllipse(oldright.x() - sizeEye/2, oldright.y() - sizeEye/2, sizeEye, sizeEye);

        // draw left pupil
        p->setPen(QPen(NoPen));
        p->setBrush(QBrush(black));
        p->drawEllipse(pos.x() - sizeEye/2, pos.y() - sizeEye/2, sizeEye, sizeEye);

    //oldright = pos;
    }
}
