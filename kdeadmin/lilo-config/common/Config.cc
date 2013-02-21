/* Config.cc
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
#include "Config.h"
void ConfigFile::set(String const &key, String const &value, bool const &quote, bool const &removeIfEmpty, String const &prefix)
{
	String re="[ \t]*" + key + "[ \t]*=";
	String s=grep(re);
	if(value.empty() && removeIfEmpty) {
		if(!s.empty())
			remove(s);
	} else if(s.empty()) {
		if(quote)
			insert(end(), prefix + key + "=\"" + value + "\"");
		else
			insert(end(), prefix + key + "=" + value);
	} else {
		for(iterator it=begin(); it!=end(); it++)
			if(!(*it).regex(re).empty()) {
				if(quote)
					(*it)=prefix + key + "=\"" + value +"\"";
				else
					(*it)=prefix + key + "=" + value;
				break;
			}
	}
}
String const ConfigFile::get(String const &key, String const &dflt, bool const &unquote)
{
	String s=grep("[ \t]*" + key + "[ \t]*=").simplifyWhiteSpace();
	if(s.empty())
		return dflt;
	s=s.mid(s.locate("=")+2).simplifyWhiteSpace();
	if(unquote) {
		if(s.chr(0)=='"')
			s=s.mid(2);
		if(s.chr(s.size()-1)=='"')
			s=s.left(s.size()-1);
	}
	return s.simplifyWhiteSpace();
}
