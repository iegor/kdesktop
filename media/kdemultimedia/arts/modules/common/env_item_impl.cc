#include "artsmodulescommon.h"
#include "debug.h"
#include "env_item_impl.h"

using namespace Arts;
using namespace std;

Environment::Item_impl::Item_impl()
	: _active(false)
{
}

Environment::Item_impl::~Item_impl()
{
	// Items can't be deleted while they are still inside a Container
	arts_assert(_active == false);
}

Environment::Container Environment::Item_impl::parent()
{
	Container p = _parent;
	return p;
}

void Environment::Item_impl::setContainer(Environment::Container container)
{
	if(container.isNull()) // remove from container
	{
		arts_return_if_fail(_active == true);

		_parent = container;
		_active = false;
	}
	else // add to container
	{
		Container p = _parent;
		arts_return_if_fail(p.isNull() && _active == false);

		_parent = container;
		_active = true;
	}
	active_changed(_active);
}

bool Environment::Item_impl::active()
{
	return _active;
}
