/*
    Copyright (C) 2001-2003 KSVG Team
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
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <kaboutdata.h>
#include <kinstance.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include "ksvg_plugin.h"
#include "ksvg_factory.moc"

#include <kdebug.h>
#include <qstringlist.h>
#include <qregexp.h>

extern "C"
{
	void *init_libksvgplugin()
	{
		KGlobal::locale()->insertCatalogue("ksvgplugin");
		return new KSVGPluginFactory();
	}
}

KInstance *KSVGPluginFactory::s_instance = 0;
KAboutData *KSVGPluginFactory::s_about = 0;

KSVGPluginFactory::KSVGPluginFactory(QObject *parent, const char *name) : KParts::Factory(parent, name)
{
}

KSVGPluginFactory::~KSVGPluginFactory()
{
 	delete s_instance;
	s_instance = 0;
	delete s_about;
	
	s_about = 0;
}

KParts::Part *KSVGPluginFactory::createPartObject(QWidget *parentWidget, const char *wname, QObject *parent, const char *name, const char *, const QStringList &args)
{
	// Get the width and height of the <embed>
	// TODO : <object>
	unsigned int width = 0, height = 0;
	bool dummy;
	QRegExp r1("(WIDTH)(\\s*=\\s*\")(\\d+)(\\w*)(\")");
	QRegExp r2("(HEIGHT)(\\s*=\\s*\")(\\d+)(\\w*)(\")");
	for(QValueListConstIterator<QString> it = args.begin(); it != args.end(); ++it) 
	{
		if(r1.search(*it) > -1)
			width = r1.cap(3).toUInt(&dummy);
		if(r2.search(*it) > -1)
			height = r2.cap(3).toUInt(&dummy);
	}
	
	return new KSVGPlugin(parentWidget, wname, parent, name, width, height);
}

KInstance *KSVGPluginFactory::instance()
{
	if(!s_instance)
	{
		s_about = new KAboutData("ksvg", I18N_NOOP("KSVG"), "0.1", "KSVG\nFreedom for veKtors",KAboutData::License_GPL_V2,"(c) 2001-2003, The KSVG Team",0,"http://svg.kde.org");
		s_about->addAuthor("Rob Buis", 0, "buis@kde.org");
		s_about->addAuthor("Nikolas Zimmermann", 0, "wildfox@kde.org");
		s_about->addCredit("Adrian Page", 0);
		s_about->addCredit("Andreas Streichardt", 0, "mop@spaceregents.de");
		s_instance = new KInstance(s_about);
	}
	
	return s_instance;
}

KSVGPluginBrowserExtension::KSVGPluginBrowserExtension(KSVGPlugin *parent)
: KParts::BrowserExtension(parent, "KSVGPlugin BrowserExtension")
{
}

KSVGPluginBrowserExtension::~KSVGPluginBrowserExtension()
{
}
