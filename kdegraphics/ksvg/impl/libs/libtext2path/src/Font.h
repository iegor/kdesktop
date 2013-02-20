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

#ifndef T2P_FONT_H
#define T2P_FONT_H

#include <list>
#include <string>

// FreeType 2 includes
#include <ft2build.h>
#include FT_FREETYPE_H

namespace T2P
{
	class Converter;

	class FontVisualParams
	{
	public:
		FontVisualParams();
		FontVisualParams(const FontVisualParams &other);
		~FontVisualParams();

		FontVisualParams &operator=(const FontVisualParams &other);

		void setWeight(int weight);
		int weight() const;

		void setSlant(int slant);
		int slant() const;

		void setSize(double size);
		double size() const;

		std::list<std::string> &fontList();

	private:
		int m_weight, m_slant;
		double m_size;

		std::list<std::string> m_fontList;
	};

	class Font
	{
	public:
		Font(Converter *context);
		~Font();

		// Build font loading request for FontConfig
		static std::string buildRequest(const FontVisualParams *fontParams, int &id);

		// Load it! :)
		bool load(const FontVisualParams *fontParams);

		FT_Face &fontFace();
		std::string fontFile() const;
		const FontVisualParams *fontParams() const;

	private:
		FT_Face m_fontFace;
		std::string m_fontFile;

		Converter *m_context;
		const FontVisualParams *m_fontParams;

		bool m_ready;
	};
}

#endif

// vim:ts=4:noet
