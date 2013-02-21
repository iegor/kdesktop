/* Files.h
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
#ifndef _FILES_H_
#define _FILES_H_ 1
#include "String.h"
class Files {
public:
	enum Types { File	= 0x1,
		     Dir	= 0x2,
		     Link	= 0x4,
		     CharDevice	= 0x8,
		     BlockDevice= 0x10,
		     Device	= CharDevice | BlockDevice,
		     Fifo	= 0x20,
		     Socket	= 0x40,
		     All	= File | Dir | Link | Device | Fifo | Socket,
		     Any	= All
	};
	static StringList &glob(String name, enum Types types=All, bool dotfiles=true, bool braces=true);
};
#endif
