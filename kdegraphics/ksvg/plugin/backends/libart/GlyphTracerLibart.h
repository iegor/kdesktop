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

#ifndef T2P_GLYPHTRACER_LIBART_H
#define T2P_GLYPHTRACER_LIBART_H

#include "GlyphTracer.h"

// FreeType 2 includes
#include <ft2build.h>
#include FT_FREETYPE_H

namespace T2P
{
	class Glyph;
	class BezierPath;
	class GlyphAffinePair;

	class GlyphTracerLibart : public GlyphTracer
	{
	public:
		GlyphTracerLibart();
		virtual ~GlyphTracerLibart();

		virtual void correctGlyph(GlyphAffinePair *glyphAffine);
		virtual BezierPath *allocBezierPath(int size);
		virtual void closePath(Glyph *glyph);
	};
}

#endif

// vim:ts=4:noet
