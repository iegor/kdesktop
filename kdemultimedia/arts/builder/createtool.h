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

#ifndef _CREATETOOL_H
#define _CREATETOOL_H

#include <qwidget.h>
#include <qcursor.h>

#include "structure.h"
#include "module.h"
#include "scomponent.h"

class ModulePort;
class ModuleWidget;

class MWidgetTool
{
protected:
	ModuleWidget *mwidget;

public:
	MWidgetTool(ModuleWidget *mwidget);
	virtual ~MWidgetTool();
	
	virtual void mousePressEvent(QMouseEvent *e) = 0;
	virtual void mouseMoveEvent(QMouseEvent *e) = 0;
	virtual void mouseReleaseEvent(QMouseEvent *e) = 0;
};

class CreateTool: public MWidgetTool
{
protected:
	QCursor oldCursor;
	bool oldMouseTracking;
	QRect componentRect;
	int width, height;

public:
	CreateTool(ModuleWidget *mwidget);
	virtual ~CreateTool();

	virtual void estimateSize() = 0;
	virtual void performCreate(int x, int y) = 0;

	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
};

class CreateModuleTool: public CreateTool
{
protected:
	Arts::ModuleInfo minfo;

public:
	CreateModuleTool(ModuleWidget *mwidget, const Arts::ModuleInfo& minfo);
	~CreateModuleTool();

	void estimateSize();
	void performCreate(int x, int y);
};

class CreateInterfaceTool: public CreateTool
{
protected:
	Arts::ModuleInfo minfo;

public:
	CreateInterfaceTool(ModuleWidget *mwidget, const Arts::ModuleInfo& minfo);
	~CreateInterfaceTool();

	void estimateSize();
	void performCreate(int x, int y);
};

class CreatePortTool: public CreateTool
{
protected:
	Arts::PortType type;

public:
	CreatePortTool(ModuleWidget *widget, const Arts::PortType& type);

	void estimateSize();
	void performCreate(int x, int y);
};

class MoveComponentsTool: public MWidgetTool
{
	QPoint lastPos;

public:
	MoveComponentsTool(ModuleWidget *widget, QMouseEvent *e);
	
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
};

class ConnectPortsTool: public MWidgetTool
{
	ModulePort *connectingPort;
	QPoint firstPos, lastPos;

public:
	ConnectPortsTool(ModuleWidget *widget, ModulePort *connectingPort);
	
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
};

#endif
