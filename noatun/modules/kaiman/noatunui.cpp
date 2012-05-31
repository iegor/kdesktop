#include "userinterface.h"

extern "C"
{
        KDE_EXPORT Plugin *create_plugin()
        {
                return new Kaiman();
        }
}
