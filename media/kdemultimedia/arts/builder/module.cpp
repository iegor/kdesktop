	/*

	Copyright (C) 1998-1999 Stefan Westerfeld
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

#include "module.h"
#include "drawutils.h"
#include <stdio.h>
#include <arts/debug.h>

#include <qpalette.h>
#include <qdrawutil.h>
#include <kiconloader.h>
#include <algorithm>

using namespace std;

ModulePort::ModulePort(StructureComponent *owner, const string& description,
				int drawsegment, Direction direction, Arts::PortDesc PortDesc)
{
	selected = false;

	this->owner = owner;				// whats the syntax for that again?
	this->drawsegment = drawsegment;
	this->direction = direction;
	this->PortDesc = PortDesc;
	this->description = description.c_str();

	pdID = PortDesc.ID();

	isinitarg = (PortDesc.type().connType == Arts::conn_property);
	if(isinitarg) arts_debug("port %s is an init arg", (const char *)this->description.latin1());
	arts_debug("port %s created", (const char *)this->description.latin1());
	conntype = none;
	route_owner = 0;
}

bool ModulePort::down()
{
	return (PortDesc.isConnected() || PortDesc.hasValue() || selected);
}

QColor ModulePort::color(bool isInterface)
{
	if(selected) return QColor(255, 165, 0);

	if(PortDesc.hasValue())
	{
		if(isinitarg) return QColor(180, 180, 180);
		return QColor(100, 100, 255);
	}

	if(isinitarg) return QColor(128, 128, 128);

	if(isInterface) return QColor(100, 100, 100);
	return QColor(43, 43, 168);
}

Module::Module(Arts::ModuleDesc module, Arts::StructureDesc structuredesc,
					StructureCanvas *canvas) : StructureComponent(canvas)
{
	StructureDesc = structuredesc;
	ModuleDesc = module;

	_x = ModuleDesc.x();
	_y = ModuleDesc.y();
	initModule();
}

Module::Module(const Arts::ModuleInfo& minfo, Arts::StructureDesc structuredesc,
					StructureCanvas *canvas) : StructureComponent(canvas)
{
	StructureDesc = structuredesc;
	ModuleDesc = StructureDesc.createModuleDesc(minfo);

	initModule();
}

void Module::initModule()
{
	QString iconname;
	KIconLoader iconloader;

	_selected = false;
	_visible = false;
	_height = 1;
	_name = ModuleDesc.name().c_str();
	_pixmap = NULL;

// test if pixmap available

	iconname = _name + ".xpm";

	_pixmap = new QPixmap(iconloader.loadIcon(iconname, KIcon::User));
	if(!_pixmap->height())
	{
		iconname = _name + ".png";
		delete _pixmap;
		_pixmap = new QPixmap( iconloader.loadIcon( iconname, KIcon::User ) );
		if( !_pixmap->height() )
		{
			delete _pixmap;
			_pixmap = 0;
		}
	}
// try again without Arts:: - prefix

	if(iconname.startsWith("Arts::"))
	{
		iconname = iconname.mid(6);

		_pixmap = new QPixmap(iconloader.loadIcon(iconname, KIcon::User));
		if(!_pixmap->height())
		{
			iconname.replace( iconname.length() - 4, 3, "png" );
			delete _pixmap;
			_pixmap = new QPixmap(iconloader.loadIcon(iconname, KIcon::User));
			if( !_pixmap->height() )
			{
				delete _pixmap;
				_pixmap = 0;
			}
		}
	}
/*
	FILE *test = fopen(QFile::encodeName(iconname), "r");
	if(test)
	{
		pixmap = new QPixmap(iconname);
		fclose(test);
	}
*/

// create lists with inports & outports for this module
// and bind them to it ...

	arts_debug("Getting ports...");
	vector<Arts::PortDesc >*ports = ModuleDesc.ports();
	unsigned long portpos;
	long indraw = 1, outdraw = 2;

	for(portpos = 0; portpos < ports->size(); portpos++)
	{
		Arts::PortDesc pd = (*ports)[portpos];
		ModulePort *p;

		arts_debug("CREATING %s", pd.name().c_str());
		switch(pd.type().direction)
		{
			case Arts::input:
					p = new ModulePort(this, pd.name(), indraw++,
							ModulePort::in, pd);
					inports.push_back(p);
				break;
			case Arts::output:
					p = new ModulePort(this, pd.name(), outdraw++,
							ModulePort::out, pd);
					outports.push_back(p);
				break;
			default:
				assert(false);	// shouldn't happen!
		}
	}

	delete ports;

	_width = 1 + max(inports.size(), outports.size() + 1);

	mdID = ModuleDesc.ID();
	isInterface = ModuleDesc.isInterface();
}

Module::~Module()
{
	arts_debug("hide...");
	hide();
	arts_debug("sdfmd...");
	list<ModulePort *>::iterator i;

	for(i = inports.begin(); i != inports.end(); ++i) delete *i;
	inports.clear();

	for(i = outports.begin(); i != outports.end(); ++i) delete *i;
	outports.clear();

	StructureDesc.freeModuleDesc(ModuleDesc);
	arts_debug("ok...");
        delete _pixmap;
}

bool Module::moveInternal(int x, int y)
{
	return ModuleDesc.moveTo(x, y);
}

int Module::width() const
{
	return _width;
}

int Module::height() const
{
	return _height;
}

StructureComponent::ComponentType Module::type()
{
	return ctModule;
}

bool Module::drawNeedsBackground(int segment)
{
	return (segment == 0);
}

void Module::drawSegment(QPainter *p, int cellsize, int segment)
{
	int border = cellsize / 10;  // for the logo
	int ltop = (cellsize - border)/2;
	int lbot = (cellsize + border)/2;

	QColor mcolor(43, 43, 168);
	QColor mcolorlight(164, 176, 242);

	if(isInterface)
	{
		mcolor = QColor(100, 100, 100);
		mcolorlight = QColor(160, 160, 160);
	}
    QColorGroup g( Qt::white, Qt::blue, mcolorlight, mcolor.dark(), mcolor,
							Qt::black, Qt::black );
	QBrush fill( mcolor );
	QPen textpen(QColor(255, 255, 180), 1);

	if(segment == 0)
	{
		qDrawShadePanel(p, border, border, cellsize - 2*border + 1, cellsize - 2*border + 1,
			g, false, 1, &fill);
		p->fillRect(cellsize - border - 1, ltop, cellsize, lbot - ltop + 1, fill);
		p->setPen(g.light());
		p->drawLine(cellsize - border, ltop - 1, cellsize, ltop - 1);
		p->setPen(g.dark());
		p->drawLine(cellsize - border, lbot + 1, cellsize, lbot + 1);
		if(_pixmap)
		{
			int destsize = (cellsize - 4*border);
			float sx = (float)destsize/(float)_pixmap->width();
			float sy = (float)destsize/(float)_pixmap->height();

			QWMatrix matrix;
			matrix.scale(sx, sy);
			QPixmap pmscaled = _pixmap->xForm(matrix);
			p->drawPixmap(border*2, border*2, pmscaled);
		}
		return;
	}

	p->fillRect(0, 0, cellsize, cellsize, fill);

	/*
     * take care of the bevel lines around the module
     */

	p->setPen(g.light());
	p->drawLine(0, 0, cellsize - 1, 0);
	if(segment < 2)
		p->drawLine(0, 0, 0, cellsize - 1);

	p->setPen(g.dark());
	p->drawLine(cellsize - 1, cellsize - 1, 0, cellsize - 1);
	if(segment == 0 || segment == width() - 1)
		p->drawLine(cellsize - 1, cellsize - 1, cellsize - 1, 0);

	/*
	 * now draw the ports
	 */
	int direction;

	for(direction = 0;direction < 2; direction++)
	{
		ModulePort *port = findPort(segment, direction);

		if(port)
		{
			int border = cellsize/7;
			int textwidth;
			QString label = DrawUtils::cropText(p, port->description,
												cellsize/2, textwidth);

			QBrush pbrush(port->color(isInterface));

			port->clickrect = QRect(border, direction * cellsize/2 + border,
				cellsize/2 - 2*border, cellsize/2 - 2*border);
			qDrawShadePanel(p, port->clickrect, g, port->down(), 2, &pbrush);

#if 0
			QBrush fillport(fill);
			if(port->isinitarg)
			{
				fillport = QColor(128, 128, 128);
			}

			if(port->selected)
			{
				QBrush fillorange(QColor(255, 165, 0));
				qDrawShadePanel(p, port->clickrect, g, true, 2, &fillorange);
			}
			else
			{
				if(port->PortDesc->isConnected())
				{
					qDrawShadePanel(p, port->clickrect, g, true, 2, &fillport);
				}
				else if(port->PortDesc->hasValue())
				{
					QBrush fillp(QColor(100, 100, 255));
					if(port->isinitarg)
					{
						fillp = QColor(180, 180, 180);
					}
					qDrawShadePanel(p, port->clickrect, g, true, 2, &fillp);
				}
				else	// not connected and no value();
				   qDrawShadePanel(p, port->clickrect, g, false, 2, &fillport);
			}
#endif

			p->setPen(textpen);
			p->drawText((cellsize - border)/2,
							(1 + direction) * (cellsize/2) - border, label);
		}
	}

	/*
	 * if it was the rightmost part of the module, it has the module name
	 * and the connection to the logo as well
	 */

	if(segment == 1)
	{
		// object type label
		int textwidth;
		QString label = DrawUtils::cropText(p, _name, cellsize - 4, textwidth);

		p->setPen(textpen);
		p->fillRect(1, cellsize - 16, textwidth + 7, 15, QBrush(g.dark()));
		p->drawText(4, cellsize - 5, label);

		// logo connection
		p->setPen(mcolor);
		p->drawLine(0, ltop, 0, lbot);
	}

	/*
	 * when selected, draw a line of white dots around the module
	 */

	if(selected())
	{
		QPen pen(Qt::white, 1, Qt::DotLine);

		p->setPen(pen);
		p->drawLine(0, 0, cellsize - 1, 0);
		p->drawLine(0, cellsize - 1, cellsize - 1, cellsize - 1);
		if(segment == 1)
			p->drawLine(0, 0, 0, cellsize - 1);
		if(segment == _width - 1)
			p->drawLine(cellsize - 1, 0, cellsize - 1, cellsize - 1);
	}
}

ModulePort *Module::findPort(int xoffset, int direction)
{
	list<ModulePort *>*ports;
	list<ModulePort *>::iterator i;

	long n;

	if(direction == 0) ports = &inports; else ports = &outports;

	i = ports->begin();
	n = xoffset - 1 - direction;

	if(n < (long)ports->size() && n >= 0)
	{
		while(n > 0) { n--; i++; }
		return (*i);
	}
	return(NULL);
}

ModulePort *Module::portAt(int segment, int x, int y)
{
	for(int direction = 0; direction < 2; direction++)
	{
		ModulePort *port = findPort(segment, direction);
		if(port)
		{
			QPoint clickpoint(x, y);
			if(port->clickrect.contains(clickpoint)) return port;
		}
	}
	return 0;
}

void Module::dumpPorts(list<ModulePort *>& ports)
{
	list<ModulePort *>::iterator i;
	for(i = inports.begin(); i != inports.end(); ++i) ports.push_back(*i);
	for(i = outports.begin(); i != outports.end(); ++i) ports.push_back(*i);
}

QPixmap *Module::pixmap()
{
	return _pixmap;
}

QString Module::name()
{
	return _name;
}

// vim: sw=4 ts=4 noet
