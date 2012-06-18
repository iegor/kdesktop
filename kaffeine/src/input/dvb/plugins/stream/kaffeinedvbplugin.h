/*
 * kaffeinedvbplugin.h
 *
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef KAFFEINEDVBPLUGIN_H
#define KAFFEINEDVBPLUGIN_H

#include <qpopupmenu.h>
#include <qstring.h>

#include <kparts/part.h>
#include <kconfig.h>

/*
 * Base-Class for Kaffeine DVB plugins.
 */

class KDE_EXPORT KaffeineDvbPlugin : public KParts::Part
{
	Q_OBJECT
public:
	KaffeineDvbPlugin(QObject* parent, const char* name);
	virtual ~KaffeineDvbPlugin();
	virtual QString pluginName() { return QString(); }

	// returns a "handle".
	virtual void* init( int /*sid*/ , int /*anum*/, int /*tnum*/, int /*fta*/ ) { return NULL; }

	virtual void process( void* /*handle*/, unsigned char* /*buf*/, int /*len*/ ) {};
	virtual void close( void* /*handle*/ ) {};

public slots:
	virtual void configDialog() {};
};

#endif /* KAFFEINEDVBPLUGIN_H */
