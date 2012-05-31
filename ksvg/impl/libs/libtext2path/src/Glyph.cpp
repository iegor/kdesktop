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

using namespace T2P;

Glyph::Glyph()
{
	m_bezierPath = 0;
}

Glyph::~Glyph()
{
	delete m_bezierPath;
	m_bezierPath = 0;
}

Affine &Glyph::affine()
{
	return m_affine;
}

void Glyph::setAffine(const Affine &affine)
{
	m_affine = affine;
}

const BezierPath *Glyph::bezierPath() const
{
	return const_cast<const BezierPath *>(m_bezierPath);
}

BezierPath *Glyph::modifiableBezierPath()
{
	return m_bezierPath;
}

void Glyph::setBezierPath(const BezierPath *bpath)
{
	m_bezierPath = const_cast<BezierPath *>(bpath);
}

FT_BBox *Glyph::ftBbox()
{
	return m_ftBbox;
}

// #####

GlyphAffinePair::GlyphAffinePair(const Glyph *glyph, const Affine &affine)
{
	m_glyph = glyph;
	m_affine = affine;
	m_transformatedPath = 0;
}

GlyphAffinePair::~GlyphAffinePair()
{
	// The glyphs are shared and thus not deleted (Niko)
	delete m_transformatedPath;
}

void GlyphAffinePair::setTransformatedPath(const BezierPath *path)
{
	m_transformatedPath = path;
}

const BezierPath *GlyphAffinePair::transformatedPath() const
{
	return m_transformatedPath;
}

const Glyph *GlyphAffinePair::glyph() const
{
	return m_glyph;
}

Affine &GlyphAffinePair::affine()
{
	return m_affine;
}

// #####

GlyphSet::GlyphSet()
{
	m_glyphCount = 0;
	
	m_bboxX = 0;
	m_bboxY = 0;
	m_width = 0;
	m_height = 0;
	
	m_xpen = 0;
	m_ypen = 0;

	m_underlinePosition = 0;
	m_underlineThickness = 0;
	m_overlinePosition = 0;
	m_strikeThroughPosition = 0;
	m_pixelBaseline = 0;
}

GlyphSet::~GlyphSet()
{
	m_set.clear();
}

std::vector<GlyphAffinePair *> &GlyphSet::set()
{
	return m_set;
}

std::list<float> GlyphSet::glyphXAdvance()
{
	return m_glyphXAdvance;
}

std::list<float> GlyphSet::glyphYAdvance()
{
	return m_glyphYAdvance;
}

unsigned int GlyphSet::glyphCount() const
{
	return m_glyphCount;
}

int GlyphSet::width() const
{
	return m_width;
}

int GlyphSet::height() const
{
	return m_height;
}

int GlyphSet::bboxX() const
{
	return m_bboxX;
}

int GlyphSet::bboxY() const
{
	return m_bboxY;
}

double GlyphSet::xpen() const
{
	return m_xpen;
}

double GlyphSet::ypen() const
{
	return m_ypen;
}

int GlyphSet::underlinePosition() const
{
	return m_underlinePosition;
}

int GlyphSet::underlineThickness() const
{
	return m_underlineThickness;
}

int GlyphSet::overlinePosition() const
{
	return m_overlinePosition;
}

int GlyphSet::strikeThroughPosition() const
{
	return m_strikeThroughPosition;
}

int GlyphSet::pixelBaseline() const
{
	return m_pixelBaseline;
}

// #####

GlyphLayoutParams::GlyphLayoutParams()
{
}

GlyphLayoutParams::~GlyphLayoutParams()
{
}

bool GlyphLayoutParams::tb() const
{
	return m_tb;
}

void GlyphLayoutParams::setTb(bool tb)
{
	m_tb = tb;
}

bool GlyphLayoutParams::useBidi() const
{
	return m_useBidi;
}

void GlyphLayoutParams::setUseBidi(bool bidi)
{
	m_useBidi = bidi;
}

double GlyphLayoutParams::wordSpacing() const
{
	return m_wordSpacing;
}

void GlyphLayoutParams::setWordSpacing(double wordSpacing)
{
	m_wordSpacing = wordSpacing;
}

double GlyphLayoutParams::letterSpacing() const
{
	return m_letterSpacing;
}

void GlyphLayoutParams::setLetterSpacing(double letterSpacing)
{
	m_letterSpacing = letterSpacing;
}

std::string GlyphLayoutParams::baselineShift() const
{
	return m_baseline;
}

void GlyphLayoutParams::setBaselineShift(const std::string &baseline)
{
	m_baseline = baseline;
}

int GlyphLayoutParams::glyphOrientationVertical() const
{
	return m_glyphOrientationVertical;
}

void GlyphLayoutParams::setGlyphOrientationVertical(int orient)
{
	m_glyphOrientationVertical = orient;
}

int GlyphLayoutParams::glyphOrientationHorizontal() const
{
	return m_glyphOrientationHorizontal;
}

void GlyphLayoutParams::setGlyphOrientationHorizontal(int orient)
{
	m_glyphOrientationHorizontal = orient;
}

double GlyphLayoutParams::textPathStartOffset() const
{
	return m_textPathStartOffset;
}

void GlyphLayoutParams::setTextPathStartOffset(double offset)
{
	m_textPathStartOffset = offset;
}

// #####

GlyphRenderParams::GlyphRenderParams()
{
}

GlyphRenderParams::~GlyphRenderParams()
{
}

Font *GlyphRenderParams::font() const
{
	return m_font;
}

void GlyphRenderParams::setFont(Font *font)
{
	m_font = font;
}
	
const GlyphLayoutParams *GlyphRenderParams::layout() const
{
	return m_layout;
}

void GlyphRenderParams::setLayout(const GlyphLayoutParams *layout)
{
	m_layout = layout;
}

unsigned int GlyphRenderParams::glyphIndex() const
{
	return m_glyphIndex;
}

void GlyphRenderParams::setGlyphIndex(unsigned int glyphIndex)
{
	m_glyphIndex = glyphIndex;
}

unsigned int GlyphRenderParams::lastGlyph() const
{
	return m_lastGlyph;
}

void GlyphRenderParams::setLastGlyph(unsigned int lastGlyph)
{
	m_lastGlyph = lastGlyph;
}

unsigned short GlyphRenderParams::character() const
{
	return m_character;
}

void GlyphRenderParams::setCharacter(unsigned short character)
{
	m_character = character;
}

// vim:ts=4:noet
