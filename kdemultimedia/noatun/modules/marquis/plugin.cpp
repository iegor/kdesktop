#include "marquis.h"

extern "C"
{
	KDE_EXPORT Plugin *create_plugin()
	{
		return new Marquis();
	}
}
