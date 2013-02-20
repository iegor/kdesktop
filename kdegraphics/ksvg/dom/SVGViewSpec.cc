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

#include "SVGViewSpec.h"
#include "SVGViewSpecImpl.h"
#include "SVGElement.h"
#include "SVGTransformList.h"

using namespace KSVG;

SVGViewSpec::SVGViewSpec() : SVGZoomAndPan(), SVGFitToViewBox()
{
	impl = new SVGViewSpecImpl();
	impl->ref();
}

SVGViewSpec::SVGViewSpec(const SVGViewSpec &other) : SVGZoomAndPan(other), SVGFitToViewBox(other), impl(0)
{
	(*this) = other;
}

SVGViewSpec &SVGViewSpec::operator =(const SVGViewSpec &other)
{
	SVGZoomAndPan::operator=(other);
	SVGFitToViewBox::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGViewSpec::SVGViewSpec(SVGViewSpecImpl *other) : SVGZoomAndPan(other), SVGFitToViewBox(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGViewSpec::~SVGViewSpec()
{
	if(impl)
		impl->deref();
}

SVGTransformList SVGViewSpec::transform() const
{
	if(!impl) return SVGTransformList(0);
	return SVGTransformList(impl->transform());
}

SVGElement SVGViewSpec::viewTarget() const
{
	if(!impl) return SVGElement(0);
	return SVGElement(impl->viewTarget());
}

DOM::DOMString SVGViewSpec::viewBoxString() const
{
	if(!impl) return DOM::DOMString();
	return impl->viewBoxString();
}

DOM::DOMString SVGViewSpec::preserveAspectRatioString() const
{
	if(!impl) return DOM::DOMString();
	return impl->preserveAspectRatioString();
}

DOM::DOMString SVGViewSpec::transformString() const
{
	if(!impl) return DOM::DOMString();
	return impl->transformString();
}

DOM::DOMString SVGViewSpec::viewTargetString() const
{
	if(!impl) return DOM::DOMString();
	return impl->viewTargetString();
}

// vim:ts=4:noet
