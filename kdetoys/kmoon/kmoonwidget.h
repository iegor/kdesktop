/*
 *   kmoon - a moon phase indicator
 *   Copyright (C) 1998,2000  Stephan Kulow
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef KMOON
#define KMOON

#include <qwidget.h>
#include <qimage.h>
#include <sys/types.h>
#include <time.h>
#include <ksystemtray.h>
class QPopupMenu;

class MoonWidget : public QWidget
{
    Q_OBJECT

public:
    MoonWidget(QWidget *parent = 0, const char *name = 0);
    ~MoonWidget();

    void calcStatus( time_t time );

    int angle() const { return _angle; }
    void setAngle(int angle);

    bool northHemi() const { return _north; }
    void setNorthHemi(bool b);

    bool mask() const { return _mask; }
    void setMask(bool b);

protected:
    int old_w, old_h;
    int counter,  old_counter;
    int _angle, old_angle;
    int _mask, old_mask;
    bool old_north, _north;

    QPixmap pixmap;
    QString tooltip;

    void paintEvent( QPaintEvent *e);
    void resizeEvent( QResizeEvent *e);

private:
    QImage loadMoon(int index);
    void renderGraphic();
};

#endif
