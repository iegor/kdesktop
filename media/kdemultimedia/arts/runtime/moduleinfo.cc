    /*

    Copyright (C) 2000 Stefan Westerfeld
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

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#include "moduleinfo.h"

using namespace std;

static void gatherPorts(Arts::InterfaceDef& idef, Arts::ModuleInfo& minfo,
										map<string, bool>& done)
{
	Arts::InterfaceRepo interfaceRepo=Arts::Dispatcher::the()->interfaceRepo();

	vector<string>::iterator ii = idef.inheritedInterfaces.begin();
	while(ii != idef.inheritedInterfaces.end())
	{
		Arts::InterfaceDef inherited = interfaceRepo.queryInterface(*ii++);
		gatherPorts(inherited,minfo,done);
	}

	if(idef.name == "Arts::Object" || idef.name == "Arts::SynthModule")
	{
		// don't gather members of these basic interfaces as ports
		return;
	}
	vector<Arts::AttributeDef>::iterator i;
	for(i=idef.attributes.begin(); i != idef.attributes.end(); i++)
	{
		// 1 = direction, 10000 = connectiontype
		long complete = 0;
		Arts::PortType ptype;

		if(i->flags & Arts::streamIn)
		{
			ptype.direction = Arts::input;
			complete += 1;
		}
		else if(i->flags & Arts::streamOut)
		{
			ptype.direction = Arts::output;
			complete += 1;
		}

		ptype.dataType = i->type;

		if(i->flags & Arts::attributeStream)
		{
			ptype.connType = Arts::conn_stream;
			complete += 10000;
		}
		else if(i->flags & Arts::attributeAttribute)
		{
			ptype.connType = Arts::conn_property;
			complete += 10000;
		}

		ptype.isMultiPort = (i->flags & Arts::streamMulti);

		if(complete == 10001 && !done[i->name])
		{
			minfo.portnames.push_back(i->name);
			minfo.ports.push_back(ptype);
			done[i->name] = true;
		}
	}
}

Arts::ModuleInfo makeModuleInfo(const string& name)
{
	Arts::InterfaceRepo interfaceRepo=Arts::Dispatcher::the()->interfaceRepo();
	Arts::InterfaceDef idef = interfaceRepo.queryInterface(name);
	Arts::ModuleInfo minfo;

	if(!idef.name.empty())
	{
		map<string,bool> done;
		minfo.name = name;
		minfo.isStructure = false;
		minfo.isInterface = false;

		gatherPorts(idef,minfo,done);
	}
	return minfo;
}

