/**************************************************************************

    rhythmview.h  - The RhythmView widget
    Copyright (C) 1998  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#ifndef _rhythmview_h_
#define _rhythmview_h_

#include <qwidget.h>
#include <kled.h>

class RhythmView : public QWidget
{
    int num,den;

    KLed **lamps;
    
public:
    RhythmView (QWidget *parent, const char *name);
    ~RhythmView();

    void setRhythm(int numerator,int denominator);

    void Beat(int i); // Sets the beat number

    void Beat(void); // Just increase the beat

    QSize sizeHint();
    QSizePolicy sizePolicy();

private:
    void resizeEvent(QResizeEvent *);

};

#endif
