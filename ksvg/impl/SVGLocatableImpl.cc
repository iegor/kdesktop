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

#include "SVGRectImpl.h"
#include "SVGShapeImpl.h"
#include "SVGContainerImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGElementImpl.h"
#include "SVGLocatableImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGDocumentImpl.h"

using namespace KSVG;

#include "SVGLocatableImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGLocatableImpl::SVGLocatableImpl()
{
	m_nearestViewportElement = 0;
	m_farthestViewportElement = 0;
	m_cachedScreenCTM = SVGSVGElementImpl::createSVGMatrix();
	m_cachedScreenCTMIsValid = false;
}

SVGLocatableImpl::~SVGLocatableImpl()
{
	if(m_nearestViewportElement)
		m_nearestViewportElement->deref();
	if(m_farthestViewportElement)
		m_farthestViewportElement->deref();
	if(m_cachedScreenCTM)
		m_cachedScreenCTM->deref();
}

SVGRectImpl *SVGLocatableImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	return ret;
}

SVGMatrixImpl *SVGLocatableImpl::getCTM()
{
	SVGMatrixImpl *ret = SVGSVGElementImpl::createSVGMatrix();
	return ret;
}

SVGMatrixImpl *SVGLocatableImpl::getScreenCTM()
{
	SVGMatrixImpl *ret = SVGSVGElementImpl::createSVGMatrix();
	ret->copy(m_cachedScreenCTM);

	return ret;
}

SVGMatrixImpl *SVGLocatableImpl::getTransformToElement(SVGElementImpl *)
{
	SVGMatrixImpl *ret = SVGSVGElementImpl::createSVGMatrix();
	return ret;
}

void SVGLocatableImpl::updateCachedScreenCTM(const SVGMatrixImpl *parentScreenCTM)
{
	m_cachedScreenCTM->copy(parentScreenCTM);

	const SVGMatrixImpl *local = localMatrix();

	if(local)
		m_cachedScreenCTM->multiply(local);
	m_cachedScreenCTMIsValid = true;

	// Notify the element
	onScreenCTMUpdated();

	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(this);

	if(shape)
	{
		// TODO: Update due to matrix animations
		//if(shape->item())
		//	shape->item()->update(updateReason);

		SVGElementImpl *element = dynamic_cast<SVGElementImpl *>(this);

		DOM::Node node = element->firstChild();
		for(; !node.isNull(); node = node.nextSibling())
		{
			SVGElementImpl *child = element->ownerDoc()->getElementFromHandle(node.handle());
			SVGLocatableImpl *locatable = dynamic_cast<SVGLocatableImpl *>(child);

			if(child && locatable)
				locatable->updateCachedScreenCTM(m_cachedScreenCTM);
		}
	}
}

void SVGLocatableImpl::checkCachedScreenCTM(const SVGMatrixImpl *parentScreenCTM)
{
	if(m_cachedScreenCTMIsValid)
	{
		SVGElementImpl *element = dynamic_cast<SVGElementImpl *>(this);
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(this);

		if(shape)
		{
			DOM::Node node = element->firstChild();
			for(; !node.isNull(); node = node.nextSibling())
			{
				SVGElementImpl *child = element->ownerDoc()->getElementFromHandle(node.handle());
				SVGLocatableImpl *locatable = dynamic_cast<SVGLocatableImpl *>(child);

				if(child && locatable)
					locatable->checkCachedScreenCTM(m_cachedScreenCTM);
			}
		}
	}
	else
		updateCachedScreenCTM(parentScreenCTM);
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGLocatableImpl::s_hashTable 3
 nearestViewportElement		SVGLocatableImpl::NearestViewportElement	DontDelete
 farthestViewportElement	SVGLocatableImpl::FarthestViewportElement	DontDelete
@end
@namespace KSVG
@begin SVGLocatableImplProto::s_hashTable 5
 getBBox						SVGLocatableImpl::GetBBox						DontDelete|Function 0
 getCTM							SVGLocatableImpl::GetCTM						DontDelete|Function 0
 getScreenCTM					SVGLocatableImpl::GetScreenCTM					DontDelete|Function 0
 getTransformToElement			SVGLocatableImpl::GetTransformToElement			DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGLocatable", SVGLocatableImplProto, SVGLocatableImplProtoFunc)

Value SVGLocatableImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case NearestViewportElement:
			return nearestViewportElement() ? nearestViewportElement()->cache(exec) : Undefined();
		case FarthestViewportElement:
			return farthestViewportElement() ? farthestViewportElement()->cache(exec) : Undefined();
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGLocatableImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGLocatableImpl)

	switch(id)
	{
		case SVGLocatableImpl::GetBBox:
		{
			SVGContainerImpl *container = dynamic_cast<SVGContainerImpl *>(obj);
			if(container)
				return container->getBBox()->cache(exec);
			else
			{
				SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(obj);
				if(shape)
					return shape->getBBox()->cache(exec);
				else
					return obj->getBBox()->cache(exec);
			}
		}
		case SVGLocatableImpl::GetCTM:
			return obj->getCTM()->cache(exec);
		case SVGLocatableImpl::GetScreenCTM:
			return obj->getScreenCTM()->cache(exec);
		case SVGLocatableImpl::GetTransformToElement:
			return obj->getTransformToElement(static_cast<KSVGBridge<SVGElementImpl> *>(args[0].imp())->impl())->cache(exec);
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// vim:ts=4:noet
