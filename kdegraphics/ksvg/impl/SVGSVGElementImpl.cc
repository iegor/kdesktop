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

#include <qtimer.h>
#include <qstringlist.h>
#include <qdatetime.h>

#define USE_VALGRIND 0

#if USE_VALGRIND
#include <valgrind/calltree.h>
#endif

#include "SVGLength.h"

#include "SVGRectImpl.h"
#include "SVGAngleImpl.h"
#include "SVGShapeImpl.h"
#include "SVGPointImpl.h"
#include "SVGNumberImpl.h"
#include "SVGMatrixImpl.h"

#include "SVGEventImpl.h"
#include "SVGAElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGViewSpecImpl.h"
#include "SVGTransformImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGAnimatedRectImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGPreserveAspectRatioImpl.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGAnimatedPreserveAspectRatioImpl.h"

#include "CanvasItem.h"
#include "KSVGCanvas.h"

using namespace KSVG;

#include "SVGSVGElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGSVGElementImpl::SVGSVGElementImpl(DOM::ElementImpl *impl) : SVGContainerImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGLocatableImpl(), SVGFitToViewBoxImpl(), SVGZoomAndPanImpl()
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

	m_viewport = SVGSVGElementImpl::createSVGRect();

	m_currentTranslate = SVGSVGElementImpl::createSVGPoint();

	m_currentView = new SVGViewSpecImpl();
	m_currentView->ref();

	m_currentScale = 1.0;

	m_useCurrentView = false;

	m_clip[0] = 0;
	m_clip[1] = 0;
	m_clip[2] = 0;
	m_clip[3] = 0;

	m_rootParentScreenCTM = 0;

	m_localMatrix = SVGSVGElementImpl::createSVGMatrix();
}

SVGSVGElementImpl::~SVGSVGElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
	if(m_viewport)
		m_viewport->deref();
	if(m_currentTranslate)
		m_currentTranslate->deref();
	if(m_currentView)
		m_currentView->deref();
	if(m_rootParentScreenCTM)
		m_rootParentScreenCTM->deref();
	if(m_localMatrix)
		m_localMatrix->deref();
}

bool SVGSVGElementImpl::isRootElement() const
{
	return ownerDoc()->rootElement() == this;
}

void SVGSVGElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(X))
		KSVG_SET_ALT_ATTRIBUTE(X, "0")

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(Y))
		KSVG_SET_ALT_ATTRIBUTE(Y, "0")

	// Spec: If the attribute is not specified, the effect is as if a value of "100%" were specified.
	if(KSVG_TOKEN_NOT_PARSED(Width))
		KSVG_SET_ALT_ATTRIBUTE(Width, "100%")

	// Spec: If the attribute is not specified, the effect is as if a value of "100%" were specified.
	if(KSVG_TOKEN_NOT_PARSED(Height))
		KSVG_SET_ALT_ATTRIBUTE(Height, "100%")

	// Spec: The contentScriptType should default to "text/ecmascript".
	if(KSVG_TOKEN_NOT_PARSED(ContentScriptType))
		KSVG_SET_ALT_ATTRIBUTE(ContentScriptType, "text/ecmascript")

	// Spec: The contentStyleType should default to "text/css".
	if(KSVG_TOKEN_NOT_PARSED(ContentStyleType))
		KSVG_SET_ALT_ATTRIBUTE(ContentStyleType, "text/css")

	if(m_useCurrentView)
	{
		parseViewBox(m_currentView->viewBoxString().string());
		preserveAspectRatio()->baseVal()->parsePreserveAspectRatio(m_currentView->preserveAspectRatioString().string());
	}

	m_viewport->setX(x()->baseVal()->value());
	m_viewport->setY(y()->baseVal()->value());
	m_viewport->setWidth(width()->baseVal()->value());
	m_viewport->setHeight(height()->baseVal()->value());

	if(isRootElement() && ownerDoc()->parentImage() == 0)
	{
		if(ownerDoc()->canvas())
			ownerDoc()->canvas()->setViewportDimension(int(ceil(width()->baseVal()->value() * currentScale())), int(ceil(height()->baseVal()->value() * currentScale())));

		// Special case for outermost svg element:
		// We need to register our id manually, because
		// m_ownerSVGElement is 0 in SVGElementImpl::setAttributes (Niko)
		if(!id().isNull())
			addToIdMap(id().string(), this);		
	}
}

SVGAnimatedLengthImpl *SVGSVGElementImpl::x()
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGSVGElementImpl::y()
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGSVGElementImpl::width()
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGSVGElementImpl::height()
{
	return m_height;
}

void SVGSVGElementImpl::setContentScriptType(const DOM::DOMString &contentScriptType)
{
	setAttribute("contentScriptType", contentScriptType);
}

DOM::DOMString SVGSVGElementImpl::contentScriptType() const
{
	return getAttribute("contentScriptType");
}

void SVGSVGElementImpl::setContentStyleType(const DOM::DOMString &contentStyleType)
{
	setAttribute("contentStyleType", contentStyleType);
}

DOM::DOMString SVGSVGElementImpl::contentStyleType() const
{
	return getAttribute("contentStyleType");
}

SVGRectImpl *SVGSVGElementImpl::viewport()
{
	return m_viewport;
}

SVGRectImpl *SVGSVGElementImpl::getBBox()
{
	SVGRectImpl *ret = new SVGRectImpl(getCTM()->qmatrix().invert().map(m_viewport->qrect()));
	ret->ref();
	return ret;
}

float SVGSVGElementImpl::pixelUnitToMillimeterX() const
{
	return ownerDoc()->screenPixelsPerMillimeterX();
}

float SVGSVGElementImpl::pixelUnitToMillimeterY() const
{
	return ownerDoc()->screenPixelsPerMillimeterY();
}

float SVGSVGElementImpl::screenPixelToMillimeterX() const
{
	return pixelUnitToMillimeterX();
}

float SVGSVGElementImpl::screenPixelToMillimeterY() const
{
	return pixelUnitToMillimeterY();
}

void SVGSVGElementImpl::setUseCurrentView(bool useCurrentView)
{
	m_useCurrentView = useCurrentView;
}

bool SVGSVGElementImpl::useCurrentView() const
{
	return m_useCurrentView;
}

SVGViewSpecImpl *SVGSVGElementImpl::currentView() const
{
	return m_currentView;
}

void SVGSVGElementImpl::setCurrentScale(float currentScale)
{
	if( m_currentScale != currentScale )
	{
		m_currentScale = currentScale;
		invalidateCachedMatrices();

		if(hasEventListener(SVGEvent::ZOOM_EVENT, true))
			dispatchEvent(SVGEvent::ZOOM_EVENT, false, false);
	}
}

float SVGSVGElementImpl::currentScale() const
{
	return m_currentScale;
}

void SVGSVGElementImpl::setCurrentTranslate(const QPoint &p)
{
	if(m_currentTranslate->x() != p.x() || m_currentTranslate->y() != p.y())
	{
		m_currentTranslate->setX(p.x());
		m_currentTranslate->setY(p.y());
		invalidateCachedMatrices();
		if(hasEventListener(SVGEvent::SCROLL_EVENT, true))
			dispatchEvent(SVGEvent::SCROLL_EVENT, false, false);
	}
}

SVGPointImpl *SVGSVGElementImpl::currentTranslate()
{
	return m_currentTranslate;
}

unsigned long SVGSVGElementImpl::suspendRedraw(unsigned long)
{
	return 0;
}

void SVGSVGElementImpl::unsuspendRedraw(unsigned long)
{
}

void SVGSVGElementImpl::unsuspendRedrawAll()
{
}

void SVGSVGElementImpl::forceRedraw()
{
#if USE_VALGRIND
	CALLTREE_ZERO_STATS();
#endif

	QTime timer;
	timer.start();

	if(ownerDoc() && ownerDoc()->canvas())
		ownerDoc()->canvas()->update();

	kdDebug(26000) << "forceRedraw in " << timer.elapsed()/1000.0 << " seconds" << endl;

#if USE_VALGRIND
	CALLTREE_DUMP_STATS();
#endif
}

void SVGSVGElementImpl::pauseAnimations()
{
	if(!ownerDoc()->timeScheduler()->animationsPaused())
		ownerDoc()->timeScheduler()->toggleAnimations();
}

void SVGSVGElementImpl::unpauseAnimations()
{
	if(ownerDoc()->timeScheduler()->animationsPaused())
		ownerDoc()->timeScheduler()->toggleAnimations();
}

bool SVGSVGElementImpl::animationsPaused()
{
	return ownerDoc()->timeScheduler()->animationsPaused();
}

float SVGSVGElementImpl::getCurrentTime() const
{
	return ownerDoc()->timeScheduler()->elapsed();
}

void SVGSVGElementImpl::setCurrentTime(float)
{
}

DOM::NodeList SVGSVGElementImpl::getIntersectionList(SVGRectImpl *, SVGElementImpl *)
{
	// TODO : implement me
	return DOM::NodeList();
}

DOM::NodeList SVGSVGElementImpl::getEnclosureList(SVGRectImpl *rect, SVGElementImpl */*referenceElement*/)
{
	DOM::NodeList list;

	DOM::Node node = firstChild();
	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = ownerDoc()->getElementFromHandle(node.handle());
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
		if(shape)
		{
			if(shape->isContainer())
				// TODO : pass it on to container::getEnclosureList() which should return a NodeList
				kdDebug() << "!shape" << endl;
			else
			{
				// TODO : add the shape to list if the test succeeds
				SVGRectImpl *current = shape->getBBox();
				if(rect->qrect().contains(current->qrect(), true))
					kdDebug() << "shape : " << element->nodeName().string() << " is fully enclosed" << endl;

				current->deref();
			}
		}
	}

	return list;
}

bool SVGSVGElementImpl::checkIntersection(SVGElementImpl *element, SVGRectImpl *rect)
{
	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
	if(!shape)
		return false;

	SVGRectImpl *current = shape->getBBox();
	bool result = rect->qrect().intersects(current->qrect());
	current->deref();
	return result;
}

bool SVGSVGElementImpl::checkEnclosure(SVGElementImpl *element, SVGRectImpl *rect)
{
	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
	if(!shape)
		return false;

	SVGRectImpl *current = shape->getBBox();
	bool result = rect->qrect().contains(current->qrect());
	current->deref();
	return result;
}

void SVGSVGElementImpl::deSelectAll()
{
}

SVGNumberImpl *SVGSVGElementImpl::createSVGNumber()
{
	// Spec: Creates an SVGNumber object outside of any document
	// trees. The object is initialized to a value of zero. 
	SVGNumberImpl *ret = new SVGNumberImpl();
	ret->ref();
	return ret;
}

SVGLengthImpl *SVGSVGElementImpl::createSVGLength()
{
	// Spec: Creates an SVGLength object outside of any document
	// trees. The object is initialized to the value of 0 user units. 
	SVGLengthImpl *ret = new SVGLengthImpl();
	ret->ref();
	return ret;
}

SVGAngleImpl *SVGSVGElementImpl::createSVGAngle()
{
	// Spec: Creates an SVGAngle object outside of any document
	// trees. The object is initialized to the value 0 degrees (unitless). 
	SVGAngleImpl *ret = new SVGAngleImpl();
	ret->ref();
	return ret;
}

SVGPointImpl *SVGSVGElementImpl::createSVGPoint()
{
	// Spec: Creates an SVGPoint object outside of any document
	// trees. The object is initialized to the point (0,0) in the user coordinate system. 
	SVGPointImpl *ret = new SVGPointImpl();
	ret->ref();
	return ret;
}

SVGMatrixImpl *SVGSVGElementImpl::createSVGMatrix()
{
	// Spec: Creates an SVGMatrix object outside of any document
	// trees. The object is initialized to the identity matrix.
	SVGMatrixImpl *ret = new SVGMatrixImpl(QWMatrix(1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F));
	ret->ref();
	return ret;
}

SVGRectImpl *SVGSVGElementImpl::createSVGRect()
{
	// Spec: Creates an SVGRect object outside of any document
	// trees. The object is initialized such that all values are set to 0 user units. 
	SVGRectImpl *ret = new SVGRectImpl();
	ret->ref();
	return ret;
}

SVGTransformImpl *SVGSVGElementImpl::createSVGTransform()
{
	// Spec: Creates an SVGTransform object outside of any document
	// trees. The object is initialized to an identity matrix transform (SVG_TRANSFORM_MATRIX). 
	SVGTransformImpl *transform = createSVGTransformFromMatrix(createSVGMatrix());

	// createSVGMatrix already ref's the matrix, the SVGTransformImpl->setMatrix
	// call also does this, prevent non deleting of the object by deref'ing (Niko)
	transform->matrix()->deref();

	return transform;
}

SVGTransformImpl *SVGSVGElementImpl::createSVGTransformFromMatrix(SVGMatrixImpl *mat)
{
	// Spec: Creates an SVGTransform object outside of any document
	// trees. The object is initialized to the given matrix transform (i.e., SVG_TRANSFORM_MATRIX). 
	SVGTransformImpl *ret = new SVGTransformImpl();
	ret->setMatrix(mat);
	ret->ref();
	return ret;
}

SVGElementImpl *SVGSVGElementImpl::getElementById(const DOM::DOMString &elementId)
{
	return m_map[elementId.string()];
}

void SVGSVGElementImpl::addToIdMap(const QString &id, SVGElementImpl *obj)
{
	m_map.insert(id, obj);
}

SVGMatrixImpl *SVGSVGElementImpl::getCTM()
{
	return viewBoxToViewTransform(width()->baseVal()->value(), height()->baseVal()->value());
}

const SVGMatrixImpl *SVGSVGElementImpl::localMatrix()
{
	// TODO: only update the matrix when needed and just return m_localMatrix

	m_localMatrix->reset();

	if(ownerSVGElement() == 0)
	{
		if(m_rootParentScreenCTM != 0)
			m_localMatrix->copy(m_rootParentScreenCTM);

		// We're the outermost svg element.
		// Put the zoom scale and translate into the matrix.
		m_localMatrix->translate(currentTranslate()->x(), currentTranslate()->y());
		m_localMatrix->scale(currentScale());
	}

	// Apply viewport translation.
	m_localMatrix->translate(x()->baseVal()->value(), y()->baseVal()->value());

	// And viewbox.
	SVGMatrixImpl *viewboxMatrix = viewBoxToViewTransform(width()->baseVal()->value(), height()->baseVal()->value());

	m_localMatrix->multiply(viewboxMatrix);
	viewboxMatrix->deref();

	return m_localMatrix;
}

void SVGSVGElementImpl::setClip(const QString &clip)
{
	// TODO : this routine should probably be shared between all classes that establish new viewports (Rob)
	if(!clip.startsWith("rect(") || !clip.endsWith(")"))
		return;

	QString work = clip.mid(5, clip.length() - 6);
	QStringList substrings = QStringList::split(',', clip);
	QStringList::ConstIterator it = substrings.begin();

	if(m_clip[0])
		m_clip[0]->deref();
	m_clip[0] = SVGSVGElementImpl::createSVGLength();

	if(*it != "auto")
		m_clip[0]->setValueAsString(*it);
	++it;

	if(m_clip[1])
		m_clip[1]->deref();
	m_clip[1] = SVGSVGElementImpl::createSVGLength();

	if(*it != "auto")
		m_clip[1]->setValueAsString(*it);
	++it;

	if(m_clip[2])
		m_clip[2]->deref();
	m_clip[2] = SVGSVGElementImpl::createSVGLength();

	if(*it != "auto")
		m_clip[2]->setValueAsString(*it);
	++it;

	if(m_clip[3])
		m_clip[3]->deref();
	m_clip[3] = SVGSVGElementImpl::createSVGLength();

	if(*it != "auto")
		m_clip[3]->setValueAsString(*it);
}

QRect SVGSVGElementImpl::clip()
{
	// Get viewport in user coordinates.
	QRect v(0, 0, m_viewport->qrect().width(), m_viewport->qrect().height());

	SVGMatrixImpl *ctm = getCTM();
	QRect r = ctm->qmatrix().invert().mapRect(v);
	ctm->deref();

	if(m_clip[0])
		r.setTop(static_cast<int>(r.top() + m_clip[0]->value()));
	if(m_clip[1])
		r.setRight(static_cast<int>(r.right() - m_clip[1]->value()));
	if(m_clip[2])
		r.setBottom(static_cast<int>(r.bottom() - m_clip[2]->value()));
	if(m_clip[3])
		r.setLeft(static_cast<int>(r.left() + m_clip[3]->value()));

	return r;
}

void SVGSVGElementImpl::setRootParentScreenCTM(SVGMatrixImpl *screenCTM)
{
	if(m_rootParentScreenCTM != 0)
		m_rootParentScreenCTM->deref();

	m_rootParentScreenCTM = screenCTM;
	screenCTM->ref();
}

bool SVGSVGElementImpl::prepareMouseEvent(const QPoint &p, const QPoint &a, SVGMouseEventImpl *mev)
{
	// mop: central bool var which turns to true once the current "mouseover" element has been found
	bool ret = false, dorerender = false;
	SVGElementImpl *elem = 0;
	
	SVGMatrixImpl *ctm = getCTM();
	QPoint userA = ctm->qmatrix().invert().map(a);
	ctm->deref();

	// Just check the lastTarget once (mop)
	if(ownerDoc()->lastTarget())
	{
		elem = ownerDoc()->lastTarget();
		ret = elem->prepareMouseEvent(p, userA, mev);

		// mop: only proceed if element is not the current element anymore. rest is done in the lower part
		if(!ret)
		{
			if(elem->hasEventListener(SVGEvent::MOUSEOUT_EVENT, false))
			{
				dorerender = true;
				elem->setMouseOver(false);
				elem->dispatchMouseEvent(SVGEvent::MOUSEOUT_EVENT, true, true, 0, mev->screenX(), mev->screenY(), mev->clientX(), mev->clientY(), mev->ctrlKey(), mev->altKey(), mev->shiftKey(), mev->metaKey(), mev->button(), elem);
			}

			if(elem->hasEventListener(SVGEvent::DOMFOCUSOUT_EVENT, false) && elem->focus())
			{
				dorerender = true;
				elem->setFocus(false);
				elem->dispatchEvent(SVGEvent::DOMFOCUSOUT_EVENT, true, true);
			}
			
			// mop: unset last target once we left it
			ownerDoc()->setLastTarget(0);
		}
	}

	// mop: DAMN...logic doesn't work :(
	// we cant use the results of the above check because something like this
	//  _________
	// | ____    |
	// ||____|   |
	// |_________|
	//
	// wouldn't work because even if the mousepointer would be in the inner rect it would still be inside the bbox
	// of the outer (assuming that the outer is the lastTarget). :(
	ret = false;

	// mop: just check for a node until the element where the mouse is over is found
	// mop: "ret" can be set from the above stuff too...
	//      find the element here and do the event stuff in the lower part..much cleaner
	CanvasItemList hits = ownerDoc()->canvas()->collisions(p, true);
	for(CanvasItemList::Iterator it = hits.begin(); it != hits.end(); ++it)
	{
		elem = (*it)->element();
		if(elem)
		{
			// Check if mouse is over a certain shape...
			// mop: once an element has been found check eventlisteners and leave immediately
			ret = elem->prepareMouseEvent(p, userA, mev);
			if(ret) break;
		}
	}

	// mop: has an element been found?
	if(ret)
	{
		int events = mev->target()->getEventListeners(false);

		// Dispatch mousemove, mousedown, mouseup and mouseclick events
		bool cancel = (mev->id() != SVGEvent::MOUSEMOVE_EVENT);

		if(events & 1 << mev->id())
		{
			mev->target()->dispatchMouseEvent(mev->id(), true, cancel, 0, mev->screenX(), mev->screenY(), mev->clientX(), mev->clientY(), mev->ctrlKey(), mev->altKey(), mev->shiftKey(), mev->metaKey(), mev->button(), elem);
			dorerender = true; // mop: if it has the event then rerender
		}

		// If a mouse "moves" over a shape, it's also "over" the shape
		if(mev->id() == SVGEvent::MOUSEMOVE_EVENT)
		{
			mev->target()->setMouseOver(true);
			if(events & 1 << SVGEvent::MOUSEOVER_EVENT)
			{
				mev->target()->dispatchMouseEvent(SVGEvent::MOUSEOVER_EVENT, true, cancel, 0, mev->screenX(), mev->screenY(), mev->clientX(), mev->clientY(), mev->ctrlKey(), mev->altKey(), mev->shiftKey(), mev->metaKey(), mev->button(), elem);
				dorerender = true;
			}

		}

		// Also send an domactivate + focusin event on mouseup
		bool dolinktest = true;
		if(mev->id() == SVGEvent::MOUSEUP_EVENT)
		{
			mev->target()->setFocus(true);

			if(events & 1 << SVGEvent::CLICK_EVENT)
			{
				dolinktest = mev->target()->dispatchEvent(SVGEvent::CLICK_EVENT, true, true);
				dorerender = true;
			}

			if(events & 1 << SVGEvent::DOMACTIVATE_EVENT)
			{
				mev->target()->dispatchEvent(SVGEvent::DOMACTIVATE_EVENT, true, true);
				dorerender = true;
			}

			if(events & 1 << SVGEvent::DOMFOCUSIN_EVENT)
			{
				mev->target()->dispatchEvent(SVGEvent::DOMFOCUSIN_EVENT, true, true);
				dorerender = true;
			}
		}

		// Hyperlink support
		SVGAElementImpl* link=0;
		if(dolinktest && !mev->defaultPrevented())
		{
			link = SVGAElementImpl::getLink(elem);
			if(link)
			{
				mev->setURL(link->href()->baseVal());
				emit ownerDoc()->gotURL(link->target()->baseVal().string());
			}
		}

		// The mouse is over a shape, so we have a target..we need to register that for a mouseout
		ownerDoc()->setLastTarget(mev->target());
	}

	// mop: all events may trigger changed style, add elements etc. this is definately needed :(
	if(dorerender)
		ownerDoc()->rerender();

	return dorerender; // mop: some kind of event has been found and executed
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGSVGElementImpl::s_hashTable 23
 x							SVGSVGElementImpl::X						DontDelete|ReadOnly
 y							SVGSVGElementImpl::Y						DontDelete|ReadOnly
 width						SVGSVGElementImpl::Width					DontDelete|ReadOnly
 height						SVGSVGElementImpl::Height					DontDelete|ReadOnly
 viewport					SVGSVGElementImpl::Viewport					DontDelete|ReadOnly
 contentScriptType			SVGSVGElementImpl::ContentScriptType		DontDelete
 contentStyleType			SVGSVGElementImpl::ContentStyleType			DontDelete
 pixelUnitToMillimeterX 	SVGSVGElementImpl::PixelUnitToMillimeterX	DontDelete|ReadOnly
 pixelUnitToMillimeterY 	SVGSVGElementImpl::PixelUnitToMillimeterY	DontDelete|ReadOnly
 screenPixelToMillimeterX 	SVGSVGElementImpl::ScreenPixelToMillimeterX DontDelete|ReadOnly
 screenPixelToMillimeterY 	SVGSVGElementImpl::ScreenPixelToMillimeterY DontDelete|ReadOnly
 useCurrentView				SVGSVGElementImpl::UseCurrentView			DontDelete
 currentScale				SVGSVGElementImpl::CurrentScale				DontDelete
 currentTranslate			SVGSVGElementImpl::CurrentTranslate			DontDelete|ReadOnly
 onunload					SVGSVGElementImpl::OnUnload					DontDelete
 onerror					SVGSVGElementImpl::OnError					DontDelete
 onresize					SVGSVGElementImpl::OnResize					DontDelete
 onzoom						SVGSVGElementImpl::OnZoom					DontDelete
 onscroll					SVGSVGElementImpl::OnScroll					DontDelete
@end
@namespace KSVG
@begin SVGSVGElementImplProto::s_hashTable 29
 createSVGNumber					SVGSVGElementImpl::CreateSVGNumber				DontDelete|Function 0
 createSVGLength					SVGSVGElementImpl::CreateSVGLength				DontDelete|Function 0
 createSVGAngle						SVGSVGElementImpl::CreateSVGAngle				DontDelete|Function 0
 createSVGPoint						SVGSVGElementImpl::CreateSVGPoint				DontDelete|Function 0
 createSVGMatrix					SVGSVGElementImpl::CreateSVGMatrix				DontDelete|Function 0
 createSVGRect						SVGSVGElementImpl::CreateSVGRect				DontDelete|Function 0
 createSVGTransform					SVGSVGElementImpl::CreateSVGTransform			DontDelete|Function 0
 createSVGTransformFromMatrix		SVGSVGElementImpl::CreateSVGTransformFromMatrix	DontDelete|Function 1
 suspendRedraw						SVGSVGElementImpl::SuspendRedraw				DontDelete|Function 1
 unsuspendRedraw					SVGSVGElementImpl::UnsuspendRedraw				DontDelete|Function 1
 unsuspendRedrawAll					SVGSVGElementImpl::UnsuspendRedrawAll			DontDelete|Function 0
 forceRedraw						SVGSVGElementImpl::ForceRedraw					DontDelete|Function 0
 pauseAnimations					SVGSVGElementImpl::PauseAnimations				DontDelete|Function 0
 unpauseAnimations					SVGSVGElementImpl::UnpauseAnimations			DontDelete|Function 0
 animationsPaused					SVGSVGElementImpl::AnimationsPaused				DontDelete|Function 0
 getCurrentTime						SVGSVGElementImpl::GetCurrentTime				DontDelete|Function 0
 setCurrentTime						SVGSVGElementImpl::SetCurrentTime				DontDelete|Function 1
 getIntersectionList				SVGSVGElementImpl::GetIntersectionList			DontDelete|Function 2
 getEnclosureList					SVGSVGElementImpl::GetEnclosureList				DontDelete|Function 2
 checkIntersection					SVGSVGElementImpl::CheckIntersection			DontDelete|Function 2
 checkEnclosure						SVGSVGElementImpl::CheckEnclosure				DontDelete|Function 2
 deselectAll						SVGSVGElementImpl::DeselectAll					DontDelete|Function 0
 getElementById						SVGSVGElementImpl::GetElementById				DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGSVGElement", SVGSVGElementImplProto, SVGSVGElementImplProtoFunc)

Value SVGSVGElementImpl::getValueProperty(ExecState *exec, int token) const
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
		case Viewport:
			return m_viewport->cache(exec);
		case ContentScriptType:
			return String(contentScriptType().string());
		case ContentStyleType:
			return String(contentStyleType().string());
		case PixelUnitToMillimeterX:
			return Number(pixelUnitToMillimeterX());
		case PixelUnitToMillimeterY:
			return Number(pixelUnitToMillimeterY());
		case ScreenPixelToMillimeterX:
			return Number(screenPixelToMillimeterX());
		case ScreenPixelToMillimeterY:
			return Number(screenPixelToMillimeterY());
		case UseCurrentView:
			return Boolean(useCurrentView());
		case CurrentScale:
			return Number(currentScale());
		case CurrentTranslate:
			return m_currentTranslate->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGSVGElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case ContentScriptType:
			setContentScriptType(value.toString(exec).string());
			break;
		case ContentStyleType:
			setContentStyleType(value.toString(exec).string());
			break;
		case CurrentScale:
			m_currentScale = value.toNumber(exec);
			break;
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
		case OnUnload:
			// Spec: only applicable to outermost 'svg' elements
			if(isRootElement())
				setEventListener(SVGEvent::UNLOAD_EVENT, ownerDoc()->createEventListener(value.toString(exec).qstring()));
			break;
		case OnError:
			setEventListener(SVGEvent::ERROR_EVENT, ownerDoc()->createEventListener(value.toString(exec).qstring()));
			break;
		case OnResize:
			// Spec: only applicable to outermost 'svg' elements
			if(isRootElement())
				setEventListener(SVGEvent::RESIZE_EVENT, ownerDoc()->createEventListener(value.toString(exec).qstring()));
			break;
		case OnZoom:
			// Spec: only applicable to outermost 'svg' elements
			if(isRootElement())
				setEventListener(SVGEvent::ZOOM_EVENT, ownerDoc()->createEventListener(value.toString(exec).qstring()));
			break;
		case OnScroll:
			// Spec: only applicable to outermost 'svg' elements
			if(isRootElement())
				setEventListener(SVGEvent::SCROLL_EVENT, ownerDoc()->createEventListener(value.toString(exec).qstring()));
			break;
		default:
			kdWarning() << k_funcinfo << "unhandled token " << token << endl;
	}
}

Value SVGSVGElementImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGSVGElementImpl)
	
	switch(id)
	{
		case SVGSVGElementImpl::CreateSVGNumber:
			return obj->createSVGNumber()->cache(exec);
		case SVGSVGElementImpl::CreateSVGLength:
			return obj->createSVGLength()->cache(exec);
		case SVGSVGElementImpl::CreateSVGAngle:
			return obj->createSVGAngle()->cache(exec);
		case SVGSVGElementImpl::CreateSVGPoint:
			return obj->createSVGPoint()->cache(exec);
		case SVGSVGElementImpl::CreateSVGMatrix:
			return obj->createSVGMatrix()->cache(exec);
		case SVGSVGElementImpl::CreateSVGRect:
			return obj->createSVGRect()->cache(exec);
		case SVGSVGElementImpl::CreateSVGTransform:
			return obj->createSVGTransform()->cache(exec);
		case SVGSVGElementImpl::CreateSVGTransformFromMatrix:
			return obj->createSVGTransformFromMatrix(static_cast<KSVGBridge<SVGMatrixImpl> *>(args[0].imp())->impl())->cache(exec);
		case SVGSVGElementImpl::GetElementById:
		{
			// Keep in sync with SVGDocumentImpl's version.
			Value ret;

			SVGElementImpl *element = obj->getElementById(args[0].toString(exec).string());

			if(element)
				ret = getDOMNode(exec, *element);
			else
			{
				element = obj->ownerDoc()->recursiveSearch(*(obj->ownerDoc()), args[0].toString(exec).string());
				if(!element)
					return Null();

				ret = getDOMNode(exec, *element);
			}

			return ret;
		}
		case SVGSVGElementImpl::GetCurrentTime:
			return Number(obj->getCurrentTime());
		case SVGSVGElementImpl::SetCurrentTime:
			obj->setCurrentTime(args[0].toNumber(exec));
			return Undefined();
		case SVGSVGElementImpl::DeselectAll:
			obj->deSelectAll();
			return Undefined();
		case SVGSVGElementImpl::PauseAnimations:
			obj->pauseAnimations();
			return Undefined();
		case SVGSVGElementImpl::UnpauseAnimations:
			obj->unpauseAnimations();
			return Undefined();
		case SVGSVGElementImpl::AnimationsPaused:
			return Boolean(obj->animationsPaused());
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;	
	}
	
	return Undefined();
}

// vim:ts=4:noet
