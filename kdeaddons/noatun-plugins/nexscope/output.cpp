#include "nex.h"
#include <stdlib.h>

SDL_Surface *OutputSDL::surface=0;

OutputSDL::OutputSDL()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WM_SetCaption("Nex","noatun");

	bool fullscreen=false;
	Uint32 flags = SDL_SWSURFACE | (fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE);

	surface = SDL_SetVideoMode(width, height, 32, flags);

	SDL_ShowCursor(1);

}


OutputSDL::~OutputSDL()
{
	SDL_FreeSurface(surface);
}


int OutputSDL::display(Bitmap *source)
{
	memcpy(surface->pixels, source->pixels(), source->bytes());
	SDL_UpdateRect(surface, 0, 0, 0, 0);
	
	SDL_Event event;
	
	while ( SDL_PollEvent(&event) > 0 )
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return Exit;
		default:
			break;
		}
	}
	return None;
}


