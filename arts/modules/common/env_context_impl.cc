#include "artsmodulescommon.h"
#include <debug.h>

using namespace std;

namespace Arts {
namespace Environment {

class Context_impl : virtual public Context_skel {
protected:
	struct ContextEntry {
		ContextEntry(const string& name, Object object)
			: name(name), object(object)
		{
		}
		ContextEntry(const ContextEntry& entry)
			: name(entry.name), object(entry.object)
		{
		}
		string name;
		Object object;
	};
	list<ContextEntry> entries;

	list<ContextEntry>::iterator findEntry(const string& name)
	{
		list<ContextEntry>::iterator i = entries.begin();
		for(i = entries.begin(); i != entries.end(); i++)
			if(i->name == name) return i;

		return entries.end();
	}

	list<ContextEntry>::iterator findEntry(Object object)
	{
		list<ContextEntry>::iterator i = entries.begin();
		for(i = entries.begin(); i != entries.end(); i++)
			if(object._isEqual(i->object)) return i;

		return entries.end();
	}


public:
	void addEntry(const string& name, Object object)
	{
		arts_return_if_fail(findEntry(name) != entries.end());
		entries.push_back(ContextEntry(name, object));
	}

	string lookupEntry(Object object)
	{
		list<ContextEntry>::iterator i = findEntry(object);

		if(i == entries.end())
			return "";
		else
			return i->name;
	}
	
	void removeEntry(Object object)
	{
		list<ContextEntry>::iterator i = findEntry(object);

		arts_return_if_fail(i != entries.end());
		entries.erase(i);
	}
};
REGISTER_IMPLEMENTATION(Context_impl);
}
}

