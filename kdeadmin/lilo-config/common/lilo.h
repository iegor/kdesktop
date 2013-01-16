/* lilo.h
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
#ifndef _LILO_H_
#define _LILO_H_ 1
#include "Config.h"
#include <iostream>

class liloimage:public ConfigFile {
public:
	bool isLinux() const;
};

class liloimages:public std::list<liloimage> {
public:
	void remove(String const &s);
	liloimage *find(String const &s) const;
	liloimage *operator[](String const &s) const { return find(s); }
};

class liloconf {
	friend std::ostream &operator <<(std::ostream &os, liloconf const &l);
	friend std::ostream &operator <<(std::ostream &os, liloconf const *l);
public:
	liloconf(String const &filename="/etc/lilo.conf");
	void set(StringList const &s);
	void set(String const &s);
	void writeFile(String const &filename);
	bool const isOk();
	String const liloOut();
	bool install(bool probeonly=false);
	bool probe();
	void addLinux(String const &label, String const &kernel, String const &root="", String const &initrd="", bool optional=false, String const &append="", String const &vga="", bool readonly=true, String const &literal="", String const &ramdisk="");
	void addOther(String const &name, String const &partition, bool const &optional=false, String const &chain="");
	void remove(String const &label);
	void removeKernel(String const &kernel);
	StringList const &entries() const;
	String const dflt() const;
	void setDefault(String const &dflt);
	operator String() const;
protected:
	void check() { install(true); }
	bool			ok;
	String			output;
public:
	bool			checked;
	ConfigFile		defaults;
	liloimages		images;
};
std::ostream &operator <<(std::ostream &os, liloconf const &l);
std::ostream &operator <<(std::ostream &os, liloconf const *l);
#endif
