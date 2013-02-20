/*
// Copyright (C) 2000 Julien Carme
// Copyright (C) 2001 Neil Stevens <neil@qualityassistant.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "file.h"
#include "main.h"
#include "renderer.h"
#include "compute.h"
#include "display.h"

#include <SDL.h>
#include <SDL_keysym.h>
#include <stdlib.h>

#define wrap(a) ( a < 0 ? 0 : ( a > 255 ? 255 : a ))

t_interpol* vector_field;
t_effect current_effect;
t_screen_parametres scr_par;
t_general_parametres gen_par;
int old_color=0,color=0;
int t_last_color,t_last_effect;
int mode_interactif=0;

void check_events(void)
{
	SDL_Event event;
	byte* keystate;
	int i;
	int sx,sy;
	if (mode_interactif)
	{
		SDL_GetMouseState(&sx,&sy);
		current_effect.spectral_shift=sx;
	}

	SDL_PollEvent(&event);

	if (event.type == SDL_KEYDOWN)
	{
		switch(event.key.keysym.sym)
		{
		case SDLK_RETURN:
			mode_interactif=!mode_interactif;
			break;
		case SDLK_SPACE:
			toggle_fullscreen();
			break;
		case SDLK_TAB:
			load_random_effect(&current_effect);
			t_last_effect=0;
			break;
		case SDLK_F12:
			if (t_last_color>32)
			{
				t_last_color=0;
				old_color=color;
				color=(color+1)%NB_PALETTES;
			}
			break;
		case SDLK_m:
			if (mode_interactif) save_effect(&current_effect);
			break;
		case SDLK_w:
			if (mode_interactif) current_effect.mode_spectre=(current_effect.mode_spectre+1)%5;
			break;
		default:
			break;
		}
	}
	else if (event.type == SDL_QUIT)
	{
		finished = 1;
		return;
	}

	keystate = SDL_GetKeyState(NULL);
	if (mode_interactif)
	{
		if ( keystate[SDLK_a])
			current_effect.curve_color=wrap(current_effect.curve_color-32);
		if ( keystate[SDLK_z])
			current_effect.curve_color=wrap(current_effect.curve_color+32);
		if ( keystate[SDLK_q])
			current_effect.spectral_color=wrap(current_effect.spectral_color-32);
		if ( keystate[SDLK_s])
			current_effect.spectral_color=wrap(current_effect.spectral_color+32);

		for (i=0;i<10;i++)
		{
			if ( keystate[SDLK_F1+i])
				current_effect.num_effect=i%NB_FCT;
		}

		if (keystate[SDLK_d]) {
			current_effect.spectral_amplitude=(current_effect.spectral_amplitude-1);
		}
		if (keystate[SDLK_f]) {
			current_effect.spectral_amplitude=(current_effect.spectral_amplitude+1);
		}
		if (keystate[SDLK_e]) {
			current_effect.curve_amplitude=(current_effect.curve_amplitude-1);
		}
		if (keystate[SDLK_r]) {
			current_effect.curve_amplitude=(current_effect.curve_amplitude+1);
		}
	}
}


int check_finished(void)
{
	return finished;
}


void init_renderer1(void)
{
	scr_par.width=config.xres;
	scr_par.height=config.yres;
	scr_par.scale=config.sres;

	gen_par.t_between_effects=config.teff;
	gen_par.t_between_colors=config.tcol;

	init_sdl();
}

void init_renderer2(void)
{
	generate_colors();
	load_effects();
	load_random_effect(&current_effect);

	vector_field=(t_interpol*)malloc(scr_par.width*scr_par.height*NB_FCT*sizeof(t_interpol));
	generate_vector_field(vector_field);
}


void renderer(void)
{
	while (check_finished() != 1)
	{
		check_events();
		blur(&vector_field[scr_par.width*scr_par.height*current_effect.num_effect]);
		spectral(&current_effect,pcm_data);
		curve(&current_effect);
		if (t_last_color<=32)
		{
			change_color(old_color,
				     color,
				     t_last_color*8);
		}
		t_last_color+=1;
		t_last_effect+=1;
		if (t_last_effect%gen_par.t_between_effects==0)
		{
			if (!mode_interactif) {
				load_random_effect(&current_effect);
				t_last_effect=0;
			}
		}
		if (t_last_color%gen_par.t_between_colors==0)
		{
			if (!mode_interactif) {
				old_color=color;
				color=rand()%NB_PALETTES;
				t_last_color=0;
			}
		}
	}
	/*fprintf(stderr,"exiting thread\n");*/
	close_renderer();
}

void close_renderer(void)
{
	free(vector_field);
	close_sdl();
	SDL_mutexV(synchro_mutex);
}
