    /*

    Copyright (C) 2001 Stefan Westerfeld
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
#include "debug.h"
#include <stdlib.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <set>
#include <cstring>


using namespace Arts;
using namespace std;

namespace Arts {

class ArtsBuilderLoader_impl : virtual public ArtsBuilderLoader_skel {
protected:
	set<string> sourceDirs;

	string lastDataVersion;
	vector<TraderEntry> _traderEntries;
	vector<ModuleDef> _modules;

public:
	Object loadObject(Arts::TraderOffer offer)
	{
		StructureDesc structureDesc;

		vector<string> strseq;

		// load file
		vector<string> *filenames = offer.getProperty("File");
		if(filenames->size() == 1)
		{
			string& filename = filenames->front();
			arts_info("ArtsBuilderLoader: filename = %s", filename.c_str());

			ifstream infile(filename.c_str());
			string line;
			while(getline(infile,line)) strseq.push_back(line);
		}
		delete filenames;

		structureDesc.loadFromList(strseq);
		if(structureDesc.name() != offer.interfaceName())
		{
			arts_warning("failed (name = %s).",structureDesc.name().c_str());
			return Object::null();
		}

		StructureBuilder builder;
		builder.addFactory(LocalFactory());

		return builder.createObject(structureDesc);
	}

	vector<string> *listFiles(const string& pathname, const char *extension)
	{
		vector<string> *result = new vector<string>();

		unsigned long extlen = strlen(extension);
		DIR *dir = opendir(pathname.c_str());
		if(dir != 0)
		{
			struct dirent *de;
			while((de = readdir(dir)) != 0)
			{
				if(strlen(de->d_name) > extlen &&
						strncmp(&de->d_name[strlen(de->d_name)-extlen],
							extension,extlen) == 0)
					result->push_back(de->d_name);
			}
			closedir(dir);
		}
		return result;
	}

	void collectInterfaces(const InterfaceDef& interface,
			               map<string, bool>& implemented)
	{
		if(!implemented[interface.name])
		{
			implemented[interface.name] = true;

			vector<string>::const_iterator ii;
			for(ii = interface.inheritedInterfaces.begin();
				ii != interface.inheritedInterfaces.end(); ii++)
			{
				InterfaceDef id;
				id = Dispatcher::the()->interfaceRepo().queryInterface(*ii);
				collectInterfaces(id, implemented);
			}
		}
	}

	string getInterfacesList(const InterfaceDef& interface)
	{
		map<string, bool> implemented;
		map<string, bool>::iterator ii;
		string result;

		collectInterfaces(interface, implemented);

		for(ii = implemented.begin(); ii != implemented.end(); ii++)
			result += ii->first + ",";
		result += "Arts::Object";
		return result;
	}

	void scanArtsFile(const string& filename)
	{
		StructureDesc structureDesc;
		vector<string> strseq;

		// load file
		{
			ifstream infile(filename.c_str());
			string line;
			int inmodule = 0;

			while(getline(infile,line))
			{
				/*
				 * TODO - maybe there is a cleaner way?
				 *
				 * the following six lines are a bit hackish code to skip
				 * the module sections of the structures
				 *
				 * the problem with the module sections is this:
				 * we can't be sure that every module is known to the type
				 * system before we registered them with the type system,
				 * but as this code should be able to initially register .arts
				 * files with the type system, we can't rely that it has been
				 * done already (if we could, what would be the point of
				 * running this?)
				 */
				if(strncmp(line.c_str(), "module=", 7) == 0)
					inmodule = 1;

				if(strncmp(line.c_str(), "{", 1) == 0 && inmodule == 1)
					inmodule = 2;

				if(strncmp(line.c_str(), "}", 1) == 0 && inmodule == 2)
					inmodule = 0;

				if(inmodule == 0)
					strseq.push_back(line);
			}
		}

		structureDesc.loadFromList(strseq);
		string name = structureDesc.name();


		arts_debug("%s [%s]\n",filename.c_str(),name.c_str());

		/* add to _modules */
		StructureBuilder builder;
		ModuleDef md = builder.createTypeInfo(structureDesc);
		_modules.push_back(md);

		arts_assert(md.moduleName == name);
		arts_assert(!md.interfaces.empty());

		const InterfaceDef& id = md.interfaces.front();

		/* add to _traderEntries */
		
		TraderEntry entry;
		entry.interfaceName = name;
		entry.lines.push_back("Buildable=true");
		entry.lines.push_back("Interface="+getInterfacesList(id));
		entry.lines.push_back("Language=aRts");
		entry.lines.push_back("File="+filename);

		_traderEntries.push_back(entry);
		/*
		 * TODO: more entries like
		 * Author="Stefan Westerfeld <stefan@space.twc.de>"
		 * URL="http://www.arts-project.org"
		 * License=...
		 */
	}

	void rescan()
	{
		lastDataVersion = dataVersion();
		
		_traderEntries.clear();
		_modules.clear();

		set<string>::iterator si;
		for(si = sourceDirs.begin(); si != sourceDirs.end(); si++)
		{
			vector<string> *files = listFiles(*si, ".arts");
			vector<string>::iterator i;
			for(i = files->begin(); i != files->end(); i++)
				scanArtsFile(*si + "/" +*i);
			delete files;
		}
	}

	string dataVersion()
	{
		/*
		 * change this string if you change the loading algorithm to force
		 * rescanning even with the same data
		 */
		string result = "ArtsBuilderLoader:1.1:";

		bool first = true;

		set<string>::iterator i;
		for(i = sourceDirs.begin(); i != sourceDirs.end(); i++)
		{
			const string& filename = *i;

			if(!first) result += ",";
			first = false;

			struct stat st;
			if( stat(filename.c_str(), &st) == 0 )
			{
				char mtime[32];
				sprintf(mtime,"[%ld]",st.st_mtime);
				result += filename + mtime;
			}
			else
				result += filename + "[-1]";
		}
		return result;
	}

	vector<TraderEntry> *traderEntries()
	{
		if(dataVersion() != lastDataVersion)
			rescan();

		return new vector<TraderEntry>(_traderEntries);
	}

	vector<ModuleDef> *modules()
	{
		if(dataVersion() != lastDataVersion)
			rescan();

		return new vector<ModuleDef>(_modules);
	}

	ArtsBuilderLoader_impl()
	{
		sourceDirs.insert(EXAMPLES_DIR);

		const char *home = getenv("HOME");
		if(home) sourceDirs.insert(home+string("/arts/structures"));
	}
};

REGISTER_IMPLEMENTATION(ArtsBuilderLoader_impl);
}
