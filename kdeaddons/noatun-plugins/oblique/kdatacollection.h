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

#ifndef KDATACOLLECTION_H
#define KDATACOLLECTION_H

#include <qstring.h>
#include <qstringlist.h>

class KConfig;

/**
 * This class allows you to have a set of files.  Some of which can be included
 * with your application, and some can be created by your users.
 * Examples of a use for this function can be:
 *<ul>
 * <li>Noatun's Equalizer preset:  Each Equalizer preset is in its own XML
 * file, and the user can delete, create new ones, and modify old ones</li>
 *</ul>
 *
 *<pre>
 * KDataCollection profiles("appname/ui_profiles");
 * QStringList letUserSelectOne = profiles.names();
 * QString fileToOpen = profiles.file(theOneUserSelected);
 * QString fileToWriteTo = profiles.saveFile(theOneUserSelected);
 *</pre>
 *
 * @author Charles Samuels <charles@kde.org>
 **/
class KDataCollection
{
	KConfig *mConfig;
	QString mGroup, mEntry, mDir;
	const char *mDatadir;

	struct Private;
	Private *d;

public:
	/**
	 * constructor.  This gives you most control over the destination of
	 * settings, @p dir is the second argument to locate(datadir, ...)
	 *
	 * @param datadir is what is passed to locate, this is "appdata" by default
	 **/
	KDataCollection(
			KConfig *config, const QString &group, const QString &entry,
			const char *datadir, const QString &dir
		);

	/**
	 * constructor.  This gives you most control over the destination of
	 * settings, @p dir is the second argument to locate("appdata", ...)
	 **/
	KDataCollection(
			KConfig *config, const QString &group, const QString &entry,
			const QString &dir
		);

	/**
	 * constructor.  The entry in the KConfig group will be named the same as
	 * @p dir.
	 *
	 * otherwise the same as the previous function
	 **/
	KDataCollection(
			KConfig *config, const QString &group,
			const QString &dir
		);

	/**
	 * constructor.  The group will be "KDataCollection", The entry in the
	 * KConfig group will be named the same as
	 * @p dir.
	 *
	 * otherwise the same as the previous function
	 **/
	KDataCollection(KConfig *config, const QString &dir);

	/**
	 * constructor.  the KConfig is assumed to be KGlobal::config()
	 *
	 * otherwise the same as the previous function
	 **/
	KDataCollection(const QString &dir);


	/**
	 * returns a list of existant, non hidden files
	 **/
	QStringList names() const;

	/**
	 * deletes the file if it is in KDEHOME, or marks it as hidden if it's a
	 * system file
	 **/
	void remove(const QString &name);

	/**
	 * @returns the filename for a file named @p name, if @p create
	 * is true, it will create the file if it doesn't exist, if @p create is false,
	 * it will return an empty string, unless the file already exists
	 *
	 * if you want to modify this file, you should use saveFile instead
	 **/
	QString file(const QString &name, bool create=true);

	/**
	 * @returns the filename for a file you can save into.  If @p create is
	 * false, it'll return an empty string if the file doesn't already exist in
	 * KDEHOME
	 *
	 * This function will not create the file, only return what the name is in
	 * theory.
	 *
	 * It will not return a file if the Kiosk framework claims that it's
	 * restricted
	 **/
	QString saveFile(const QString &name, bool create=true);

private:
	void init(
			KConfig *config, const QString &group, const QString &entry,
			const char *datadir, const QString &dir
		);
};

#endif
