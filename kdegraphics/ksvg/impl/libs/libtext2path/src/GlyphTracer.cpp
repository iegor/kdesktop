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

#include "GlyphTracer.h"

using namespace T2P;

GlyphTracer::GlyphTracer()
{
	m_moveTo = 0;
	m_lineTo = 0;
	m_conicBezier = 0;
	m_cubicBezier = 0;
	m_outlineMethods = 0;
}

GlyphTracer::~GlyphTracer()
{
	delete m_outlineMethods;
}

void GlyphTracer::setMoveto(FT_Outline_MoveToFunc funcPtr)
{
	m_moveTo = funcPtr;
}

void GlyphTracer::setLineto(FT_Outline_LineToFunc funcPtr)
{
	m_lineTo = funcPtr;
}

void GlyphTracer::setConicBezier(FT_Outline_ConicToFunc funcPtr)
{
	m_conicBezier = funcPtr;
}

void GlyphTracer::setCubicBezier(FT_Outline_CubicToFunc funcPtr)
{
	m_cubicBezier = funcPtr;
}

FT_Outline_Funcs *GlyphTracer::outlineFuncs()
{
	if(m_outlineMethods)
		return m_outlineMethods;
	
	FT_Outline_Funcs *ret = new FT_Outline_Funcs();
	ret->move_to = m_moveTo;
	ret->line_to = m_lineTo;
	ret->conic_to = m_conicBezier;
	ret->cubic_to = m_cubicBezier;
	ret->shift = 0;
	ret->delta = 0;

	m_outlineMethods = ret;
	return m_outlineMethods;
}

// vim:ts=4:noet
