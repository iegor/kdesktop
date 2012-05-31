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
#include <iostream>
#include <fontconfig/fontconfig.h>

#include "Font.h"
#include "Tools.h"
#include "Converter.h"

// Macros
#define FT_TRUNC(x)		((x) >> 6)
#define FT_TOFLOAT(x)	((x) * (1.0 / 64.0))
#define FT_FROMFLOAT(x)	((int) floor ((x) * 64.0 + 0.5))

using namespace T2P;

FontVisualParams::FontVisualParams()
{
	m_size = 0.0;
	m_slant = 0;
	m_weight = 0;
}

FontVisualParams::FontVisualParams(const FontVisualParams &other)
{
	(*this) = other;
}

FontVisualParams::~FontVisualParams()
{
}

FontVisualParams &FontVisualParams::operator=(const FontVisualParams &other)
{
	m_size = other.m_size;
	m_slant = other.m_slant;
	m_weight = other.m_weight;
	m_fontList = other.m_fontList;
	
	return *this;
}

void FontVisualParams::setWeight(int weight)
{
	m_weight = weight;
}

int FontVisualParams::weight() const
{
	return m_weight;
}

void FontVisualParams::setSlant(int slant)
{
	m_slant = slant;
}

int FontVisualParams::slant() const
{
	return m_slant;
}

void FontVisualParams::setSize(double size)
{
	m_size = size;
}

double FontVisualParams::size() const
{
	return m_size;
}

std::list<std::string> &FontVisualParams::fontList()
{
	return m_fontList;
}

// #####

Font::Font(Converter *context) : m_context(context)
{
	m_ready = false;

	m_fontFace = 0;
	m_fontParams = 0;
}

Font::~Font()
{
	// Release font face
	if(m_ready && m_fontFace)
	{
		// FIXME: Debug that!
		//std::cout << "CALLING DONE FACE " << m_fontFace << std::endl;
		FT_Done_Face(m_fontFace);
	}

	delete m_fontParams;
}

std::string Font::buildRequest(const FontVisualParams *fontParams, int &id)
{
	// Use FontConfig to locate & select fonts and use
	// FreeType2 to open them
	FcPattern *pattern;
	std::string fileName;

	pattern = FcPatternBuild(0,
							 FC_WEIGHT, FcTypeInteger, fontParams->weight(),
							 FC_SLANT, FcTypeInteger, fontParams->slant(),
							 FC_SIZE, FcTypeDouble, fontParams->size(),
							 NULL);

	// Add multiple font names
	std::list<std::string> &fontList = const_cast<FontVisualParams *>(fontParams)->fontList();

	for(std::list<std::string>::const_iterator it = fontList.begin(); it != fontList.end(); ++it)
	{
		std::string string = *it;

		if(!string.empty())
			FcPatternAddString(pattern, FC_FAMILY, reinterpret_cast<const FcChar8 *>(string.c_str()));
	}

	// Always load vertical layout
	FcPatternAddBool(pattern, FC_VERTICAL_LAYOUT, true);

	// Disable hinting
	FcPatternAddBool(pattern, FC_HINTING, false);

	// Perform the default font pattern modification operations.
	FcDefaultSubstitute(pattern);
	FcConfigSubstitute(FcConfigGetCurrent(), pattern, FcMatchPattern);

	// Match the pattern!
	FcResult result;
	FcPattern *match = FcFontMatch(0, pattern, &result);

	// Destroy pattern
	FcPatternDestroy(pattern);

	// Get index & filename
	FcChar8 *temp;

	if(match)
	{
		FcPattern *pattern = FcPatternDuplicate(match);

		// Get index & filename
		if(FcPatternGetString(pattern, FC_FILE, 0, &temp) != FcResultMatch ||
		   FcPatternGetInteger(pattern, FC_INDEX, 0, &id) != FcResultMatch)
		{
			std::cout << "Font::buildRequest(), could not load font file for requested font \"" << Tools::joinList('|', fontList) << "\"" << std::endl;
			return fileName;
		}

		fileName = reinterpret_cast<const char *>(temp);

		// Kill pattern
		FcPatternDestroy(pattern);
	}

	// Kill pattern
	FcPatternDestroy(match); 

	return fileName;
}

bool Font::load(const FontVisualParams *fontParams)
{	
	// Build FontConfig request pattern
	int id = -1;
	std::string filename = Font::buildRequest(fontParams, id);

	// Load font directly using FreeType2
	std::cout << "Font::load(), loading " << filename << " for requested font \"" << Tools::joinList('|', const_cast<FontVisualParams *>(fontParams)->fontList()) << "\"" << std::endl;

	FT_Error error = FT_New_Face(m_context->library(), filename.c_str(), id, &m_fontFace);
	if(error)
	{
		std::cout << "Font::load(), could not load font. Aborting!" << std::endl;
		return false;
	}
	if(!FT_IS_SCALABLE(m_fontFace))
	{
		std::cout << "Font::load(), font does not contain outlines. Aborting!" << std::endl;
		FT_Done_Face(m_fontFace);
		m_fontFace = 0;
		return false;
	}

	// Choose unicode charmap
	for(int charmap = 0; charmap < m_fontFace->num_charmaps; charmap++)
	{
		if(m_fontFace->charmaps[charmap]->encoding == ft_encoding_unicode)
		{
			FT_Error error = FT_Set_Charmap(m_fontFace, m_fontFace->charmaps[charmap]);

			if(error)
			{
				std::cout << "Font::load(), unable to select unicode charmap. Aborting!" << std::endl;

				FT_Done_Face(m_fontFace);
				m_fontFace = 0;

				return false;
			}
		}
	}

	m_fontParams = fontParams;
	m_fontFile = filename;
	m_ready = true;

	return true;
}

FT_Face &Font::fontFace()
{
	return m_fontFace;
}

std::string Font::fontFile() const
{
	return m_fontFile;
}

const FontVisualParams *Font::fontParams() const
{
	return m_fontParams;
}

// vim:ts=4:noet
