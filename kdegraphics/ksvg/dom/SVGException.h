/*
    Copyright (C) 2001-2003 KSVG Team
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
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef SVGException_H
#define SVGException_H

namespace KSVG
{
// TODO : use when DOM is full and/or ecma binding comes into play
class SVGException 
{
public:
	SVGException(unsigned short code) { m_code = code; }
	SVGException(const SVGException &other) { m_code = other.m_code; }
	SVGException &operator=(const SVGException &other)
	{
		m_code = other.m_code;
		return *this;
	}
	~SVGException() {}

	enum ExceptionCode {
		SVG_WRONG_TYPE_ERR			= 0,
		SVG_INVALID_VALUE_ERR		= 1,
		SVG_MATRIX_NOT_INVERTABLE	= 2
	};
	unsigned short m_code;
};

};

#endif

// vim:ts=4:noet
