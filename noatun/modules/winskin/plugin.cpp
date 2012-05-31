#include <kglobal.h>
#include <klocale.h>

#include "winSkinConfig.h"
#include "waSkin.h"

extern "C" {
    KDE_EXPORT Plugin *create_plugin() {
	WaSkin *new_skin = new WaSkin();
	new WinSkinConfig(new_skin, new_skin->skinManager());
	return new_skin;
    }
}
