/**************************************************************************

    klcdnumber.cpp  - The KLCDNumber widget (displays a lcd number)
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
#include "klcdnumber.h"
#include <qpainter.h>
#include <stdio.h>
#include "ktrianglebutton.h"

#define BUTTONWIDTH 9

KLCDNumber::KLCDNumber(int _numDigits,QWidget *parent,const char *name)
    : QWidget(parent,name)
{
    initDigits();
    setUserChangeValue=false;
    numDigits=_numDigits;
    value=0;
    minValue=0;
    maxValue=1000;
    oldvalue=-1;
    //    setPalette( QPalette (QColor(0,0,0)));
    upBtn=NULL;
    downBtn=NULL;
    setUserDefaultValue=false;
    doubleclicked=false;
    setLCDBackgroundColor(0,0,0);
    setLCDColor(100,255,100);
}

KLCDNumber::KLCDNumber(bool _setUserChangeValue,int _numDigits,QWidget *parent,const char *name)
    : QWidget(parent,name)
{
    initDigits();
    setUserChangeValue=_setUserChangeValue;
    numDigits=_numDigits;
    value=0;
    minValue=0;
    maxValue=1000;
    oldvalue=-1;
//    setBackgroundColor(QColor(0,0,0));
//    setPalette( QPalette (QColor(0,0,0)));
    upBtn=NULL;
    downBtn=NULL;
    setUserDefaultValue=false;
    doubleclicked=false;
    setLCDBackgroundColor(0,0,0);
    setLCDColor(100,255,100);
    if (setUserChangeValue)
    {
        upBtn=new KTriangleButton(KTriangleButton::Right,this,"Up");
        downBtn=new KTriangleButton(KTriangleButton::Left,this,"Down");
        upBtn->setGeometry(width()-BUTTONWIDTH,0,BUTTONWIDTH,height());
        downBtn->setGeometry(0,0,BUTTONWIDTH,height());
        connect(upBtn,SIGNAL(clicked()),this,SLOT(increaseValue()));
        connect(downBtn,SIGNAL(clicked()),this,SLOT(decreaseValue()));
        connect(upBtn,SIGNAL(clickedQuickly()),this,SLOT(increaseValueFast()));
        connect(downBtn,SIGNAL(clickedQuickly()),this,SLOT(decreaseValueFast()));
    };
}

void KLCDNumber::initDigits (void)
{
Digit[0] = digit(true,true,true,false,true,true,true);
Digit[1] = digit(false,false,true,false,false,true,false);  
Digit[2] = digit(true,false,true,true,true,false,true);
Digit[3] = digit(true,false,true,true,false,true,true);
Digit[4] = digit(false,true,true,true,false,true,false);
Digit[5] = digit(true,true,false,true,false,true,true);
Digit[6] = digit(true,true,false,true,true,true,true);
Digit[7] = digit(true,false,true,false,false,true,false);
Digit[8] = digit(true,true,true,true,true,true,true);
Digit[9] = digit(true,true,true,true,false,true,true);
Digit[10] = digit(false,false,false,false,false,false,false);
}

void KLCDNumber::resizeEvent ( QResizeEvent *)
{
    if (setUserChangeValue)
    {
        upBtn->setGeometry(width()-BUTTONWIDTH,0,BUTTONWIDTH,height());
        downBtn->setGeometry(0,0,BUTTONWIDTH,height());
    }
}

void KLCDNumber::drawVerticalBar(QPainter *qpaint,int x,int y,int w,int h,int d)
{
    /* If d is 0 it means that it is a bar on the left and if 1 it is on the right */
    /*
     |\                   /|
     | |                 | |
     | | = 0         1 = | |
     | |                 | |
     |/                   \|
     */

    y++;
    h-=2;
    if (d==0)
    {
        for (int i=x;i<x+w;i++)
        {
            qpaint->drawLine(i,y,i,y+h);
            y++;
            h-=2;
        }
    } else {
        for (int i=x+w;i>x;i--)
        {
            qpaint->drawLine(i,y,i,y+h);
            y++;
            h-=2;
        }
    }
}

void KLCDNumber::drawHorizBar(QPainter *qpaint,int x,int y,int w,int h,int d)
{
    // 0 is upper, 1 is bottom and 2 is the middle bar.
    x++;
    w-=3;
    if (d==0)
    {
        for (int i=y;i<y+h;i++)
        {
            qpaint->drawLine(x,i,x+w,i);
            x++;
            w-=2;
        }
    }
    else if (d==1)
    {
        for (int i=y+h;i>y;i--)
        {
            qpaint->drawLine(x,i,x+w,i);
            x++;
            w-=2;
        }
    }
    else
    {
        for (int i=0;i<=h/2;i++)
        {
            qpaint->drawLine(x,y-i,x+w,y-i);
            qpaint->drawLine(x,y+i,x+w,y+i);
            x++;
            w-=2;
        }
        
    };

}

void KLCDNumber::drawDigit(QPainter *qpaint,int x,int y,int w,int h,digit d)
{
if (d.nw) drawVerticalBar(qpaint,x,y,w/5,h/2,0);
if (d.ne) drawVerticalBar(qpaint,x+w*4/5,y,w/5,h/2,1);
if (d.sw) drawVerticalBar(qpaint,x,y+h/2,w/5,h/2,0);
if (d.se) drawVerticalBar(qpaint,x+w*4/5,y+h/2,w/5,h/2,1);
if (d.up) drawHorizBar (qpaint,x,y,w,w/5,0);
if (d.bt) drawHorizBar (qpaint,x,y+h-w/5,w,w/5,1);
if (d.md) drawHorizBar (qpaint,x,y+h/2,w,w/5,2);
}


void KLCDNumber::setValue(double v)
{
    oldvalue=value;
    if (v<minValue)
        value=minValue;
    else if (v>maxValue)
        value=maxValue;
    else
        value=v;

}

void KLCDNumber::display (double v)
{
    setValue(v);
    repaint(FALSE);
}

void KLCDNumber::display (int v)
{
    display((double)v);
}


void KLCDNumber::paintEvent ( QPaintEvent * )
{
    QPainter qpaint(this);
    qpaint.fillRect(0,0,width(),height(),backgcolor);
    qpaint.setPen(LCDcolor);
    
    QString numStr;
    numStr.setNum((int) value);
    numStr = numStr.rightJustify( numDigits, ' ', true);

    int dx,dy,dw,dh;
    if (setUserChangeValue)
    {
        dx=BUTTONWIDTH;
        dy=height()/10;
        dh=height()-dy*2;
        dw=(width()-(BUTTONWIDTH*2))/numDigits;
    } else
    {
        dx=0;
        dy=height()/10;
        dh=height()-dy*2;
        dw=width()/numDigits;
    };
    int sep=dw/10;

    for(int i=0; i < numDigits; i++)
    {
        char dig = numStr[i].latin1();
        if ((dig>='0')&&(dig<='9'))
            drawDigit(&qpaint,dx+sep,dy,dw-sep*2,dh,Digit[dig-'0']);
        else 
            drawDigit(&qpaint,dx+sep,dy,dw-sep*2,dh,KLCDNumber::Digit[10]);
        dx+=dw;
    };
}

void KLCDNumber::setUserSetDefaultValue(bool _userSetDefaultValue)
{
    if (setUserDefaultValue!=_userSetDefaultValue)
    {
        setUserDefaultValue=_userSetDefaultValue;
        /*        if (setUserDefaultValue)
         connect();
         else
         disconnect();
         */
    }
    
}

void KLCDNumber::setDefaultValue(double v)
{
    defaultValue=v;
}


void KLCDNumber::decreaseValue()
{
    display( value-1 );
    emit valueChanged( value );
}

void KLCDNumber::increaseValue()
{
    display( value+1 );
    emit valueChanged( value );
}

void KLCDNumber::decreaseValueFast()
{
    display( value-1 );
}

void KLCDNumber::increaseValueFast()
{
    display( value+1 );
}

void KLCDNumber::mouseDoubleClickEvent (QMouseEvent *)
{
    doubleclicked=true;
    defaultValueClicked();
}

void KLCDNumber::mousePressEvent (QMouseEvent *e)
{
    startTimer(200);
    QWidget::mousePressEvent(e);
}

void KLCDNumber::timerEvent(QTimerEvent *)
{
    killTimers();
    doubleclicked=false;

}


void KLCDNumber::defaultValueClicked()
{
    if (setUserDefaultValue)
    {
        display( defaultValue );
        emit valueChanged( value );
    }
}

void KLCDNumber::setLCDBackgroundColor(int r,int g,int b)
{
    backgcolor=QColor(r,g,b);
    repaint(FALSE);
}

void KLCDNumber::setLCDColor(int r,int g,int b)
{
    LCDcolor=QColor(r,g,b);
    repaint(FALSE);
}

void KLCDNumber::setRange(double min, double max)
{
    minValue=min;
    maxValue=max;
}

QSize KLCDNumber::sizeHint()
{
    return QSize(83,28);
}
#include "klcdnumber.moc"
