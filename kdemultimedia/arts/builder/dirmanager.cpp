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

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include <qfile.h>

#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <klocale.h>

#include "dirmanager.h"

using namespace std;

const char *DirManager::mapDir()
{
	static char *d = 0;

	if(!d) d = directory("/maps",i18n("instrument map files"));
	return d;
}

const char *DirManager::sessionDir()
{
	static char *d = 0;

	if(!d) d = directory("/sessions",
		i18n("sessions (save files of the positions of all sliders/buttons)"));

	return d;
}

const char *DirManager::structureDir()
{
	static char *d = 0;

	if(!d) d = directory("/structures",i18n("structures (signal flow graphs)"));
	return d;
}

const char *DirManager::baseDir()
{
	static char *d = 0;

	if(!d) d = directory("",i18n("all aRts files/folders"));
	return d;
}

char *DirManager::directory(const char *subdir, const QString &desc)
{
	const char *home = getenv("HOME");

	if(home == 0) return strdup("");
	
	QCString dirname = QCString(home) + "/arts" + subdir;

	struct stat buf;
	if(stat(dirname.data(), &buf) == -1)
	{
		QString message;
		QString dir = QFile::decodeName(dirname);
		message = i18n("You need the folder %1.\n"
			"It will be used to store %2.\nShould I create it now?")
			.arg(dir).arg(desc);

		if(KMessageBox::questionYesNo(0,message,i18n("aRts Folder Missing"),i18n("Create Folder"),i18n("Do Not Create"))
			== KMessageBox::Yes)
		{
			KStandardDirs::makeDir(dir);
		}
	}

	return strdup(dirname.data());
}
