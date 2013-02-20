/*
    Copyright (C) 2003 Nikolas Zimmermann <wildfox@kde.org>
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <iostream>

#include <kdebug.h>

#include "Glyph.h"
#include "Point.h"
#include "BezierPathLibart.h"
#include "GlyphTracerLibart.h"

#include <libart_lgpl/art_bpath.h>

#include <config.h>

#ifdef HAVE_FREETYPE_2_2_x
  #define FT_VECTOR_PARAMETER const FT_Vector
#else
  #define FT_VECTOR_PARAMETER FT_Vector
#endif

using namespace T2P;

int traceMoveto(FT_VECTOR_PARAMETER *to, void *obj)
{
	Glyph *glyph = reinterpret_cast<Glyph *>(obj);
	Affine &affine = glyph->affine();
	BezierPathLibart *path = static_cast<BezierPathLibart *>(glyph->modifiableBezierPath());
	Point p = affine.mapPoint(Point(to->x, to->y));

	int index = path->m_array.count();
	if(index == 0 ||
       p.x() != path->m_array[index - 1].x3 ||
       p.y() != path->m_array[index - 1].y3)
	{
		path->m_array.resize(index + 1);
		path->m_array[index].code = ART_MOVETO;
		path->m_array[index].x3 = p.x();
		path->m_array[index].y3 = p.y();
	}

	return 0;
}

int traceLineto(FT_VECTOR_PARAMETER *to, void *obj)
{
	Glyph *glyph = reinterpret_cast<Glyph *>(obj);
	Affine &affine = glyph->affine();
	BezierPathLibart *path = static_cast<BezierPathLibart *>(glyph->modifiableBezierPath());
	Point p = affine.mapPoint(Point(to->x, to->y));

	int index = path->m_array.count();
	ArtBpath *last = &path->m_array[index - 1];

	if((p.x() != last->x3) || (p.y() != last->y3))
	{
		path->m_array.resize(index + 1);
		path->m_array[index].code = ART_LINETO;
		path->m_array[index].x3 = p.x();
		path->m_array[index].y3 = p.y();
	}

	return 0;
}

int traceConicBezier(FT_VECTOR_PARAMETER *control, FT_VECTOR_PARAMETER *to, void *obj)
{
	Glyph *glyph = reinterpret_cast<Glyph *>(obj);
	Affine &affine = glyph->affine();
	BezierPathLibart *path = static_cast<BezierPathLibart *>(glyph->modifiableBezierPath());

	int index = path->m_array.count();
	if(!(index > 0))
		return -1;

	path->m_array.resize(index + 1);
	ArtBpath *s = &path->m_array[index - 1];
	ArtBpath *e = &path->m_array[index];

	e->code = ART_CURVETO;

	Point c = affine.mapPoint(Point(control->x, control->y));
	Point p = affine.mapPoint(Point(to->x, to->y));
	e->x3 = p.x();
	e->y3 = p.y();

	path->m_array[index].x1 = c.x() - (c.x() - s->x3) / 3;
	path->m_array[index].y1 = c.y() - (c.y() - s->y3) / 3;
	path->m_array[index].x2 = c.x() + (e->x3 - c.x()) / 3;
	path->m_array[index].y2 = c.y() + (e->y3 - c.y()) / 3;

	return 0;
}

int traceCubicBezier(FT_VECTOR_PARAMETER *control1, FT_VECTOR_PARAMETER *control2, FT_VECTOR_PARAMETER *to, void *obj)
{
	Glyph *glyph = reinterpret_cast<Glyph *>(obj);
	Affine &affine = glyph->affine();
	BezierPathLibart *path = static_cast<BezierPathLibart *>(glyph->modifiableBezierPath());

	Point p = affine.mapPoint(Point(to->x, to->y));
	Point c1 = affine.mapPoint(Point(control1->x, control1->y));
	Point c2 = affine.mapPoint(Point(control2->x, control2->y));

	int index = path->m_array.count();
	path->m_array.resize(index + 1);
	path->m_array[index].code = ART_CURVETO;
	path->m_array[index].x1 = c1.x();
	path->m_array[index].y1 = c1.y();
	path->m_array[index].x2 = c2.x();
	path->m_array[index].y2 = c2.y();
	path->m_array[index].x3 = p.x();
	path->m_array[index].y3 = p.y();

	return 0;
}

GlyphTracerLibart::GlyphTracerLibart() : GlyphTracer()
{
	setMoveto(*traceMoveto);
	setLineto(*traceLineto);
	setConicBezier(*traceConicBezier);
	setCubicBezier(*traceCubicBezier);
}

GlyphTracerLibart::~GlyphTracerLibart()
{
}

void GlyphTracerLibart::correctGlyph(GlyphAffinePair *glyphAffine)
{
	// Take bezier path belonging to glyph (Note: that one is _UNMODIFIABLE_, once calculated)
	const BezierPathLibart *path = static_cast<const BezierPathLibart *>(glyphAffine->glyph()->bezierPath());

	// Create a new empty path with the same size
	ArtBpath *transformed = art_bpath_affine_transform(path->m_array.data(), glyphAffine->affine().data());
	BezierPathLibart *transformatedPath = new BezierPathLibart(transformed);
	art_free(transformed);
	glyphAffine->setTransformatedPath(transformatedPath);
}

BezierPath *GlyphTracerLibart::allocBezierPath(int)
{
	BezierPathLibart *bpath = new BezierPathLibart();
	//if(size != 0)
	//	bpath->m_array.resize(size);

	return bpath;
}	  

void GlyphTracerLibart::closePath(Glyph *glyph)
{
	BezierPathLibart *path = static_cast<BezierPathLibart *>(glyph->modifiableBezierPath());
	int index = path->m_array.count();
	path->m_array.resize(index + 1);
	path->m_array[index].code = ART_END;
}

// vim:ts=4:noet
