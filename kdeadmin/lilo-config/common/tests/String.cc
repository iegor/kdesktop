/*
**
** Copyright (C) 2000 by Bernhard Rosenkraenzer
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

using namespace std;

#include <String.h>
int main(int argc, char **argv)
{
	int bugs=0;
	String s="Microsoft sucks! Red Hat doesn't suck!";
	String t="Microsoft sucks! Destroy Microsoft! Microsoft must die!";
	String u="Microsoft sucks!";
	if(s.left(16)!="Microsoft sucks!") {
		cerr << "left() seems to be broken." << endl;
		bugs++;
	}
	if(s.right(21) != "Red Hat doesn't suck!") {
		cerr << "right() seems to be broken." << endl;
		bugs++;
	}
	if(s.mid(18) != 'R') {
		cerr << "mid() with one parameter seems to be broken." << endl;
		bugs++;
	}
	if(s.mid(18, 7) != "Red Hat") {
		cerr << "mid() with 2 parameters seems to be broken." << endl;
		bugs++;
	}
	if(s.regex("Red ?[Hh]at|RED ?HAT")!="Red Hat") {
		cerr << "regex() seems to be broken." << endl;
		bugs++;
	}
	if(!s.contains("Red Hat")) {
		cerr << "contains() seems to be broken." << endl;
		bugs++;
	}
	if(t.replace("Microsoft", "Windows", false) != "Windows sucks! Destroy Microsoft! Microsoft must die!") {
		cerr << "replace() [once] seems to be broken." << endl;
		bugs++;
	}
	if(t.replace("Microsoft", "Windows") != "Windows sucks! Destroy Windows! Windows must die!") {
		cerr << "replace() [all] seems to be broken." << endl;
		bugs++;
	}
	if(u.replace("Microsoft", "Windows") != "Windows sucks!") {
		cerr << "replace() [all, test case 2] seems to be broken." << endl;
		bugs++;
	}
	
	if(bugs==0)
		cout << "No bugs in String class detected." << endl;
	return bugs;
}
