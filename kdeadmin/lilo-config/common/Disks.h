/* Disks.h
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
** Contributions by A. Seigo and W. Bastian.
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#ifndef _DISKS_H_
#define _DISKS_H_ 1
#include "String.h"
#include <list>
#include <map>
class ptable {
public:
	ptable(StringList const &disks=disklist());
	ptable(String const &disk);
	static StringList disklist();
	static StringList partlist();
	static String mountpoint(String const &device, bool fstab_fallback=false);
	static String device(String const &mountpt, bool fstab_fallback=false);
protected:
	void scandisk(String const &disk);
public:
	StringList		partition;
	std::map<String,int>	id;
	std::map<String,String>	mountpt;
};
#endif
