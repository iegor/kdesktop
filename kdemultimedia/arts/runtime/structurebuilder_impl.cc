    /*

    Copyright (C) 2000,2001 Stefan Westerfeld
                            stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "artsbuilder.h"
#include "artsflow.h"
#include "connect.h"
#include "debug.h"
#include "flowsystem.h"
#include "stdsynthmodule.h"
#include "dynamicrequest.h"
#include "dynamicskeleton.h"
#include "startupmanager.h"
#include <list>

//#define STRUCTBUILDER_DEBUG 1

using namespace Arts;
using namespace std;

class StructureBuilder_impl : virtual public StructureBuilder_skel {
protected:
	list<ObjectFactory> factories;
public:
	void addFactory(ObjectFactory factory);
	Object createObject(StructureDesc structure);
	ModuleDef createTypeInfo(StructureDesc structure);
};

REGISTER_IMPLEMENTATION(StructureBuilder_impl);

typedef DynamicSkeleton<SynthModule_skel> SynthModule_dskel;

class Structure_impl : virtual public SynthModule_dskel,
                       virtual public StdSynthModule {
protected:
	list<Object> structureObjects;

	struct ForwardMethod {
		string method;
		Object destObject;
		string destMethod;
	};

	list<ForwardMethod> forwardMethods;

public:
	Structure_impl(StructureDesc structure, list<ObjectFactory>& factories);
	void streamInit();
	void streamEnd();

	void process(long methodID, Buffer *request, Buffer *result);
};

void StructureBuilder_impl::addFactory(ObjectFactory factory)
{
	factories.push_back(factory);
}

ModuleDef StructureBuilder_impl::createTypeInfo(StructureDesc structure)
{
	ModuleDef md;
	InterfaceDef id;

/* convert structure to InterfaceDef id */
	md.moduleName = id.name = structure.name();
	id.inheritedInterfaces.push_back("Arts::SynthModule");

	vector<string> *otherInterfaces = structure.inheritedInterfaces();
	vector<string>::iterator ii;
	for(ii = otherInterfaces->begin(); ii != otherInterfaces->end(); ii++)
		id.inheritedInterfaces.push_back(*ii);
	delete otherInterfaces;

	vector<StructurePortDesc> *ports = structure.ports();
	vector<StructurePortDesc>::iterator pi;
	for(pi = ports->begin(); pi != ports->end(); pi++)
	{
		const Arts::PortType& type = pi->type();

		// if we inherited the port from a parent interface, we don't need to
		// list it in our interface description
		if(pi->inheritedInterface().empty())
		{
			AttributeDef ad;
			ad.name = pi->name();

			// This is a little tricky, as input ports (which are bringing data
			// from outside into the structure) are saved as output ports (and
			// output ports as input ports).
			ad.flags = AttributeType(
					((type.direction == input)?streamOut:streamIn) |
					((type.connType == conn_stream)?attributeStream:attributeAttribute)
					);
			ad.type = type.dataType;

			id.attributes.push_back(ad);
		}
	}
	delete ports;

	md.interfaces.push_back(id);

	return md;
}

namespace Arts {
static class StructureBuilderCleanUp : public StartupClass {
public:
	vector<long> types;
	void startup() { };
	void shutdown() {
		vector<long>::iterator i;
		for(i = types.begin(); i != types.end(); i++)
			Dispatcher::the()->interfaceRepo().removeModule(*i);
		types.clear();
	}
	virtual ~StructureBuilderCleanUp() {}
} structureBuilderCleanUp;
}

Object StructureBuilder_impl::createObject(StructureDesc structure)
{
	ModuleDef md = createTypeInfo(structure);

	// FIXME: find some faster way of ensuring type consistency than creating
	// the thing from scratch every time
	structureBuilderCleanUp.types.push_back(Dispatcher::the()->interfaceRepo().insertModule(md));
	Object obj = Object::_from_base(new Structure_impl(structure, factories));
	return obj;
}

Structure_impl::Structure_impl(StructureDesc structureDesc,
									list<ObjectFactory>& factories)
	: SynthModule_dskel(structureDesc.name())
{
	map<long, Object> moduleMap;
	vector<ModuleDesc> *modules = structureDesc.modules();
	vector<ModuleDesc>::iterator mi;

	// create each object
	for(mi = modules->begin(); mi != modules->end(); mi++)
	{
		ModuleDesc& md = *mi;

#ifdef STRUCTBUILDER_DEBUG
		cout << "create " << md.name() << endl;
#endif
		Object o = Object::null(); //SubClass(md.name());

		Object_skel *skel = 0;
		skel = ObjectManager::the()->create(md.name());
		if(skel) o = Object::_from_base(skel);

#ifdef STRUCTBUILDER_DEBUG
		if(o.isNull()) cout << "no local creator for " << md.name() << endl;
#endif
		list<ObjectFactory>::iterator fi = factories.begin();
		while(o.isNull() && fi != factories.end())
		{
			o = fi->createObject(md.name());
			fi++;
		}

#ifdef STRUCTBUILDER_DEBUG
		if(o.isNull()) cout << "no remote creator for " << md.name() << endl;
#endif
		assert(!o.isNull());
		moduleMap[md.ID()] = o;
		structureObjects.push_back(o);
	}

	// connect objects and set values
	for(mi = modules->begin(); mi != modules->end(); mi++)
	{
		Object& object = moduleMap[mi->ID()];

		vector<PortDesc> *ports = mi->ports();
		vector<PortDesc>::iterator pi;

		for(pi = ports->begin(); pi != ports->end(); pi++)
		{
			PortDesc& pd = *pi;
			const Arts::PortType& ptype = pd.type();

			if(pd.hasValue())
			{
				// set values
#ifdef STRUCTBUILDER_DEBUG
				cout << "value " << mi->name() << "." << pi->name() << endl;
#endif

				if(ptype.connType == conn_property)
				{
					DynamicRequest req(object);
					req.method("_set_"+pi->name());
					req.param(pd.value());

					bool requestOk = req.invoke();
					arts_assert(requestOk);
				}
				else
				{
					if(ptype.dataType == "float")
						setValue(object,pi->name(),pd.floatValue());
					else
						arts_warning("unexpected property type %s",	
													ptype.dataType.c_str());
						//setStringValue(object,pd.stringValue());
				}
			}
			else if(pd.isConnected() && ptype.direction == output)
			{
				// create connections

				vector<PortDesc> *connections = pd.connections();
				vector<PortDesc>::iterator ci;

				for(ci = connections->begin(); ci != connections->end(); ci++)
				{
					if(!ci->parent().isNull())	// structureport otherwise
					{
						Object& dest = moduleMap[ci->parent().ID()];
#ifdef STRUCTBUILDER_DEBUG
						cout << "connect " << mi->name() << "." << pi->name()
					     	 << " to " << ci->parent().name()
							 << "." << ci->name() << endl;
#endif
						connect(object,pd.name(),dest,ci->name());
					}
				}
				delete connections;
			}
		}
		delete ports;
	}
	delete modules;

	// create ports (should be done via dynamic impl class...)

	vector<StructurePortDesc> *ports = structureDesc.ports();
	vector<StructurePortDesc>::iterator pi;

	for(pi = ports->begin(); pi != ports->end(); pi++)
	{
		Arts::StructurePortDesc& pd = *pi;
		if(pd.isConnected())
		{
			// create connections

			vector<PortDesc> *connections = pd.connections();
			vector<PortDesc>::iterator ci;

			for(ci = connections->begin(); ci != connections->end(); ci++)
			{
				Object& dest = moduleMap[ci->parent().ID()];
#ifdef STRUCTBUILDER_DEBUG
				cout << "virtualize " << pi->name()
				     << " to " << ci->parent().name() << "." << ci->name()
					 << endl;
#endif

				_node()->virtualize(pd.name(),dest._node(),ci->name());

				if(pd.type().connType == conn_property)
				{
					ForwardMethod fm;
					fm.method = "_set_"+pd.name();
					fm.destObject = dest;
					fm.destMethod = "_set_"+ci->name();
					forwardMethods.push_back(fm);
				}
			}
			delete connections;
		}
	}
	delete ports;
}

void Structure_impl::streamInit()
{
	list<Object>::iterator i;

#ifdef STRUCTBUILDER_DEBUG
	cout << "vstructure: got streamInit()" << endl;
#endif

	for(i=structureObjects.begin(); i != structureObjects.end(); i++)
	{
		if(i->_base()->_isCompatibleWith("Arts::SynthModule"))
			i->_node()->start();
	}
}

void Structure_impl::streamEnd()
{
	list<Object>::iterator i;

#ifdef STRUCTBUILDER_DEBUG
	cout << "vstructure: got streamEnd()" << endl;
#endif

	for(i=structureObjects.begin(); i != structureObjects.end(); i++)
		if(i->_base()->_isCompatibleWith("Arts::SynthModule"))
			i->_node()->stop();
}

void Structure_impl::process(long methodID, Buffer *request, Buffer *result)
{
	const MethodDef& methodDef = getMethodDef(methodID);

	arts_debug("Structure_impl: got method, method ID=%ld name='%s'",
				methodID, methodDef.name.c_str());

	list<ForwardMethod>::iterator fi;
	for(fi = forwardMethods.begin(); fi != forwardMethods.end(); fi++)
	{
		if(fi->method == methodDef.name)
		{
			Any a;
			a.type = methodDef.signature[0].type;

			while(request->remaining() > 0)
				a.value.push_back(request->readByte());

			DynamicRequest(fi->destObject).method(fi->destMethod).param(a).invoke();
		}
	}
}
