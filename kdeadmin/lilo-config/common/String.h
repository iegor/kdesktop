/* String.h
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
/* Somewhat more sane string handling functions...                        *
 * Should be portable, therefore we aren't simply using QString.          */

#ifndef _STRING_H_
#define _STRING_H_ 1
#include <string>
#include <list>
#include <iostream>
#include <stdarg.h>

class String;
class StringList;

typedef std::list<String> strlist;

class String:public std::string {
public:
	String():std::string("") { }
	String(char const * const &s):std::string(s) { }
	String(std::string const &s):std::string(s) { }
	String const operator +(char const &s);
	String const operator +(char const * const s);
	bool operator ==(char s);
	bool operator !=(char s);
	operator char * () const { return cstr(); }
	char * cstr() const;
	bool cmp(char const * const s) const;
	bool casecmp(char const * const s) const;
	bool contains(String const &s, bool cs=true) const;
	int locate(String const &s, bool cs=true, unsigned int startat=0) const;
	void sprintf(const char *format, ...);
	bool readfile(String filename);
	String simplifyWhiteSpace() const;
	String left(unsigned int num=1) const;
	String right(unsigned int num=1) const;
	String mid(unsigned int start, unsigned int num=0) const;
	String &regex(String const &expr, bool cs=true) const;
	String &replace(String const &what, String const &with, bool all=true) const;
	static String escapeForRegExp(String const &s);
	char chr(unsigned int index) const { if(index>=size()) return 0; else return data()[index]; }
	int length() const { return size(); } // For compatibility with QString
	char const *latin1() const { return cstr(); } // For compatibility with QString
};

class StringList:public strlist {
public:
	StringList() { clear(); }
	StringList(String const &s);
	StringList(char **strs, int num=-1);
	bool readfile(String const &filename);
	bool writefile(String const &filename) const;
	void operator +=(String const &s) { insert(end(), s); }
	void operator +=(char const * const &s) { insert(end(), s); }
	void operator +=(StringList const &s);
	void operator +=(StringList const * const s);
	operator String() const;
	bool contains(String const &s) const;
	void remove(String const &s);
	void add(String const &s) { insert(end(), s); }
	String const &grep(String const &s) const;
	void sort(bool cs=true);
};

std::ostream &operator <<(std::ostream &os, String const &s);
std::ostream &operator <<(std::ostream &os, String const *s);
std::ostream &operator <<(std::ostream &os, StringList const &s);
std::ostream &operator <<(std::ostream &os, StringList const *s);
#endif
