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

#include "compatibility.h"
#include "debug.h"
#include <iostream>

#undef DEBUG_COMPATIBILITY

using namespace std;

string Arts::OldFormatTranslator::newModuleName(const string& module)
{
#ifdef DEBUG_COMPATIBILITY
	arts_debug("COMPAT: %s", module.c_str());
#endif

	if(module.substr(0,10) == "Interface_") return "Arts::"+module;
	if(module.substr(0,6) == "Synth_") return "Arts::"+module;
	return module;
}

string Arts::OldFormatTranslator::newPortName(const string& module, const string& port)
{
#ifdef DEBUG_COMPATIBILITY
	arts_debug("COMPAT: %s.%s", module.c_str(), port.c_str());
#endif

	if(module == "Arts::Synth_MUL") {
		if(port == "invalue") return "invalue1";
		if(port == "faktor") return "invalue2";
	}
	if(module == "Arts::Synth_ADD") {
		if(port == "invalue") return "invalue1";
		if(port == "addit") return "invalue2";
	}
	return port;
}
