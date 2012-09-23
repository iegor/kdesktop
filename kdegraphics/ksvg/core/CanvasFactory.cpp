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

#include <qfile.h>

#include <kdebug.h>
#include <ksimpleconfig.h>
#include <kparts/componentfactory.h>

#include "KSVGCanvas.h"
#include "CanvasFactory.h"

using namespace KSVG;

CanvasFactory *CanvasFactory::s_factory = 0;

CanvasFactory::CanvasFactory()
{
	m_canvasList.setAutoDelete(true);
}

CanvasFactory::~CanvasFactory()
{
}

CanvasFactory *CanvasFactory::self()
{
	if(!s_factory)
		s_factory = new CanvasFactory();
	
	return s_factory;
}

void CanvasFactory::cleanup()
{
	m_canvasList.clear();
}

void CanvasFactory::queryCanvas()
{
	m_canvasList.clear();

	QValueList<KService::Ptr> traderList = KTrader::self()->query("KSVG/Renderer", "(Type == 'Service')");
	KTrader::OfferList::Iterator it(traderList.begin());
	for( ; it != traderList.end(); ++it)
	{
		KService::Ptr ptr = (*it);
			
		QString name = ptr->property("Name").toString();
		QString internal = ptr->property("X-KSVG-InternalName").toString();
		if(name.isEmpty() || internal.isEmpty())
			continue;

		CanvasInfo *cinfo = new CanvasInfo();
		cinfo->service = ptr;
		cinfo->canvas = 0;
		cinfo->name = name;
		cinfo->internal = internal;
	
		m_canvasList.append(cinfo);
	}

	if(m_canvasList.isEmpty())
	{
		kdError(26001) << "Couldn't load any canvas!!! FATAL ERROR." << endl;
		return;
	}
}

KSVGCanvas *CanvasFactory::loadCanvas(int width, int height)
{
	queryCanvas();

	KSimpleConfig *config = new KSimpleConfig("ksvgpluginrc", false);
	config->setGroup("Canvas");
	QString load = config->readEntry("ActiveCanvas", "libart");
	delete config;

	QPtrListIterator<CanvasInfo> it(m_canvasList);
	CanvasInfo *info = it.current();
	while((info = it.current()) != 0)
	{
		if(info->internal == load)
		{
			QStringList args;
			args.prepend(QString::number(width));
			args.prepend(QString::number(height));
		
			info->canvas = KParts::ComponentFactory::createInstanceFromLibrary<KSVGCanvas>(QFile::encodeName(info->service->library()), 0, 0, args);

			if(info->canvas)
				return info->canvas;
			else
			{
				kdError(26001) << "Failed to load canvas: " << load << " FATAL ERROR." << endl;
				break;
			}
		}

		++it;
	}

	return 0;
}

int CanvasFactory::itemInList(KSVGCanvas *canvas)
{
	QPtrListIterator<CanvasInfo> it(m_canvasList);
	CanvasInfo *info = it.current();
	unsigned int i = 0;
	while((info = it.current()) != 0)
	{
		if(info->canvas == canvas)
			return i;

		i++;
		++it;
	}

	return 0;
}

QString CanvasFactory::internalNameFor(const QString &name)
{
	QPtrListIterator<CanvasInfo> it(m_canvasList);
	CanvasInfo *info = it.current();
	while((info = it.current()) != 0)
	{
		if(info->name == name)
			return info->internal;

		++it;
	}

	return QString::null;
}	

void CanvasFactory::deleteCanvas(KSVGCanvas *canvas)
{
	QPtrListIterator<CanvasInfo> it(m_canvasList);
	CanvasInfo *info = it.current();
	while((info = it.current()) != 0)
	{
		if(info->canvas == canvas)
		{
			delete info->canvas;
			info->canvas = 0;
		}

		++it;
	}
}
	
QPtrList<CanvasInfo> CanvasFactory::canvasList()
{
	return m_canvasList;
}

// vim:ts=4:noet
