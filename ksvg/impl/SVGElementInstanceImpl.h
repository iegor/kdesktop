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

#ifndef SVGElementInstanceImpl_H
#define SVGElementInstanceImpl_H

#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGElementImpl;
class SVGUseElementImpl;
class SVGElementInstanceImpl;
class SVGElementInstanceListImpl;
class SVGElementInstanceImpl : public DOM::DomShared
{
public:
	SVGElementInstanceImpl();
	virtual ~SVGElementInstanceImpl();

	SVGElementImpl *correspondingElement() const;
	SVGUseElementImpl *correspondingUseElement() const;
	SVGElementInstanceImpl *parentNode() const;
	SVGElementInstanceListImpl *childNodes() const;
	SVGElementInstanceImpl *firstChild() const;
	SVGElementInstanceImpl *lastChild() const;
	SVGElementInstanceImpl *previousSibling() const;
	SVGElementInstanceImpl *nextSibling() const;

	void setCorrespondingElement(SVGElementImpl *);

private:
	SVGElementImpl *m_correspondingElement;
	SVGUseElementImpl *m_correspondingUseElement;
	SVGElementInstanceImpl *m_parentNode;
	SVGElementInstanceListImpl *m_childNodes;
	SVGElementInstanceImpl *m_firstChild;
	SVGElementInstanceImpl *m_lastChild;
	SVGElementInstanceImpl *m_previousSibling;
	SVGElementInstanceImpl *m_nextSibling;

public:
	//KSVG_FORWARDGET
};

}

#endif

// vim:ts=4:noet
