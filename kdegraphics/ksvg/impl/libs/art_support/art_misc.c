#include <libart_lgpl/art_vpath.h>
#include <libart_lgpl/art_bpath.h>
#include <libart_lgpl/art_misc.h>
#include <libart_lgpl/art_affine.h>
#include <libart_lgpl/art_svp_render_aa.h>

#include "art_misc.h"

extern double ceil(double x);
extern double floor(double x);

/**
 * art_vpath_render_bez: Render a bezier segment into the vpath.
 * @p_vpath: Where the pointer to the #ArtVpath structure is stored.
 * @pn_points: Pointer to the number of points in *@p_vpath.
 * @pn_points_max: Pointer to the number of points allocated.
 * @x0: X coordinate of starting bezier point.
 * @y0: Y coordinate of starting bezier point.
 * @x1: X coordinate of first bezier control point.
 * @y1: Y coordinate of first bezier control point.
 * @x2: X coordinate of second bezier control point.
 * @y2: Y coordinate of second bezier control point.
 * @x3: X coordinate of ending bezier point.
 * @y3: Y coordinate of ending bezier point.
 * @flatness: Flatness control.
 *
 * Renders a bezier segment into the vector path, reallocating and
 * updating *@p_vpath and *@pn_vpath_max as necessary. *@pn_vpath is
 * incremented by the number of vector points added.
 *
 * This step includes (@x0, @y0) but not (@x3, @y3).
 *
 * The @flatness argument guides the amount of subdivision. The Adobe
 * PostScript reference manual defines flatness as the maximum
 * deviation between the any point on the vpath approximation and the
 * corresponding point on the "true" curve, and we follow this
 * definition here. A value of 0.25 should ensure high quality for aa
 * rendering.
 **/
 void
ksvg_art_vpath_render_bez (ArtVpath **p_vpath, int *pn, int *pn_max,
		double x0, double y0,
		double x1, double y1,
		double x2, double y2,
		double x3, double y3,
		double flatness)
{
	double x3_0, y3_0;
	double z3_0_dot;
	double z1_dot, z2_dot;
	double z1_perp, z2_perp;
	double max_perp_sq;

	double x_m, y_m;
	double xa1, ya1;
	double xa2, ya2;
	double xb1, yb1;
	double xb2, yb2;

	/* It's possible to optimize this routine a fair amount.

	   First, once the _dot conditions are met, they will also be met in
	   all further subdivisions. So we might recurse to a different
	   routine that only checks the _perp conditions.

	   Second, the distance _should_ decrease according to fairly
	   predictable rules (a factor of 4 with each subdivision). So it might
	   be possible to note that the distance is within a factor of 4 of
	   acceptable, and subdivide once. But proving this might be hard.

	   Third, at the last subdivision, x_m and y_m can be computed more
	   expeditiously (as in the routine above).

	   Finally, if we were able to subdivide by, say 2 or 3, this would
	   allow considerably finer-grain control, i.e. fewer points for the
	   same flatness tolerance. This would speed things up downstream.

	   In any case, this routine is unlikely to be the bottleneck. It's
	   just that I have this undying quest for more speed...

*/

	x3_0 = x3 - x0;
	y3_0 = y3 - y0;

	/* z3_0_dot is dist z0-z3 squared */
	z3_0_dot = x3_0 * x3_0 + y3_0 * y3_0;

	/* todo: this test is far from satisfactory. */
	if (z3_0_dot < 0.001)
		goto nosubdivide;

	/* we can avoid subdivision if:

	   z1 has distance no more than flatness from the z0-z3 line

	   z1 is no more z0'ward than flatness past z0-z3

	   z1 is more z0'ward than z3'ward on the line traversing z0-z3

	   and correspondingly for z2 */

	/* perp is distance from line, multiplied by dist z0-z3 */
	max_perp_sq = flatness * flatness * z3_0_dot;
	z1_perp = (y1 - y0) * x3_0 - (x1 - x0) * y3_0;
	if (z1_perp * z1_perp > max_perp_sq)
		goto subdivide;

	z2_perp = (y3 - y2) * x3_0 - (x3 - x2) * y3_0;
	if (z2_perp * z2_perp > max_perp_sq)
		goto subdivide;

	z1_dot = (x1 - x0) * x3_0 + (y1 - y0) * y3_0;
	if (z1_dot < 0 && z1_dot * z1_dot > max_perp_sq)
		goto subdivide;

	z2_dot = (x3 - x2) * x3_0 + (y3 - y2) * y3_0;
	if (z2_dot < 0 && z2_dot * z2_dot > max_perp_sq)
		goto subdivide;

	if (z1_dot + z1_dot > z3_0_dot)
		goto subdivide;

	if (z2_dot + z2_dot > z3_0_dot)
		goto subdivide;

nosubdivide:
	/* don't subdivide */
	art_vpath_add_point (p_vpath, pn, pn_max,
			ART_LINETO, x3, y3);
	return;

subdivide:

	xa1 = (x0 + x1) * 0.5;
	ya1 = (y0 + y1) * 0.5;
	xa2 = (x0 + 2 * x1 + x2) * 0.25;
	ya2 = (y0 + 2 * y1 + y2) * 0.25;
	xb1 = (x1 + 2 * x2 + x3) * 0.25;
	yb1 = (y1 + 2 * y2 + y3) * 0.25;
	xb2 = (x2 + x3) * 0.5;
	yb2 = (y2 + y3) * 0.5;
	x_m = (xa2 + xb1) * 0.5;
	y_m = (ya2 + yb1) * 0.5;
#ifdef VERBOSE
	printf ("%g,%g %g,%g %g,%g %g,%g\n", xa1, ya1, xa2, ya2,
			xb1, yb1, xb2, yb2);
#endif
	ksvg_art_vpath_render_bez (p_vpath, pn, pn_max,
			x0, y0, xa1, ya1, xa2, ya2, x_m, y_m, flatness);
	ksvg_art_vpath_render_bez (p_vpath, pn, pn_max,
			x_m, y_m, xb1, yb1, xb2, yb2, x3, y3, flatness);
}

#define RENDER_LEVEL 4
#define RENDER_SIZE (1 << (RENDER_LEVEL))

/**
 * ksvg_art_bez_path_to_vec: Create vpath from bezier path.
 * @bez: Bezier path.
 * @flatness: Flatness control.
 *
 * Creates a vector path closely approximating the bezier path defined by
 * @bez. The @flatness argument controls the amount of subdivision. In
 * general, the resulting vpath deviates by at most @flatness pixels
 * from the "ideal" path described by @bez.
 *
 * Return value: Newly allocated vpath.
 **/
 ArtVpath *
ksvg_art_bez_path_to_vec(const ArtBpath *bez, double flatness)
{
	ArtVpath *vec;
	int vec_n, vec_n_max;
	int bez_index;
	double x, y;

	vec_n = 0;
	vec_n_max = RENDER_SIZE;
	vec = art_new (ArtVpath, vec_n_max);

	/* Initialization is unnecessary because of the precondition that the
	   bezier path does not begin with LINETO or CURVETO, but is here
	   to make the code warning-free. */
	x = 0;
	y = 0;

	bez_index = 0;
	do
	{
#ifdef VERBOSE
		printf ("%s %g %g\n",
				bez[bez_index].code == ART_CURVETO ? "curveto" :
				bez[bez_index].code == ART_LINETO ? "lineto" :
				bez[bez_index].code == ART_MOVETO ? "moveto" :
				bez[bez_index].code == ART_MOVETO_OPEN ? "moveto-open" :
				"end", bez[bez_index].x3, bez[bez_index].y3);
#endif
		/* make sure space for at least one more code */
		if (vec_n >= vec_n_max)
			art_expand (vec, ArtVpath, vec_n_max);
		switch (bez[bez_index].code)
		{
			case ART_MOVETO_OPEN:
			case ART_MOVETO:
			case ART_LINETO:
				x = bez[bez_index].x3;
				y = bez[bez_index].y3;
				vec[vec_n].code = bez[bez_index].code;
				vec[vec_n].x = x;
				vec[vec_n].y = y;
				vec_n++;
				break;
			case ART_END:
				vec[vec_n].code = ART_END;
				vec[vec_n].x = 0;
				vec[vec_n].y = 0;
				vec_n++;
				break;
			case ART_END2:
				vec[vec_n].code = (ArtPathcode)ART_END2;
				vec[vec_n].x = bez[bez_index].x3;
				vec[vec_n].y = bez[bez_index].y3;
				vec_n++;
				break;
			case ART_CURVETO:
#ifdef VERBOSE
				printf ("%g,%g %g,%g %g,%g %g,%g\n", x, y,
						bez[bez_index].x1, bez[bez_index].y1,
						bez[bez_index].x2, bez[bez_index].y2,
						bez[bez_index].x3, bez[bez_index].y3);
#endif
				ksvg_art_vpath_render_bez (&vec, &vec_n, &vec_n_max,
						x, y,
						bez[bez_index].x1, bez[bez_index].y1,
						bez[bez_index].x2, bez[bez_index].y2,
						bez[bez_index].x3, bez[bez_index].y3,
						flatness);
				x = bez[bez_index].x3;
				y = bez[bez_index].y3;
				break;
		}
	}
	while (bez[bez_index++].code != ART_END);
	return vec;
}

/* Private functions for the rgb affine image compositors - primarily,
*    the determination of runs, eliminating the need for source image
*       bbox calculation in the inner loop. */

/* Determine a "run", such that the inverse affine of all pixels from
*    (x0, y) inclusive to (x1, y) exclusive fit within the bounds
*       of the source image.
*
*          Initial values of x0, x1, and result values stored in first two
*             pointer arguments.
*             */

#define EPSILON 1e-6

 void ksvg_art_rgb_affine_run (int *p_x0, int *p_x1, int y,
		int src_width, int src_height,
		const double affine[6])
{
	int x0, x1;
	double z;
	double x_intercept;
	int xi;

	x0 = *p_x0;
	x1 = *p_x1;

	/* do left and right edges */
	if (affine[0] > EPSILON)
	{
		z = affine[2] * (y + 0.5) + affine[4];
		x_intercept = -z / affine[0];
		xi = ceil (x_intercept + EPSILON - 0.5);
		if (xi > x0)
			x0 = xi;
		x_intercept = (-z + src_width) / affine[0];
		xi = ceil (x_intercept - EPSILON - 0.5);
		if (xi < x1)
			x1 = xi;
	}
	else if (affine[0] < -EPSILON)
	{
		z = affine[2] * (y + 0.5) + affine[4];
		x_intercept = (-z + src_width) / affine[0];
		xi = ceil (x_intercept + EPSILON - 0.5);
		if (xi > x0)
			x0 = xi;
		x_intercept = -z / affine[0];
		xi = ceil (x_intercept - EPSILON - 0.5);
		if (xi < x1)
			x1 = xi;
	}
	else
	{
		z = affine[2] * (y + 0.5) + affine[4];
		if (z < 0 || z >= src_width)
		{
			*p_x1 = *p_x0;
			return;
		}
	}
	/* do top and bottom edges */
	if (affine[1] > EPSILON)
	{
		z = affine[3] * (y + 0.5) + affine[5];
		x_intercept = -z / affine[1];
		xi = ceil (x_intercept + EPSILON - 0.5);
		if (xi > x0)
			x0 = xi;
		x_intercept = (-z + src_height) / affine[1];
		xi = ceil (x_intercept - EPSILON - 0.5);
		if (xi < x1)
			x1 = xi;
	}
	else if (affine[1] < -EPSILON)
	{
		z = affine[3] * (y + 0.5) + affine[5];
		x_intercept = (-z + src_height) / affine[1];
		xi = ceil (x_intercept + EPSILON - 0.5);
		if (xi > x0)
			x0 = xi;
		x_intercept = -z / affine[1];
		xi = ceil (x_intercept - EPSILON - 0.5);
		if (xi < x1)
			x1 = xi;
	}
	else
	{
		z = affine[3] * (y + 0.5) + affine[5];
		if (z < 0 || z >= src_height)
		{
			*p_x1 = *p_x0;
			return;
		}
	}

	*p_x0 = x0;
	*p_x1 = x1;
}

/**
 * ksvg_art_rgb_affine: Affine transform source RGB image and composite.
 * @dst: Destination image RGB buffer.
 * @x0: Left coordinate of destination rectangle.
 * @y0: Top coordinate of destination rectangle.
 * @x1: Right coordinate of destination rectangle.
 * @y1: Bottom coordinate of destination rectangle.
 * @dst_rowstride: Rowstride of @dst buffer.
 * @src: Source image RGB buffer.
 * @src_width: Width of source image.
 * @src_height: Height of source image.
 * @src_rowstride: Rowstride of @src buffer.
 * @affine: Affine transform.
 * @level: Filter level.
 * @alphagamma: #ArtAlphaGamma for gamma-correcting the compositing.
 * @alpha: Alpha, range 0..256.
 *
 * Affine transform the source image stored in @src, compositing over
 * the area of destination image @dst specified by the rectangle
 * (@x0, @y0) - (@x1, @y1). As usual in libart, the left and top edges
 * of this rectangle are included, and the right and bottom edges are
 * excluded.
 *
 * The @alphagamma parameter specifies that the alpha compositing be done
 * in a gamma-corrected color space. Since the source image is opaque RGB,
 * this argument only affects the edges. In the current implementation,
 * it is ignored.
 *
 * The @level parameter specifies the speed/quality tradeoff of the
 * image interpolation. Currently, only ART_FILTER_NEAREST is
 * implemented.
 *
 * KSVG additions : we have changed this function to support an alpha level as well.
*                  also we made sure compositing an rgba image over an rgb buffer works.
**/
 void ksvg_art_rgb_affine (art_u8 *dst, int x0, int y0, int x1, int y1, int dst_rowstride,
		const art_u8 *src,
		int src_width, int src_height, int src_rowstride,
		const double affine[6],
		ArtFilterLevel level,
		ArtAlphaGamma *alphagamma,
		int alpha)
{
	/* Note: this is a slow implementation, and is missing all filter
	   levels other than NEAREST. It is here for clarity of presentation
	   and to establish the interface. */
	int x, y;
	double inv[6];
	art_u8 *dst_p, *dst_linestart;
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int run_x0, run_x1;

	dst_linestart = dst;
	art_affine_invert (inv, affine);

	if(alpha == 255)
		for (y = y0; y < y1; y++)
		{
			pt.y = y + 0.5;
			run_x0 = x0;
			run_x1 = x1;
			ksvg_art_rgb_affine_run (&run_x0, &run_x1, y, src_width, src_height,
					inv);
			dst_p = dst_linestart + (run_x0 - x0) * 3;
			for (x = run_x0; x < run_x1; x++)
			{
				pt.x = x + 0.5;
				art_affine_point (&src_pt, &pt, inv);
				src_x = floor (src_pt.x);
				src_y = floor (src_pt.y);
				src_p = src + (src_y * src_rowstride) + src_x * 4;
				dst_p[0] = dst_p[0] + (((src_p[2] - dst_p[0]) * src_p[3] + 0x80) >> 8);
				dst_p[1] = dst_p[1] + (((src_p[1] - dst_p[1]) * src_p[3] + 0x80) >> 8);
				dst_p[2] = dst_p[2] + (((src_p[0] - dst_p[2]) * src_p[3] + 0x80) >> 8);
				dst_p += 3;
			}
			dst_linestart += dst_rowstride;
		}
	else
		for (y = y0; y < y1; y++)
		{
			pt.y = y + 0.5;
			run_x0 = x0;
			run_x1 = x1;
			ksvg_art_rgb_affine_run (&run_x0, &run_x1, y, src_width, src_height,
					inv);
			dst_p = dst_linestart + (run_x0 - x0) * 3;
			for (x = run_x0; x < run_x1; x++)
			{
				pt.x = x + 0.5;
				art_affine_point (&src_pt, &pt, inv);
				src_x = floor (src_pt.x);
				src_y = floor (src_pt.y);
				src_p = src + (src_y * src_rowstride) + src_x * 4;
				dst_p[0] = dst_p[0] + (((src_p[2] - dst_p[0]) * alpha + 0x80) >> 8);
				dst_p[1] = dst_p[1] + (((src_p[1] - dst_p[1]) * alpha + 0x80) >> 8);
				dst_p[2] = dst_p[2] + (((src_p[0] - dst_p[2]) * alpha + 0x80) >> 8);
				dst_p += 3;
			}
			dst_linestart += dst_rowstride;
		}
}


typedef struct _ksvgArtRgbAffineClipAlphaData ksvgArtRgbAffineClipAlphaData;

struct _ksvgArtRgbAffineClipAlphaData
{
	int alphatab[256];
	art_u8 alpha;
	art_u8 *dst;
	int dst_rowstride;
	int x0, x1;
	double inv[6];
	const art_u8 *src;
	int src_width;
	int src_height;
	int src_rowstride;
	const art_u8 *mask;
	int y0;
};

static
void ksvg_art_rgb_affine_clip_run(art_u8 *dst_p, int x0, int x1, int y, const double inv[6],
	int alpha, const art_u8 *src, int src_rowstride, int src_width, int src_height)
{
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int x;

	if(alpha > 255)
		alpha = 255;

	pt.y = y;

	for(x = x0; x < x1; x++)
	{
		pt.x = x;

		art_affine_point(&src_pt, &pt, inv);

		src_x = (int)(src_pt.x);
		src_y = (int)(src_pt.y);

		if(src_x >= 0 && src_x < src_width && src_y >= 0 && src_y < src_height)
		{
			int s;
			int d;
			int tmp;
			int srcAlpha;

			src_p = src + (src_y * src_rowstride) + src_x * 4;

			srcAlpha = alpha * src_p[3] + 0x80;
			srcAlpha = (srcAlpha + (srcAlpha >> 8)) >> 8;

			d = *dst_p;
			s = src_p[2];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;
			s = src_p[1];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;
			s = src_p[0];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;
		}
		else
			dst_p += 3;
	}
}

static void
ksvg_art_rgb_affine_clip_callback (void *callback_data, int y,
														int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ksvgArtRgbAffineClipAlphaData *data = (ksvgArtRgbAffineClipAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	int *alphatab;
	int alpha;

	linebuf = data->dst;
	x0 = data->x0;
	x1 = data->x1;

	alphatab = data->alphatab;

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgb_affine_clip_run(linebuf, x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
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
					ksvg_art_rgb_affine_clip_run(linebuf + (run_x0 - x0) * 3, run_x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgb_affine_clip_run(linebuf + (run_x1 - x0) * 3, run_x1, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			ksvg_art_rgb_affine_clip_run(linebuf, x0, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
	}

	data->dst += data->dst_rowstride;
}

static
void ksvg_art_rgb_affine_clip_mask_run(art_u8 *dst_p, const art_u8 *mask, int x0, int x1, int y, const double inv[6],
	int alpha, const art_u8 *src, int src_rowstride, int src_width, int src_height)
{
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int x;

	if(alpha > 255)
		alpha = 255;

	pt.y = y;

	for(x = x0; x < x1; x++)
	{
		pt.x = x;

		art_affine_point(&src_pt, &pt, inv);

		src_x = (int)(src_pt.x);
		src_y = (int)(src_pt.y);

		if(src_x >= 0 && src_x < src_width && src_y >= 0 && src_y < src_height)
		{
			int s;
			int d;
			int tmp;
			int srcAlpha;

			src_p = src + (src_y * src_rowstride) + src_x * 4;

			srcAlpha = alpha * src_p[3] + 0x80;
			srcAlpha = (srcAlpha + (srcAlpha >> 8)) >> 8;

			srcAlpha = (srcAlpha * *mask++) + 0x80;
			srcAlpha = (srcAlpha + (srcAlpha >> 8)) >> 8;

			d = *dst_p;
			s = src_p[2];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;
			s = src_p[1];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;
			s = src_p[0];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;
		}
		else
		{
			dst_p += 3;
			mask++;
		}
	}
}

static void
ksvg_art_rgb_affine_clip_mask_callback (void *callback_data, int y,
														int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ksvgArtRgbAffineClipAlphaData *data = (ksvgArtRgbAffineClipAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	int *alphatab;
	int alpha;
	const art_u8 *maskbuf;

	linebuf = data->dst;
	x0 = data->x0;
	x1 = data->x1;

	alphatab = data->alphatab;
	maskbuf = data->mask + (y - data->y0) * (x1 - x0);

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgb_affine_clip_mask_run(linebuf, maskbuf, x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
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
					ksvg_art_rgb_affine_clip_mask_run(linebuf + (run_x0 - x0) * 3, maskbuf + (run_x0 - x0), run_x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgb_affine_clip_mask_run(linebuf + (run_x1 - x0) * 3, maskbuf + (run_x1 - x0), run_x1, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			ksvg_art_rgb_affine_clip_mask_run(linebuf, maskbuf, x0, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
	}

	data->dst += data->dst_rowstride;
}

static
void ksvg_art_rgba_affine_clip_run(art_u8 *dst_p, int x0, int x1, int y, const double inv[6],
	int alpha, const art_u8 *src, int src_rowstride, int src_width, int src_height)
{
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int x;

	if(alpha > 255)
		alpha = 255;

	pt.y = y;

	for(x = x0; x < x1; x++)
	{
		pt.x = x;

		art_affine_point(&src_pt, &pt, inv);

		src_x = (int)(src_pt.x);
		src_y = (int)(src_pt.y);

		if(src_x >= 0 && src_x < src_width && src_y >= 0 && src_y < src_height)
		{
			int s;
			int d;
			int tmp;
			int srcAlpha;

			src_p = src + (src_y * src_rowstride) + src_x * 4;

			srcAlpha = alpha * src_p[3] + 0x80;
			srcAlpha = (srcAlpha + (srcAlpha >> 8)) >> 8;

			d = *dst_p;
			s = src_p[2];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;
			s = src_p[1];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;
			s = src_p[0];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;

			tmp = srcAlpha * (255 - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;
		}
		else
			dst_p += 4;
	}
}

static void
ksvg_art_rgba_affine_clip_callback (void *callback_data, int y,
														int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ksvgArtRgbAffineClipAlphaData *data = (ksvgArtRgbAffineClipAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	int *alphatab;
	int alpha;

	linebuf = data->dst;
	x0 = data->x0;
	x1 = data->x1;

	alphatab = data->alphatab;

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgba_affine_clip_run(linebuf, x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
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
					ksvg_art_rgba_affine_clip_run(linebuf + (run_x0 - x0) * 4, run_x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgba_affine_clip_run(linebuf + (run_x1 - x0) * 4, run_x1, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			ksvg_art_rgba_affine_clip_run(linebuf, x0, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
	}

	data->dst += data->dst_rowstride;
}

static
void ksvg_art_rgba_affine_clip_mask_run(art_u8 *dst_p, const art_u8 *mask, int x0, int x1, int y, const double inv[6],
	int alpha, const art_u8 *src, int src_rowstride, int src_width, int src_height)
{
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int x;

	if(alpha > 255)
		alpha = 255;

	pt.y = y;

	for(x = x0; x < x1; x++)
	{
		pt.x = x;

		art_affine_point(&src_pt, &pt, inv);

		src_x = (int)(src_pt.x);
		src_y = (int)(src_pt.y);

		if(src_x >= 0 && src_x < src_width && src_y >= 0 && src_y < src_height)
		{
			int s;
			int d;
			int tmp;
			int srcAlpha;

			src_p = src + (src_y * src_rowstride) + src_x * 4;

			srcAlpha = alpha * src_p[3] + 0x80;
			srcAlpha = (srcAlpha + (srcAlpha >> 8)) >> 8;

			srcAlpha = (srcAlpha * *mask++) + 0x80;
			srcAlpha = (srcAlpha + (srcAlpha >> 8)) >> 8;

			d = *dst_p;
			s = src_p[2];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;
			s = src_p[1];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;
			s = src_p[0];

			tmp = srcAlpha * (s - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;

			d = *dst_p;

			tmp = srcAlpha * (255 - d) + 0x80;
			tmp = (tmp + (tmp >> 8)) >> 8;

			*dst_p++ = d + tmp;
		}
		else
		{
			dst_p += 4;
			mask++;
		}
	}
}

static void
ksvg_art_rgba_affine_clip_mask_callback (void *callback_data, int y,
														int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ksvgArtRgbAffineClipAlphaData *data = (ksvgArtRgbAffineClipAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	int *alphatab;
	int alpha;
	const art_u8 *maskbuf;

	linebuf = data->dst;
	x0 = data->x0;
	x1 = data->x1;

	alphatab = data->alphatab;
	maskbuf = data->mask + (y - data->y0) * (x1 - x0);

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgba_affine_clip_mask_run(linebuf, maskbuf, x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
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
					ksvg_art_rgba_affine_clip_mask_run(linebuf + (run_x0 - x0) * 4, maskbuf + (run_x0 - x0), run_x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgba_affine_clip_mask_run(linebuf + (run_x1 - x0) * 4, maskbuf + (run_x1 - x0), run_x1, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			ksvg_art_rgba_affine_clip_mask_run(linebuf, maskbuf, x0, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
	}

	data->dst += data->dst_rowstride;
}

/**
 * ksvg_art_rgb_affine_clip: Affine transform source RGB image and composite, with clipping path.
 * @svp: Clipping path.
 * @dst: Destination image RGB buffer.
 * @x0: Left coordinate of destination rectangle.
 * @y0: Top coordinate of destination rectangle.
 * @x1: Right coordinate of destination rectangle.
 * @y1: Bottom coordinate of destination rectangle.
 * @dst_rowstride: Rowstride of @dst buffer.
 * @src: Source image RGB buffer.
 * @src_width: Width of source image.
 * @src_height: Height of source image.
 * @src_rowstride: Rowstride of @src buffer.
 * @affine: Affine transform.
 * @level: Filter level.
 * @alphagamma: #ArtAlphaGamma for gamma-correcting the compositing.
 * @alpha: Alpha, range 0..256.
 *
 * Affine transform the source image stored in @src, compositing over
 * the area of destination image @dst specified by the rectangle
 * (@x0, @y0) - (@x1, @y1). As usual in libart, the left and top edges
 * of this rectangle are included, and the right and bottom edges are
 * excluded.
 *
 * The @alphagamma parameter specifies that the alpha compositing be done
 * in a gamma-corrected color space. Since the source image is opaque RGB,
 * this argument only affects the edges. In the current implementation,
 * it is ignored.
 *
 * The @level parameter specifies the speed/quality tradeoff of the
 * image interpolation. Currently, only ART_FILTER_NEAREST is
 * implemented.
 *
 * KSVG additions : we have changed this function to support an alpha level as well.
*                  also we made sure compositing an rgba image over an rgb buffer works.
**/
void ksvg_art_rgb_affine_clip(const ArtSVP *svp, art_u8 *dst, int x0, int y0, int x1, int y1, int dst_rowstride, int dst_channels,
		const art_u8 *src,
		int src_width, int src_height, int src_rowstride,
		const double affine[6],
		int alpha, const art_u8 *mask)
{
	ksvgArtRgbAffineClipAlphaData data;
	int i;
	int a, da;

	data.alpha = alpha;

	a = 0x8000;
	da = (alpha * 66051 + 0x80) >> 8;	/* 66051 equals 2 ^ 32 / (255 * 255) */

	for(i = 0; i < 256; i++)
	{
		data.alphatab[i] = a >> 16;
		a += da;
	}

	data.dst = dst;
	data.dst_rowstride = dst_rowstride;
	data.x0 = x0;
	data.x1 = x1;
	data.y0 = y0;
	data.mask = mask;

	art_affine_invert(data.inv, affine);

	data.src = src;
	data.src_width = src_width;
	data.src_height = src_height;
	data.src_rowstride = src_rowstride;

	if(dst_channels == 3)
	{
		if(mask)
			art_svp_render_aa(svp, x0, y0, x1, y1, ksvg_art_rgb_affine_clip_mask_callback, &data);
		else
			art_svp_render_aa(svp, x0, y0, x1, y1, ksvg_art_rgb_affine_clip_callback, &data);
	}
	else
	{
		if(mask)
			art_svp_render_aa(svp, x0, y0, x1, y1, ksvg_art_rgba_affine_clip_mask_callback, &data);
		else
			art_svp_render_aa(svp, x0, y0, x1, y1, ksvg_art_rgba_affine_clip_callback, &data);
	}
}


static
void ksvg_art_rgb_texture_run(art_u8 *dst_p, int x0, int x1, int y, const double inv[6],
	int alpha, const art_u8 *src, int src_rowstride, int src_width, int src_height)
{
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int x;
	int srcAlpha;

	if(alpha > 255)
		alpha = 255;

	/* TODO: optimise and filter? */
	pt.y = y + 0.5;

	for(x = x0; x < x1; x++)
	{
		int s;
		int d;
		int tmp;
		int tmp2;

		pt.x = x + 0.5;

		art_affine_point(&src_pt, &pt, inv);

		src_x = (int)floor(src_pt.x);
		src_y = (int)floor(src_pt.y);

		if(src_x < 0)
		{
			/* Can't assume % behaviour with negative values */
			src_x += ((src_x / -src_width) + 1) * src_width;
		}

		if(src_y < 0)
		{
			src_y += ((src_y / -src_height) + 1) * src_height;
		}

		src_x %= src_width;
		src_y %= src_height;

		src_p = src + (src_y * src_rowstride) + src_x * 4;

		/* Pattern source is in RGBA format, premultiplied.
		 * alpha represents fill/stroke/group opacity.
		 *
		 * Multiply source alpha by 'alpha' then composite over.
		 * For each channel, d = d + alpha * (s - srcAlpha * d).
		 */
		
		srcAlpha = src_p[3];

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = alpha * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = alpha * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = alpha * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;
	}
}

static void
ksvg_art_rgb_texture_callback (void *callback_data, int y,
														int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ksvgArtRgbAffineClipAlphaData *data = (ksvgArtRgbAffineClipAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	int *alphatab;
	int alpha;

	linebuf = data->dst;
	x0 = data->x0;
	x1 = data->x1;

	alphatab = data->alphatab;

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgb_texture_run(linebuf, x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
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
					ksvg_art_rgb_texture_run(linebuf + (run_x0 - x0) * 3, run_x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgb_texture_run(linebuf + (run_x1 - x0) * 3, run_x1, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			ksvg_art_rgb_texture_run(linebuf, x0, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
	}

	data->dst += data->dst_rowstride;
}

static
void ksvg_art_rgb_texture_mask_run(art_u8 *dst_p, const art_u8 *mask, int x0, int x1, int y, const double inv[6],
	int alpha, const art_u8 *src, int src_rowstride, int src_width, int src_height)
{
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int x;
	int srcAlpha;

	if(alpha > 255)
		alpha = 255;

	/* TODO: optimise and filter? */
	pt.y = y + 0.5;

	for(x = x0; x < x1; x++)
	{
		int s;
		int d;
		int am;
		int tmp;
		int tmp2;

		pt.x = x + 0.5;

		art_affine_point(&src_pt, &pt, inv);

		src_x = (int)floor(src_pt.x);
		src_y = (int)floor(src_pt.y);

		if(src_x < 0)
		{
			/* Can't assume % behaviour with negative values */
			src_x += ((src_x / -src_width) + 1) * src_width;
		}

		if(src_y < 0)
		{
			src_y += ((src_y / -src_height) + 1) * src_height;
		}

		src_x %= src_width;
		src_y %= src_height;

		src_p = src + (src_y * src_rowstride) + src_x * 4;

		/* Pattern source is in RGBA format, premultiplied.
		 * alpha represents fill/stroke/group opacity.
		 *
		 * Multiply source alpha by 'alpha' and mask value then composite over.
		 * For each channel, d = d + alpha * mask * (s - srcAlpha * d).
		 */
		
		am = (alpha * *mask++) + 0x80;
		am = (am + (am >> 8)) >> 8;

		srcAlpha = src_p[3];

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = am * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = am * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = am * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;
	}
}

static void
ksvg_art_rgb_texture_mask_callback (void *callback_data, int y,
														int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ksvgArtRgbAffineClipAlphaData *data = (ksvgArtRgbAffineClipAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	int *alphatab;
	int alpha;
	const art_u8 *maskbuf;

	linebuf = data->dst;
	x0 = data->x0;
	x1 = data->x1;

	alphatab = data->alphatab;

	maskbuf = data->mask + (y - data->y0) * (x1 - x0);

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgb_texture_mask_run(linebuf, maskbuf, x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
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
					ksvg_art_rgb_texture_mask_run(linebuf + (run_x0 - x0) * 3, maskbuf + (run_x0 - x0), run_x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgb_texture_mask_run(linebuf + (run_x1 - x0) * 3, maskbuf + (run_x1 - x0), run_x1, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			ksvg_art_rgb_texture_mask_run(linebuf, maskbuf, x0, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
	}

	data->dst += data->dst_rowstride;
}

static
void ksvg_art_rgba_texture_run(art_u8 *dst_p, int x0, int x1, int y, const double inv[6],
	int alpha, const art_u8 *src, int src_rowstride, int src_width, int src_height)
{
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int x;
	int srcAlpha;

	if(alpha > 255)
		alpha = 255;

	/* TODO: optimise and filter? */
	pt.y = y + 0.5;

	for(x = x0; x < x1; x++)
	{
		int s;
		int d;
		int tmp;
		int tmp2;

		pt.x = x + 0.5;

		art_affine_point(&src_pt, &pt, inv);

		src_x = (int)floor(src_pt.x);
		src_y = (int)floor(src_pt.y);

		if(src_x < 0)
		{
			/* Can't assume % behaviour with negative values */
			src_x += ((src_x / -src_width) + 1) * src_width;
		}

		if(src_y < 0)
		{
			src_y += ((src_y / -src_height) + 1) * src_height;
		}

		src_x %= src_width;
		src_y %= src_height;

		src_p = src + (src_y * src_rowstride) + src_x * 4;

		/* Pattern source is in RGBA format, premultiplied.
		 * alpha represents fill/stroke/group opacity.
		 *
		 * Multiply source alpha by 'alpha' then composite over.
		 * For each colour channel, d = d + alpha * (s - srcAlpha * d).
		 */
		
		srcAlpha = src_p[3];

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = alpha * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = alpha * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = alpha * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		/* dstAlpha = dstAlpha + srcAlpha * alpha * (1 - dstAlpha) */
		d = *dst_p;

		tmp = srcAlpha * alpha + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = tmp * (255 - d) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;
		src_p++;
	}
}

static void
ksvg_art_rgba_texture_callback (void *callback_data, int y,
														int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ksvgArtRgbAffineClipAlphaData *data = (ksvgArtRgbAffineClipAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	int *alphatab;
	int alpha;

	linebuf = data->dst;
	x0 = data->x0;
	x1 = data->x1;

	alphatab = data->alphatab;

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgba_texture_run(linebuf, x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
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
					ksvg_art_rgba_texture_run(linebuf + (run_x0 - x0) * 4, run_x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgba_texture_run(linebuf + (run_x1 - x0) * 4, run_x1, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			ksvg_art_rgba_texture_run(linebuf, x0, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
	}

	data->dst += data->dst_rowstride;
}

static
void ksvg_art_rgba_texture_mask_run(art_u8 *dst_p, const art_u8 *mask, int x0, int x1, int y, const double inv[6],
	int alpha, const art_u8 *src, int src_rowstride, int src_width, int src_height)
{
	const art_u8 *src_p;
	ArtPoint pt, src_pt;
	int src_x, src_y;
	int x;
	int srcAlpha;

	if(alpha > 255)
		alpha = 255;

	/* TODO: optimise and filter? */
	pt.y = y + 0.5;

	for(x = x0; x < x1; x++)
	{
		int s;
		int d;
		int am;
		int tmp;
		int tmp2;

		pt.x = x + 0.5;

		art_affine_point(&src_pt, &pt, inv);

		src_x = (int)floor(src_pt.x);
		src_y = (int)floor(src_pt.y);

		if(src_x < 0)
		{
			/* Can't assume % behaviour with negative values */
			src_x += ((src_x / -src_width) + 1) * src_width;
		}

		if(src_y < 0)
		{
			src_y += ((src_y / -src_height) + 1) * src_height;
		}

		src_x %= src_width;
		src_y %= src_height;

		src_p = src + (src_y * src_rowstride) + src_x * 4;

		/* Pattern source is in RGBA format, premultiplied.
		 * alpha represents fill/stroke/group opacity.
		 *
		 * Multiply source alpha by 'alpha' and mask value then composite over.
		 * For each channel, d = d + alpha * mask * (s - srcAlpha * d).
		 */
		
		am = (alpha * *mask++) + 0x80;
		am = (am + (am >> 8)) >> 8;

		srcAlpha = src_p[3];

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = am * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = am * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		d = *dst_p;
		s = *src_p++;

		tmp = srcAlpha * d + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = am * (s - tmp) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;

		/* dstAlpha = dstAlpha + srcAlpha * alpha * mask * (1 - dstAlpha) */
		d = *dst_p;

		tmp = srcAlpha * am + 0x80;
		tmp = (tmp + (tmp >> 8)) >> 8;

		tmp2 = tmp * (255 - d) + 0x80;
		tmp2 = (tmp2 + (tmp2 >> 8)) >> 8;

		*dst_p++ = d + tmp2;
		src_p++;
	}
}

static void
ksvg_art_rgba_texture_mask_callback (void *callback_data, int y,
														int start, ArtSVPRenderAAStep *steps, int n_steps)
{
	ksvgArtRgbAffineClipAlphaData *data = (ksvgArtRgbAffineClipAlphaData *)callback_data;
	art_u8 *linebuf;
	int run_x0, run_x1;
	art_u32 running_sum = start;
	int x0, x1;
	int k;
	int *alphatab;
	int alpha;
	const art_u8 *maskbuf;

	linebuf = data->dst;
	x0 = data->x0;
	x1 = data->x1;

	alphatab = data->alphatab;

	maskbuf = data->mask + (y - data->y0) * (x1 - x0);

	if(n_steps > 0)
	{
		run_x1 = steps[0].x;
		if(run_x1 > x0)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgba_texture_mask_run(linebuf, maskbuf, x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
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
					ksvg_art_rgba_texture_mask_run(linebuf + (run_x0 - x0) * 4, maskbuf + (run_x0 - x0), run_x0, run_x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
			}
		}
		running_sum += steps[k].delta;
		if(x1 > run_x1)
		{
			alpha = (running_sum >> 16) & 0xff;
			if(alpha)
				ksvg_art_rgba_texture_mask_run(linebuf + (run_x1 - x0) * 4, maskbuf + (run_x1 - x0), run_x1, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
		}
	}
	else
	{
		alpha = (running_sum >> 16) & 0xff;
		if(alpha)
			ksvg_art_rgba_texture_mask_run(linebuf, maskbuf, x0, x1, y, data->inv, alphatab[alpha], data->src, data->src_rowstride, data->src_width, data->src_height);
	}

	data->dst += data->dst_rowstride;
}

/**
 * ksvg_art_rgb_texture: Affine transform source RGB image and composite, with clipping path.
 * @svp: Clipping path.
 * @dst: Destination image RGB buffer.
 * @x0: Left coordinate of destination rectangle.
 * @y0: Top coordinate of destination rectangle.
 * @x1: Right coordinate of destination rectangle.
 * @y1: Bottom coordinate of destination rectangle.
 * @dst_rowstride: Rowstride of @dst buffer.
 * @src: Source image RGB buffer.
 * @src_width: Width of source image.
 * @src_height: Height of source image.
 * @src_rowstride: Rowstride of @src buffer.
 * @affine: Affine transform.
 * @level: Filter level.
 * @alphagamma: #ArtAlphaGamma for gamma-correcting the compositing.
 * @alpha: Alpha, range 0..256.
 *
 * Affine transform the source image stored in @src, compositing over
 * the area of destination image @dst specified by the rectangle
 * (@x0, @y0) - (@x1, @y1). As usual in libart, the left and top edges
 * of this rectangle are included, and the right and bottom edges are
 * excluded.
 *
 * The @alphagamma parameter specifies that the alpha compositing be done
 * in a gamma-corrected color space. Since the source image is opaque RGB,
 * this argument only affects the edges. In the current implementation,
 * it is ignored.
 *
 * The @level parameter specifies the speed/quality tradeoff of the
 * image interpolation. Currently, only ART_FILTER_NEAREST is
 * implemented.
 *
 * KSVG additions : we have changed this function to support an alpha level as well.
*                  also we made sure compositing an rgba image over an rgb buffer works.
**/
void ksvg_art_rgb_texture(const ArtSVP *svp, art_u8 *dst, int x0, int y0, int x1, int y1, int dst_rowstride,
		int dst_channels,
		const art_u8 *src,
		int src_width, int src_height, int src_rowstride,
		const double affine[6],
		ArtFilterLevel level,
		ArtAlphaGamma *alphaGamma,
		int alpha,
		const art_u8 *mask)
{
	ksvgArtRgbAffineClipAlphaData data;
	int i;
	int a, da;

	data.alpha = alpha;

	a = 0x8000;
	da = (alpha * 66051 + 0x80) >> 8;	/* 66051 equals 2 ^ 32 / (255 * 255) */

	for(i = 0; i < 256; i++)
	{
		data.alphatab[i] = a >> 16;
		a += da;
	}

	data.dst = dst;
	data.dst_rowstride = dst_rowstride;
	data.x0 = x0;
	data.x1 = x1;
	
	data.inv[0] = affine[0];
	data.inv[1] = affine[1];
	data.inv[2] = affine[2];
	data.inv[3] = affine[3];
	data.inv[4] = affine[4];
	data.inv[5] = affine[5];

	data.src = src;
	data.src_width = src_width;
	data.src_height = src_height;
	data.src_rowstride = src_rowstride;

	data.mask = mask;
	data.y0 = y0;

	if(mask)
	{
		if(dst_channels == 3)
			art_svp_render_aa(svp, x0, y0, x1, y1, ksvg_art_rgb_texture_mask_callback, &data);
		else
			art_svp_render_aa(svp, x0, y0, x1, y1, ksvg_art_rgba_texture_mask_callback, &data);
	}
	else
	{
		if(dst_channels == 3)
			art_svp_render_aa(svp, x0, y0, x1, y1, ksvg_art_rgb_texture_callback, &data);
		else
			art_svp_render_aa(svp, x0, y0, x1, y1, ksvg_art_rgba_texture_callback, &data);
	}
}

/**
 * ksvg_art_svp_move: moves an svp relatively to the current position. 
 * @svp: SVP to move.
 * @dx: relative amount to move horizontally.
 * @dy: relative amount to move vertically.
 *
 * Note : this function always moves the svp, not taking into account render buffer
 * boundaries.
 **/
void ksvg_art_svp_move(ArtSVP *svp, int dx, int dy)
{
	int i, j;
	ArtSVPSeg *seg;

	if(dx == 0 && dy == 0) return;
	for(i = 0;i < svp->n_segs;i++)
	{
		seg = &svp->segs[i];
		for(j = 0;j < seg->n_points;j++)
		{
			seg->points[j].x += dx;
			seg->points[j].y += dy;
		}
		seg->bbox.x0 += dx;
		seg->bbox.y0 += dy;
		seg->bbox.x1 += dx;
		seg->bbox.y1 += dy;
	}
}

