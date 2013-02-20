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

#ifndef RENDERER_H
#define RENDERER_H

void check_events(void);
int check_finished(void);
void init_renderer1(void);
void init_renderer2(void);
void renderer(void);
void close_renderer(void);

typedef unsigned char byte;

typedef struct t_coord {
	int x,y;
} t_coord;

typedef struct t_complex {
	float x,y;
} t_complex;

typedef struct t_interpol {
	unsigned int coord ;  /* coordinates of the top left pixel.*/
	unsigned int weight; /*32 bits = 4*8= weights of the four corners */
} t_interpol;

typedef struct t_effect {
	int num_effect;
	int x_curve;
	int curve_color;
	int curve_amplitude;
	int spectral_amplitude;
	int spectral_color;
	int mode_spectre;
	int spectral_shift;
} t_effect;

typedef struct t_screen_parametres {
	int width;
	int height;
	int scale;
	int bpp;    /*bytes per pixels. */
} t_screen_parametres;

typedef struct t_general_parametres {
    int t_between_effects;
    int t_between_colors;
} t_general_parametres;

extern t_screen_parametres scr_par;

#endif
