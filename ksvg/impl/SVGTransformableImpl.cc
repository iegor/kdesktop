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

#include <kdebug.h>

#include "SVGMatrixImpl.h"
#include "SVGHelperImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGTransformableImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"

using namespace KSVG;

#include "SVGTransformableImpl.lut.h"
#include "ksvg_bridge.h"

SVGTransformableImpl::SVGTransformableImpl() : SVGLocatableImpl()
{
	KSVG_EMPTY_FLAGS
		
	m_transform = new SVGAnimatedTransformListImpl();
	m_transform->ref();

	m_localMatrix = 0;
}

SVGTransformableImpl::SVGTransformableImpl(const SVGTransformableImpl &other) : SVGLocatableImpl()
{
	(*this) = other;
}

SVGTransformableImpl::~SVGTransformableImpl()
{
	if(m_transform)
		m_transform->deref();
	if(m_localMatrix)
		m_localMatrix->deref();
}

SVGTransformableImpl &SVGTransformableImpl::operator=(const SVGTransformableImpl &other)
{
	SVGTransformListImpl *otherTransform = other.m_transform->baseVal();

	// Concat computed values
	for(unsigned int i = 0;i < otherTransform->numberOfItems(); i++)
	{
		SVGTransformImpl *trafo = otherTransform->getItem(i);
		m_transform->baseVal()->insertItemBefore(SVGSVGElementImpl::createSVGTransformFromMatrix(trafo->matrix()), i);
	}

	return *this;
}

SVGAnimatedTransformListImpl *SVGTransformableImpl::transform() const
{
	return m_transform;
}

SVGMatrixImpl *SVGTransformableImpl::getCTM()
{
	SVGMatrixImpl *ctm = SVGSVGElementImpl::createSVGMatrix();

	SVGElementImpl *element = dynamic_cast<SVGElementImpl *>(this);
	Q_ASSERT(element);

	DOM::Node parentNde = element->parentNode();

	if(!parentNde.isNull() && parentNde.nodeType() != DOM::Node::DOCUMENT_NODE)
	{
		SVGElementImpl *parent = element->ownerDoc()->getElementFromHandle(parentNde.handle());
		SVGLocatableImpl *locatableParent = dynamic_cast<SVGLocatableImpl *>(parent);

		if(locatableParent)
		{
			SVGMatrixImpl *parentCTM = locatableParent->getCTM();
			ctm->multiply(parentCTM);
			parentCTM->deref();
		}
	}

	if(m_localMatrix)
	{
		ctm->multiply(m_localMatrix);
	}

	return ctm;
}

void SVGTransformableImpl::updateLocalMatrix()
{
	if(m_transform->baseVal()->numberOfItems()>0)
	{
		if(m_localMatrix)
			m_localMatrix->deref();

		m_localMatrix = m_transform->baseVal()->concatenate();
	}
	else
	{
		if(m_localMatrix)
		{
			m_localMatrix->deref();
			m_localMatrix = 0;
		}
	}

	invalidateCachedMatrices();
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGTransformableImpl::s_hashTable 2
 transform    SVGTransformableImpl::Transform  DontDelete
@end
*/

Value SVGTransformableImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case Transform:
			return m_transform->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGTransformableImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Transform:
			SVGHelperImpl::parseTransformAttribute(m_transform->baseVal(), value.toString(exec).qstring());
			updateLocalMatrix();
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
