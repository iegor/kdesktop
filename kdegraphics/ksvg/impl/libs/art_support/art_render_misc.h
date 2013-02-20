/* This file is part of the KDE project.
 * art_render_misc.c: Here I store some routines I feel should be in libart :)
 *
 * Copyright (C) 2001-2002 KSVG Team
 *
 * This code is adapted from :
 *
 * art_render_gradient.h: Gradient image source for modular rendering.
 *
 * Libart_LGPL - library of basic graphic primitives
 * Copyright (C) 2000 Raph Levien
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
 *
 * Authors: Raph Levien <raph@acm.org>
 *          Alexander Larsson <alla@lysator.liu.se>
 */

#ifndef __ART_RENDER_MISC_H__
#define __ART_RENDER_MISC_H__

#ifdef LIBART_COMPILATION
#include "art_filterlevel.h"
#include "art_render.h"
#include "art_render_gradient.h"
#else
#include <libart_lgpl/art_filterlevel.h>
#include <libart_lgpl/art_render.h>
#include <libart_lgpl/art_render_gradient.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
	ART_KSVG_SRGB_INTERPOLATION,
	ART_KSVG_LINEARRGB_INTERPOLATION
} ArtKSVGGradientInterpolation;

typedef struct _ArtKSVGGradientRadial ArtKSVGGradientRadial;

struct _ArtKSVGGradientRadial {
  double affine[6]; /* transforms user coordinates to unit circle */
  double fx, fy;    /* focal point in unit circle coords */
  int n_stops;
  ArtGradientSpread spread;
  ArtGradientStop *stops;
  ArtKSVGGradientInterpolation interpolation;
};

void
art_ksvg_render_gradient_radial (ArtRender *render,
			    const ArtKSVGGradientRadial *gradient,
			    ArtFilterLevel level);

typedef struct _ArtKSVGGradientLinear ArtKSVGGradientLinear;

struct _ArtKSVGGradientLinear {
	double affine[6]; /* transforms screen gradient vector to unit vector (1, 0) */
  ArtGradientSpread spread;
  int n_stops;
  ArtGradientStop *stops;
  ArtKSVGGradientInterpolation interpolation;
};

void
art_ksvg_render_gradient_linear (ArtRender *render,
			    const ArtKSVGGradientLinear *gradient,
			    ArtFilterLevel level);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ART_RENDER_MISC_H__ */
