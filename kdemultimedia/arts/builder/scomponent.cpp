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

#include "scomponent.h"
//#include <arts/debug.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <stdio.h>

StructureComponent::StructureComponent(StructureCanvas *canvas)
: _x(0), _y(0)
{
	this->canvas = canvas;
}

StructureComponent::~StructureComponent()
{
}

void StructureComponent::hide()
{
	if(_visible)
	{
		_visible = false;
		redraw();
	}
}

void StructureComponent::show()
{
	if(!_visible)
	{
		_visible = true;
		redraw();
	}
}

bool StructureComponent::visible()
{
	return(_visible);
}

void StructureComponent::redraw()
{
	canvas->redrawRect(x(), y(), width(), height());
}

bool StructureComponent::move(int newx, int newy)
{
	bool success = moveInternal(newx, newy);
	
	if(success)
	{
		hide();
		redraw();

		_x = newx;
		_y = newy;

		show();
		redraw();
	}

	return success;
}

int StructureComponent::x() const
{
	return _x;
}

int StructureComponent::y() const
{
	return _y;
}

bool StructureComponent::selected()
{
	return _selected;
}

void StructureComponent::setSelected(bool newselection)
{
	if(newselection != _selected)
	{
		_selected = newselection;
		redraw();
	}
}

