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

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define NB_PALETTES 5
#define wrap(a) ( a < 0 ? 0 : ( a > 255 ? 255 : a ))
#define assign_max(p,a) ( *p = ( *p > a ? *p : a ))
#define PI 3.14159

#define VIDEO_FLAGS (SDL_HWSURFACE|SDL_HWPALETTE)

void generate_colors(void);
void change_color(int old_p,int p,int w);
void compute_surface(t_interpol* vector_field);
void display_surface(void);
void blur(t_interpol* vector_field);
void spectral(t_effect* current_effect,short data[2][512]);
void line(int _x1, int _y1, int _x2, int _y2, int _c);
void curve(t_effect* current_effect);
void init_sdl(void);
void toggle_fullscreen(void);
void close_sdl(void);
#endif
