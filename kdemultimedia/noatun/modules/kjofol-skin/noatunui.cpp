#include "kjloader.h"

extern "C"
{
	KDE_EXPORT Plugin *create_plugin()
	{
		return new KJLoader();
	}
}
