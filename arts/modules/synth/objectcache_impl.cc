/*

    Copyright (C) 2000 Stefan Westerfeld
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

#include "artsmodulessynth.h"
#include <iostream>

using namespace Arts;
using namespace std;

namespace Arts {


class ObjectCache_impl : public ObjectCache_skel {
protected:
	typedef map<string, list<Object> *> ObjectCacheMap;
	ObjectCacheMap objects;

public:
	~ObjectCache_impl()
	{
		ObjectCacheMap::iterator i;
		for(i=objects.begin(); i != objects.end(); i++)
		{
			cout << "ObjectCache: deleting remaining " <<
				i->first << " objects" << endl;
			delete i->second;
		}
	}

	void put(Object obj, const string& name)
	{
		list<Object> *l = objects[name];

		if(l == 0) objects[name] = l = new list<Object>;
		l->push_back(obj);	
	}

	Object get(const string& name)
	{
		list<Object> *l = objects[name];
		if(l && !l->empty())
		{
			Object result = l->front();
			l->pop_front();

			return result;
		}
		return Object::null();
	}
};

REGISTER_IMPLEMENTATION(ObjectCache_impl);
}

