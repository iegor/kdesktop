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

#include <kdebug.h>

#include "AggCanvas.h"
#include "AggCanvasFactory.h"

using namespace KSVG;

K_EXPORT_COMPONENT_FACTORY(libksvgrendereragg, AggCanvasFactory)

AggCanvasFactory::AggCanvasFactory()
{
}

AggCanvasFactory::~AggCanvasFactory()
{
}

QObject *AggCanvasFactory::createObject(QObject *, const char *, const char *, const QStringList &args)
{
	unsigned int width = (*args.at(1)).toUInt();
	unsigned int height = (*args.at(0)).toUInt();
	return new AggCanvas(width, height);
}

// vim:ts=4:noet
