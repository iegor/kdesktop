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

#ifndef T2P_GLYPHTRACER_H
#define T2P_GLYPHTRACER_H

// FreeType 2 includes
#include <ft2build.h>
#include FT_FREETYPE_H

namespace T2P
{
	class Glyph;
	class BezierPath;
	class GlyphAffinePair;

	class GlyphTracer
	{
	public:
		GlyphTracer();
		virtual ~GlyphTracer();

		// Needs to be implemented
		virtual void correctGlyph(GlyphAffinePair *glyphAffine) = 0;
		virtual BezierPath *allocBezierPath(int size) = 0;
		virtual void closePath(Glyph *glyph) = 0;

		// FreeType glyph tracing functions
		void setMoveto(FT_Outline_MoveToFunc funcPtr);
		void setLineto(FT_Outline_LineToFunc funcPtr);
		void setConicBezier(FT_Outline_ConicToFunc funcPtr);
		void setCubicBezier(FT_Outline_CubicToFunc funcPtr);

		FT_Outline_Funcs *outlineFuncs();

	private:
		FT_Outline_Funcs *m_outlineMethods;

		FT_Outline_MoveToFunc m_moveTo;
		FT_Outline_LineToFunc m_lineTo;
		FT_Outline_ConicToFunc m_conicBezier;
		FT_Outline_CubicToFunc m_cubicBezier;
	};
}

#endif

// vim:ts=4:noet
