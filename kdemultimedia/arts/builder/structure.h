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

#ifndef __STRUCTURE_H_
#define __STRUCTURE_H_

#include "artsbuilder.h"
#include "module.h"
#include "scomponent.h"
#include "structureport.h"
#include "artsflow.h"
#include <vector>

using namespace std;

class ExecutableStructure
{
public:
	ExecutableStructure();
	virtual ~ExecutableStructure();

	virtual bool startExecute() = 0;
	virtual void stopExecute();
	virtual bool isExecuting();
	virtual bool saveSession(const char *filename);
};

class Structure :public ExecutableStructure
{
	Arts::StructureDesc StructureDesc;
	Arts::SynthModule structure;
	StructureCanvas *canvas;

	std::list<Module *> ModuleList;
	std::list<StructureComponent *> ComponentList;

public:
	Structure();
	~Structure();

	void setCanvas(StructureCanvas *canvas);

	bool startExecute();
	void stopExecute();

	bool valid();
	void load(const char *filename);
	void retrieve(const char *pubname);
	void loadFromList(const std::vector<std::string>& strseq);
	void saveInto(FILE *file);
	bool save(const char *filename);
	void clear();
	void publish();

	long countSelected();
	void deleteSelected();

	std::string name();
	void rename(const char *newname);

	void addInheritedInterface(const char *iface);
	void removeInheritedInterface(const char *iface);
	vector<std::string> *inheritedInterfaces();

	Module *createModule(const Arts::ModuleInfo& minfo);
	StructurePort *createStructurePort(const Arts::PortType& type);

	StructureComponent *componentAt(long x, long y, bool ignore_selected);

	std::list<Module *> *getModuleList();
	std::list<StructureComponent *> *getComponentList();
};

#endif
