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

#ifndef KSVGFactory_H
#define KSVGFactory_H

#include <kparts/factory.h>
#include <kparts/browserextension.h>

class KAboutData;
class KInstance;

class KSVGPluginFactory : public KParts::Factory
{
Q_OBJECT
public:
	KSVGPluginFactory(QObject *parent = 0, const char *name = 0);
	virtual ~KSVGPluginFactory();
	
	virtual KParts::Part *createPartObject(QWidget *parentWidget = 0, const char *widgetName = 0, QObject *parent = 0, const char *name = 0, const char *classname = "KParts::Part", const QStringList &args = QStringList());
	
	static KInstance *instance();

private:
	static KInstance *s_instance;
	static KAboutData *s_about;
};

class KSVGPluginBrowserExtension : public KParts::BrowserExtension
{
Q_OBJECT
friend class KSVGPlugin;

public:
	KSVGPluginBrowserExtension(KSVGPlugin *parent);
	virtual ~KSVGPluginBrowserExtension();
};

#endif

// vim:ts=4:noet
