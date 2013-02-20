/* Synaescope - a pretty noatun visualization (based on P. Harrison's Synaesthesia)
   Copyright (C) 1997 Paul Francis Harrison <pfh@yoyo.cc.monash.edu.au>
                 2001 Charles Samuels <charles@kde.org>
   Copyright (C) 2001 Neil Stevens <multivac@fcmail.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "syna.h"
#include <iostream>
#include <SDL_syswm.h>
#include "SDL.h"

static SDL_Surface *surface;

SDL_Surface *CreateScreen(Uint16 w, Uint16 h, Uint8 bpp, Uint32 flags)
{
	SDL_Surface *screen;

	/* Set the video mode */
	screen = SDL_SetVideoMode(w, h, bpp, flags);

	return screen;
}

void SdlScreen::setPalette(unsigned char *palette)
{
	SDL_Color sdlPalette[256];

	for(int i=0;i<256;i++) {
		sdlPalette[i].r = palette[i*3+0];
		sdlPalette[i].g = palette[i*3+1];
		sdlPalette[i].b = palette[i*3+2];
	}

	SDL_SetColors(surface, sdlPalette, 0, 256);
}

bool SdlScreen::init(int,int,int width,int height,bool fullscreen) 
{
	Uint32 videoflags;

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		//char str[1000];
		//printf(str, "Could not initialize SDL library: %s\n",SDL_GetError());
		return false;
	}

	SDL_WM_SetCaption("Synaescope","synaescope");

	/* See if we try to get a hardware colormap */
	videoflags = SDL_SWSURFACE | (fullscreen?SDL_FULLSCREEN:0);

	surface = CreateScreen(width, height, 8, videoflags);
	if (!surface)
	{
		error("requesting screen dimensions");
	}

	core->outWidth = width;
	core->outHeight = height;

	SDL_EnableUNICODE(1);
	SDL_ShowCursor(0);

	return true;
}

void SdlScreen::end(void)
{
	SDL_Quit();
}

bool SdlScreen::inputUpdate(int &mouseX,int &mouseY,int &mouseButtons,char &keyHit)
{
	SDL_Event event;

	keyHit = 0;

	while ( SDL_PollEvent(&event) > 0 ) {
		switch (event.type) {
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				if ( event.button.state == SDL_PRESSED ) 
					mouseButtons |= 1 << event.button.button;
				else	
					mouseButtons &= ~( 1 << event.button.button );
				mouseX = event.button.x;
				mouseY = event.button.y;
				break;
			case SDL_MOUSEMOTION :
				mouseX = event.motion.x;
				mouseY = event.motion.y;
				break;
			case SDL_KEYDOWN:
				/* Ignore key releases */
				if ( event.key.state == SDL_RELEASED ) {
				  break;
				}

				if(event.key.keysym.sym == SDLK_SPACE)
				{
					SDL_WM_ToggleFullScreen(surface);
					mouseButtons = 0;
				}

				if (event.key.keysym.unicode > 255)
					break;

				keyHit = event.key.keysym.unicode;
				return true;
			case SDL_QUIT:
				//keyHit = 'q';        
				//return false;
				break;
			default:
				break;
		}
	}
	return true;
}

int SdlScreen::sizeUpdate(void) { return 0; }

void SdlScreen::show(void) { 
	SDL_LockSurface(surface);

	register unsigned long *ptr2 = (unsigned long*)core->output();
	unsigned long *ptr1 = (unsigned long*)( surface->pixels );
	int i = core->outWidth*core->outHeight/4;

	do {
		// Asger Alstrup Nielsen's (alstrup@diku.dk)
		// optimized 32 bit screen loop
		register unsigned int const r1 = *(ptr2++);
		register unsigned int const r2 = *(ptr2++);

		//if (r1 || r2) {
#ifdef LITTLEENDIAN
		register unsigned int const v = 
			((r1 & 0x000000f0ul) >> 4)
			| ((r1 & 0x0000f000ul) >> 8)
			| ((r1 & 0x00f00000ul) >> 12)
			| ((r1 & 0xf0000000ul) >> 16);
		*(ptr1++) = v | 
			( ((r2 & 0x000000f0ul) << 12)
			  | ((r2 & 0x0000f000ul) << 8)
			  | ((r2 & 0x00f00000ul) << 4)
			  | ((r2 & 0xf0000000ul)));
#else
		register unsigned int const v = 
			((r2 & 0x000000f0ul) >> 4)
			| ((r2 & 0x0000f000ul) >> 8)
			| ((r2 & 0x00f00000ul) >> 12)
			| ((r2 & 0xf0000000ul) >> 16);
		*(ptr1++) = v | 
			( ((r1 & 0x000000f0ul) << 12)
			  | ((r1 & 0x0000f000ul) << 8)
			  | ((r1 & 0x00f00000ul) << 4)
			  | ((r1 & 0xf0000000ul)));
#endif
		//} else ptr1++;
	} while (--i); 

	SDL_UnlockSurface(surface);
	SDL_UpdateRect(surface, 0, 0, 0, 0);
}

int SdlScreen::winID(void)
{
	SDL_SysWMinfo info;
	info.version.major = SDL_MAJOR_VERSION;
	info.version.minor = SDL_MINOR_VERSION;
	info.version.patch = SDL_PATCHLEVEL;
	info.subsystem = SDL_SYSWM_X11;

	info.info.x11.wmwindow = 0xdeadbeef;
	SDL_GetWMInfo(&info);
	return info.info.x11.wmwindow;
}
