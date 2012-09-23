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
#include <klocale.h>

#include "SVGPatternElement.h"
#include "SVGPatternElementImpl.h"

#include "CanvasFactory.h"
#include "KSVGCanvas.h"
#include "CanvasItems.h"
#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGUnitConverter.h"
#include "SVGShapeImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGRectImpl.h"

using namespace KSVG;

#include "SVGPatternElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

QValueList<SVGPatternElementImpl *> SVGPatternElementImpl::m_patternElements;

SVGPatternElementImpl::SVGPatternElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGURIReferenceImpl(), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGFitToViewBoxImpl(), SVGPaintServerImpl()
{
	KSVG_EMPTY_FLAGS

	m_patternUnits = new SVGAnimatedEnumerationImpl();
	m_patternUnits->ref();

	m_patternContentUnits = new SVGAnimatedEnumerationImpl();
	m_patternContentUnits->ref();

	m_patternTransform = new SVGAnimatedTransformListImpl();
	m_patternTransform->ref();

	m_x = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_x->ref();

	m_y = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_y->ref();

	m_width = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_width->ref();

	m_height = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_height->ref();

	m_converter = new SVGUnitConverter();
	m_converter->add(m_x);
	m_converter->add(m_y);
	m_converter->add(m_width);
	m_converter->add(m_height);

	m_patternElements.append(this);

	m_canvas = 0;
	m_location = this;

	m_tileCache.setMaxTotalCost(1024 * 1024);
}

SVGPatternElementImpl::~SVGPatternElementImpl()
{
	if(m_patternUnits)
		m_patternUnits->deref();
	if(m_patternContentUnits)
		m_patternContentUnits->deref();
	if(m_patternTransform)
		m_patternTransform->deref();
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
	delete m_converter;
	m_patternElements.remove(this);
}

SVGAnimatedEnumerationImpl *SVGPatternElementImpl::patternUnits() const
{
	return m_patternUnits;
}

SVGAnimatedEnumerationImpl *SVGPatternElementImpl::patternContentUnits() const
{
	return m_patternContentUnits;
}

SVGAnimatedTransformListImpl *SVGPatternElementImpl::patternTransform() const
{
	return m_patternTransform;
}

SVGAnimatedLengthImpl *SVGPatternElementImpl::x() const
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGPatternElementImpl::y() const
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGPatternElementImpl::width() const
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGPatternElementImpl::height() const
{
	return m_height;
}

void SVGPatternElementImpl::createItem(KSVGCanvas *c)
{
	if(!c)
		c = ownerDoc()->canvas();

	if(!m_paintServer)
		m_paintServer = c->createPaintServer(this);
}

void SVGPatternElementImpl::removeItem(KSVGCanvas *)
{
	delete m_paintServer;
	m_paintServer = 0;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPatternElementImpl::s_hashTable 11
 x						SVGPatternElementImpl::X					DontDelete|ReadOnly
 y						SVGPatternElementImpl::Y					DontDelete|ReadOnly
 width					SVGPatternElementImpl::Width				DontDelete|ReadOnly
 height    	 			SVGPatternElementImpl::Height				DontDelete|ReadOnly
 patternUnits			SVGPatternElementImpl::PatternUnits			DontDelete|ReadOnly
 patternContentUnits	SVGPatternElementImpl::PatternContentUnits	DontDelete|ReadOnly
 patternTransform		SVGPatternElementImpl::PatternTransform		DontDelete|ReadOnly
@end
*/

Value SVGPatternElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE
	
	switch(token)
	{
		case X:
			if(!attributeMode)
				return m_x->cache(exec);
			else
				return Number(m_x->baseVal()->value());
		case Y:
			if(!attributeMode)
				return m_y->cache(exec);
			else
				return Number(m_y->baseVal()->value());
		case Width:
			if(!attributeMode)
				return m_width->cache(exec);
			else
				return Number(m_width->baseVal()->value());
		case Height:
			if(!attributeMode)
				return m_height->cache(exec);
			else
				return Number(m_height->baseVal()->value());
		case PatternUnits:
			if(!attributeMode)
				return m_patternUnits->cache(exec);
			else
				return Number(m_patternUnits->baseVal());
		case PatternContentUnits:
			if(!attributeMode)
				return m_patternContentUnits->cache(exec);
			else
				return Number(m_patternContentUnits->baseVal());
		case PatternTransform:
			//if(!attributeMode)
				return m_patternTransform->cache(exec);
			//else
			//	return Number(m_patternTransform->baseVal()->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPatternElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case X:
			converter()->modify(x(), value.toString(exec).qstring());
			break;
		case Y:
			converter()->modify(y(), value.toString(exec).qstring());
			break;
		case Width:
			converter()->modify(width(), value.toString(exec).qstring());
			if(width()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute width of element <pattern> is illegal"));
			break;
		case Height:
			converter()->modify(height(), value.toString(exec).qstring());
			if(height()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute height of element <pattern> is illegal"));
			break;
		case PatternUnits:
			if(value.toString(exec).qstring() == "userSpaceOnUse")
				m_patternUnits->setBaseVal(SVGUnitTypes::SVG_UNIT_TYPE_USERSPACEONUSE);
			else
				m_patternUnits->setBaseVal(SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);
			break;
		case PatternContentUnits:
			if(value.toString(exec).qstring() == "userSpaceOnUse")
				m_patternContentUnits->setBaseVal(SVGUnitTypes::SVG_UNIT_TYPE_USERSPACEONUSE);
			else
				m_patternContentUnits->setBaseVal(SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);
			break;
		case PatternTransform:
			m_patternTransform->baseVal()->clear();
			SVGHelperImpl::parseTransformAttribute(m_patternTransform->baseVal(), value.toString(exec).qstring());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGPatternElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if attribute not specified, use a value of 0
	if(KSVG_TOKEN_NOT_PARSED(X))
		KSVG_SET_ALT_ATTRIBUTE(X, "0")

	// Spec: if attribute not specified, use a value of 0
	if(KSVG_TOKEN_NOT_PARSED(Y))
		KSVG_SET_ALT_ATTRIBUTE(Y, "0")

	// Spec: if attribute not specified, use objectBoundingBox
	if(KSVG_TOKEN_NOT_PARSED(PatternUnits))
		KSVG_SET_ALT_ATTRIBUTE(PatternUnits, "objectBoundingBox")

 	// Spec: If attribute not specified, use userSpaceOnUse
 	if(KSVG_TOKEN_NOT_PARSED(PatternContentUnits))
		KSVG_SET_ALT_ATTRIBUTE(PatternContentUnits, "userSpaceOnUse")

	// Spec: default value
	if(KSVG_TOKEN_NOT_PARSED(PreserveAspectRatio))
		KSVG_SET_ALT_ATTRIBUTE(PreserveAspectRatio, "xMidYMid meet")
}

void SVGPatternElementImpl::flushCachedTiles()
{
	QValueList<SVGPatternElementImpl *>::iterator it;

	for(it = m_patternElements.begin(); it != m_patternElements.end(); it++)
	{
		SVGPatternElementImpl *pattern = *it;

		if(pattern->paintServer())
			pattern->paintServer()->resetFinalized();
	}
}

QImage SVGPatternElementImpl::createTile(SVGShapeImpl *referencingElement, int imageWidth, int imageHeight)
{
	converter()->finalize(referencingElement, ownerSVGElement(), patternUnits()->baseVal());

	QImage image(imageWidth, imageHeight, 32);
	image.setAlphaBuffer(true);

	if(m_canvas == 0)
	{
		m_canvas = CanvasFactory::self()->loadCanvas(image.width(), image.height());
		m_canvas->setBackgroundColor(qRgba(0, 0, 0, 0));
	}

	m_canvas->setup(image.bits(), image.width(), image.height());

	SVGMatrixImpl *baseMatrix = SVGSVGElementImpl::createSVGMatrix();

	// Set the scale to map the tile onto the integral sized image
	double xScale = static_cast<double>(imageWidth) / width()->baseVal()->value();
	double yScale = static_cast<double>(imageHeight) / height()->baseVal()->value();

	baseMatrix->scaleNonUniform(xScale, yScale);

	if(hasAttribute("viewBox"))
	{
		SVGMatrixImpl *viewboxMatrix = viewBoxToViewTransform(width()->baseVal()->value(), height()->baseVal()->value());

		baseMatrix->multiply(viewboxMatrix);
		viewboxMatrix->deref();
	}
	else
	{
		if(patternContentUnits()->baseVal() == SVGPatternElement::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX)
		{
			// Get local coordinate bounding box
			SVGRectImpl *rect = referencingElement->getBBox();
			
			baseMatrix->translate(rect->qrect().x(), rect->qrect().y());
			baseMatrix->scaleNonUniform(rect->qrect().width(), rect->qrect().height());
			rect->deref();
		}
	}

	for(DOM::Node node = m_location->firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = ownerDoc()->getElementFromHandle(node.handle());
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
		SVGTestsImpl *tests = dynamic_cast<SVGTestsImpl *>(element);
		SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(element);

		bool ok = tests ? tests->ok() : true;
		if(element && shape && style && ok && style->getVisible() && style->getDisplay())
		{
			SVGLocatableImpl *locatable = dynamic_cast<SVGLocatableImpl *>(element);
			if(locatable)
				locatable->updateCachedScreenCTM(baseMatrix);

			element->createItem(m_canvas);
			if(shape->item())
			{
				shape->item()->setReferenced(true);
				m_canvas->invalidate(shape->item(), true);
			}
		}
	}

	baseMatrix->deref();

	m_canvas->update(float(1));

	if(getOverflow())
	{
		QPtrList<CanvasItem> items = m_canvas->allItems();
		QRect allItemsBBox;

		QPtrListIterator<CanvasItem> it(items);
		CanvasItem *item;

		while((item = *it) != 0)
		{
			QRect bbox = item->bbox();
			allItemsBBox |= bbox;
			++it;
		}

		if(allItemsBBox.left() < 0 || allItemsBBox.right() >= imageWidth || allItemsBBox.top() < 0 || allItemsBBox.bottom() >= imageHeight)
		{
			// Get the range in whole-tile units that covers the bounding box, where (0, 0) is the
			// usual tile position.
			int tileLeft = (allItemsBBox.left() - (imageWidth - 1)) / imageWidth;
			int tileRight = allItemsBBox.right() / imageWidth;
			int tileTop = (allItemsBBox.top() - (imageHeight - 1)) / imageHeight;
			int tileBottom = allItemsBBox.bottom() / imageHeight;

			for(int tileX = tileLeft; tileX <= tileRight; tileX++)
			{
				for(int tileY = tileTop; tileY <= tileBottom; tileY++)
				{
					if(tileX != 0 || tileY !=0)
					{
						QPoint panPoint(-(tileX * imageWidth), -(tileY * imageHeight));
						m_canvas->update(panPoint, false);
					}
				}
			}
		}
	}

	for(DOM::Node node = m_location->firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = ownerDoc()->getElementFromHandle(node.handle());

		if(element)
			element->removeItem(m_canvas);
	}

	return image;
}

void SVGPatternElementImpl::reference(const QString &href)
{
	// Copy attributes
	SVGElementImpl *src = ownerSVGElement()->getElementById(href);
	
	if(src)
	{
		SVGHelperImpl::copyAttributes(src, this);

		// Spec: Change location to referenced element so we
		// can take the children elements to render from there
		if(m_location == this)
			m_location = src;
	}
}

void SVGPatternElementImpl::finalizePaintServer()
{
	// Clear out any cached tiles since we may be being refinalised after an image
	// inside a pattern has finished loading.
	m_tileCache.clear();

	QString _href = SVGURIReferenceImpl::getTarget(href()->baseVal().string());
	if(!_href.isEmpty())
		reference(_href);
}

SVGPatternElementImpl::Tile SVGPatternElementImpl::createTile(SVGShapeImpl *referencingElement)
{
	converter()->finalize(referencingElement, ownerSVGElement(), patternUnits()->baseVal());

	SVGTransformableImpl *transformable = dynamic_cast<SVGTransformableImpl *>(referencingElement);
	SVGMatrixImpl *matrix = 0;
	if(transformable)
		matrix = transformable->getScreenCTM();
	else
		matrix = SVGSVGElementImpl::createSVGMatrix();

	matrix->translate(x()->baseVal()->value(), y()->baseVal()->value());

	SVGMatrixImpl *patTransform = patternTransform()->baseVal()->concatenate();
	if(patTransform)
	{
		matrix->multiply(patTransform);
		patTransform->deref();
	}
	
	double xScale, yScale;
	matrix->removeScale(&xScale, &yScale);

	double tileWidth = width()->baseVal()->value() * xScale;
	double tileHeight = height()->baseVal()->value() * yScale;

	int imageWidth = static_cast<int>(tileWidth + 0.5);
	int imageHeight = static_cast<int>(tileHeight + 0.5);

	Tile tile;

	if(imageWidth > 0 && imageHeight > 0)
	{
		QSize size(imageWidth, imageHeight);
		QImage image;

		if(!m_tileCache.find(size, image))
		{
			image = createTile(referencingElement, imageWidth, imageHeight);
			m_tileCache.insert(size, image, image.width() * image.height() * 4);
		}

		// Map integral tile dimensions onto its true size
		double adjustXScale = tileWidth / imageWidth;
		double adjustYScale = tileHeight / imageHeight;

		matrix->scaleNonUniform(adjustXScale, adjustYScale);
		QWMatrix screenToTile = matrix->qmatrix().invert();

		tile = Tile(image, screenToTile);
	}

	matrix->deref();

	return tile;
}

