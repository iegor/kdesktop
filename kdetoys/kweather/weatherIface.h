#ifndef __WE_IFACE_H
#define __WE_IFACE_H

#include <dcopobject.h>
class weatherIface : virtual public DCOPObject
{
	K_DCOP
public:

	k_dcop:
	/** Cause KWeather to refrsh it's data **/
	virtual void refresh(QString) = 0;
};

#endif
