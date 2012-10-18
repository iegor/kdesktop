    /*

    Copyright (C) 1999 Stefan Westerfeld <stefan@space.twc.de>,
	                   Hans Meine <hans_meine@gmx.net>

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

#ifndef MWIDGET_H
#define MWIDGET_H

#include "structure.h"
#include "module.h"
#include "scomponent.h"

#include "qttableview.h"

class AutoRouter;
class MWidgetTool;

class ModuleWidget :public QtTableView, public StructureCanvas
{
	Q_OBJECT
	friend class MWidgetTool;
	friend class CreateTool;
	friend class CreateInterfaceTool;
	friend class CreateModuleTool;
	friend class CreatePortTool;
	friend class MoveComponentsTool;
	friend class ConnectPortsTool;

protected:
	int cellsize, cols, rows;
	int updateDepth;

	MWidgetTool *activeTool;
	AutoRouter *autorouter;
	Structure *structure;

// for connecting ports

	ModulePort *selectedPort;

	std::list<Module *> *ModuleList;
	std::list<QRect> UpdateList;

	void paintCell(QPainter *p, int y, int x);
	void paintCellBackground(QPainter *p, int y, int x);
	void paintConnection(QPainter *p, int x, int y, int arx, int ary);
	void paintConnections(QPainter *p, int y, int x);
	void mousePressEvent( QMouseEvent *e );
	void mouseMoveEvent( QMouseEvent *e );
	void mouseReleaseEvent( QMouseEvent *e );

	bool hasSpace(StructureComponent *c, int destx, int desty,
				  bool ignore_selected = false);

	void reRoute();
	void setSelectAll(bool newstate);

	void beginUpdate();
	void redrawCells(QRect &r);
	void redrawAll();
	void endUpdate();

// for StructureComponents:

	void redrawRect(int x, int y, int width, int height);

// for CreateTool:

	bool insertModule(Module *newModule);
	StructurePort *insertPort(const Arts::PortType& type, int x, int y);

// for this and ConnectTool:

	void findAt(int windowX, int windowY,
				StructureComponent *&component,
				ModulePort *&port);

signals:
	void portSelected( ModulePort * );
	void componentSelected( StructureComponent * );
	void modified(bool); // the bool mod. flag is always true for easier connects

public:
	Structure *theStructure();

	void addModule(const Arts::ModuleInfo& minfo);
	void addInterface(const Arts::ModuleInfo& minfo);
	void addPort(const Arts::PortType& type);
	void reInit();
	void leaveTool(MWidgetTool *tool, bool wasModified= false);

	QPoint componentPos(const StructureComponent *component) const;
	QPoint portPos(const ModulePort *port) const;

	ModuleWidget( Structure *structure, QWidget *parent = 0,
				  const char *name = 0, WFlags f = 0);

	~ModuleWidget();

public slots:
	void autoRedrawRouter();
	void setZoom(int zoom);
	void delModule();
	void selectAll();
	void unselectAll();

	void selectPort( ModulePort *port, bool newMode = true );
	void selectComponent( StructureComponent *component, bool onlyThis = true );
	void startConnection( ModulePort * );
	void portPropertiesChanged( ModulePort *port );
};

#endif // MWIDGET_H
