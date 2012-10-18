#include <kcmodule.h>

#include "playlist.h"


extern "C"
{
	KDE_EXPORT Plugin *create_plugin()
	{
		return new SplitPlaylist();
	}
}

