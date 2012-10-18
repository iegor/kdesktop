#include "noatunarts.h"
#include <list>
#include <algorithm>

using namespace Arts;
using namespace std;

static bool compareArtsObjects(const Noatun::Listener &left, const Noatun::Listener &right)
{
	return left._isEqual(right);
}

list<Noatun::Listener>::iterator find(list<Noatun::Listener> &v, const Noatun::Listener &is, 
		bool (*compare)(const Noatun::Listener& left, const Noatun::Listener& right))
{
	for (list<Noatun::Listener>::iterator i=v.begin(); i!=v.end(); ++i)
	{
		if ((*compare)(is, *i))
			return i;
	}
	
	return v.end();
}

static void sendMessage(Noatun::Listener l)
{
	l.message();
}

namespace Noatun
{

class Session_impl : public Session_skel
{
	list<Listener> listeners;
	
	long mPid;
	
public:

	~Session_impl()
	{
		for_each(listeners.begin(), listeners.end(), sendMessage);
	}
	
	long pid() { return mPid; }
	void pid(long p) { mPid=p; }

	
	void addListener(Noatun::Listener listener)
	{
		listeners.push_back(listener);
	}

	void removeListener(Noatun::Listener listener)
	{
		list<Listener>::iterator i=
			find(listeners, listener, &compareArtsObjects);
		if (i!=listeners.end())
			listeners.erase(i);
	}

};

class Listener_impl : public Listener_skel
{

private:
	virtual void message()
	{
		// hmm
	}

};


REGISTER_IMPLEMENTATION(Session_impl);
REGISTER_IMPLEMENTATION(Listener_impl);


} // namespace Noatun

