/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/
// $Id: viewer.cpp 465369 2005-09-29 14:33:08Z mueller $

#include "viewer.h"

namespace KImageViewer
{
	Viewer::Viewer( QObject * parent, const char * name )
		: KParts::ReadWritePart( parent, name )
	{
	}

	Viewer::~Viewer()
	{
	}
} //namespace

#include "viewer.moc"
// vim: sw=4 ts=4
