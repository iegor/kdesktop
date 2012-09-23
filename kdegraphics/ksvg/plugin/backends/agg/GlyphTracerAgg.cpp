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

#include "Glyph.h"
#include "Point.h"
#include "BezierPathAgg.h"
#include "GlyphTracerAgg.h"

using namespace T2P;

int traceMoveto(FT_Vector *to, void *obj)
{
	Glyph *glyph = reinterpret_cast<Glyph *>(obj);
	Affine &affine = glyph->affine();
	BezierPathAgg *bpath = static_cast<BezierPathAgg *>(glyph->modifiableBezierPath());
	Point p = affine.mapPoint(Point(to->x, to->y));
	bpath->m_storage.move_to(p.x(), p.y());
	return 0;
}

int traceLineto(FT_Vector *to, void *obj)
{
	Glyph *glyph = reinterpret_cast<Glyph *>(obj);
	Affine &affine = glyph->affine();
	BezierPathAgg *bpath = static_cast<BezierPathAgg *>(glyph->modifiableBezierPath());
	Point p = affine.mapPoint(Point(to->x, to->y));
	bpath->m_storage.line_to(p.x(), p.y());
	return 0;
}

int traceConicBezier(FT_Vector *control, FT_Vector *to, void *obj)
{
	Glyph *glyph = reinterpret_cast<Glyph *>(obj);
	Affine &affine = glyph->affine();
	BezierPathAgg *bpath = static_cast<BezierPathAgg *>(glyph->modifiableBezierPath());
	Point c = affine.mapPoint(Point(control->x, control->y));
	Point p = affine.mapPoint(Point(to->x, to->y));

	double sx, sy;
	bpath->m_storage.vertex(bpath->m_storage.total_vertices() - 1, &sx, &sy);
	bpath->m_storage.curve4(c.x() - (c.x() - sx) / 3, c.y() - (c.y() - sy) / 3,
						  c.x() + (p.x() - c.x()) / 3, c.y() + (p.y() - c.y()) / 3, p.x(), p.y());

	return 0;
}

int traceCubicBezier(FT_Vector *control1, FT_Vector *control2, FT_Vector *to, void *obj)
{
	Glyph *glyph = reinterpret_cast<Glyph *>(obj);
	Affine &affine = glyph->affine();
	BezierPathAgg *bpath = static_cast<BezierPathAgg *>(glyph->modifiableBezierPath());
	Point p = affine.mapPoint(Point(to->x, to->y));
	Point c1 = affine.mapPoint(Point(control1->x, control1->y));
	Point c2 = affine.mapPoint(Point(control2->x, control2->y));
	bpath->m_storage.curve4(c1.x(), c1.y(), c2.x(), c2.y(), p.x(), p.y());
	return 0;
}

GlyphTracerAgg::GlyphTracerAgg() : GlyphTracer()
{
	setMoveto(*traceMoveto);
	setLineto(*traceLineto);
	setConicBezier(*traceConicBezier);
	setCubicBezier(*traceCubicBezier);
}

GlyphTracerAgg::~GlyphTracerAgg()
{
}

void GlyphTracerAgg::correctGlyph(GlyphAffinePair *glyphAffine)
{

	// Take bezier path belonging to glyph (Note: that one is _UNMODIFIABLE_, once calculated)
	const BezierPathAgg *path = static_cast<const BezierPathAgg *>(glyphAffine->glyph()->bezierPath());

	BezierPathAgg *transformatedPath = static_cast<BezierPathAgg *>(allocBezierPath(0));
	Affine &affine = glyphAffine->affine();
	transformatedPath->copy_from(path->m_storage, affine);
	glyphAffine->setTransformatedPath(transformatedPath);
}

BezierPath *GlyphTracerAgg::allocBezierPath(int)
{
	return new BezierPathAgg();
}	  

void GlyphTracerAgg::closePath(Glyph *glyph)
{
	BezierPathAgg *bpath = static_cast<BezierPathAgg *>(glyph->modifiableBezierPath());
	bpath->m_storage.close_polygon();
}

// vim:ts=4:noet
