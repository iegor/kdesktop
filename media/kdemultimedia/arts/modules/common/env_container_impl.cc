#include "artsmodulescommon.h"
#include "../runtime/sequenceutils.h"
#include <debug.h>

using namespace std;

namespace Arts {
namespace Environment {

class Container_impl : virtual public Container_skel {
protected:
	string _dataDirectory;
	Context _context;
	vector<Item> _items;

	Container self() { return Container::_from_base(_copy()); }
public:
	~Container_impl()
	{
		// tell items we're going to leave before actually going away
		clear();
	}
	string dataDirectory()
	{
		return _dataDirectory;
	}
	void dataDirectory(const string& newDataDirectory)
	{
		if(newDataDirectory != _dataDirectory)
		{
			_dataDirectory = newDataDirectory;
			dataDirectory_changed(newDataDirectory);
		}
	}
	Context context()
	{
		return _context;
	}
	void context(Context newContext)
	{
		_context = newContext;
	}
	vector<Item> *items()
	{
		return new vector<Item>(_items);
	}
	vector<string> *saveToList()
	{
		vector<string> *result = new vector<string>;

		vector<Item>::iterator ii;
		for(ii=_items.begin(); ii != _items.end(); ii++)
		{
			sqprintf(result,"item=%s",ii->_interfaceName().c_str());

			vector<string> *itemresult = ii->saveToList();
			addSubStringSeq(result,itemresult);
			delete itemresult;
		}
		return result;
	}

	void clear()
	{
		/* FIXME: performance ;) */
		while(!_items.empty())
			removeItem(_items.front());
	}

   	void loadFromList(const vector<string>& strlist)
	{
		string cmd,param;
		unsigned long i;

		clear();
	
		for(i=0;i<strlist.size();i++)
		{
			if(parse_line(strlist[i],cmd,param))  // otherwise: empty or comment
			{
				if(cmd == "item")
				{
					Item item = createItem(param);
					vector<string> *itemlist = getSubStringSeq(&strlist,i);

					if(!item.isNull())
						item.loadFromList(*itemlist);
					else
					{
						// error handling
						assert(false);
					}
					delete itemlist;
				}
			}
		}
	}

	vector<Item>::iterator findItem(Item item)
	{
		vector<Item>::iterator i;
		for(i = _items.begin(); i != _items.end(); i++)
			if(i->_isEqual(item)) return i;

		return _items.end();
	}

	void addItem(Item item)
	{
		vector<Item>::iterator i = findItem(item);
		arts_return_if_fail(i == _items.end());

		_items.push_back(item);
		item.setContainer(self());
	}

	Item createItem(const string& name)
	{
		Item item = SubClass(name);
		addItem(item);
		return item;
	}

	void removeItem(Item item)
	{
		vector<Item>::iterator i = findItem(item);
		arts_return_if_fail(i != _items.end());

		_items.erase(i);
		item.setContainer(Container::null());
	}
};
REGISTER_IMPLEMENTATION(Container_impl);
}
}

