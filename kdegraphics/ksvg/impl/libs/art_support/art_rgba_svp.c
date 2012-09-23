/* Libart_LGPL - library of basic graphic primitives
 * Copyright (C) 1998 Raph Levien
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/* Render a sorted vector path into an RGB buffer. */

#include <X11/Xos.h>

#include "art_rgba_svp.h"

#include <libart_lgpl/art_svp.h>
#include <libart_lgpl/art_svp_render_aa.h>
#include <libart_lgpl/art_rgba.h>
#include <libart_lgpl/art_rgb.h>

/* RGBA renderers */

typedef struct _ArtKSVGRgbaSVPAlphaData ArtKSVGRgbaSVPAlphaData;

struct _ArtKSVGRgbaSVPAlphaData {
  int alphatab[256];
  art_u8 r, g, b, alpha;
  art_u32 rgba;
  art_u8 *buf;
	art_u8 *mask;
  int rowstride;
  int x0, x1;
	int y0;
};

/**
 * art_rgba_fill_run: fill an RGBA buffer a solid RGB color.
 * @buf: Buffer to fill.
 * @r: Red, range 0..255.
 * @g: Green, range 0..255.
 * @b: Blue, range 0..255.
 * @n: Number of RGB triples to fill.
 *
 * Fills a buffer with @n copies of the (@r, @g, @b) triple, solid
 * alpha. Thus, locations @buf (inclusive) through @buf + 4 * @n
 * (exclusive) are written.
 **/
static void
art_ksvg_rgba_fill_run (art_u8 *buf, art_u8 r, art_u8 g, art_u8 b, int n)
{
	int i;
#if X_BYTE_ORDER == X_BIG_ENDIAN
	art_u32 src_rgba;
#else
	art_u32 src_abgr;
#endif

#if X_BYTE_ORDER == X_BIG_ENDIAN
	src_rgba = (r << 24) | (g << 16) | (b << 8) | 255;
#else
	src_abgr = (255 << 24) | (b << 16) | (g << 8) | r;
#endif
	for(i = 0; i < n; i++)
	{
#if X_BYTE_ORDER == X_BIG_ENDIAN
		((art_u32 *)buf)[i] = src_rgba;
#else
		((art_u32 *)buf)[i] = src_abgr;
#endif
	}    
}

/**
 * art_rgba_run_alpha: Render semitransparent color over RGBA buffer.
 * @buf: Buffer for rendering.
 * @r: Red, range 0..255.
 * @g: Green, range 0..255.
 * @b: Blue, range 0..255.
 * @alpha: Alpha, range 0..255.
 * @n: Number of RGB triples to render.
 *
 * Renders a sequential run of solid (@r, @g, @b) color over @buf with
 * opacity @alpha. Note that the range of @alpha is 0..255, in contrast
 * to art_rgb_run_alpha, which has a range of 0..256.
 **/
static void
art_ksvg_rgba_run_alpha (art_u8 *buf, art_u8 r, art_u8 g, art_u8 b, int alpha, int n)
{
	int i;
	int v;
	int tmp;

	if(alpha > 255)
		alpha = 255;

	for(i = 0; i < n; i++)
	{
		v = *buf;
		tmp = (r - v) * alpha + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);

		v = *buf;
		tmp = (g - v) * alpha + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);

		v = *buf;
		tmp = (b - v) * alpha + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);

		v = *buf;
		tmp = (255 - alpha) * v + 0x80;
		*buf++ = alpha + ((tmp + (tmp >> 8)) >> 8);
	}
}

static void
art_ksvg_rgba_mask_run_alpha (art_u8 *buf, art_u8 *mask, art_u8 r, art_u8 g, art_u8 b, int alpha, int n)
{
	int i;
	int v;
	int am;
	int tmp;

	if(alpha > 255)
		alpha = 255;

	for(i = 0; i < n; i++)
	{
		am = (alpha * *mask++) + 0x80;
		am = (am + (am >> 8)) >> 8;

		v = *buf;
		tmp = (r - v) * am + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);

		v = *buf;
		tmp = (g - v) * am + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);

		v = *buf;
		tmp = (b - v) * am + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);

		v = *buf;
		tmp = (255 - am) * v + 0x80;
		*buf++ = am + ((tmp + (tmp >> 8)) >> 8);
	}
}

static void
art_ksvg_rgba_svp_alpha_callback(void *callback_data, int y,
			    int start, ArtSVPRenderAAStep *steps, int n_steps)
{
  ArtKSVGRgbaSVPAlphaData *data = (ArtKSVGRgbaSVPAlphaData *)callback_data;
  art_u8 *linebuf;
  int run_x0, run_x1;
  art_u32 running_sum = start;
  int x0, x1;
  int k;
  art_u8 r, g, b;
  int *alphatab;
  int alpha;

  linebuf = data->buf;
  x0 = data->x0;
  x1 = data->x1;

  r = data->r;
  g = data->g;
  b = data->b;
  alphatab = data->alphatab;

  if (n_steps > 0)
    {
      run_x1 = steps[0].x;
      if (run_x1 > x0)
	{
	  alpha = (running_sum >> 16) & 0xff;
	  if (alpha)
	    art_ksvg_rgba_run_alpha (linebuf,
			       r, g, b, alphatab[alpha],
			       run_x1 - x0);
	}

      for (k = 0; k < n_steps - 1; k++)
	{
	  running_sum += steps[k].delta;
	  run_x0 = run_x1;
	  run_x1 = steps[k + 1].x;
	  if (run_x1 > run_x0)
	    {
	      alpha = (running_sum >> 16) & 0xff;
	      if (alpha)
		art_ksvg_rgba_run_alpha (linebuf + (run_x0 - x0) * 4,
				   r, g, b, alphatab[alpha],
				   run_x1 - run_x0);
	    }
	}
      running_sum += steps[k].delta;
      if (x1 > run_x1)
	{
	  alpha = (running_sum >> 16) & 0xff;
	  if (alpha)
	    art_ksvg_rgba_run_alpha (linebuf + (run_x1 - x0) * 4,
			       r, g, b, alphatab[alpha],
			       x1 - run_x1);
	}
    }
  else
    {
      alpha = (running_sum >> 16) & 0xff;
      if (alpha)
	art_ksvg_rgba_run_alpha (linebuf,
			   r, g, b, alphatab[alpha],
			   x1 - x0);
    }

  data->buf += data->rowstride;
}

static void
art_ksvg_rgba_svp_alpha_opaque_callback(void *callback_data, int y,
				   int start,
				   ArtSVPRenderAAStep *steps, int n_steps)
{
  ArtKSVGRgbaSVPAlphaData *data = (ArtKSVGRgbaSVPAlphaData *)callback_data;
  art_u8 *linebuf;
  int run_x0, run_x1;
  art_u32 running_sum = start;
  int x0, x1;
  int k;
  art_u8 r, g, b;
  art_u32 rgba;
  int *alphatab;
  int alpha;

  linebuf = data->buf;
  x0 = data->x0;
  x1 = data->x1;

  r = data->r;
  g = data->g;
  b = data->b;
  rgba = data->rgba;
  alphatab = data->alphatab;

  if (n_steps > 0)
    {
      run_x1 = steps[0].x;
      if (run_x1 > x0)
	{
	  alpha = running_sum >> 16;
	  if (alpha)
	    {
	      if (alpha >= 255)
		art_ksvg_rgba_fill_run (linebuf,
				  r, g, b,
				  run_x1 - x0);
	      else
		art_ksvg_rgba_run_alpha (linebuf,
				   r, g, b, alphatab[alpha],
				   run_x1 - x0);
	    }
	}

      for (k = 0; k < n_steps - 1; k++)
	{
	  running_sum += steps[k].delta;
	  run_x0 = run_x1;
	  run_x1 = steps[k + 1].x;
	  if (run_x1 > run_x0)
	    {
	      alpha = running_sum >> 16;
	      if (alpha)
		{
		  if (alpha >= 255)
		    art_ksvg_rgba_fill_run (linebuf + (run_x0 - x0) * 4,
				      r, g, b,
				      run_x1 - run_x0);
		  else
		    art_ksvg_rgba_run_alpha (linebuf + (run_x0 - x0) * 4,
				       r, g, b, alphatab[alpha],
				       run_x1 - run_x0);
		}
	    }
	}
      running_sum += steps[k].delta;
      if (x1 > run_x1)
	{
	  alpha = running_sum >> 16;
	  if (alpha)
	    {
	      if (alpha >= 255)
		art_ksvg_rgba_fill_run (linebuf + (run_x1 - x0) * 4,
				  r, g, b,
				  x1 - run_x1);
	      else
		art_ksvg_rgba_run_alpha (linebuf + (run_x1 - x0) * 4,
				   r, g, b, alphatab[alpha],
				   x1 - run_x1);
	    }
	}
    }
  else
    {
      alpha = running_sum >> 16;
      if (alpha)
	{
	  if (alpha >= 255)
	    art_ksvg_rgba_fill_run (linebuf,
			      r, g, b,
			      x1 - x0);
	  else
	    art_ksvg_rgba_run_alpha (linebuf,
			       r, g, b, alphatab[alpha],
			       x1 - x0);
	}
    }
  data->buf += data->rowstride;
}

static void
art_ksvg_rgba_svp_alpha_mask_callback(void *callback_data, int y,
																 int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ArtKSVGRgbaSVPAlphaData *data = (ArtKSVGRgbaSVPAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	art_u8 r, g, b;
	int *alphatab;
	int alpha;
	art_u8 *maskbuf;

	linebuf = data->buf;
	x0 = data->x0;
	x1 = data->x1;

	r = data->r;
	g = data->g;
	b = data->b;
	alphatab = data->alphatab;

	maskbuf = data->mask + (y - data->y0) * (data->x1 - data->x0);

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				art_ksvg_rgba_mask_run_alpha (linebuf, maskbuf,
													 r, g, b, alphatab[alpha],
													 run_x1 - x0);
		}

		for(k = 0; k < n_steps - 1; k++)
		{
			running_sum += steps[k].delta;
			run_x0 = run_x1;
			run_x1 = steps[k + 1].x;
			if(run_x1 > run_x0)
			{
				alpha = (running_sum >> 16) & 0xff;
				if(alpha)
					art_ksvg_rgba_mask_run_alpha (linebuf + (run_x0 - x0) * 4, maskbuf + (run_x0 - x0),
														 r, g, b, alphatab[alpha],
														 run_x1 - run_x0);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				art_ksvg_rgba_mask_run_alpha (linebuf + (run_x1 - x0) * 4, maskbuf + (run_x1 - x0) ,
													 r, g, b, alphatab[alpha],
													 x1 - run_x1);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			art_ksvg_rgba_mask_run_alpha (linebuf,	maskbuf,
												 r, g, b, alphatab[alpha],
												 x1 - x0);
	}

	data->buf += data->rowstride;
}

/**
 * art_rgb_svp_alpha: Alpha-composite sorted vector path over RGB buffer.
 * @svp: The source sorted vector path.
 * @x0: Left coordinate of destination rectangle.
 * @y0: Top coordinate of destination rectangle.
 * @x1: Right coordinate of destination rectangle.
 * @y1: Bottom coordinate of destination rectangle.
 * @rgba: Color in 0xRRGGBBAA format.
 * @buf: Destination RGB buffer.
 * @rowstride: Rowstride of @buf buffer.
 * @alphagamma: #ArtAlphaGamma for gamma-correcting the compositing.
 *
 * Renders the shape specified with @svp over the @buf RGB buffer.
 * @x1 - @x0 specifies the width, and @y1 - @y0 specifies the height,
 * of the rectangle rendered. The new pixels are stored starting at
 * the first byte of @buf. Thus, the @x0 and @y0 parameters specify
 * an offset within @svp, and may be tweaked as a way of doing
 * integer-pixel translations without fiddling with @svp itself.
 *
 * The @rgba argument specifies the color for the rendering. Pixels of
 * entirely 0 winding number are left untouched. Pixels of entirely
 * 1 winding number have the color @rgba composited over them (ie,
 * are replaced by the red, green, blue components of @rgba if the alpha
 * component is 0xff). Pixels of intermediate coverage are interpolated
 * according to the rule in @alphagamma, or default to linear if
 * @alphagamma is NULL.
 **/
void
art_ksvg_rgba_svp_alpha(const ArtSVP *svp,
		   int x0, int y0, int x1, int y1,
		   art_u32 rgba,
		   art_u8 *buf, int rowstride,
		   ArtAlphaGamma *alphagamma,
			 art_u8 *mask)
{
  ArtKSVGRgbaSVPAlphaData data;
  int r, g, b;
  int i;
  int a, da;
	int alpha;

  r = (rgba >> 24) & 0xff;
  g = (rgba >> 16) & 0xff;
  b = (rgba >> 8) & 0xff;
	alpha = rgba & 0xff;

  data.r = r;
  data.g = g;
  data.b = b;
  data.alpha = alpha;
  data.rgba = rgba;
	data.mask = mask;

  a = 0x8000;
  da = (alpha * 66051 + 0x80) >> 8; /* 66051 equals 2 ^ 32 / (255 * 255) */

  for (i = 0; i < 256; i++)
    {
      data.alphatab[i] = a >> 16;
      a += da;
    }

  data.buf = buf;
  data.rowstride = rowstride;
  data.x0 = x0;
  data.x1 = x1;
	data.y0 = y0;

	if(mask)
		art_svp_render_aa (svp, x0, y0, x1, y1, art_ksvg_rgba_svp_alpha_mask_callback, &data);
	else
	{
		if (alpha == 255)
			art_svp_render_aa (svp, x0, y0, x1, y1, art_ksvg_rgba_svp_alpha_opaque_callback, &data);
		else
			art_svp_render_aa (svp, x0, y0, x1, y1, art_ksvg_rgba_svp_alpha_callback, &data);
	}
}

/* RGB renderers */

static void
art_ksvg_rgb_mask_run_alpha(art_u8 *buf, art_u8 *mask, art_u8 r, art_u8 g, art_u8 b, int alpha, int n)
{
	int i;
	int v;
	int am;
	int tmp;

	if(alpha > 255)
		alpha = 255;

	for(i = 0; i < n; i++)
	{
		am = (alpha * *mask++) + 0x80;
		am = (am + (am >> 8)) >> 8;

		v = *buf;
		tmp = (r - v) * am + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);

		v = *buf;
		tmp = (g - v) * am + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);

		v = *buf;
		tmp = (b - v) * am + 0x80;
		*buf++ = v + ((tmp + (tmp >> 8)) >> 8);
	}
}

static void
art_ksvg_rgb_svp_alpha_mask_callback(void *callback_data, int y,
																 int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ArtKSVGRgbaSVPAlphaData *data = (ArtKSVGRgbaSVPAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	art_u8 r, g, b;
	int *alphatab;
	int alpha;
	art_u8 *maskbuf;

	linebuf = data->buf;
	x0 = data->x0;
	x1 = data->x1;

	r = data->r;
	g = data->g;
	b = data->b;
	alphatab = data->alphatab;

	maskbuf = data->mask + (y - data->y0) * (data->x1 - data->x0);

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				art_ksvg_rgb_mask_run_alpha (linebuf, maskbuf,
													 r, g, b, alphatab[alpha],
													 run_x1 - x0);
		}

		for(k = 0; k < n_steps - 1; k++)
		{
			running_sum += steps[k].delta;
			run_x0 = run_x1;
			run_x1 = steps[k + 1].x;
			if(run_x1 > run_x0)
			{
				alpha = (running_sum >> 16) & 0xff;
				if(alpha)
					art_ksvg_rgb_mask_run_alpha (linebuf + (run_x0 - x0) * 3, maskbuf + (run_x0 - x0),
														 r, g, b, alphatab[alpha],
														 run_x1 - run_x0);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				art_ksvg_rgb_mask_run_alpha (linebuf + (run_x1 - x0) * 3, maskbuf + (run_x1 - x0) ,
													 r, g, b, alphatab[alpha],
													 x1 - run_x1);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			art_ksvg_rgb_mask_run_alpha (linebuf,	maskbuf,
												 r, g, b, alphatab[alpha],
												 x1 - x0);
	}

	data->buf += data->rowstride;
}

/**
 * art_rgb_svp_alpha: Alpha-composite sorted vector path over RGB buffer.
 * @svp: The source sorted vector path.
 * @x0: Left coordinate of destination rectangle.
 * @y0: Top coordinate of destination rectangle.
 * @x1: Right coordinate of destination rectangle.
 * @y1: Bottom coordinate of destination rectangle.
 * @rgba: Color in 0xRRGGBBAA format.
 * @buf: Destination RGB buffer.
 * @rowstride: Rowstride of @buf buffer.
 * @alphagamma: #ArtAlphaGamma for gamma-correcting the compositing.
 *
 * Renders the shape specified with @svp over the @buf RGB buffer.
 * @x1 - @x0 specifies the width, and @y1 - @y0 specifies the height,
 * of the rectangle rendered. The new pixels are stored starting at
 * the first byte of @buf. Thus, the @x0 and @y0 parameters specify
 * an offset within @svp, and may be tweaked as a way of doing
 * integer-pixel translations without fiddling with @svp itself.
 *
 * The @rgba argument specifies the color for the rendering. Pixels of
 * entirely 0 winding number are left untouched. Pixels of entirely
 * 1 winding number have the color @rgba composited over them (ie,
 * are replaced by the red, green, blue components of @rgba if the alpha
 * component is 0xff). Pixels of intermediate coverage are interpolated
 * according to the rule in @alphagamma, or default to linear if
 * @alphagamma is NULL.
 **/
void
art_ksvg_rgb_svp_alpha_mask(const ArtSVP *svp,
												int x0, int y0, int x1, int y1,
												art_u32 rgba,
												art_u8 *buf, int rowstride,
												ArtAlphaGamma *alphagamma,
												art_u8 *mask)
{
	ArtKSVGRgbaSVPAlphaData data;
	int r, g, b, alpha;
	int i;
	int a, da;

	r = rgba >> 24;
	g = (rgba >> 16) & 0xff;
	b = (rgba >> 8) & 0xff;
	alpha = rgba & 0xff;

	data.r = r;
	data.g = g;
	data.b = b;
	data.alpha = alpha;
	data.mask = mask;

	a = 0x8000;
	da = (alpha * 66051 + 0x80) >> 8;	/* 66051 equals 2 ^ 32 / (255 * 255) */

	for(i = 0; i < 256; i++)
	{
		data.alphatab[i] = a >> 16;
		a += da;
	}

	data.buf = buf;
	data.rowstride = rowstride;
	data.x0 = x0;
	data.x1 = x1;
	data.y0 = y0;

	art_svp_render_aa(svp, x0, y0, x1, y1, art_ksvg_rgb_svp_alpha_mask_callback, &data);
}

