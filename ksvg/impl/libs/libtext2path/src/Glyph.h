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

#ifndef T2P_GLYPH_H
#define T2P_GLYPH_H

#include <list>
#include <vector>
#include <string>

#include "Affine.h"
#include "BezierPath.h"

// FreeType 2 includes
#include <ft2build.h>
#include FT_FREETYPE_H

namespace T2P
{
	class Font;
	
	// Represent one single glyph in cache
	class Glyph
	{
	public:
		Glyph();
		~Glyph();

		Affine &affine(); // Initial tracing affine
		void setAffine(const Affine &affine);
		
		const BezierPath *bezierPath() const; // Non-modifyable bezierPath belonging to this glyph
		BezierPath *modifiableBezierPath(); // ONLY USED BY GLYPHTRACER, DO NOT USE SOMEWHERE ELSE!
		void setBezierPath(const BezierPath *bpath);
		
		FT_BBox *ftBbox();

	private:
		Affine m_affine;
		FT_BBox m_ftBbox[4];
		BezierPath *m_bezierPath;
	};

	// Is created for every character which needs to
	// be rendered and has to be deleted by the "client"
	class GlyphAffinePair
	{
	public:
		GlyphAffinePair(const Glyph *glyph, const Affine &affine);
		~GlyphAffinePair();

		void setTransformatedPath(const BezierPath *path);
		const BezierPath *transformatedPath() const;

		const Glyph *glyph() const;
		Affine &affine();

	private:
		const Glyph *m_glyph;
		const BezierPath *m_transformatedPath;
		Affine m_affine;
	};

	class GlyphSet
	{
	public:
		GlyphSet();
		~GlyphSet();

		std::vector<GlyphAffinePair *> &set();
		std::list<float> glyphXAdvance();
		std::list<float> glyphYAdvance();

		unsigned int glyphCount() const;

		int width() const;
		int height() const;

		int bboxX() const;
		int bboxY() const;

		double xpen() const;
		double ypen() const;
		
		int underlinePosition() const;
		int underlineThickness() const;
		int overlinePosition() const;
		int strikeThroughPosition() const;
		int pixelBaseline() const;
		
	private:
		friend class Converter;
		unsigned int m_glyphCount; // Number of glyphs in the set
		
		int m_bboxX, m_bboxY; // Bounding box locations (x,y) + (x + width, y + width)
		int m_width, m_height;
		
		double m_xpen, m_ypen; // relative pen locations
		
		int m_underlinePosition, m_underlineThickness, m_overlinePosition, m_strikeThroughPosition, m_pixelBaseline;

		std::vector<GlyphAffinePair *> m_set; // Bezier paths in the set

		std::list<float> m_glyphXAdvance; // List of advance values needed ie. for text paths
		std::list<float> m_glyphYAdvance;
	};

	class GlyphLayoutParams
	{
	public:
		GlyphLayoutParams();
		~GlyphLayoutParams();

		bool tb() const;
		void setTb(bool tb);

		bool useBidi() const;
		void setUseBidi(bool bidi);

		double wordSpacing() const;
		void setWordSpacing(double wordSpacing);

		double letterSpacing() const;
		void setLetterSpacing(double letterSpacing);

		std::string baselineShift() const;
		void setBaselineShift(const std::string &baseline);

		int glyphOrientationVertical() const;
		void setGlyphOrientationVertical(int orient);

		int glyphOrientationHorizontal() const;
		void setGlyphOrientationHorizontal(int orient);

		// textOnPath specific stuff
		double textPathStartOffset() const;
		void setTextPathStartOffset(double offset);

	private:
		bool m_tb; // Top-To-Bottom or Bottom-To-Top ?
		bool m_useBidi; // Use Bidi ? 
		double m_wordSpacing, m_letterSpacing; // word/character spacing
		double m_textPathStartOffset; // range: 0.0 - 1.0; start offset in the path
		int m_glyphOrientationVertical, m_glyphOrientationHorizontal; // Degrees...
		std::string m_baseline; // baseline description, using same system as svg
	};

	class GlyphRenderParams
	{
	public:
		GlyphRenderParams();
		~GlyphRenderParams();

		Font *font() const;
		void setFont(Font *font);
		
		const GlyphLayoutParams *layout() const;
		void setLayout(const GlyphLayoutParams *layout);
		
		unsigned int glyphIndex() const;
		void setGlyphIndex(unsigned int glyphIndex);
		
		unsigned int lastGlyph() const;
		void setLastGlyph(unsigned int lastGlyph);

		unsigned short character() const;
		void setCharacter(unsigned short character);

	private:
		Font *m_font;
		const GlyphLayoutParams *m_layout; // Glyph layouting params
	
		unsigned int m_glyphIndex; // 'character' index in font
		unsigned int m_lastGlyph;	// Kerning
		unsigned short m_character; // Unicode glyph to process
	};
}

#endif

// vim:ts=4:noet
