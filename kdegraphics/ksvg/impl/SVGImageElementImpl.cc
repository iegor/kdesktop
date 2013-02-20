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

#include <qimage.h>
#include <qtimer.h>

#include <kdebug.h>

#include "CanvasItem.h"
#include "KSVGCanvas.h"
#include "KSVGHelper.h"

#include "SVGRectImpl.h"
#include "SVGEventImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGTransformImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGImageElementImpl.moc"
#include "SVGColorProfileElementImpl.h"
#include "SVGPreserveAspectRatioImpl.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGAnimatedPreserveAspectRatioImpl.h"
#include "SVGPatternElementImpl.h"

using namespace KSVG;

#include "SVGImageElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGImageElementImpl::SVGImageElementImpl(DOM::ElementImpl *impl) : QObject(), SVGShapeImpl(impl), SVGURIReferenceImpl(), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
	KSVG_EMPTY_FLAGS

	m_x = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_x->ref();
	m_x->baseVal()->setValueAsString("-1");

	m_y = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_y->ref();
	m_y->baseVal()->setValueAsString("-1");

	m_width = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_width->ref();
	m_width->baseVal()->setValueAsString("-1");

	m_height = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_height->ref();
	m_height->baseVal()->setValueAsString("-1");

	m_preserveAspectRatio = 0;

	m_doc = 0;
	m_image = 0;
	m_svgRoot = 0;
	m_colorProfile = 0;
	m_colorProfileApplied = false;
}

SVGImageElementImpl::~SVGImageElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
	if(m_preserveAspectRatio)
		m_preserveAspectRatio->deref();
	if(m_doc)
		m_doc->deref();

	delete m_image;
}

SVGAnimatedLengthImpl *SVGImageElementImpl::x()
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGImageElementImpl::y()
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGImageElementImpl::width()
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGImageElementImpl::height()
{
	return m_height;
}

SVGAnimatedPreserveAspectRatioImpl *SVGImageElementImpl::preserveAspectRatio() const
{
	return m_preserveAspectRatio;
}

/*
@namespace KSVG
@namespace KSVG
@begin SVGImageElementImpl::s_hashTable 7
 x						SVGImageElementImpl::X						DontDelete|ReadOnly
 y						SVGImageElementImpl::Y						DontDelete|ReadOnly
 width					SVGImageElementImpl::Width					DontDelete|ReadOnly
 height					SVGImageElementImpl::Height					DontDelete|ReadOnly
 preserveAspectRatio	SVGImageElementImpl::PreserveAspectRatio	DontDelete|ReadOnly
 href					SVGImageElementImpl::Href					DontDelete|ReadOnly
@end
*/

Value SVGImageElementImpl::getValueProperty(ExecState *exec, int token) const
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
		case PreserveAspectRatio:
			if(m_preserveAspectRatio)
				return m_preserveAspectRatio->cache(exec);
			else
				return Undefined();
		case Href:
			SVGURIReferenceImpl::getValueProperty(exec, token);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGImageElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case X:
			x()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Y:
			y()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Width:
			width()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Height:
			height()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case PreserveAspectRatio:
			if(!preserveAspectRatio())
			{
				m_preserveAspectRatio = new SVGAnimatedPreserveAspectRatioImpl();
				m_preserveAspectRatio->ref();
			}

			preserveAspectRatio()->baseVal()->parsePreserveAspectRatio(value.toString(exec).qstring());
			break;
		case Href:
			SVGURIReferenceImpl::putValueProperty(exec, SVGURIReferenceImpl::Href, value, attr);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

SVGRectImpl *SVGImageElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	ret->setX(m_x->baseVal()->value());
	ret->setY(m_y->baseVal()->value());
	ret->setWidth(m_width->baseVal()->value());
	ret->setHeight(m_height->baseVal()->value());
	return ret;
}

void SVGImageElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	if(KSVG_TOKEN_NOT_PARSED(X))
		KSVG_SET_ALT_ATTRIBUTE(X, "0")

	if(KSVG_TOKEN_NOT_PARSED(Y))
		KSVG_SET_ALT_ATTRIBUTE(Y, "0")

	if(KSVG_TOKEN_NOT_PARSED(PreserveAspectRatio))
	{
		setAttribute("preserveAspectRatio", "xMidYMid meet");
		KSVG_SET_ALT_ATTRIBUTE(PreserveAspectRatio, "xMidYMid meet")
	}
}

void SVGImageElementImpl::slotParsingFinished(bool error, const QString &errorDesc)
{
	if(error)
		kdDebug(26003) << "Finished with error : " << errorDesc << endl;
	else
		kdDebug(26003) << "Finished without errors!" << endl;

	m_svgRoot = m_doc->rootElement();
}

void SVGImageElementImpl::slotLoadingFinished()
{
	ownerDoc()->notifyImageLoaded(this);
}

void SVGImageElementImpl::createItem(KSVGCanvas *c)
{
	if(!m_item)
	{
		if(!c)
			c = ownerDoc()->canvas();

		m_item = c->createImage(this);
		c->insert(m_item);

		if(href()->baseVal().string().endsWith(".svg") || href()->baseVal().string().endsWith(".svgz"))
		{
			if(!m_svgRoot)
			{
				m_doc = new SVGDocumentImpl(true, false, this);
				m_doc->ref();
				m_doc->attach(c);

				connect(m_doc, SIGNAL(finishedParsing(bool, const QString &)), this, SLOT(slotParsingFinished(bool, const QString &)));
				connect(m_doc, SIGNAL(finishedLoading()), this, SLOT(slotLoadingFinished()));

				KURL file;

				if(!KURL::isRelativeURL(href()->baseVal().string()))
					file = KURL(href()->baseVal().string());
				else
					file = KURL(ownerDoc()->baseUrl(), href()->baseVal().string());

				m_doc->open(file);

				// The svg image will add items to the canvas in parallel with the main
				// document, so the z-order will not be correct. Get it to fix this when everything's
				// finished loading.
				ownerDoc()->resortZIndicesOnFinishedLoading();
				ownerDoc()->notifyImageLoading(this);
			}
			else
				m_svgRoot->createItem(c);
		}
		else
		{
			if(!m_image)
			{
				ownerDoc()->newImageJob(this);
				ownerDoc()->notifyImageLoading(this);
			}
		}
	}
}

void SVGImageElementImpl::removeItem(KSVGCanvas *c)
{
	if(m_item && c)
	{
		if(m_svgRoot)
			m_svgRoot->removeItem(c);

		c->removeItem(m_item);
		m_item = 0;
	}
}

void SVGImageElementImpl::setupSVGElement(SVGSVGElementImpl *svg)
{
	// Set up the root svg for an svg image.
	svg->setAttributeInternal("x", QString("%1").arg(x()->baseVal()->value()));
	svg->setAttributeInternal("y", QString("%1").arg(y()->baseVal()->value()));
	svg->setAttributeInternal("width", QString("%1").arg(width()->baseVal()->value()));
	svg->setAttributeInternal("height", QString("%1").arg(height()->baseVal()->value()));

	QString par = getAttribute("preserveAspectRatio").string().stripWhiteSpace();

	if(par.startsWith("defer"))
	{
		if(svg->getAttribute("preserveAspectRatio").isEmpty())
		{
			par.remove("defer");
			svg->setAttribute("preserveAspectRatio", par);
			svg->setAttributeInternal("preserveAspectRatio", par);
		}
	}
	else
	{
		svg->setAttribute("preserveAspectRatio", par);
		svg->setAttributeInternal("preserveAspectRatio", par);
	}

	svg->setAttributes();
	svg->setRootParentScreenCTM(getScreenCTM());
}

void SVGImageElementImpl::onScreenCTMUpdated()
{
	if(m_svgRoot)
	{
		SVGMatrixImpl *ctm = getScreenCTM();

		m_svgRoot->setRootParentScreenCTM(ctm);
		m_svgRoot->invalidateCachedMatrices();
		m_svgRoot->ownerDoc()->syncCachedMatrices();
	}
}

bool SVGImageElementImpl::prepareMouseEvent(const QPoint &p, const QPoint &, SVGMouseEventImpl *mev)
{
	// TODO : pointer-events should be stored here, not in SVGStylableImpl.
	SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(this);
	if(!style || style->getPointerEvents() == PE_NONE)
		return false;
	bool test = true;
	switch(style->getPointerEvents())
	{
		case PE_VISIBLE:
		case PE_VISIBLE_PAINTED:
		case PE_VISIBLE_FILL:
		case PE_VISIBLE_STROKE:		test = style->getVisible(); break;
		case PE_PAINTED:
		case PE_FILL:
		case PE_STROKE:
		case PE_ALL:				break;
		default: test = false;
	};

	if(test && m_item)
	{
		if(m_item->bbox().contains(p))
		{
			mev->setTarget(dynamic_cast<SVGElementImpl *>(this));
			return true;
		}
	}

	return false;
}

void SVGImageElementImpl::setImage(QImage *image)
{
	m_image = image;

	if(m_image)
	{
		*m_image = m_image->convertDepth(32);

		if(m_colorProfile != 0 && !m_colorProfileApplied)
		{
			m_colorProfileApplied = true;
			applyColorProfile();
		}

		SVGPatternElementImpl::flushCachedTiles();

		if(m_item)
		{
			ownerDoc()->canvas()->invalidate(m_item, false);
			ownerDoc()->rerender();
		}
	}

	ownerDoc()->notifyImageLoaded(this);
}

QImage SVGImageElementImpl::scaledImage()
{
	SVGMatrixImpl *matrix = imageMatrix();
	double sx, sy;

	matrix->removeScale(&sx, &sy);
	matrix->deref();

	QImage img;

	if(sx != 1 || sy != 1)
	{
		int scaledWidth = static_cast<int>(m_image->width() * sx + 0.5);
		int scaledHeight = static_cast<int>(m_image->height() * sy + 0.5);

		img = m_image->smoothScale(scaledWidth, scaledHeight);
	}
	else
		img = *m_image;

	return img;
}

SVGMatrixImpl *SVGImageElementImpl::imageMatrix()
{
	SVGMatrixImpl *ctm = getScreenCTM();

	ctm->translate(x()->baseVal()->value(), y()->baseVal()->value());

	SVGMatrixImpl *viewboxMatrix = preserveAspectRatio()->baseVal()->getCTM(0, 0, image()->width(), image()->height(), 0, 0, width()->baseVal()->value(), height()->baseVal()->value());
	
	ctm->multiply(viewboxMatrix);
	viewboxMatrix->deref();

	return ctm;
}

SVGMatrixImpl *SVGImageElementImpl::scaledImageMatrix()
{
	SVGMatrixImpl *matrix = imageMatrix();
	double sx, sy;

	matrix->removeScale(&sx, &sy);

	if(sx != 1 || sy != 1)
	{
		int imageWidth = static_cast<int>(m_image->width() * sx + 0.5);
		int imageHeight = static_cast<int>(m_image->height() * sy + 0.5);

		double trueWidth = m_image->width() * sx;
		double trueHeight = m_image->height() * sy;

		matrix->scaleNonUniform(trueWidth / imageWidth, trueHeight / imageHeight);
	}

	return matrix;
}

KSVGPolygon SVGImageElementImpl::clippingShape()
{
	KSVGRectangle viewport(0, 0, width()->baseVal()->value(), height()->baseVal()->value());
	SVGMatrixImpl *matrix = preserveAspectRatio()->baseVal()->getCTM(0, 0, image()->width(), image()->height(), 0, 0, width()->baseVal()->value(), height()->baseVal()->value());
	KSVGPolygon p = matrix->inverseMap(viewport);
	matrix->deref();

	matrix = imageMatrix();
	p = matrix->map(p);
	matrix->deref();

	return p;
}

QString SVGImageElementImpl::fileName() const
{
	return href()->baseVal().string();
}

void SVGImageElementImpl::applyColorProfile()
{
	m_image = m_colorProfile->correctImage(m_image);
}

void SVGImageElementImpl::applyColorProfile(SVGColorProfileElementImpl *profile, SVGImageElementImpl *image)
{
	// Only apply once, if it's the same (Niko)
	if(image->m_colorProfile == profile)
		return;

	image->m_colorProfile = profile;

	if(image->m_image)
	{
		// Image is already painted, we apply the color profile and repaint it
		image->applyColorProfile();

		if(image->item())
		{
			image->ownerDoc()->canvas()->invalidate(image->item(), false);
			image->ownerDoc()->rerender();
		}
	}
}

// vim:ts=4:noet
