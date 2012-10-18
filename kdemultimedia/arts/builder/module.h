	/*

	Copyright (C) 1998 Stefan Westerfeld
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

#ifndef __MODULE_H_
#define __MODULE_H_

#include <list>

#include <qpixmap.h>
#include <qstring.h>
#include <qrect.h>
#include <ksimpleconfig.h>

#include "artsbuilder.h"
#include "scomponent.h"

class ModulePort
{
	//ModulePort *connection;
	
public:
	Arts::PortDesc PortDesc;
	long pdID;

	enum ConnType {none, source, dest, value, conf};
	enum Direction {in, out};

	bool 				 selected;
	StructureComponent	*owner;

	QString		description;
	QRect		clickrect;
	ConnType	conntype;
	Direction	direction;
	int			drawsegment;
	bool		isinitarg;

	long		route_owner;

	ModulePort( StructureComponent *owner, const std::string& description,
				int drawsegment, Direction direction, Arts::PortDesc PortDesc);

	bool down();
	QColor color(bool isInterface);
};

class Module :public StructureComponent
{
protected:
	Arts::StructureDesc StructureDesc;
	Arts::ModuleDesc    ModuleDesc;

	QPixmap *_pixmap;
	QString _name;

	int _width, _height;
	bool moveInternal(int x, int y);

	void initModule();

public:
	Module( Arts::ModuleDesc moduledesc, Arts::StructureDesc structuredesc,
			StructureCanvas *canvas);
	Module( const Arts::ModuleInfo& minfo, Arts::StructureDesc structuredesc,
			StructureCanvas *canvas);
	virtual ~Module();

	ModulePort *findPort(int xoffset, int direction);

	bool isInterface;

	long mdID;
	std::list<ModulePort *> inports, outports;

// StructureComponent interface

	int width() const;
	int height() const;
	ComponentType type();

	ModulePort *portAt(int segment, int x, int y);
	void dumpPorts(std::list<ModulePort *>& ports);

	bool drawNeedsBackground(int segment);
	void drawSegment(QPainter *dest, int cellsize, int segment);
	QPixmap *pixmap();
	QString name();
 };

#endif
