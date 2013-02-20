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

#include <SDL.h>

typedef struct t_config {
    int xres,yres,sres;
    int teff,tcol;
} t_config;

extern int finished;
extern SDL_mutex *synchro_mutex,*access_mutex;
extern short pcm_data[2][512];
extern t_config config;
extern SDL_Surface *screen;
