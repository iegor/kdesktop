/*
    Copyright (C) 2003 KSVG Team
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

#include "SVGTextContentElementImpl.h"
#include "SVGTextPositioningElementImpl.h"
#include "KSVGTextChunk.h"

using namespace KSVG;

// Text chunks (class to store text data)
KSVGTextChunk::KSVGTextChunk()
{
}

KSVGTextChunk::~KSVGTextChunk()
{
}

unsigned int KSVGTextChunk::count() const
{
	return m_text.count();
}

QString KSVGTextChunk::getText(unsigned int index) const
{
	return m_text[index];
}

SVGTextPositioningElementImpl *KSVGTextChunk::getTextElement(unsigned int index)
{
	SVGTextContentElementImpl *content = getTextContentElement(index);
	return dynamic_cast<SVGTextPositioningElementImpl *>(content);
}

SVGTextContentElementImpl *KSVGTextChunk::getTextContentElement(unsigned int index)
{
	return m_textElements.at(index);
}

void KSVGTextChunk::clear()
{
	m_text.clear();
	m_textElements.clear();
}

void KSVGTextChunk::addText(const QString &text, SVGTextContentElementImpl *textElement)
{
	m_text.append(text);
	m_textElements.append(textElement);
}

// vim:ts=4:noet
