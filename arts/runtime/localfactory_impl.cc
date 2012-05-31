#include "artsbuilder.h"

using namespace Arts;
using namespace std;

class LocalFactory_impl : virtual public LocalFactory_skel {
public:
	Object createObject(const string& name)
	{
		return SubClass(name);
	}
};

REGISTER_IMPLEMENTATION(LocalFactory_impl);

