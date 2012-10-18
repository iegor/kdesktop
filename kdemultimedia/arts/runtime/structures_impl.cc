#include "artsbuilder.h"
#include "weakreference.h"
#include "moduleinfo.h"
#include "compatibility.h"
#include "sequenceutils.h"
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace Arts;

typedef WeakReference<PortDesc> PortDesc_wref;
typedef WeakReference<ModuleDesc> ModuleDesc_wref;
typedef WeakReference<StructureDesc> StructureDesc_wref;

class PortDesc_impl :virtual public Arts::PortDesc_skel {
protected:
	string _name;
	PortType _type;
	vector<PortDesc_wref> _connections;
	ModuleDesc_wref _parent;
	bool _isConnected;
	bool _hasValue;
	Any	 _value;
	long _ID;
	long _oldID;
	list<long> oldConnections;

	void removeNullConnections();

public:
	~PortDesc_impl();

	inline PortDesc self() { return PortDesc::_from_base(_copy()); }
	void constructor(ModuleDesc parent, const string& name, const PortType& type);

	void disconnectAll();
	long ID();
	ModuleDesc parent();
	string name();
	PortType type();
	bool isConnected();
	bool hasValue();
	void hasValue(bool newvalue);
	vector<PortDesc> *connections();
	float floatValue();
	void floatValue( float _new_value );

	string stringValue();
	void stringValue( const string& _new_value );

	Any value();
	void value( const Any& _new_value );

	bool connectTo( PortDesc port );
	void internalConnectInput( PortDesc port );
	void disconnectFrom( PortDesc port );

	void loadFromList(const vector<string>& list);
	vector<string> *saveToList();

	void internalReConnect( const vector<PortDesc>& allports );
	long internalOldID();
};

class ModuleDesc_impl : virtual public ModuleDesc_skel {
private:
	long _ID;
	StructureDesc_wref _parent;
	string _name;
	long _x, _y;
	vector<PortDesc> _ports;
	long collectPorts( const Arts::ModuleInfo& info );

	bool _isInterface, _isStructure;

	inline ModuleDesc self() { return ModuleDesc::_from_base(_copy()); }

public:
	long ID();
	StructureDesc parent();
	string name();
	vector<PortDesc> *ports();
	long height();
	long width();
	long x();
	long y();
	bool moveTo( long x, long y );
	void constructor( StructureDesc parent, const ModuleInfo& info );

	void loadFromList(const vector<string>& list);
	vector<string> *saveToList();

	~ModuleDesc_impl();

	bool isInterface();
	bool isStructure();
	Arts::PortDesc findPort(const string& name);
};

class StructureDesc_impl : virtual public Arts::StructureDesc_skel {
protected:
	bool _valid;
	vector<ModuleDesc> _modules;
	vector<StructurePortDesc> _ports;	/* only structure ports which are part of the interface */
	vector<string> _inheritedInterfaces;
	long nextID;
	ModuleInfo _externalInterface;

	inline StructureDesc self() { return StructureDesc::_from_base(_copy()); }
public:
	string name();
	void name(const string& newName);	

	vector<string> *saveToList();
	void loadFromList(const vector<string>& list);
	vector<ModuleDesc> *modules();
	vector<StructurePortDesc> *ports();
	vector<string> *inheritedInterfaces();

	void clear();
	long obtainID();
	long width();
	long height();
	bool valid();

	ModuleDesc createModuleDesc( const ModuleInfo& info );
	ModuleDesc createModuleDesc( const string& name );
	void freeModuleDesc(ModuleDesc moduledesc);

	// external interface
	StructurePortDesc createStructurePortDesc(const PortType& type, const string& name);
	void freeStructurePortDesc(StructurePortDesc portdesc);
	void moveStructurePortDesc(StructurePortDesc portdesc, long newposition);

	ModuleInfo externalInterface();

	void addInheritedInterface(const string& iface);
	void removeInheritedInterface(const string& iface);

	StructureDesc_impl();
	~StructureDesc_impl();
};

class StructurePortDesc_impl :
	virtual public PortDesc_impl,
	virtual public StructurePortDesc_skel
{
protected:
	StructureDesc_wref _parentStructure;
	long _x, _y, _position;
	string _inheritedInterface;

	inline StructurePortDesc self() {
		return StructurePortDesc::_from_base(_copy());
	}
public:
	void constructor(StructureDesc parent, const string& name,
					const PortType& type);
	~StructurePortDesc_impl();

	long x();
	long y();
	long position();
	void lowerPosition();
	void raisePosition();
	void rename(const string& newname);
	string inheritedInterface();
	void inheritedInterface(const string& iface);

	void internalSetPosition(long position);
	StructureDesc parentStructure();
	bool moveTo( long X, long Y );

	void loadFromList(const vector<string>& list);
	vector<string> *saveToList();
};

REGISTER_IMPLEMENTATION(PortDesc_impl);
REGISTER_IMPLEMENTATION(ModuleDesc_impl);
REGISTER_IMPLEMENTATION(StructureDesc_impl);
REGISTER_IMPLEMENTATION(StructurePortDesc_impl);

/*
#include "structures.h"
*/
#include "debug.h"
#include <vector>
#include <algorithm>

#define dname(dir) ((dir)==Arts::input?"input":"output")
#define pstat \
	printf("port name %s, direction %s, id %d\n",_Name.c_str(),dname(_Type.direction),_ID);

void PortDesc_impl::constructor(ModuleDesc parent, const string& name,
													const PortType& type)
{
#if 0
	if(parent)
	{
		char * pname = parent->Name();
		describe("PortDesc."+string(pname)+string(".")+name);
	}
	else
	{
		describe("PortDesc.Structure."+name);
	}
#endif
	_name = name;
	_type = type;
	_parent = parent;
	_isConnected = false;
	_hasValue = false;
	_value.type = _type.dataType;

	if(!parent.isNull())
	{
		StructureDesc sd = parent.parent();
		_ID = sd.obtainID();
	}
	// else: assume that some smart object which derives from us will set the ID accordingly
	//  -> for instance StructurePortDesc_impl does so
}

#if 0 /* PORT */
void PortDesc_impl::cleanUp()
{
	disconnectAll();
	delete _Connections;
}
#endif

/*
 * This is new and related to weak references, it purges all null references from _connections
 */
void PortDesc_impl::removeNullConnections()
{
	vector<PortDesc_wref>::iterator i = _connections.begin();

	while(i != _connections.end())
	{
		PortDesc pd = *i;
		if(pd.isNull())
		{
			_connections.erase(i);
			i = _connections.begin();
			printf("removeNullConnections() removed something (shouldn't happen)\n");
		}
		else i++;
	}

	_isConnected = !_connections.empty();
}

void PortDesc_impl::disconnectAll()
{
	// disconnect all connected ports
	while(!_connections.empty())
	{
		PortDesc pd = _connections.front();

		if(pd.isNull())		// weak references can automatically disappear
			_connections.erase(_connections.begin());
		else
			pd.disconnectFrom(self());
	}
}

PortDesc_impl::~PortDesc_impl()
{
}

// Implementation for interface PortDesc
long PortDesc_impl::ID()
{
	return _ID; 
}

ModuleDesc PortDesc_impl::parent()
{
	return _parent; 
}

string PortDesc_impl::name()
{
	return _name;
}

PortType PortDesc_impl::type()
{
	return _type; 
}

bool PortDesc_impl::isConnected()
{
	if(_isConnected) removeNullConnections();
	return _isConnected; 
}

bool PortDesc_impl::hasValue()
{
	return _hasValue; 
}

void PortDesc_impl::hasValue(bool newvalue)
{
	if(_hasValue != newvalue)
	{
		assert(newvalue == false);
		_hasValue = newvalue;
	}
}

vector<PortDesc> *PortDesc_impl::connections()
{
	vector<PortDesc_wref>::iterator i;
	vector<PortDesc> *result = new vector<PortDesc>;

	for(i = _connections.begin(); i != _connections.end(); i++)
	{
		PortDesc pd = *i;
		if(!pd.isNull()) result->push_back(pd);
	}
	return result; 
}

float PortDesc_impl::floatValue()
{
	assert(_hasValue);
	assert(_type.dataType == "float");

	Buffer b;
	b.write(_value.value);
	return b.readFloat();
}

void PortDesc_impl::floatValue( float _new_value )
{
	assert(!_isConnected);
	assert(_type.direction == Arts::input);
	assert(_type.dataType == "float");
	assert(_value.type == "float");

	Buffer b;
	b.writeFloat(_new_value);
	b.read(_value.value, b.size());
	_hasValue = true;
}

string PortDesc_impl::stringValue()
{
	assert(_hasValue);
	assert(_type.dataType == "string");
	assert(_value.type == "string");

	string result;
	Buffer b;
	b.write(_value.value);
	b.readString(result);
	return result;
}

void PortDesc_impl::stringValue( const string& _new_value )
{
	assert(!_isConnected);	// shouldn't happen, but check anyway
	assert(_type.direction == Arts::input);
	assert(_type.dataType == "string");

	Buffer b;
	b.writeString(_new_value);
	b.read(_value.value, b.size());
	_hasValue = true;
}

Any PortDesc_impl::value()
{
	assert(_hasValue);
	return _value;
}

void PortDesc_impl::value( const Any& _new_value )
{
	_value = _new_value;
	_hasValue = true;
}

bool PortDesc_impl::connectTo( PortDesc port )
{
	removeNullConnections();

	// check if we are already connected to that port:

	unsigned long i;
	for(i=0;i<_connections.size();i++)
	{
		PortDesc pd = _connections[i];
		if(pd.ID() == port.ID()) return true;
	}

	const PortType& rType = port.type();

	// only stream or event channels may be connected
	if( _type.connType != rType.connType )
		return false;

	// TODO: eventually check conditions when it is legal to connect property
	// ports, and when it is insane (_Type.connType == Arts::property)
	//
	// for incoming structure ports, for instance, it is perfectly allright

	// only same data type connections allowed
	if( _type.dataType != rType.dataType )
		return false;

	// only opposite directions
	if( _type.direction == rType.direction )
		return false;

	// always first connect the input port to the output port and
	// then the other direction

	if( _type.direction == Arts::input )
	{
		if(!_isConnected || _type.isMultiPort)
		{
			assert(_connections.empty() || _type.isMultiPort);
			_connections.push_back(port);

			port.internalConnectInput(self());

			_isConnected = true;
			_hasValue = false;
			return true;
		}
	}
	if( _type.direction == Arts::output )
		return port.connectTo(self());

	return false;
}

void PortDesc_impl::internalConnectInput( PortDesc port )
{
	_connections.push_back(port);
	_isConnected = true;
}

void PortDesc_impl::disconnectFrom( PortDesc port )
{
	removeNullConnections();

	unsigned long found = 0;

	artsdebug("port %ld disconnecting from port %ld\n",ID(),port.ID());

	vector<PortDesc_wref>::iterator i = _connections.begin();
	while(!found && i != _connections.end())
	{
		Arts::PortDesc other = *i;
		if(!other.isNull() && other.ID() == port.ID())
		{
			_connections.erase(i);
			i = _connections.begin();
			found++;
		}
		else i++;
	}

	_isConnected = !_connections.empty();

	ModuleDesc parent = _parent;
	if(parent.isNull())
		artsdebug("_Parent = <some structure>, isConnected = %d\n",_isConnected);
	else
		artsdebug("_Parent = %s, isConnected = %d\n",parent.name().c_str(),_isConnected);

	if(found)
		port.disconnectFrom(self());
}

// Implementation for interface ModuleDesc
long ModuleDesc_impl::ID()
{
	return _ID; 
}

StructureDesc ModuleDesc_impl::parent()
{
	return _parent; 
}

string ModuleDesc_impl::name()
{
	return _name;
}

vector<PortDesc> *ModuleDesc_impl::ports()
{
	return new vector<PortDesc>(_ports);
}

long ModuleDesc_impl::x()
{
	return _x;
}

long ModuleDesc_impl::y()
{
	return _y;
}

long ModuleDesc_impl::width()
{
	assert(false);
	return 0; 
}

long ModuleDesc_impl::height()
{
	assert(false);
	return 0; 
}


bool ModuleDesc_impl::moveTo( long x, long y )
{
	// FIXME: collision checking!
	_x = x;
	_y = y;

	return(true);
}


// Implementation for interface StructureDesc
long StructureDesc_impl::width()
{
	assert(false);
	return 0; 
}

long StructureDesc_impl::height()
{
	assert(false);
	return 0; 
}

/*
 * Query the module for it's paramenters
 */

void ModuleDesc_impl::constructor( StructureDesc parent,
								const Arts::ModuleInfo& info )
{
	_name = info.name;
	_x = -1;		// no position assigned
	_y = -1;
	_ID = parent.obtainID();
	_parent = parent;
	_isInterface = info.isInterface;
	_isStructure = info.isStructure;

	collectPorts(info);
}

ModuleDesc_impl::~ModuleDesc_impl()
{
}

bool ModuleDesc_impl::isInterface()
{
	return _isInterface;
}

bool ModuleDesc_impl::isStructure()
{
	return _isStructure;
}


PortDesc ModuleDesc_impl::findPort(const string& name)
{
	vector<PortDesc>::iterator p;

	for(p = _ports.begin(); p != _ports.end(); p++)
	{
		if(name == p->name()) return *p;
	}

	return PortDesc::null();
}

long ModuleDesc_impl::collectPorts( const Arts::ModuleInfo& info )
{
	vector<PortType>::const_iterator i;
	vector<string>::const_iterator ni = info.portnames.begin();
	long portcount = 0;

	for(i=info.ports.begin(); i != info.ports.end(); i++)
	{
		const PortType& porttype = *i;
		const string& portname = *ni++;

		artsdebug("#%d: %s\n",portcount,portname.c_str());

		PortDesc pd(self(),portname,porttype);
		_ports.push_back(pd);
		portcount++;
	}
	return(portcount);
}

ModuleDesc StructureDesc_impl::createModuleDesc( const ModuleInfo& info )
{
	Arts::ModuleDesc result = createModuleDesc(info.name);

	assert(!result.isNull());
	return result;
}

ModuleDesc StructureDesc_impl::createModuleDesc( const string& name )
{
	/* FIXME: need new comment
	 * to create a representation of a specified synth module, we
	 *
	 * - create an instance of this synth module by contacting the
	 *   module server and telling him to do so (result is a C++ class)
	 *
	 * - create an instance of a ModuleDesc, and tell it to query the
	 *   module for it's parameters (result is a CORBA object)
	 *
	 * - destroy the synth module (C++ class) again and return a reference
	 *   to the CORBA object
	 */
/*
	ModuleServer<SynthModule> *MS_SynthModule;
	MS_SynthModule = (ModuleServer<SynthModule> *)SynthModule::get_MS();

	SynthModule *m = (SynthModule *)MS_SynthModule->getModule(name);
*/
#if 0
	Arts::ModuleInfo_var info = ModuleBroker->lookupModule(name);
	if(!info) return 0;
#endif
	const Arts::ModuleInfo& info = makeModuleInfo(name);
  	Arts::ModuleDesc moduledesc = ModuleDesc(self(),info);
	_modules.push_back(moduledesc);
	return moduledesc;
}

void StructureDesc_impl::freeModuleDesc(ModuleDesc moduledesc)
{
	vector<ModuleDesc>::iterator i;

	for(i=_modules.begin();i != _modules.end(); i++)
	{
		Arts::ModuleDesc current = *i;

		if(current.ID() == moduledesc.ID())
		{
			_modules.erase(i);		// will get freed automagically
			return;
		}
	}
}

vector<ModuleDesc> *StructureDesc_impl::modules()
{
	vector<ModuleDesc> *retval = new vector<ModuleDesc>(_modules);
	return(retval);
}

void StructureDesc_impl::addInheritedInterface(const string& iface)
{
	_inheritedInterfaces.push_back(iface);
}

void StructureDesc_impl::removeInheritedInterface(const string& iface)
{
	vector<string> newII;
	vector<string>::iterator ii;

	for(ii = _inheritedInterfaces.begin(); ii != _inheritedInterfaces.end(); ii++)
		if(*ii != iface)
			newII.push_back(*ii);

	_inheritedInterfaces = newII;
}

vector<string> *StructureDesc_impl::inheritedInterfaces()
{
	return new vector<string>(_inheritedInterfaces);
}

StructureDesc_impl::StructureDesc_impl()
{
	arts_debug("PORT: created structuredesc_impl");
	nextID = 0;
	_valid = true;
	_externalInterface.name = "unknown";		// FIXME
	_externalInterface.isStructure = true;
	_externalInterface.isInterface = false;
}

StructureDesc_impl::~StructureDesc_impl()
{
	artsdebug("StructureDesc released...\n");
}

long StructureDesc_impl::obtainID()
{
	return(nextID++);
}

bool StructureDesc_impl::valid()
{
	return(_valid);
}

void StructureDesc_impl::clear()
{
	_modules.clear();
	_ports.clear();
	_inheritedInterfaces.clear();
	_valid = true;
}

// "file" management

vector<string> *PortDesc_impl::saveToList()
{
	vector<string> *list = new vector<string>;

	sqprintf(list,"id=%ld",_ID);
	if(_hasValue)
	{
		if(_type.dataType == "string")
		{
			sqprintf(list,"string_data=%s",stringValue().c_str());
		}
		else if(_type.dataType == "float")
		{
			sqprintf(list,"audio_data=%2.5f",floatValue());
		}
		else
		{
			Buffer b;
			_value.writeType(b);
			sqprintf(list,"any_data=%s",b.toString("value").c_str());
		}
	}

	if(_isConnected)
	{
		vector<PortDesc_wref>::iterator i;

		for(i=_connections.begin();i != _connections.end(); i++)
		{
			Arts::PortDesc port = *i;
			if(!port.isNull()) sqprintf(list,"connect_to=%ld",port.ID());
		}
	}
	return list;
}

vector<string> *ModuleDesc_impl::saveToList()
{
	vector<string> *list = new vector<string>;
	vector<PortDesc>::iterator i;

	sqprintf(list,"id=%ld",_ID);
	sqprintf(list,"x=%ld",_x);
	sqprintf(list,"y=%ld",_y);
	for(i=_ports.begin();i != _ports.end();i++)
	{
		PortDesc pd = *i;
		sqprintf(list,"port=%s",pd.name().c_str());

		vector<string> *portlist = pd.saveToList();
		addSubStringSeq(list,portlist);
		delete portlist;
	}
	return list;
}

vector<string> *StructureDesc_impl::saveToList()
{
	vector<string> *list = new vector<string>;
	vector<ModuleDesc>::iterator mi;
	vector<StructurePortDesc>::iterator pi;
	vector<string>::iterator ii;

	sqprintf(list,"name=%s",_externalInterface.name.c_str());
	for(mi=_modules.begin();mi != _modules.end();mi++)
	{
		ModuleDesc md = *mi;
		sqprintf(list,"module=%s",md.name().c_str());

		vector<string> *modulelist = md.saveToList();
		addSubStringSeq(list,modulelist);
		delete modulelist;
	}
	for(pi=_ports.begin(); pi!=_ports.end(); pi++)
	{
		Arts::StructurePortDesc spd = *pi;
		sqprintf(list,"structureport");

		vector<string> *portlist= spd.saveToList();
		addSubStringSeq(list,portlist);
		delete portlist;
	}
	for(ii=_inheritedInterfaces.begin(); ii != _inheritedInterfaces.end(); ii++)
		sqprintf(list,"interface=%s",ii->c_str());

	return list;
}

void PortDesc_impl::internalReConnect( const vector<PortDesc>& allports )
{
	vector<PortDesc>::const_iterator i;

	for(i=allports.begin(); i != allports.end(); i++)
	{
		PortDesc pd = (*i);
		long oid = pd.internalOldID();

		if(find(oldConnections.begin(),oldConnections.end(),oid)
													!= oldConnections.end())
		{
			connectTo(pd);
		}
	}
}

long PortDesc_impl::internalOldID()
{
	return _oldID;
}

void PortDesc_impl::loadFromList(const vector<string>& list)
{
	unsigned long i;
	string cmd,param;
	for(i=0;i<list.size();i++)
	{
		if(parse_line(list[i],cmd,param))	// otherwise: empty or comment
		{
			if(cmd == "audio_data") {
				floatValue(atof(param.c_str()));
			} else if(cmd == "string_data") {
				stringValue(param);
			} else if(cmd == "any_data") {
				Buffer b;
				if(b.fromString(param,"value"))
				{
					Any any;
					any.readType(b);
					if(!b.readError() && !b.remaining())
						value(any);
				}
			} else if(cmd == "id") {
				_oldID = atol(param.c_str());
			} else if(cmd == "connect_to") {
				oldConnections.push_back(atol(param.c_str()));
			}
		}
	}
}

void ModuleDesc_impl::loadFromList(const vector<string>& list)
{
	artsdebug("mlist-----------\n");
	unsigned long i;
	string cmd, param;
	for(i=0;i<list.size();i++)
	{
		if(parse_line(list[i],cmd,param))	// otherwise: empty or comment
		{
			artsdebug("MD: load-> cmd was %s\n",cmd.c_str());
			if(cmd == "port")
			{
				string portName =
					OldFormatTranslator::newPortName(_name,param);
				PortDesc pd = PortDesc::null();
				vector<PortDesc>::iterator pi;

				for(pi=_ports.begin(); pi != _ports.end(); pi++)
				{
					artsdebug("pdi = %s, portName = %s\n",pi->name().c_str(),
					                                   portName.c_str());
					if(pi->name() == portName) pd = *pi;
				}
				assert(!pd.isNull());

				vector<string> *plist = getSubStringSeq(&list,i);
				pd.loadFromList(*plist);
				delete plist;
			} else if(cmd == "x") {
				_x = atol(param.c_str());
				artsdebug("X set to %ld (param was %s)\n",_x,param.c_str());
			} else if(cmd == "y") {
				_y = atol(param.c_str());
				artsdebug("Y set to %ld (param was %s)\n",_y,param.c_str());
			}
		}
	}
	artsdebug("-----------mlist\n");
}

void StructureDesc_impl::loadFromList(const vector<string>& list)
{
	string cmd,param;
	unsigned long i;
	vector<PortDesc> allports;

	clear();
	_externalInterface.name = (const char *)"unknown";

	artsdebug("loadFromList; listlen = %ld\n",list.size());
	for(i=0;i<list.size();i++)
	{
		if(parse_line(list[i],cmd,param))	// otherwise: empty or comment
		{
			artsdebug("SD: load-> cmd was %s\n",cmd.c_str());
			if(cmd == "module")
			{
				string newName = OldFormatTranslator::newModuleName(param);
				ModuleDesc md = createModuleDesc(newName);

				vector<string> *mlist = getSubStringSeq(&list,i);

				if(!md.isNull())
				{
					md.loadFromList(*mlist);

					// PORT: order changed
					vector<PortDesc> *pd = md.ports();
					vector<PortDesc>::iterator pi;
					for(pi = pd->begin(); pi != pd->end();pi++) 
						allports.push_back(*pi);

					delete pd;
				}
				else
				{
					// module couldn't be found
					_valid = false;
				}
				delete mlist;
			}
			else if(cmd == "name")
			{
				_externalInterface.name = param;
			}
			else if(cmd == "interface")
			{
				_inheritedInterfaces.push_back(param);
			}
			else if(cmd == "structureport")
			{
				// just to have valid values to pass to the new (to be loaded)
				// port:
				PortType type;
				type.direction = Arts::input;
				type.dataType = "float";
				type.connType = Arts::conn_stream;
				type.isMultiPort = false;

				StructurePortDesc spd =
					createStructurePortDesc(type,"unknown");

				vector<string> *splist = getSubStringSeq(&list,i);
				spd.loadFromList(*splist);
				delete splist;

				// yes; this is a port as well
				allports.push_back(spd);
			}
		}
	}

	for(i=0;i<allports.size();i++)
		allports[i].internalReConnect(allports);
}

void StructureDesc_impl::name(const string& name)
{
	_externalInterface.name = name;
}

string StructureDesc_impl::name()
{
	return _externalInterface.name;
}

long extint_pscore(StructurePortDesc p)
{
	long result = p.position(); //p->X()*1000+p->Y();
	if(p.type().direction == Arts::input) result += 5000000;

	return result;
}

bool extint_port_compare(StructurePortDesc p1, StructurePortDesc p2)
{
	long p1s = extint_pscore(p1);
	long p2s = extint_pscore(p2);

	artsdebug("compare; [%s] = %d  ;  [%s] = %d\n",	p1.name().c_str(),p1s,
													p2.name().c_str(),p2s);
	return (p1s < p2s);
// return -1;
	//if(p1s == p2s) return 0;
	//return 1;
}

ModuleInfo StructureDesc_impl::externalInterface()
{
	ModuleInfo result = _externalInterface;
	vector<StructurePortDesc> sorted_ports = _ports;
	vector<StructurePortDesc>::iterator p;
	unsigned long l;
/* PORT:
	for(l=0;l<_Ports->length();l++) sorted_ports.push_back((*_Ports)[l]);
*/
	sort(sorted_ports.begin(),sorted_ports.end(),extint_port_compare);

	l = 0;
	for(p=sorted_ports.begin();p != sorted_ports.end();p++)
	{
		string pname = p->name();
		PortType ptype = p->type();

		if(ptype.direction == Arts::input)
			ptype.direction = Arts::output;
		else
			ptype.direction = Arts::input;

		artsdebug("externalInterface; sorted ports: %d => %s\n",l,pname.c_str());
		result.ports.push_back(ptype);
		result.portnames.push_back(pname);
		l++;
	}
	return result;
}

vector<StructurePortDesc> *StructureDesc_impl::ports()
{
	return new vector<StructurePortDesc>(_ports);
}

StructurePortDesc StructureDesc_impl::createStructurePortDesc(
		const Arts::PortType& type, const string& name)
{
	artsdebug("creating new port %s\n",name.c_str());
	StructurePortDesc port(self(), name, type);
	_ports.push_back(port);

	// set the Position (put it at the end of the ports)
	unsigned long i, count = 0;
	for(i=0;i<_ports.size();i++)
	{
		if(_ports[i].type().direction == type.direction) count++;
	}
	assert(count > 0);	// we just inserted one ;)
	port.internalSetPosition(count-1);
	return port;
}

void StructureDesc_impl::freeStructurePortDesc(StructurePortDesc portdesc)
{
	vector<StructurePortDesc>::iterator i;

	for(i=_ports.begin(); i != _ports.end(); i++)
	{
		if(i->ID() == portdesc.ID())
		{
			_ports.erase(i);
			return;
		}
	}
}

void StructureDesc_impl::moveStructurePortDesc(StructurePortDesc
											portdesc, long newposition)
{
	const Arts::PortType& type = portdesc.type();

	unsigned long i;
	long count = 0;
	for(i=0;i<_ports.size();i++)
	{
		if(_ports[i].type().direction == type.direction) count++;
	}

	if(newposition < 0) newposition = 0;
	if(newposition > count-1) newposition = count-1;

	if(newposition == portdesc.position()) return;

	int delta, lower, upper;

	if(newposition > portdesc.position())
	{
		// if the port gets a higher position, move all ports that
		// are between it's current position and its new position down one
		lower = portdesc.position();
		upper = newposition;
		delta = -1;
	}
	else
	{
		// if the port gets a lower position, move all ports that
		// are between it's current position and its new position up one
		lower = newposition;
		upper = portdesc.position();
		delta = 1;
	}

	for(i=0;i<_ports.size();i++)
	{
		StructurePortDesc pd  = _ports[i];
		
		if(pd.type().direction == type.direction)
		{
			if(pd.ID() != portdesc.ID() &&
				pd.position() >= lower && pd.position() <= upper)
			{
				pd.internalSetPosition(pd.position()+delta);
			}
		}

	}
	portdesc.internalSetPosition(newposition);
}

void StructurePortDesc_impl::constructor(StructureDesc parent,
								const string& name, const PortType& type)
{
	PortDesc_impl::constructor(ModuleDesc::null(),name,type);
	_parentStructure = parent;
	_ID = parent.obtainID();
	_x = 0;
	_y = 0;
	_position = 0;
}

StructurePortDesc_impl::~StructurePortDesc_impl()
{
	// this destructor is required to make some compilers (egcs-1.1.2) compile
}

long StructurePortDesc_impl::x()
{	
	return _x;
}

long StructurePortDesc_impl::y()
{
	return _y;
}

long StructurePortDesc_impl::position()
{
	return _position;
}

void StructurePortDesc_impl::lowerPosition()
{
	StructureDesc parent = _parentStructure;		// weak reference

	if(!parent.isNull())
		parent.moveStructurePortDesc(self(), _position-1);
}

void StructurePortDesc_impl::raisePosition()
{
	StructureDesc parent = _parentStructure;		// weak reference

	if(!parent.isNull())
		parent.moveStructurePortDesc(self(), _position+1);
}

void StructurePortDesc_impl::rename(const string& newname)
{
	_name = newname;
}

void StructurePortDesc_impl::inheritedInterface(const string& iface)
{
	_inheritedInterface = iface;
}

string StructurePortDesc_impl::inheritedInterface()
{
	return _inheritedInterface;
}

// only used by the structure to reorder the ports
void StructurePortDesc_impl::internalSetPosition(long position)
{
	_position = position;
}

StructureDesc StructurePortDesc_impl::parentStructure()
{
	return _parentStructure;
}

bool StructurePortDesc_impl::moveTo( long X, long Y )
{
	// FIXME: check space
	_x = X;
	_y = Y;

	return true;
}

/*
  override load & save behaviour this kind of port requires that we save the type
  of the port as well, that means all of the porttype:

	enum PortDirection {input, output};
	enum PortDataType {audio_data, string_data};
	enum PortConnType {stream, event, property};
	struct PortType {
		PortDirection direction;
		PortDataType dataType;
		PortConnType connType;
	};

  so when saved, it will look like that: 

	{
      name=fasel
      x=4
      y=2
	  type
	  {
	    direction=input/output
	    datatype=audio/string
	    conntype=stream/event/property
	  }
	  data
      {
	    [original port saves here]
	  }
	}
*/

PortType loadTypeFromList(const vector<string>& list)
{
	unsigned long i,loadstate = 0;
	string cmd,param;
	Arts::PortType result;

	for(i=0;i<list.size();i++)
	{
		if(parse_line(list[i],cmd,param))	// otherwise: empty or comment
		{
			artsdebug("PortType: load-> cmd was %s\n",cmd.c_str());
			if(cmd == "direction")
			{
				if(param == "input") {
					result.direction = Arts::input;
				}
				else if(param == "output") {
					result.direction = Arts::output;
				}
				else assert(false);

				loadstate += 1;
			} else if(cmd == "datatype") {
				if(param == "audio") {
					result.dataType = "float";
				}
				else if(param == "string") {
					result.dataType = "string";
				}
				else assert(false);

				loadstate += 100;
			} else if(cmd == "conntype") {
				if(param == "stream") {
					result.connType = Arts::conn_stream;
				}
				else if(param == "event") {
					result.connType = Arts::conn_event;
				}
				else if(param == "property") {
					result.connType = Arts::conn_property;
					artsdebug("got property stuff\n");
				}
				else assert(false);

				loadstate += 10000;
			}
		}
	}
	assert(loadstate == 10101); // should see every member exactly once
	result.isMultiPort = false;
	return result;
}

void StructurePortDesc_impl::loadFromList(const vector<string>& list)
{
	artsdebug("structureportlist-----------\n");
	unsigned long i;
	string cmd,param;
	vector<string> *typelist = 0, *datalist = 0;
	bool haveType = false, haveData = false;
		// need both to do restore, type first

	for(i=0;i<list.size();i++)
	{
		if(parse_line(list[i],cmd,param))	// otherwise: empty or comment
		{
			artsdebug("StructurePortDesc: load-> cmd was %s\n",cmd.c_str());
			if(cmd == "type")
			{
				assert(!haveType);	// only allowed once
				haveType = true;
				typelist = getSubStringSeq(&list,i);
			} else if(cmd == "data") {
				assert(!haveData);	// only allowed once
				haveData = true;
				datalist = getSubStringSeq(&list,i);
			} else if(cmd == "x") {
				_x = atol(param.c_str());
				artsdebug("X set to %ld (param was %s)\n",_x,param.c_str());
			} else if(cmd == "y") {
				_y = atol(param.c_str());
				artsdebug("Y set to %ld (param was %s)\n",_y,param.c_str());
			} else if(cmd == "position") {
				_position = atol(param.c_str());
				artsdebug("Position set to %ld (param was %s)\n",_position,
						param.c_str());
			} else if(cmd == "name") {
				_name = param;
				artsdebug("Name set to %s\n",_name.c_str());
			} else if(cmd == "interface") {
				_inheritedInterface = param;
				artsdebug("Interface set to %s\n",_inheritedInterface.c_str());
			}
		}
	}
	assert(haveType && haveData);

	_type = loadTypeFromList(*typelist);

	if(_type.connType == Arts::conn_property) artsdebug("have property here\n");
	PortDesc_impl::loadFromList(*datalist);	

	delete typelist;
	delete datalist;
	artsdebug("-----------structureportlist\n");
}

vector<string> *saveTypeToList(const PortType& type)
{
	vector<string> *list = new vector<string>;

	switch(type.direction)
	{
		case Arts::input: sqprintf(list,"direction=input");
					break;
		case Arts::output: sqprintf(list,"direction=output");
					break;
		default: assert(false);		// should never happen!
	}
	if(type.dataType == "float")
	{
		sqprintf(list,"datatype=audio");
	}
	else if(type.dataType == "string")
	{
		sqprintf(list,"datatype=string");
	}
	else
	{
		assert(false);				// should never happen!
	}
	switch(type.connType)
	{
		case Arts::conn_stream: sqprintf(list,"conntype=stream");
					break;
		case Arts::conn_event: sqprintf(list,"conntype=event");
					break;
		case Arts::conn_property: sqprintf(list,"conntype=property");
					break;
		default: assert(false);		// should never happen!
	}
	
	return list;
}

vector<string> *StructurePortDesc_impl::saveToList()
{
	vector<string> *list = new vector<string>;
	sqprintf(list,"name=%s",_name.c_str());
	sqprintf(list,"x=%ld",_x);
	sqprintf(list,"y=%ld",_y);
	sqprintf(list,"position=%ld",_position);

	if(!_inheritedInterface.empty())
		sqprintf(list, "interface=%s",_inheritedInterface.c_str());

	sqprintf(list,"type");

	vector<string> *typelist = saveTypeToList(_type);
	addSubStringSeq(list,typelist);
	delete typelist;

	sqprintf(list,"data");

	vector<string> *portlist = PortDesc_impl::saveToList();
	addSubStringSeq(list,portlist);
	delete portlist;

	return list;
}
