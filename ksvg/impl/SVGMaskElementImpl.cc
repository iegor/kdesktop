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

#include <cfloat>

#include <kdebug.h>
#include <qimage.h>

#include "SVGMaskElement.h"

#include "SVGRectImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGMaskElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGUnitConverter.h"
#include "SVGShapeImpl.h"
#include "SVGMatrixImpl.h"
#include "KSVGCanvas.h"
#include "CanvasItems.h"
#include "CanvasFactory.h"
#include "KSVGHelper.h"

#include <X11/Xos.h>

using namespace KSVG;

#include "SVGMaskElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_ecma.h"

SVGMaskElementImpl::SVGMaskElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGBBoxTarget()
{
	KSVG_EMPTY_FLAGS

	m_x = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_x->ref();

	m_y = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_y->ref();

	m_width = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_width->ref();

	m_height = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_height->ref();

	m_maskUnits = new SVGAnimatedEnumerationImpl();
	m_maskUnits->ref();

	m_maskContentUnits = new SVGAnimatedEnumerationImpl();
	m_maskContentUnits->ref();

	m_converter = new SVGUnitConverter();
	m_converter->add(m_x);
	m_converter->add(m_y);
	m_converter->add(m_width);
	m_converter->add(m_height);

	m_canvas = 0;

	m_maskCache.setMaxTotalCost(1024 * 1024);
}

SVGMaskElementImpl::~SVGMaskElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
	if(m_maskUnits)
		m_maskContentUnits->deref();
	if(m_maskUnits)
		m_maskContentUnits->deref();
	delete m_converter;
	if(m_canvas)
		delete m_canvas;
}

SVGAnimatedEnumerationImpl *SVGMaskElementImpl::maskUnits() const
{
	return m_maskUnits;
}

SVGAnimatedEnumerationImpl *SVGMaskElementImpl::maskContentUnits() const
{
	return m_maskContentUnits;
}

SVGAnimatedLengthImpl *SVGMaskElementImpl::x() const
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGMaskElementImpl::y() const
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGMaskElementImpl::width() const
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGMaskElementImpl::height() const
{
	return m_height;
}

/*
@namespace KSVG
@begin SVGMaskElementImpl::s_hashTable 7
 maskUnits			SVGMaskElementImpl::MaskUnits			DontDelete|ReadOnly
 maskContentUnits	SVGMaskElementImpl::MaskContentUnits	DontDelete|ReadOnly
 x					SVGMaskElementImpl::X					DontDelete|ReadOnly
 y					SVGMaskElementImpl::Y					DontDelete|ReadOnly
 width				SVGMaskElementImpl::Width				DontDelete|ReadOnly
 height				SVGMaskElementImpl::Height				DontDelete|ReadOnly
@end
*/
Value SVGMaskElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE
		
	switch(token)
	{
		case MaskUnits:
			if(!attributeMode)
				return m_maskUnits->cache(exec);
			else
				return Number(m_maskUnits->baseVal());
		case MaskContentUnits:
			if(!attributeMode)
				return m_maskContentUnits->cache(exec);
			else
				return Number(m_maskContentUnits->baseVal());
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
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGMaskElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
    // This class has just ReadOnly properties, only with the Internal flag set
    // it's allowed to modify those.
    if(!(attr & KJS::Internal))
        return;

    switch(token)
    {
        case MaskUnits:
            if(value.toString(exec).qstring() == "objectBoundingBox")
                m_maskUnits->setBaseVal(SVGMaskElement::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);
            else
                m_maskUnits->setBaseVal(SVGMaskElement::SVG_UNIT_TYPE_USERSPACEONUSE);
            break;
        case MaskContentUnits:
            if(value.toString(exec).qstring() == "objectBoundingBox")
                m_maskContentUnits->setBaseVal(SVGMaskElement::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);
            else
                m_maskContentUnits->setBaseVal(SVGMaskElement::SVG_UNIT_TYPE_USERSPACEONUSE);
            break;
		case X:
			converter()->modify(x(), value.toString(exec).qstring());
			break;
		case Y:
			converter()->modify(y(), value.toString(exec).qstring());
			break;
		case Width:
			converter()->modify(width(), value.toString(exec).qstring());
			break;
		case Height:
			converter()->modify(height(), value.toString(exec).qstring());
			break;
        default:
            kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
    }
}

SVGRectImpl *SVGMaskElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	ret->setX(m_x->baseVal()->value());
	ret->setY(m_y->baseVal()->value());
	ret->setWidth(m_width->baseVal()->value());
	ret->setHeight(m_height->baseVal()->value());
	return ret;
}

void SVGMaskElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

    // Spec: if attribute not specified, use objectBoundingBox
    if(KSVG_TOKEN_NOT_PARSED(MaskUnits))
        KSVG_SET_ALT_ATTRIBUTE(MaskUnits, "objectBoundingBox")

	// Spec: if attribute not specified, use userSpaceOnUse
    if(KSVG_TOKEN_NOT_PARSED(MaskContentUnits))
        KSVG_SET_ALT_ATTRIBUTE(MaskContentUnits, "userSpaceOnUse")

	// Spec: if attribute not specified, use "-10%"
    if(KSVG_TOKEN_NOT_PARSED(X))
        KSVG_SET_ALT_ATTRIBUTE(X, "-10%");

	// Spec: if attribute not specified, use "-10%"
    if(KSVG_TOKEN_NOT_PARSED(Y))
        KSVG_SET_ALT_ATTRIBUTE(Y, "-10%");

	// Spec: if attribute not specified, use "120%"
    if(KSVG_TOKEN_NOT_PARSED(Width))
        KSVG_SET_ALT_ATTRIBUTE(Width, "120%");

	// Spec: if attribute not specified, use "120%"
    if(KSVG_TOKEN_NOT_PARSED(Height))
        KSVG_SET_ALT_ATTRIBUTE(Height, "120%");
}

SVGMaskElementImpl::Mask SVGMaskElementImpl::createMask(SVGShapeImpl *referencingElement, int imageWidth, int imageHeight)
{
	converter()->finalize(referencingElement, ownerSVGElement(), maskUnits()->baseVal());

	Q_UINT32 *imageBits = new Q_UINT32[imageWidth * imageHeight];

	if(m_canvas == 0)
		m_canvas = CanvasFactory::self()->loadCanvas(imageWidth, imageHeight);

	m_canvas->setup(reinterpret_cast<unsigned char *>(imageBits), imageWidth, imageHeight);
	m_canvas->setBackgroundColor(qRgba(0, 0, 0, 0));

	SVGMatrixImpl *baseMatrix = SVGSVGElementImpl::createSVGMatrix();

	// Set the scale to map the mask onto the image
	double xScale = static_cast<double>(imageWidth) / width()->baseVal()->value();
	double yScale = static_cast<double>(imageHeight) / height()->baseVal()->value();

	baseMatrix->scaleNonUniform(xScale, yScale);

	SVGRectImpl *bbox = referencingElement->getBBox();

	if(maskUnits()->baseVal() == SVGMaskElement::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX)
		baseMatrix->translate(-(bbox->x() + x()->baseVal()->value()), -(bbox->y() + y()->baseVal()->value()));
	else
		baseMatrix->translate(-x()->baseVal()->value(), -y()->baseVal()->value());

	if(maskContentUnits()->baseVal() == SVGMaskElement::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX)
	{
		baseMatrix->translate(bbox->x(), bbox->y());
		baseMatrix->scaleNonUniform(bbox->width(), bbox->height());
	}

	for(DOM::Node node = firstChild(); !node.isNull(); node = node.nextSibling())
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

	m_canvas->update(float(1));

	for(DOM::Node node = firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = ownerDoc()->getElementFromHandle(node.handle());
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
		SVGTestsImpl *tests = dynamic_cast<SVGTestsImpl *>(element);
		SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(element);

		bool ok = tests ? tests->ok() : true;
		if(element && shape && style && ok && style->getVisible() && style->getDisplay())
		{
			if(shape)
				shape->removeItem(m_canvas);
		}
	}


	{
		// Note: r and b reversed
		//QImage maskImage(reinterpret_cast<unsigned char *>(imageBits), imageWidth, imageHeight, 32, 0, 0, QImage::IgnoreEndian);
		//maskImage.setAlphaBuffer(true);
		//maskImage.save("mask.png", "PNG");
	}

	QByteArray maskData(imageWidth * imageHeight);
    const double epsilon = DBL_EPSILON;

	// Convert the rgba image into an 8-bit mask, according to the specs.
	for(int i = 0; i < imageWidth * imageHeight; i++)
	{
		Q_UINT32 rgba = imageBits[i];

#if X_BYTE_ORDER == X_LITTLE_ENDIAN
		double r = (rgba & 0xff) / 255.0;
		double g = ((rgba >> 8) & 0xff) / 255.0;
		double b = ((rgba >> 16) & 0xff) / 255.0;
		double a = ((rgba >> 24) & 0xff) / 255.0;
#else
		double a = (rgba & 0xff) / 255.0;
		double b = ((rgba >> 8) & 0xff) / 255.0;
		double g = ((rgba >> 16) & 0xff) / 255.0;
		double r = ((rgba >> 24) & 0xff) / 255.0;
#endif

		// Remove pre-multiplication by alpha.
		if(a > epsilon)
		{
			r /= a;
			g /= a;
			b /= a;
		}

		// Convert to linear RGB
		r = KSVGHelper::linearRGBFromsRGB(int(r * 255 + 0.5)) / 255.0;
		g = KSVGHelper::linearRGBFromsRGB(int(g * 255 + 0.5)) / 255.0;
		b = KSVGHelper::linearRGBFromsRGB(int(b * 255 + 0.5)) / 255.0;

		// Convert 'luminance to alpha'
		double luminanceAlpha = 0.2125 * r + 0.7154 * g + 0.0721 * b;

		// Multiply by alpha.
		double maskValue = luminanceAlpha * a;

		maskData[i] = static_cast<unsigned char>(maskValue * 255 + 0.5);
	}

	delete [] imageBits;

	baseMatrix->deref();
	bbox->deref();

	// The screenToMask matrix is calculated each time the mask is used so we don't
	// need to set it here.
	QWMatrix tempMatrix;

	return Mask(maskData, tempMatrix, imageWidth, imageHeight);
}

SVGMaskElementImpl::Mask SVGMaskElementImpl::createMask(SVGShapeImpl *referencingElement)
{
	converter()->finalize(referencingElement, ownerSVGElement(), maskUnits()->baseVal());

	SVGMatrixImpl *refCTM = 0;
	SVGLocatableImpl *locatableRef = dynamic_cast<SVGLocatableImpl *>(referencingElement);
	if(locatableRef)
		refCTM = locatableRef->getScreenCTM();
	else
		refCTM = SVGSVGElementImpl::createSVGMatrix();

	double xScale, yScale;

	refCTM->removeScale(&xScale, &yScale);
	refCTM->deref();

	int imageWidth = static_cast<int>(width()->baseVal()->value() * xScale + 0.5);
	int imageHeight = static_cast<int>(height()->baseVal()->value() * yScale + 0.5);

	Mask mask;

	if(imageWidth > 0 && imageHeight > 0)
	{
		CacheKey key(referencingElement, imageWidth, imageHeight);

		if(!m_maskCache.find(key, mask))
		{
			mask = createMask(referencingElement, imageWidth, imageHeight);
			m_maskCache.insert(key, mask, imageWidth * imageHeight);
		}

		// Generate a mask-coordinates to screen-coordinates matrix
		SVGMatrixImpl *matrix = 0;
		if(locatableRef)
			matrix = locatableRef->getScreenCTM();
		else
			matrix = SVGSVGElementImpl::createSVGMatrix();

		if(maskUnits()->baseVal() == SVGMaskElement::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX)
		{
			SVGRectImpl *bbox = referencingElement->getBBox();
			matrix->translate(bbox->x() + x()->baseVal()->value(), bbox->y() + y()->baseVal()->value());
			bbox->deref();
		}
		else
			matrix->translate(x()->baseVal()->value(), y()->baseVal()->value());

		matrix->scaleNonUniform(1 / xScale, 1 / yScale);

		QWMatrix screenToMask = matrix->qmatrix().invert();
		matrix->deref();

		mask.setScreenToMask(screenToMask);
	}
	
	return mask;
}

QByteArray SVGMaskElementImpl::maskRectangle(SVGShapeImpl *shape, const QRect& screenRectangle)
{
	QByteArray cumulativeMask;

	do
	{
		SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(shape);

		if(style && style->hasMask())
		{
			SVGElementImpl *element = shape->ownerDoc()->rootElement()->getElementById(style->getMask());

			if(element)
			{
				SVGMaskElementImpl *maskElement = dynamic_cast<SVGMaskElementImpl *>(element);

				if(maskElement)
				{
					SVGMaskElementImpl::Mask mask = maskElement->createMask(shape);
					
					if(!mask.isEmpty())
					{
						QByteArray maskData = mask.rectangle(screenRectangle);

						if(cumulativeMask.size() == 0)
							cumulativeMask = maskData;
						else
						{
							int size = cumulativeMask.size();

							// Multiply into the cumulative mask (using fast divide by 255)
							for(int i = 0; i < size; i++)
							{
								int tmp = maskData[i] * cumulativeMask[i] + 0x80;
								cumulativeMask[i] = (tmp + (tmp >> 8)) >> 8;
							}
						}
					}
				}
			}
		}

		DOM::Node parentNode = shape->parentNode();

		if(!parentNode.isNull())
		{
			SVGElementImpl *parent = shape->ownerDoc()->getElementFromHandle(parentNode.handle());

			if(parent)
				shape = dynamic_cast<SVGShapeImpl *>(parent);
			else
				shape = 0;
		}
		else
			shape = 0;

	} while(shape);

	return cumulativeMask;
}

SVGMaskElementImpl::Mask::Mask(const QByteArray& mask, const QWMatrix& screenToMask, int width, int height)
	:	m_width(width), m_height(height), m_mask(mask), m_screenToMask(screenToMask)
{
}

QByteArray SVGMaskElementImpl::Mask::rectangle(int screenX, int screenY, int width, int height)
{
	QByteArray rect(width * height);

	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			rect[x + y * width] = value(screenX + x, screenY + y);
		}
	}

	return rect;
}

QByteArray SVGMaskElementImpl::Mask::rectangle(const QRect& rect)
{
	return rectangle(rect.x(), rect.y(), rect.width(), rect.height());
}

// vim:ts=4:noet
