/*
    Copyright (C) 2003 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef CANVASFACTORY_H
#define CANVASFACTORY_H

#include <ktrader.h>

#include <qstring.h>
#include <qptrlist.h>

namespace KSVG
{

class KSVGCanvas;
class CanvasInfo
{
public:
	KService::Ptr service;
	KSVGCanvas *canvas;
	QString name, internal;
};

class CanvasFactory
{
public:
	CanvasFactory();
	~CanvasFactory();

	static CanvasFactory *self();

	void cleanup();
	KSVGCanvas *loadCanvas(int width, int height);

	int itemInList(KSVGCanvas *canvas);
	QString internalNameFor(const QString &name);
	void deleteCanvas(KSVGCanvas *canvas);
	
	QPtrList<CanvasInfo> canvasList();

private:
	void queryCanvas();

	static CanvasFactory *s_factory;
	QPtrList<CanvasInfo> m_canvasList;
};

}

#endif

// vim:ts=4:noet
