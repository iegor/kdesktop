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

#include "renderer.h"
#include "main.h"
#include "display.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <SDL.h>


SDL_Color color_table[NB_PALETTES][256];
short current_colors[256];

byte* surface1;
byte* surface2;

void generate_colors(void)
{
	int i,k;
	float colors[NB_PALETTES][2][3]={{{1,1,1},{1,1,1}},
					 {{2,1.5,0},{0,0.5,2}},
					 {{0,1,2},{0,1,0}},
					 {{0,2,1},{0,0,1}},
					 {{2,0,0},{0,1,1}}};

	for (k=0;k<NB_PALETTES;k++)
	{
		for ( i=0; i<128; i++ )
		{
			color_table[k][i].r = colors[k][0][0]*i;
			color_table[k][i].g = colors[k][0][1]*i;
			color_table[k][i].b = colors[k][0][2]*i;
		}

		for ( i=0; i<128; i++ )
		{
			color_table[k][i+128].r = colors[k][0][0]*127+colors[k][1][0]*i;
			color_table[k][i+128].g = colors[k][0][1]*127+colors[k][1][1]*i;
			color_table[k][i+128].b = colors[k][0][2]*127+colors[k][1][2]*i;
		}
	}
}

void change_color(int t2,int t1,int w)
{
	unsigned int i;

	for (i=0;i<255;i++)
	{
		int r,g,b;
		r = (color_table[t1][i].r*w+color_table[t2][i].r*(256-w) )>>11;
		g = (color_table[t1][i].g*w+color_table[t2][i].g*(256-w) )>>10;
		b = (color_table[t1][i].b*w+color_table[t2][i].b*(256-w) )>>11;
		current_colors[i]=(r<<11)+(g<<5)+b;
	}
}

void compute_surface(t_interpol* vector_field)
{
	int i,j;
	int add_dest=0;
	int add_src;
	t_interpol *interpol;
	register byte* ptr_pix;
	int color;
	byte* ptr_swap;

	for (j=0;j<scr_par.height;j++)
	{
		for (i=0;i<scr_par.width;i++)
		{
			interpol=&vector_field[add_dest];
			add_src=(interpol->coord & 0xFFFF)*scr_par.width+(interpol->coord>>16);
			ptr_pix=&((byte*)surface1)[add_src];
			color=(*(ptr_pix)*(interpol->weight>>24)
			       +*(ptr_pix+1)*((interpol->weight & 0xFFFFFF)>>16)
			       +*(ptr_pix+scr_par.width)*((interpol->weight & 0xFFFF)>>8)
			       +*(ptr_pix+scr_par.width+1)*(interpol->weight & 0xFF))>>8;

			if (color>255)
				surface2[add_dest]=255;
			else
				surface2[add_dest]=color;
			add_dest++;
		}
	}

	ptr_swap=surface1;
	surface1=surface2;
	surface2=ptr_swap;
}

void display_surface(void)
{
	int i,j;

	if (scr_par.scale>1)
	{
		for (i=0;i<scr_par.height;i++)
		{
			short* pdest=(short*)(screen->pixels+i*screen->pitch*scr_par.scale);
			byte* psrc=surface1+i*scr_par.width;

			if (scr_par.scale==2)
			{
				for (j=1; j<scr_par.width; j++)
				{
					*pdest = current_colors[*psrc++];
					pdest++;
					*pdest = *(pdest-1);
      					pdest++;
				}

				memcpy(screen->pixels+i*screen->pitch*2+screen->pitch,
					screen->pixels+i*screen->pitch*2,screen->pitch);
			}
/*		    else
		    {
			for (j=1;j<scr_par.width;j++) {
			    *(pdest++)=current_colors[*psrc++];
			    *(pdest++)=*(pdest-1);
			    *(pdest++)=*(pdest-1);
			}
			memcpy(screen->pixels+i*screen->pitch*3+screen->pitch,
			       screen->pixels+i*screen->pitch*3,screen->pitch);
			memcpy(screen->pixels+i*screen->pitch*3+screen->pitch*2,
			       screen->pixels+i*screen->pitch*3,screen->pitch);

		    }
*/
		}
	}
	else
	{
		byte* psrc=surface1;
		for (i=0;i<scr_par.height;i++)
		{
			short* pdest=(short*)(screen->pixels+i*screen->pitch);
			for (j=0;j<scr_par.width;j++)
			*pdest++=current_colors[*psrc++];
		}
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void blur(t_interpol* vector_field)
{
	compute_surface(vector_field);
	display_surface();
}

void plot1(int x,int y,int c);
void plot1(int x,int y,int c)
{
	if (x>0 && x<scr_par.width-3 && y>0 && y<scr_par.height-3)
		assign_max(&(surface1)[x+y*scr_par.width],c);
}

void plot2(int x,int y,int c);
void plot2(int x,int y,int c)
{
	int ty;

	if (x>0 && x<scr_par.width-3 && y>0 && y<scr_par.height-3) {
		ty = y*scr_par.width;
		assign_max(&(surface1)[x+ty],c);
		assign_max(&(surface1)[x+1+ty],c);
		assign_max(&(surface1)[x+ty+scr_par.width],c);
		assign_max(&(surface1)[x+1+ty+scr_par.width],c);
	}
}


void plot3(int x,int y,int c);
void plot3(int x,int y,int c)
{
	int ty;

	if (x>0 && x<scr_par.width-3 && y>0 && y<scr_par.height-3) {
		ty = y*scr_par.width;
		assign_max(&(surface1)[x+ty],c/3);
		assign_max(&(surface1)[x+1+ty],c>>1);
		assign_max(&(surface1)[x+ty+scr_par.width],c>>1);
		assign_max(&(surface1)[x+1+ty+scr_par.width],c);
		assign_max(&(surface1)[x+ty+(scr_par.width<<1)],c/3);
		assign_max(&(surface1)[x+2+ty+(scr_par.width<<1)],c/3);
		assign_max(&(surface1)[x+1+ty+(scr_par.width<<1)],c>>1);
		assign_max(&(surface1)[x+2+ty+scr_par.width],c>>1);
		assign_max(&(surface1)[x+2+ty+scr_par.width],c/3);
	}
}

#if 0
void line(int x1,int y1,int x2,int y2,int c)
{
	int i,j;
	float x,y,vx,vy,d;
	const float step=1;

	if (x1==x2 && y1==y2)
		plot3(x1,y1,255);
	else {
		x=x1;
		y=y1;
		d=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
		vx=step*(x2-x1)/d;
		vy=step*(y2-y1)/d;
		for (i=0;i<(int)(d/step)+1;i++) {
			plot1(x,y,c);
			x+=vx;
			y+=vy;
		}
	}
}
#endif

#define SWAP(x,y)	\
	x = x + y;	\
	y = x - y;	\
	x = x - y;

void line(int _x1, int _y1, int _x2, int _y2, int _c)
{
	int dx, dy, cxy, dxy;
	/* calculate the distances */
	dx = abs(_x1 - _x2);
	dy = abs(_y1 - _y2);

	cxy = 0;
	if (dy > dx)
	{
		/* Follow Y axis */
		if (_y1 > _y2)
		{
			SWAP(_y1, _y2);
			SWAP(_x1, _x2);
		}

		if (_x1 > _x2)
			dxy = -1;
		else
			dxy = 1;

		for (_y1=_y1; _y1<_y2; _y1++)
		{
			cxy += dx;
			if (cxy >= dy)
			{
				_x1+= dxy;
				cxy -= dy;
			}
			plot1(_x1, _y1, _c);
		}
	}
	else
	{
		/* Follow X axis */
		if (_x1 > _x2)
		{
			SWAP(_x1, _x2);
			SWAP(_y1, _y2);
		}

		if (_y1 > _y2)
			dxy = -1;
		else
			dxy = 1;

		for (_x1=_x1; _x1<_x2; _x1++)
		{
			cxy += dy;
			if (cxy >= dx)
			{
				_y1+=dxy;
				cxy -= dx;
			}
		 	plot1(_x1, _y1, _c);
		}
	}
}

struct sincos
{
	int i;
	float *f;
};

/* Little optimization for cos/sin functions */
static struct sincos cosw = { 0, NULL };
static struct sincos sinw = { 0, NULL };

void spectral(t_effect* current_effect,short data[2][512])
{
	int i, halfheight, halfwidth;
	float old_y1,old_y2;
	float _y1=(((data[0][0]+data[1][0])>>9)*current_effect->spectral_amplitude*scr_par.height)>>12;
	float _y2=(((data[0][0]+data[1][0])>>9)*current_effect->spectral_amplitude*scr_par.height)>>12;
	const int density_lines=5;
	const int step=4;
	const int shift=(current_effect->spectral_shift*scr_par.height)>>8;

	if (cosw.i != scr_par.width || sinw.i != scr_par.width)
	{
		free(cosw.f);
		free(sinw.f);
		sinw.f = cosw.f = NULL;
		sinw.i = cosw.i = 0;
	}

	if (cosw.i == 0 || cosw.f == NULL)
	{
	 	float halfPI  = (float)PI/2;
		cosw.i = scr_par.width;
		cosw.f = malloc(sizeof(float)*scr_par.width);
		for (i=0; i<scr_par.width;i+=step)
			cosw.f[i] = cos((float)i/scr_par.width*PI+halfPI);
	}

	if (sinw.i == 0 || sinw.f == NULL)
	{
		float halfPI = (float)PI/2;
		sinw.i = scr_par.width;
		sinw.f = malloc(sizeof(float)*scr_par.width);
		for (i=0; i<scr_par.width;i+=step)
			sinw.f[i] = sin((float)i/scr_par.width*PI+halfPI);
	}

	if (current_effect->mode_spectre==3)
	{
		if (_y1<0)
			_y1=0;

		if (_y2<0)
			_y2=0;
	}

	halfheight = scr_par.height >> 1;
	halfwidth  = scr_par.width >> 1;
	for (i=step;i<scr_par.width;i+=step)
	{
		old_y1=_y1;
		old_y2=_y2;
		_y1=((data[1][(i<<9)/scr_par.width/density_lines]>>8)*
		    current_effect->spectral_amplitude*scr_par.height)>>12;
		_y2=((data[0][(i<<9)/scr_par.width/density_lines]>>8)*
		    current_effect->spectral_amplitude*scr_par.height)>>12;

		switch (current_effect->mode_spectre)
		{
		case 0:
			line(i-step,halfheight+shift+old_y2,
			     i,halfheight+shift+_y2,
			     current_effect->spectral_color);
			break;
		case 1:
			line(i-step,halfheight+shift+old_y1,
			     i,halfheight+shift+_y1,
			     current_effect->spectral_color);
			line(i-step,halfheight-shift+old_y2,
			     i,halfheight-shift+_y2,
			     current_effect->spectral_color);
			break;
		case 2:
			line(i-step,halfheight+shift+old_y1,
			     i,halfheight+shift+_y1,
			     current_effect->spectral_color);
			line(i-step,halfheight-shift+old_y1,
			     i,halfheight-shift+_y1,
			     current_effect->spectral_color);
			line(halfwidth+shift+old_y2,i-step,
			     halfwidth+shift+_y2,i,
			     current_effect->spectral_color);
			line(halfwidth-shift+old_y2,i-step,
			     halfwidth-shift+_y2,i,
			     current_effect->spectral_color);
			break;
		case 3:
			if (_y1<0)
				_y1=0;
			if (_y2<0)
				_y2=0;
		case 4:
			line(halfwidth  + cosw.f[i-step] * (shift+old_y1),
			     halfheight + sinw.f[i-step] * (shift+old_y1),
			     halfwidth  + cosw.f[i]      * (shift+_y1),
			     halfheight + sinw.f[i]      * (shift+_y1),
			     current_effect->spectral_color);
			line(halfwidth  - cosw.f[i-step] * (shift+old_y2),
			     halfheight + sinw.f[i-step] * (shift+old_y2),
			     halfwidth  - cosw.f[i]      * (shift+_y2),
			     halfheight + sinw.f[i]      * (shift+_y2),
			     current_effect->spectral_color);
			break;
		}
	}

	if (current_effect->mode_spectre==3 || current_effect->mode_spectre==4)
		line(halfwidth  + cosw.f[scr_par.width - step] * (shift+_y1),
		     halfheight + sinw.f[scr_par.width - step] * (shift+_y1),
		     halfwidth  - cosw.f[scr_par.width - step] * (shift+_y2),
		     halfheight + sinw.f[scr_par.width - step] * (shift+_y2),
		     current_effect->spectral_color);
}

void curve(t_effect* current_effect)
{
	int i,j,k;
	float v,vr;
	float x,y;
	float amplitude=(float)current_effect->curve_amplitude/256;

	for (j=0;j<2;j++)
	{
		v=80;
		vr=0.001;
		k=current_effect->x_curve;
		for (i=0;i<64;i++)
		{
			x=cos((float)(k)/(v+v*j*1.34))*scr_par.height*amplitude;
			y=sin((float)(k)/(1.756*(v+v*j*0.93)))*scr_par.height*amplitude;
			plot2(x*cos((float)k*vr)+y*sin((float)k*vr)+scr_par.width/2,
			      x*sin((float)k*vr)-y*cos((float)k*vr)+scr_par.height/2,
			      current_effect->curve_color);
			k++;
		}
	}
	current_effect->x_curve=k;
}


void init_sdl(void)
{
	surface1=(byte*)malloc(scr_par.width*scr_par.height);
	surface2=(byte*)malloc(scr_par.width*scr_par.height);

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_WM_SetCaption("Tyler", 0L);
	screen = SDL_SetVideoMode(scr_par.width * scr_par.scale,
		scr_par.height * scr_par.scale, 16, VIDEO_FLAGS);

	if ( screen == NULL )
	{
		fprintf(stderr, "Couldn't init video mode: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_ShowCursor(0);
	SDL_EnableKeyRepeat(0,0);
}

void toggle_fullscreen(void)
{
	SDL_WM_ToggleFullScreen(screen);
}

void close_sdl(void)
{
	SDL_Quit();
}
