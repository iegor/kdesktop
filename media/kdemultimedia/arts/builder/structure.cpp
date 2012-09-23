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

#include "structure.h"
#include "soundserver.h"
#include "kartsserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <kdebug.h>
#include <arts/debug.h>

using namespace std;

ExecutableStructure::ExecutableStructure()
{
}

ExecutableStructure::~ExecutableStructure()
{
	// to make destructor virtual
	// stop execution here?
}

void ExecutableStructure::stopExecute()
{
	arts_debug("TODO: PORT: freeStructure");
}

bool ExecutableStructure::isExecuting()
{
	arts_debug("TODO:PORT:isExecuting()");
	return false;
}

bool ExecutableStructure::saveSession(const char *filename)
{
	arts_debug("TODO:PORT:saveSession()");
#if 0
	assert(execID);

	ArtsCorba::StringSeq_var data;
	arts_debug("saveSession");
	if(Synthesizer->saveSession(execID,true,data))
	{
		arts_debug("ok");
		FILE *file = fopen(filename,"w");
		if(!file) return false;

		unsigned long i;
		for(i=0;i<data->length();i++) fprintf(file,"%s\n",(char *)(*data)[i]);
		fclose(file);

		return true;
	}
	arts_debug("failed");
#endif
	return false;
}

Structure::Structure() :ExecutableStructure()
{
	canvas = 0;
}

void Structure::setCanvas(StructureCanvas *canvas)
{
	this->canvas = canvas;
}

Structure::~Structure()
{
	clear();
	arts_debug("~Structure (releasing structuredesc from structure)");
}

bool Structure::startExecute()
{
#if 0
	assert(!execID);

	arts_debug("PORT: TODO startExecute()");
	// just in case synthesis has been halted before,
	// restart it and hope we'll have enough computing power now
	//Synthesizer->Reset();
	//execID = Synthesizer->createStructure(StructureDesc,preferredservers);
	assert(execID);
#endif

	/* connect to the sound server */
	Arts::SimpleSoundServer server = KArtsServer().server();

	if(server.isNull())
		return false;

	/* move a copy of the structure to the server, so that there will be
	   no latencies in querying what to connect to what */
	vector<string> *savePtr = StructureDesc.saveToList();
	Arts::StructureDesc remoteSD;
	remoteSD = Arts::DynamicCast(server.createObject("Arts::StructureDesc"));
	assert(!remoteSD.isNull());
	remoteSD.loadFromList(*savePtr);
	delete savePtr;

	/* create a structure builder on the server */
	Arts::StructureBuilder builder;
	builder = Arts::DynamicCast(server.createObject("Arts::StructureBuilder"));

	/* create a local factory - this will enable the builder to create gui qt
	   widgets (which need to reside within an qt application to work) */

	Arts::LocalFactory factory;
	builder.addFactory(factory);
	
	/* create the structure on the server */
	structure = Arts::DynamicCast(builder.createObject(remoteSD));

	if (structure.isNull())
		return false;

	structure.start();

	return true;
}

void Structure::stopExecute()
{
	
	// TODO:PORT: verify this code
	structure = Arts::SynthModule::null();
}

void Structure::publish()
{
	arts_debug("PORT: TODO publish()");
	//Synthesizer->publishStructureDesc(StructureDesc);
}

bool Structure::valid()
{
	return StructureDesc.valid();
}

string Structure::name()
{
	return StructureDesc.name();
}

void Structure::rename(const char *newname)
{
	StructureDesc.name(newname);
}

void Structure::addInheritedInterface(const char *iface)
{
	StructureDesc.addInheritedInterface(iface);
}

vector<string> *Structure::inheritedInterfaces()
{
	return StructureDesc.inheritedInterfaces();
}

void Structure::removeInheritedInterface(const char *iface)
{
	StructureDesc.removeInheritedInterface(iface);
}

void Structure::saveInto(FILE *file)
{
	vector<string> *list = StructureDesc.saveToList();

	vector<string>::iterator i;

	for(i = list->begin(); i != list->end(); i++)
		fprintf(file, "%s\n", (*i).c_str());

	delete list;
}

bool Structure::save(const char *filename)
{
	FILE *file = fopen(filename,"w");
	if (!file)
		return false;
	
	saveInto(file);
	
	fclose(file);

	return true;
}

void Structure::clear()
{
	list<StructureComponent *>::iterator ci;

	arts_debug("clc");
/*
	for(ci = ComponentList.begin(); ci != ComponentList.end(); ++ci)
		delete (*ci);

	ComponentList.erase(ComponentList.begin(), ComponentList.end());
	ModuleList.erase(ModuleList.begin(), ModuleList.end());
*/
	for(ci = ComponentList.begin(); ci != ComponentList.end(); ++ci)
		(*ci)->setSelected(true);
	deleteSelected();

	arts_debug("sdc");
	// shouldn't do much, now that we deleted every single component of
	// the structure, but we to it anyway, just to be sure.
	StructureDesc.clear();
}

void Structure::retrieve(const char *pubname)
{
	arts_debug("PORT: TODO: retrieve");
#if 0
	arts_debug("retrieve... %s",pubname);
	ArtsCorba::StructureDesc_var psd = Synthesizer->lookupStructureDesc(pubname);

	arts_debug("psdlookup ok");
	if(psd)
	{
		arts_debug("starting savetolist");
		ArtsCorba::StringSeq_var strseq=psd->saveToList();
		arts_debug("savetolist ok");
		loadFromList(strseq);
		arts_debug("loadfromlist ok");
	}
	arts_debug("retrieve... ok");
#endif
}

void Structure::load(const char *filename)
{
	ifstream infile(filename);
	string line;
	vector<string> strseq;

	while(getline(infile,line))
		strseq.push_back(line);

	loadFromList(strseq);
#if 0
	FILE *infile = fopen(filename,"r");
	ArtsCorba::StringSeq_var strseq = new ArtsCorba::StringSeq;

	char line[1024];
	unsigned long i = 0;

	while(fgets(line,1024,infile))
	{
		// cut eventual CR and/or LFs at the end of the line
		while(strlen(line) && line[strlen(line)-1] < 14)
			line[strlen(line)-1] = 0;

		strseq->length(i+1);
		(*strseq)[i++] = CORBA::string_dup(line);
	}
	fclose(infile);

	arts_debug(">>loadfromlist...");
	loadFromList(strseq);
	arts_debug("<<loadfromlist...");
#endif
}

void Structure::loadFromList(const vector<string>& strseq)
{
	assert(canvas);

	arts_debug(">>clear");
	clear();
	arts_debug("<<clear");

	StructureDesc.loadFromList(strseq);

	vector<Arts::ModuleDesc> *modules = StructureDesc.modules();
	vector<Arts::ModuleDesc>::iterator mi;

	for(mi=modules->begin(); mi != modules->end(); ++mi)
	{
		Module *m = new Module(*mi,StructureDesc,canvas);

		m->show();
		ModuleList.push_back(m);
		ComponentList.push_back(m);
	}
	delete modules;

	vector<Arts::StructurePortDesc> *ports = StructureDesc.ports();
	vector<Arts::StructurePortDesc>::iterator pi;

	for(pi=ports->begin(); pi != ports->end(); ++pi)
	{
		StructurePort *p = new StructurePort(*pi,StructureDesc,canvas);

		p->show();
		ComponentList.push_back(p);
	}
	delete ports;
}

Module *Structure::createModule(const Arts::ModuleInfo& minfo)
{
	assert(canvas);
	Module *m = new Module(minfo,StructureDesc,canvas);

	ComponentList.push_back(m);
	ModuleList.push_back(m);
	return m;
}

StructurePort *Structure::createStructurePort(const Arts::PortType& type)
{	// TODO:PORT: verify this code
#if 0
	arts_debug("TODO:PORT:createStructurePort");
#endif
// portname generation
	unsigned long portindex = 1,l,baselen;;
	char name[100];

	string namebase;
	if(type.direction == Arts::input) {
		// this is a port where our structure puts its results
		// so it is an input port, that is named out
		namebase = "out"; baselen = 3;
    } else {
		// this is a port where our structure gets data from
		// so it is an output port, that is named in
		namebase = "in"; baselen = 2;
	}

	vector<Arts::StructurePortDesc> *sps = StructureDesc.ports();

	for(l=0;l<sps->size();l++) {
		string thisname = (*sps)[l].name();
		if(strncmp(thisname.c_str(), namebase.c_str(), baselen) == 0 &&
           strlen(thisname.c_str()) > baselen)
		{
			unsigned long index2 = atol(&thisname.c_str()[baselen]);
			if(index2 >= portindex) portindex = index2+1;
		}
	}
	delete sps;

	sprintf(name,"%s%ld",namebase.c_str(),portindex);
	arts_debug("new Portname: %s",name);
	Arts::StructurePortDesc spd =
		StructureDesc.createStructurePortDesc(type,name);

	assert(canvas);
	StructurePort *s = new StructurePort(spd,StructureDesc,canvas);
	ComponentList.push_back(s);
	return s;
}

list<Module *> *Structure::getModuleList()
{
	return(&ModuleList);
}

list<StructureComponent *> *Structure::getComponentList()
{
	return(&ComponentList);
}

long Structure::countSelected()
{
	list<StructureComponent *>::iterator ci;
	long targets = 0;

	for(ci=ComponentList.begin();ci!=ComponentList.end();++ci)
		if((*ci)->selected()) targets++;

	return targets;
}

void Structure::deleteSelected()
{
	arts_debug("deleteSelected...");

	// remove module from the ModuleList
	list<Module *>::iterator mi;
	for(mi=ModuleList.begin();mi!=ModuleList.end();)
	{
		if((*mi)->selected())
		{
			mi = ModuleList.erase(mi);
		}
		else mi++;
	}

	// disconnect ports (it might be useful to get this right in the model
	// instead of doing it in the view - however at it works without end
	// user visible bugs like this)
	
	list<StructureComponent *>::iterator ci;
	list<ModulePort *> allPorts;

	for(ci = ComponentList.begin(); ci!=ComponentList.end(); ++ci)
		if((*ci)->selected())
			(*ci)->dumpPorts(allPorts);

	list<ModulePort *>::iterator pi;
	for(pi = allPorts.begin(); pi != allPorts.end(); ++pi)
		(*pi)->PortDesc.disconnectAll();

	// and from the ComponentList (the future ;)

	ci = ComponentList.begin();
	while(ci!=ComponentList.end())
	{
		if((*ci)->selected())
		{
			delete (*ci);
			ci = ComponentList.erase(ci);
		}
		else ci++;
	}

	arts_debug("deleteSelected ok.");
}

StructureComponent *Structure::componentAt(long x, long y, bool ignore_selected)
{
	list<StructureComponent *>::iterator ci;

	for(ci=ComponentList.begin();ci!=ComponentList.end();++ci)
	{
		StructureComponent *c = *ci;

		if(x >= c->x() && x < c->x()+c->width() &&
           y >= c->y() && y < c->y()+c->height())
		{
			if((c->selected() && !ignore_selected) || !c->selected()) return c;
		}
	}

	return 0;
}
