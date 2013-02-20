/*
	This file is part of the KDE libraries
	Copyright (C) 2003 Charles Samuels <charles@kde.org>

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

#include "kdatacollection.h"

#include <kconfig.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <qfile.h>
#include <qfileinfo.h>


KDataCollection::KDataCollection(
		KConfig *config, const QString &group, const QString &entry,
		const char *datadir, const QString &dir
	)
{
	init(config, group, entry, datadir, dir);
}

KDataCollection::KDataCollection(
		KConfig *config, const QString &group, const QString &entry,
		const QString &dir
	)
{
	init(config, group, entry, "appdata", dir);
}

KDataCollection::KDataCollection(
		KConfig *config, const QString &group, const QString &dir
	)
{
	init(config, group, dir, "appdata", dir);
}

KDataCollection::KDataCollection(KConfig *config, const QString &dir)
{
	init(config, "KDataCollection", dir, "appdata", dir);
}

KDataCollection::KDataCollection(const QString &dir)
{
	init(KGlobal::config(), "KDataCollection", dir, "appdata", dir);
}

void KDataCollection::init(
		KConfig *config, const QString &group, const QString &entry,
		const char *datadir, const QString &dir
	)
{
	mConfig = config;
	mGroup = group;
	mEntry = entry;
	mDir = dir;
	mDatadir = datadir;
}

QStringList KDataCollection::names() const
{
	KConfigGroup g(mConfig, mGroup);

	// these are the entries I have
	QStringList n = g.readListEntry(mEntry);
	QStringList fs = KGlobal::dirs()->findAllResources(mDatadir, mDir+"/*", false, true);
	QStringList total;

	for (QStringList::Iterator i(fs.begin()); i != fs.end(); ++i)
	{
		QFileInfo fi(*i);
		QString name = fi.fileName();
		if (!n.contains(name))
		{
			total.append(name);
		}
	}

	return total;
}

void KDataCollection::remove(const QString &name)
{
	KConfigGroup g(mConfig, mGroup);
	QString location = file(name);
	if (location.isEmpty()) return;
	if (location == saveFile(name, false))
	{
		QFile(location).remove();
		// is there a system one too?
		location = file(name, false);
		if (location.isEmpty()) return;
	}

	QStringList n = g.readListEntry(mEntry);
	if (n.contains(name)) return;
	n.append(name);
	g.writeEntry(mEntry, n);
}

QString KDataCollection::file(const QString &name, bool create)
{
	QString path = ::locate(mDatadir, mDir+"/"+name);

	if (path.isEmpty() && create)
	{
		path = saveFile(name, true);
	}
	return path;
}

QString KDataCollection::saveFile(const QString &name, bool create)
{
	if (!KGlobal::dirs()->isRestrictedResource(mDatadir, mDir+"/"+name))
	{
		QString s = KGlobal::dirs()->saveLocation(mDatadir, mDir, create);

		if (s.length() && create)
		{
			s += "/" + name;
			QFile(s).open(IO_ReadWrite); // create it
		}
		return s;
	}
	return QString::null;
}


