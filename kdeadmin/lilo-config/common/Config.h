/* Config.h
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
#ifndef _CONFIGFILE_H_
#define _CONFIGFILE_H_ 1

#include "String.h"
class ConfigFile:public StringList
{
public:
	void	set(String const &key, String const &value, bool const &quote=true, bool const &removeIfEmpty=true, String const &prefix="");
	String const get(String const &key, String const &dflt="", bool const &unquote=true);
};

#endif
