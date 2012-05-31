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

#ifndef T2P_CONVERTER_H
#define T2P_CONVERTER_H

#include "Cache.h"

// FreeType 2 includes
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H

namespace T2P
{
	class BezierPath;

	class Font;
	class Glyph;
	class Affine;
	class GlyphSet;
	class Rectangle;
	class GlyphTracer;
	class GlyphAffinePair;
	class FontVisualParams;
	class GlyphLayoutParams;
	class GlyphRenderParams;

	typedef myboost::shared_ptr<Font> SharedFont;
	typedef myboost::shared_ptr<Glyph> SharedGlyph;

	class Converter
	{
	public:
		Converter(GlyphTracer *tracer);
		~Converter();
	
		// Is initialized? 
		void init();
		bool ready();

		// Kerning control
		void setKerning(bool mode);

		SharedFont requestFont(const FontVisualParams *params);
		GlyphAffinePair *requestGlyph(GlyphRenderParams *params, Rectangle &bbox, Affine &affine, bool onlyLatin);

		GlyphSet *calcString(Font *font, const unsigned short *text, unsigned int length, Affine &affine, const GlyphLayoutParams *params, BezierPath *bpath = 0);
		SharedGlyph calcGlyph(const GlyphRenderParams *params, Affine &affine, bool onlyLatin);
	
		void selectGlyph(GlyphRenderParams *params);

	protected:
		friend class Font;
		
		// Internal
		FT_Library library() { return m_library; }
	
	private:
		std::string cacheFontKey(const FontVisualParams *params) const;
		std::string cacheGlyphKey(const GlyphRenderParams *params) const;

		FT_Library m_library;

		GlyphTracer *m_glyphTracer;

		Cache<Glyph> m_glyphCache;
		Cache<Font> m_fontCache;
	
		bool m_init, m_kerning;
	};
}

#endif

// vim:ts=4:noet
