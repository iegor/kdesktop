#ifndef RENDERERS_H
#define RENDERERS_H

#include <qstring.h>

class Renderer;

namespace Creators
{



#ifdef RENDERERS_CPP 
#define REGISTER(function, cl) \
Renderer *function() \
{ \
	return new cl; \
}
#else
#define REGISTER(function, cl) \
Renderer *function();
#endif

REGISTER(fade, Fade)
REGISTER(doubler, Doubler)
REGISTER(waveform, HorizontalPair);
REGISTER(hartley, Hartley);

#undef REGISTER
};


#endif

