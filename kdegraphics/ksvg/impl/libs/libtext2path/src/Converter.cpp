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

#include <math.h>

#include "myboost/shared_ptr.hpp"
#include <fontconfig/fontconfig.h>
#include <fribidi/fribidi.h>
#include <fribidi/fribidi-types.h>

#include "Font.h"
#include "Glyph.h"
#include "Tools.h"
#include "Rectangle.h"
#include "Converter.h"
#include "QtUnicode.h"
#include "GlyphTracer.h"

// Macros
#define FT_TRUNC(x)		((x) >> 6)
#define FT_TOFLOAT(x)	((x) * (1.0 / 64.0))
#define FT_FROMFLOAT(x)	((int) floor ((x) * 64.0 + 0.5))

const double deg2rad = 0.017453292519943295769; // pi/180

using namespace T2P;

// Font engine. The core component.
Converter::Converter(GlyphTracer *tracer) : m_glyphTracer(tracer)
{
	m_init = false;
	m_kerning = true;
	m_library = 0;
}

Converter::~Converter()
{
	if(m_glyphTracer)
		delete m_glyphTracer;

	m_fontCache.clear();
	m_glyphCache.clear();

	// Close FreeType2 library
	if(m_library)
		FT_Done_FreeType(m_library);
}

void Converter::init()
{
	// Initialize FreeType2
	m_init = (FT_Init_FreeType(&m_library) == 0);
}

bool Converter::ready()
{
	return m_init;
}

void Converter::setKerning(bool mode)
{
	m_kerning = mode;
}

// Lookup font in cache or create new font
SharedFont Converter::requestFont(const FontVisualParams *params)
{
	std::string cacheKey = cacheFontKey(params);
	SharedFont cached = m_fontCache.find(cacheKey);

	// If not available in cache, create new one and cache it :)
	if(cached)
	{
		delete params;
		return cached;
	}
	else
	{
		// Create a new Font
		SharedFont newFont(new Font(this));

		// Load the font
		if(!newFont->load(params))
		{
			delete params;
			return SharedFont();
		}

		// Append to cache
		m_fontCache.insert(cacheKey, newFont);
		return newFont;
	}
}

GlyphAffinePair *Converter::requestGlyph(GlyphRenderParams *params, Rectangle &bbox, Affine &affine, bool onlyLatin)
{
	// 1. Select glyph to have glyphIndex information, 
	// 	  needed to generate the cache lookup key
	selectGlyph(params);

	SharedGlyph cached = m_glyphCache.find(cacheGlyphKey(params));

	// If not available in cache, render new one and cache it :)
	// If we're mixing ie. japanese and latin characters (TTB layout),
	// then we also have to re-calculate the glyph again with the appropriate rotation matrix (Niko)
	if(!cached || !onlyLatin)
		cached = calcGlyph(params, affine, onlyLatin);

	// Correct bezier path by setting up the correct scaling matrix
	// and multiplying with the current glyph affine
	double fontSize = params->font()->fontParams()->size();

	T2P::Affine correctAffine;
	correctAffine.scale(0.001 * fontSize, -0.001 * fontSize);
	correctAffine *= affine;

	// Get bbox information
	Point p1(FT_TRUNC(cached->ftBbox()->xMin), FT_TRUNC(cached->ftBbox()->yMin));
	Point p2(FT_TRUNC(cached->ftBbox()->xMax), FT_TRUNC(cached->ftBbox()->yMax));

	bbox.setA(affine.mapPoint(p1));
	bbox.setB(affine.mapPoint(p2));

	return new GlyphAffinePair(cached.get(), correctAffine);
}

void Converter::selectGlyph(GlyphRenderParams *params)
{
	// 1. Select glyph + test if it exists
	//		|
	//		|-> if it doesn't exist, replace by a '?' and check if exists
	//		|-> if it does exist, use it!
	params->setGlyphIndex(FT_Get_Char_Index(params->font()->fontFace(), (FT_ULong) params->character()));

	if(params->glyphIndex() == 0)
		params->setGlyphIndex(FT_Get_Char_Index(params->font()->fontFace(), '?'));

	// 2. Load glyph into font's glyphSlot, according to the GlyphLayoutParams
	int flags = FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;

	// 3. Don't pass FT_LOAD_VERTICAL_LAYOUT on TTB layouts when rendering
	// 	  a latin glyph because it needs to be rotated...
	if(params->layout()->tb())
	{
		Script script;
		SCRIPT_FOR_CHAR(script, params->character())
		if(script != Latin || params->layout()->glyphOrientationVertical() == 0)
			flags |= FT_LOAD_VERTICAL_LAYOUT;
	}

	FT_Error error = FT_Load_Glyph(params->font()->fontFace(), params->glyphIndex(), flags);
	if(error)
		params->setGlyphIndex(0);
}
	
SharedGlyph Converter::calcGlyph(const GlyphRenderParams *params, Affine &affine, bool onlyLatin)
{
	// 2. Apply kerning if needed
	if(m_kerning && params->lastGlyph() != 0 && params->glyphIndex() != 0)
	{
		FT_Vector kerningVector;
		double kx, ky;

		FT_Get_Kerning(params->font()->fontFace(), params->lastGlyph(), params->glyphIndex(), ft_kerning_default, &kerningVector);

		kx = FT_TRUNC(kerningVector.x);
		ky = FT_TRUNC(kerningVector.y);

		affine.dx() += kx + affine.m21() * ky;

		// Only apply y kerning in TB mode
		if(params->layout()->tb())
			affine.dy() += kx + affine.m22() * ky;
	}

	// 3. Prepare path tracing
	FT_OutlineGlyph ftGlyph;
	FT_Get_Glyph(params->font()->fontFace()->glyph, reinterpret_cast<FT_Glyph *>(&ftGlyph));

	// 3a. Prepare tracing matrix
	Affine traceAffine;
	traceAffine.scale(1000.0 / params->font()->fontFace()->units_per_EM);

	// 3b. Enable character rotation, if needed
	if(params->layout()->tb())
	{
		Script script;
		SCRIPT_FOR_CHAR(script, params->character())
		if(!onlyLatin && script == Latin)
		{
			FT_Matrix matrix;

			double angle = deg2rad * params->layout()->glyphOrientationVertical();
			matrix.xx = (FT_Fixed)( cos(angle) * 0x10000L);
			matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
			matrix.yx = (FT_Fixed)( sin(angle) * 0x10000L);
			matrix.yy = (FT_Fixed)( cos(angle) * 0x10000L);

			FT_Glyph_Transform(reinterpret_cast<FT_Glyph>(ftGlyph), &matrix, 0);
		}
	}

	// 4. Begin path tracing
	//    - Setup glyphOutlineing glyph
	//    - Start tracing
	//    - Close path
	FT_Outline *ftOut = &ftGlyph->outline;

	SharedGlyph glyphOutline(new Glyph());
	glyphOutline->setBezierPath(m_glyphTracer->allocBezierPath(ftOut->n_points * 2 + ftOut->n_contours + 1));
	glyphOutline->setAffine(traceAffine);

	FT_Outline_Decompose(ftOut, m_glyphTracer->outlineFuncs(), glyphOutline.get());

	m_glyphTracer->closePath(glyphOutline.get());

	// 5. Determine bounding box (aka "cbox")
	FT_Glyph_Get_CBox(reinterpret_cast<FT_Glyph>(ftGlyph), ft_glyph_bbox_unscaled, glyphOutline->ftBbox());

	// 6. Cache glyph!
	m_glyphCache.insert(cacheGlyphKey(params), glyphOutline);

	FT_Done_Glyph(reinterpret_cast<FT_Glyph>(ftGlyph));

	return glyphOutline;
}

GlyphSet *Converter::calcString(Font *font, const unsigned short *text, unsigned int length, Affine &affine, const GlyphLayoutParams *params, BezierPath *bpath)
{
	unsigned short *bidi = 0;

	// 0. Apply BiDi Rules
	if(params->useBidi())
	{
		FriBidiCharType baseDir = FRIBIDI_TYPE_N;
		FriBidiChar *unicodeIn = new FriBidiChar[length + 1];
		FriBidiChar *unicodeOut = new FriBidiChar[length + 1];
		bidi = new unsigned short[length + 1];	

		for(unsigned int i = 0; i < length; i++)
			unicodeIn[i] = text[i];
	
		fribidi_log2vis(unicodeIn, length, &baseDir, unicodeOut, 0, 0, 0); 
	
		for(unsigned int i = 0; i < length; i++)
			bidi[i] = unicodeOut[i];

		bidi[length] = 0;
		delete []unicodeIn;
		delete []unicodeOut;
	}
	else
		bidi = const_cast<unsigned short *>(text);

	// Detect character rotation if needed,
	// probably working in 0.1% of the cases :/
	Script script;	
	bool onlyLatin = true;
	for(unsigned int i = 0; i < length; i++)
	{
		SCRIPT_FOR_CHAR(script, bidi[i])
		if(script != Latin)
		{
			onlyLatin = false;
			break;
		}
	}

	// 1. Set initial font size
	double fontSize = font->fontParams()->size();
	FT_Set_Char_Size(font->fontFace(), FT_FROMFLOAT(fontSize), FT_FROMFLOAT(fontSize), 0, 0);

	// 2. Compute positioning metrics
	//	  - See http://www.freetype.org/freetype2/docs/tutorial/stepX.html (X = 1 or 2)
	int pixelHeight = (int) (FT_TOFLOAT(font->fontFace()->size->metrics.ascender - font->fontFace()->size->metrics.descender) * affine.m22());
	int pixelBaseline = (int) (FT_TOFLOAT(font->fontFace()->size->metrics.ascender) * affine.m22());
	int pixelUnderlinePosition = (int) (((font->fontFace()->ascender - font->fontFace()->underline_position - font->fontFace()->underline_thickness / 2) * fontSize / font->fontFace()->units_per_EM) * affine.m22());
	int pixelUnderlineThickness = T2PMAX(1, (int) ((font->fontFace()->underline_thickness * fontSize / font->fontFace()->units_per_EM) * affine.m22()));

	// 3. Prepare needed variables for the rendering loop
	// 	  - rendering params (layout, font...)
	// 	  - bounding box (per glyph, overall)
	// 	  - glyph matrix (overall)
	// 	  - resulting glyph sets
	GlyphRenderParams *renderParams = new GlyphRenderParams();
	renderParams->setFont(font);
	renderParams->setLayout(params);
	renderParams->setLastGlyph(0);
	renderParams->setGlyphIndex(0);

	Affine glyphAffine(affine);
	Rectangle bbox, currentBbox;
	GlyphSet *result = new GlyphSet();

	// Align to path start, if bpath != 0
	Point pathPoint, pathTangent, pathNormal;
	double pathLength = 0, pathAdvance = 0;
	double startOffset = params->textPathStartOffset();

	if(bpath)
	{
		pathLength = bpath->length();
		bpath->pointTangentNormalAt(startOffset, &pathPoint, &pathTangent);

		glyphAffine.dx() = pathPoint.x();
		glyphAffine.dy() = pathPoint.y();
		glyphAffine.rotate(atan2(pathTangent.y(), pathTangent.x()));

		std::cout << "[T2P] Starting textPath at: " << pathPoint.x() << ", " << pathPoint.y() << std::endl;
	}

	// <tspan> elements can have different baseline-shift's
	// baseline-shift support (Niko)
	double baseline = 0;
	if(!params->baselineShift().empty())
	{
		if(params->baselineShift() == "sub")
			baseline += fontSize / 5 + 1;
		else if(params->baselineShift() == "super")
			baseline -= fontSize / 3 + 1;
		else
		{
			std::string temp = params->baselineShift();
			if(temp.find("%") > 0)
				baseline -= (atof(temp.c_str()) / 100.0) * fontSize;
			else
				baseline -= atoi(temp.c_str());
		}

		if(!params->tb())
			glyphAffine.translate(0, baseline);
		else
			glyphAffine.translate(-baseline, 0);
	}

	// 4. Enter main rendering loop
	for(unsigned int i = 0; i < length; i++)
	{
		// 4a. Modify character to render
		renderParams->setCharacter(bidi[i]);

		// 4c. Get glyph outline
		GlyphAffinePair *glyphOutline = requestGlyph(renderParams, currentBbox, glyphAffine, onlyLatin);
		m_glyphTracer->correctGlyph(glyphOutline);

		// 4d. Save advance values
		result->m_glyphXAdvance.push_back((font->fontFace()->glyph->advance.x * fontSize / font->fontFace()->units_per_EM) * affine.m11());
		result->m_glyphYAdvance.push_back((font->fontFace()->glyph->advance.y * fontSize / font->fontFace()->units_per_EM) * affine.m22());

		// 4e. Misc
		//	   - union current + overall bounding box
		//	   - add glyph to list of glyphs
		//	   - apply letter & word spacing
		if(renderParams->glyphIndex() != 0)
		{
			// Remember last glyph for kerning
			renderParams->setLastGlyph(renderParams->glyphIndex());

			// Union current + overall bounding box
			bbox.bboxUnion(bbox, currentBbox);

			// Move glyph locations for the next glyph to be
			// rendered apply letter & word spacing
			bool addGlyph = true;
			if(!params->tb())
			{
				if(bpath)
				{
					double advance = startOffset + (pathAdvance / pathLength);
					if(advance < 0.0 || advance > 1.0) // dont render off-path glyphs
						addGlyph = false;

					pathAdvance += (result->m_glyphXAdvance.back() + params->letterSpacing() * affine.m11()) + ((bidi[(i + 1)] == ' ') ? params->wordSpacing() * affine.m11() : 0);

					std::cout << "[T2P] Adjusting textPath advance: " << pathAdvance << " Path Length: " << pathLength << " StartOffset: " << startOffset << " Position in Path (relative 0-1): " << pathAdvance/pathLength << " Adjusted by offset: " << startOffset + (pathAdvance / pathLength) << std::endl;

					bpath->pointTangentNormalAt(startOffset + (pathAdvance / pathLength), &pathPoint, &pathTangent, &pathNormal);

					glyphAffine.m11() = affine.m11();
					glyphAffine.m12() = affine.m12();
					glyphAffine.m21() = affine.m21();
					glyphAffine.m22() = affine.m22();
					glyphAffine.dx() = pathPoint.x();
					glyphAffine.dy() = pathPoint.y();

					glyphAffine.rotateAround(atan2(pathTangent.y(), pathTangent.x()), Point(result->m_glyphXAdvance.back() / 2, result->m_glyphYAdvance.back() / 2)); 
		
					std::cout << "[T2P] Aligning textPath to: " << pathPoint.x() << ", " << pathPoint.y() << std::endl;
					if(!params->tb())
						glyphAffine.translate(0, baseline);
					else
						glyphAffine.translate(-baseline, 0);
				}
				else
					glyphAffine.dx() += (result->m_glyphXAdvance.back() + params->letterSpacing() * affine.m11()) + ((bidi[(i + 1)] == ' ') ? params->wordSpacing() * affine.m11() : 0);
			}
			else
			{
				double advy = result->m_glyphYAdvance.back();

				Script script;
				SCRIPT_FOR_CHAR(script, bidi[i])
				if(!onlyLatin && script == Latin && params->glyphOrientationVertical() != 0)
					advy = result->m_glyphXAdvance.back();

				glyphAffine.dy() += (advy + params->letterSpacing() * affine.m22()) + ((bidi[(i + 1)] == ' ') ? params->wordSpacing() * affine.m22() : 0);
			}

			// Copy bezier path/affine pair
			if(addGlyph)
			{
				result->m_set.push_back(glyphOutline);
				result->m_glyphCount++;
			}
			else
				delete glyphOutline;
		}
	}

	// 5. Fill out resulting data structures
	result->m_underlinePosition = pixelUnderlinePosition;
	result->m_overlinePosition = pixelHeight - pixelUnderlinePosition;
	result->m_strikeThroughPosition = (result->m_underlinePosition + result->m_overlinePosition) / 2;
	result->m_pixelBaseline = pixelBaseline;
	result->m_underlineThickness = pixelUnderlineThickness;

	result->m_xpen = glyphAffine.dx() - affine.dx();
	result->m_ypen = glyphAffine.dy() - affine.dy();

	result->m_bboxX = T2PMAX(1, int(bbox.a().x()));
	result->m_bboxY = T2PMAX(1, int(bbox.a().y() - int(pixelHeight / 2)));
	result->m_height = T2PMAX(1, int(bbox.b().y() - bbox.a().y()));

	// Correct bounding box information also on
	// vertical layouts! (Niko)
	if(!params->tb())
	{
		if(bpath)
			result->m_width = int(pathAdvance);
		else
			result->m_width = T2PMAX(1, int(bbox.b().x() - bbox.a().x()));
	}
	else
		result->m_width = T2PMAX(1, pixelHeight);

	// 6. Cleanup memory
	if(text != bidi)
		delete []bidi;

	delete renderParams;

	// 7. Eventually, dump cache statistics
	// m_glyphCache.dump();
	// m_fontCache.dump();

	return result;
}

std::string Converter::cacheFontKey(const FontVisualParams *params) const
{
	// TODO: Tune the key
	std::string key;

	key += Tools::joinList('|', const_cast<FontVisualParams *>(params)->fontList());
	key += Tools::a2str(params->weight());
	key += Tools::a2str(params->slant());
	key += Tools::a2str(params->size());

	// std::cout << "Font cache key: " << key << std::endl;
	return key;
}

std::string Converter::cacheGlyphKey(const GlyphRenderParams *params) const
{
	// TODO: Tune the key
	std::string key;

	key += params->font()->fontFile();
	key += Tools::a2str(params->character());
	key += Tools::a2str(params->glyphIndex());
	key += Tools::a2str(params->font()->fontParams()->size());
	key += Tools::a2str(params->font()->fontParams()->weight());
	key += Tools::a2str(params->font()->fontParams()->slant());

	// std::cout << "Glyph cache key: " << key << std::endl;
	return key;
}
// vim:ts=4:noet
