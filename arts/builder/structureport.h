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

#ifndef STRUCTUREPORT_H
#define STRUCTUREPORT_H

#include "scomponent.h"
#include "module.h"

class StructurePort :public StructureComponent {
protected:
	Arts::StructurePortDesc SPortDesc;
	Arts::StructureDesc StructureDesc;

	ModulePort *_port;

	bool moveInternal(int x, int y);
	
public:
	StructurePort( Arts::StructurePortDesc SPortDesc,
				   Arts::StructureDesc StructureDesc, StructureCanvas *canvas);
	~StructurePort();

	void raisePosition();
	void lowerPosition();
	void rename(const char *newname);
	long id();
	long position();
	ModulePort *port();

	const char *inheritedInterface();
	void inheritedInterface(const char *iface);

	// StructureComponent Interface
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

#endif // STRUCTUREPORT_H
