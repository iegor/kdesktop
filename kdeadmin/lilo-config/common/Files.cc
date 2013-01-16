/* Files.cc
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
#include "Files.h"
#include <glob.h>
#include <sys/stat.h>
#define ISSET(x,y) ((x&y)==y)
StringList &Files::glob(String name, enum Types types, bool dotfiles, bool braces)
{
	glob_t gl;
	int flags=0;
	StringList *result;
	if(dotfiles)
		flags |= GLOB_PERIOD;
	if(braces)
		flags |= GLOB_BRACE;
	::glob(name, flags, NULL, &gl);
	if(types==Any)
		result=new StringList(gl.gl_pathv, gl.gl_pathc);
	else {
		struct stat s;
		result=new StringList;
		for(unsigned int i=0; i<gl.gl_pathc; i++) {
			if(!lstat(gl.gl_pathv[i], &s)) {
				if(S_ISLNK(s.st_mode) && !ISSET(types,Link))
					continue;
				if(S_ISREG(s.st_mode) && !ISSET(types,File))
					continue;
				if(S_ISDIR(s.st_mode) && !ISSET(types,Dir))
					continue;
				if(S_ISCHR(s.st_mode) && !ISSET(types,CharDevice))
					continue;
				if(S_ISBLK(s.st_mode) && !ISSET(types,BlockDevice))
					continue;
				if(S_ISFIFO(s.st_mode) && !ISSET(types,Fifo))
					continue;
				if(S_ISSOCK(s.st_mode) && !ISSET(types,Socket))
					continue;
				result->insert(result->end(), gl.gl_pathv[i]);
			}
		}
	}
	globfree(&gl);
	return *result;
}
