    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

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

    */

#include "structureport.h"
#include "drawutils.h"
#include <qpainter.h>
#include <qpalette.h>
#include <qdrawutil.h>
#include <qbitmap.h>
#include <assert.h>
#include <stdio.h>
#include <arts/debug.h>

using namespace std;

StructurePort::StructurePort(Arts::StructurePortDesc SPortDesc,
     Arts::StructureDesc StructureDesc, StructureCanvas *canvas)
                                        : StructureComponent(canvas)
{
	string portname = SPortDesc.name();
	this->SPortDesc = SPortDesc;
	this->StructureDesc = StructureDesc;
	_x = SPortDesc.x();
	_y = SPortDesc.y();
	_selected = false;
	_visible = false;

	ModulePort::Direction dir;
	if(SPortDesc.type().direction == Arts::input)
		dir = ModulePort::in;
	else
		dir = ModulePort::out;

	if(SPortDesc.type().connType == Arts::conn_property)
		arts_debug("got property here....");
			//       owner, description, portnr
	arts_debug(">>creating structureport: %s",portname.c_str());
	_port = new ModulePort(this, portname.c_str(), 0, dir,this->SPortDesc);
	arts_debug("<<");
}

StructurePort::~StructurePort()
{
	hide();
	StructureDesc.freeStructurePortDesc(SPortDesc);
        delete _port;
}

bool StructurePort::moveInternal(int x, int y)
{
	return SPortDesc.moveTo(x, y);
}

int StructurePort::width() const
{
	return 1;
}

int StructurePort::height() const
{
	return 1;
}

StructureComponent::ComponentType StructurePort::type()
{
	return ctPort;
}

bool StructurePort::drawNeedsBackground(int segment)
{
	assert(segment==0);
	return true;
}

void StructurePort::drawSegment(QPainter *dest, int cellsize, int segment)
{
	QString pname = QString::fromLocal8Bit(SPortDesc.name().c_str());
	assert(segment==0);

	QPainter &p = *dest;

	int direction = (_port->direction == ModulePort::out)?1:0;

// FIXME: make these color defs available at one central place, they
// are currently copied from main.cpp

	QColor mcolor(43,43,168);
	QColor mcolorlight(164,176,242);
    QColorGroup g( Qt::white, Qt::blue, mcolorlight, mcolor.dark(), mcolor,
														Qt::black, Qt::black );
	QBrush fill( mcolor );
	QPen textpen(QColor(255,255,180),1);
	int border = cellsize/10;
	int boxtop = (cellsize/2)*(1-direction);

	qDrawShadePanel(&p,0,boxtop,cellsize,cellsize/2, g, false, 1, &fill);
	p.setPen(textpen);

	// Selection box
	if(_selected)
	{
		QPen pen(Qt::white,1,Qt::DotLine);

		p.setPen(pen);
		p.drawRect(0,boxtop,cellsize,cellsize/2);
	}
						// ... doesn't look centered without the 2*border ?!?
	int textwidth;
	QString label=DrawUtils::cropText(&p, pname, cellsize-border*2, textwidth);
	p.drawText(border,border+boxtop,cellsize-border-1,(cellsize/2-1)-2*border,
      Qt::AlignCenter,label);

	int arrowwidth = cellsize/4;

	int i;
	for(i=0;i<3;i++)
	{
		QBrush fbrush;
		int delta = 0;
		switch(i)
		{
			case 0: delta = 0;
					fbrush = QBrush(g.light());
				break;
			case 1: delta = 2;
					fbrush = QBrush(g.dark());
				break;
			case 2: delta = 1;
					fbrush = fill;
				break;
		}

/**********************
-   |   |
c   |   |
-  -+   +-
d |       |
-  \     /
    \   /
     \_/
  |b| w |b|
*********************/

        int t = (cellsize/2-1)*direction;  // top
        int l = delta;       // left
		int w = arrowwidth-2;  // arrow body width
        int h = cellsize/2;  // total arrow height

		int b = (cellsize/2-arrowwidth)/2;  // x border width
/*
        int c = cellsize/10;
        int d = cellsize/6;
*/
        int c = cellsize/12;
		int d = cellsize/5;

        QPointArray a(9);
        a.setPoint(0,b+l,t);
        a.setPoint(1,b+l,t+c);
        a.setPoint(2,l,t+c);
        a.setPoint(3,l,t+c+d);
        a.setPoint(4,b+w/2+l,t+h);
        a.setPoint(5,b*2+w+l,t+c+d);
        a.setPoint(6,b*2+w+l,t+c);
        a.setPoint(7,b+w+l,t+c);
        a.setPoint(8,b+w+l,t);
        //a.setPoint(9,b+l,t);
/*
        a.setPoint(0,t,b+l);
        a.setPoint(1,t+c,b+l);
        a.setPoint(2,t+c,l);
        a.setPoint(3,t+c+d,l);
        a.setPoint(4,t+h,b+w/2+l);
        a.setPoint(5,t+c+d,b*2+w+l);
        a.setPoint(6,t+c,b*2+w+l);
        a.setPoint(7,t+c,b+w+l);
        a.setPoint(8,t,b+w+l);
        a.setPoint(9,t,b+l);
*/
		p.setPen(Qt::NoPen);
		p.setBrush(fbrush);
		p.drawPolygon(a);
        if(delta==1 && direction==0)
		{
			p.setPen(g.light());
			p.drawLine(b+l,t,b+w+l,t);
		}
/*
		p.fillRect((cellsize/2-arrowwidth)/2+delta,cellsize/2-1,
									arrowwidth,cellsize/2,fbrush);
*/
	}
	{
		int border = cellsize/7;

		QBrush pbrush(_port->color(false));

		_port->clickrect = QRect(border,direction * cellsize/2 + border,
					cellsize/2-2*border, cellsize/2-2*border);
		qDrawShadePanel(&p, _port->clickrect, g, _port->down(), 2, &pbrush);
	}
}

ModulePort *StructurePort::portAt(int segment, int x, int y)
{
	assert(segment == 0);

	QPoint clickpoint(x,y);
	if(_port->clickrect.contains(clickpoint)) return _port;
	return 0;
}

void StructurePort::dumpPorts(list<ModulePort *>& ports)
{
	ports.push_back(_port);
}

QPixmap *StructurePort::pixmap()
{
	return 0;
}

QString StructurePort::name()
{
	return QString::fromLocal8Bit(SPortDesc.name().c_str());
}

void StructurePort::raisePosition()
{
	SPortDesc.raisePosition();
}

void StructurePort::lowerPosition()
{
	SPortDesc.lowerPosition();
}

void StructurePort::rename(const char *newname)
{
	SPortDesc.rename(newname);
	canvas->redrawRect(_x,_y,1,1);
}

long StructurePort::id()
{
	return SPortDesc.ID();
}

long StructurePort::position()
{
	return SPortDesc.position();
}

ModulePort *StructurePort::port()
{
	return _port;
}

const char *StructurePort::inheritedInterface()
{
	static string ii;
	ii = SPortDesc.inheritedInterface();
	return ii.c_str();
}

void StructurePort::inheritedInterface(const char *iface)
{
	SPortDesc.inheritedInterface(iface);
}
