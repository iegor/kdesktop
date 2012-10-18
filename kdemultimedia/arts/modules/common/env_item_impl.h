
#ifndef env_item_impl_h
#define env_item_impl_h

#include "artsmodulescommon.h"
#include "weakreference.h"

namespace Arts {
namespace Environment {
typedef WeakReference<Container> Container_wref;
class Item_impl : virtual public Item_skel {
protected:
	Container_wref _parent;
	bool _active;

public:
	Item_impl();
	~Item_impl();

	bool active();
	Container parent();
	void setContainer(Container container);
};
}
}


#endif
