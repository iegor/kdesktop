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

#include "createtool.h"

#include "mwidget.h"
#include "structureport.h"

//#include <arts/debug.h>
#include <qpalette.h>
#include <qpainter.h>
#include <kmessagebox.h>
#include <klocale.h>

MWidgetTool::MWidgetTool(ModuleWidget *mwidget)
{
	this->mwidget = mwidget;
}

MWidgetTool::~MWidgetTool()
{
}

CreateTool::CreateTool(ModuleWidget *mwidget)
	: MWidgetTool(mwidget)
{
	oldCursor = mwidget->cursor();
	mwidget->setCursor(Qt::crossCursor);

	oldMouseTracking = mwidget->hasMouseTracking();
	mwidget->setMouseTracking(true);

	width = -1;
	height = -1;
	componentRect = QRect(0, 0, 0, 0);
}

CreateTool::~CreateTool()
{
	mwidget->repaint(componentRect);
	mwidget->setCursor(oldCursor);
	mwidget->setMouseTracking(oldMouseTracking);
}

void CreateTool::mousePressEvent(QMouseEvent *e)
{
	int x = mwidget->findCol(e->x());
	int y = mwidget->findRow(e->y());

	if(x < 0 || y < 0) return;

	if( e->button() == Qt::LeftButton )
		performCreate(x, y);

	// well, perhaps make this an option
	mwidget->leaveTool(this);
}

void CreateTool::mouseMoveEvent(QMouseEvent *e)
{
	if(width == -1) estimateSize();

	int x = mwidget->findCol(e->x());
	int y = mwidget->findRow(e->y());

	if(x < 0 || y < 0) return;

	int cellx, celly, cellx2, celly2;

	bool posok = mwidget->colXPos(x, &cellx) && mwidget->rowYPos(y, &celly)
     && mwidget->colXPos(x + width, &cellx2) && mwidget->rowYPos(y + height, &celly2);
	if(!posok) return;

	mwidget->repaint(componentRect);
	componentRect = QRect(cellx, celly, cellx2 - cellx, celly2 - celly);

	QPainter *p = new QPainter;

	p->begin(mwidget);
	p->save();
	p->drawRect(componentRect);
	p->restore();
	p->end();
        delete p;
}

void CreateTool::mouseReleaseEvent(QMouseEvent *e)
{
}

// module creation

void CreateModuleTool::estimateSize()
{
// yes, this is a it ugly: we create here the new module for a microsecond,
// just to see how big it is, to be able to paint an accurate preview
//
// We delete it again, and if the user finally really creates the module
// we recreate it.

	mwidget->unselectAll();

	Module *newModule = mwidget->theStructure()->createModule(minfo);
	width = newModule->width();
	height = newModule->height();
	newModule->setSelected(true);

	mwidget->theStructure()->deleteSelected();
}

void CreateModuleTool::performCreate(int x, int y)
{
	mwidget->unselectAll();
	Module *newModule = mwidget->theStructure()->createModule(minfo);
	newModule->setSelected(true);
	newModule->move(x, y);

	if(!mwidget->insertModule(newModule))
		mwidget->theStructure()->deleteSelected();
}

CreateModuleTool::CreateModuleTool(ModuleWidget *mwidget,
     const Arts::ModuleInfo& minfo) : CreateTool(mwidget)
{
	this->minfo = minfo;
}

CreateModuleTool::~CreateModuleTool()
{
}

// interface creation

void CreateInterfaceTool::estimateSize()
{
	vector<Arts::PortType>::iterator pi;

	int input_width = 0;
	int output_width = 0;

	for(pi = minfo.ports.begin(); pi != minfo.ports.end(); ++pi)
	{
		if(pi->direction == Arts::input)
			input_width++;

		if(pi->direction == Arts::output)
			output_width++;
	}
	width = max(input_width, output_width);
	height = 5;
}

void CreateInterfaceTool::performCreate(int x, int y)
{
	vector<std::string>::iterator pni;
	vector<Arts::PortType>::iterator pi;

	mwidget->theStructure()->addInheritedInterface(minfo.name.c_str());

	int input_width = 0;
	int output_width = 0;

	for(pi = minfo.ports.begin(), pni = minfo.portnames.begin();
		pi != minfo.ports.end(); ++pi, pni++)
	{
		StructurePort *port = 0;
		/*
		 * watch out: input ports (as in ports that accept data) are on the lower
		 * side of the structure, as the data flows out trough them ;)
		 */
		if(pi->direction == Arts::input)
		{
			port = mwidget->insertPort(*pi, x + input_width, y + 4);
			input_width++;
		}
		else if(pi->direction == Arts::output)
		{
			port = mwidget->insertPort(*pi, x + output_width, y);
			output_width++;
		}

		assert(port);
		port->rename(pni->c_str());
		port->inheritedInterface(minfo.name.c_str());
	}
}

CreateInterfaceTool::CreateInterfaceTool(ModuleWidget *mwidget,
     const Arts::ModuleInfo& minfo) : CreateTool(mwidget)
{
	this->minfo = minfo;

	vector<Arts::PortType>::iterator pi;
	for(pi = this->minfo.ports.begin(); pi != this->minfo.ports.end(); ++pi)
	{
		/* reverse since we're inside the interface, not outside */
		if(pi->direction == Arts::input)
			pi->direction = Arts::output;
		else if(pi->direction == Arts::output)
			pi->direction = Arts::input;
	}
}

CreateInterfaceTool::~CreateInterfaceTool()
{
}

// port creation

void CreatePortTool::estimateSize()
{
	width = height = 1;
}

void CreatePortTool::performCreate(int x, int y)
{
	mwidget->insertPort(type, x, y);
}

CreatePortTool::CreatePortTool(ModuleWidget *mwidget,
	const Arts::PortType& type) : CreateTool(mwidget)
{
	this->type = type;
}

MoveComponentsTool::MoveComponentsTool(ModuleWidget *widget, QMouseEvent *e)
	: MWidgetTool(widget),
	  lastPos(e->pos())
{
}

void MoveComponentsTool::mousePressEvent(QMouseEvent *e)
{
}

void MoveComponentsTool::mouseMoveEvent(QMouseEvent *e)
{
	int dx = (e->x() - lastPos.x())/mwidget->cellsize;
	int dy = (e->y() - lastPos.y())/mwidget->cellsize;

	if(dx == 0 && dy == 0)
		return;

	std::list<StructureComponent *>::iterator i;
	std::list<StructureComponent *> *ComponentList = mwidget->structure->getComponentList();

	for(i = ComponentList->begin();i != ComponentList->end();++i)
	{
		StructureComponent *c = *i;
		if(c->selected() && !mwidget->hasSpace(c, c->x() + dx, c->y() + dy, true))
			return;
	}

	mwidget->beginUpdate();
	for(i = ComponentList->begin();i != ComponentList->end();++i)
	{
		StructureComponent *c = *i;
		if(c->selected())
			c->move(c->x() + dx, c->y() + dy);
	}
	mwidget->endUpdate();

	lastPos.setX(lastPos.x() + dx*mwidget->cellsize);
	lastPos.setY(lastPos.y() + dy*mwidget->cellsize);
	mwidget->reRoute();
}

void MoveComponentsTool::mouseReleaseEvent(QMouseEvent *e)
{
	mwidget->leaveTool(this, true);
}

ConnectPortsTool::ConnectPortsTool(ModuleWidget *widget, ModulePort *connectingPort)
	: MWidgetTool(widget),
	  connectingPort(connectingPort)
{
	firstPos = mwidget->portPos(connectingPort) + connectingPort->clickrect.center();
}

void ConnectPortsTool::mousePressEvent(QMouseEvent *e)
{
}

void ConnectPortsTool::mouseMoveEvent(QMouseEvent *e)
{
	QPainter painter(mwidget);
	painter.setPen(Qt::white);
	mwidget->repaint(QRect(firstPos, lastPos).normalize());
	painter.drawLine(firstPos, e->pos());
	lastPos = e->pos();
}

void ConnectPortsTool::mouseReleaseEvent(QMouseEvent *e)
{
	StructureComponent *component;
	ModulePort *otherPort;
	mwidget->findAt(e->x(), e->y(), component, otherPort);
	if(otherPort && (otherPort!= connectingPort))
	{
		// user is trying to close a connection
		if(connectingPort->direction == otherPort->direction)
		{
			KMessageBox::sorry(mwidget,
							   i18n("You can only connect an IN-port with an OUT-port,\n"
									"not two ports with the same direction."));
		}
		else
		{
			if(connectingPort->direction == ModulePort::in)
			{
				if(otherPort->PortDesc.connectTo(connectingPort->PortDesc))
					mwidget->reRoute();
			} else {
				if(connectingPort->PortDesc.connectTo(otherPort->PortDesc))
					mwidget->reRoute();
			}
		}
	}
	mwidget->repaint(QRect(firstPos, lastPos).normalize());
	mwidget->leaveTool(this);
}
