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

#ifndef SVGSVGElementImpl_H
#define SVGSVGElementImpl_H

#include <dom/dom_string.h>

#include <qmap.h>

#include "SVGTestsImpl.h"
#include "SVGElementImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLocatableImpl.h"
#include "SVGContainerImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGZoomAndPanImpl.h"
#include "SVGFitToViewBoxImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGPointImpl;
class SVGAngleImpl;
class SVGNumberImpl;
class SVGLengthImpl;
class SVGMatrixImpl;
class SVGViewSpecImpl;
class SVGTransformImpl;
class SVGAnimatedLengthImpl;
class SVGAnimationElementImpl;
class SVGSVGElementImpl : public SVGContainerImpl,
						  public SVGTestsImpl,
						  public SVGLangSpaceImpl,
						  public SVGExternalResourcesRequiredImpl,
						  public SVGStylableImpl,
						  public SVGLocatableImpl,
						  public SVGFitToViewBoxImpl,
						  public SVGZoomAndPanImpl
{
public:
	SVGSVGElementImpl(DOM::ElementImpl *);
	virtual ~SVGSVGElementImpl();

	bool isRootElement() const;

	SVGAnimatedLengthImpl *x();
	SVGAnimatedLengthImpl *y();
	SVGAnimatedLengthImpl *width();
	SVGAnimatedLengthImpl *height();

	void setContentScriptType(const DOM::DOMString &);
	DOM::DOMString contentScriptType() const;

	void setContentStyleType(const DOM::DOMString &);
	DOM::DOMString contentStyleType() const;

	SVGRectImpl *viewport();
	SVGRectImpl *getBBox();

	float pixelUnitToMillimeterX() const;
	float pixelUnitToMillimeterY() const;
	float screenPixelToMillimeterX() const;
	float screenPixelToMillimeterY() const;

	void setUseCurrentView(bool);
	bool useCurrentView() const;
	SVGViewSpecImpl *currentView() const;

	void setCurrentScale(float);
	float currentScale() const;

	SVGPointImpl *currentTranslate();
	void setCurrentTranslate(const QPoint &p);

	unsigned long suspendRedraw(unsigned long max_wait_milliseconds);
	void unsuspendRedraw(unsigned long suspend_handle_id);
	void unsuspendRedrawAll();
	void forceRedraw();

	void pauseAnimations();
	void unpauseAnimations();

	bool animationsPaused();

	float getCurrentTime() const;
	void setCurrentTime(float seconds);
	DOM::NodeList getIntersectionList(SVGRectImpl *rect, SVGElementImpl *referenceElement);
	DOM::NodeList getEnclosureList(SVGRectImpl *rect, SVGElementImpl *referenceElement);
	bool checkIntersection(SVGElementImpl *element, SVGRectImpl *rect);
	bool checkEnclosure(SVGElementImpl *element, SVGRectImpl *rect);
	void deSelectAll();

	// Static creators for svg primitives
	static SVGNumberImpl *createSVGNumber();
	static SVGLengthImpl *createSVGLength();
	static SVGAngleImpl *createSVGAngle();
	static SVGPointImpl *createSVGPoint();
	static SVGMatrixImpl *createSVGMatrix();
	static SVGRectImpl *createSVGRect();
	static SVGTransformImpl *createSVGTransform();
	static SVGTransformImpl *createSVGTransformFromMatrix(SVGMatrixImpl *matrix);

	SVGElementImpl *getElementById(const DOM::DOMString &elementId);
	void addToIdMap(const QString &id, SVGElementImpl *obj);

	virtual SVGMatrixImpl *getCTM();
	virtual const SVGMatrixImpl *localMatrix();

	void setAttributes();

	bool prepareMouseEvent(const QPoint &p, const QPoint &a, SVGMouseEventImpl *event);

	virtual bool isContainer() const { return true; }

	virtual void setClip(const QString &clip);
	virtual QRect clip();

	void setRootParentScreenCTM(SVGMatrixImpl *screenCTM);

private:
	SVGAnimatedLengthImpl *m_x;
	SVGAnimatedLengthImpl *m_y;
	SVGAnimatedLengthImpl *m_width;
	SVGAnimatedLengthImpl *m_height;

	SVGRectImpl *m_viewport;

	bool m_useCurrentView;

	SVGViewSpecImpl *m_currentView;

	float m_currentScale;

	SVGPointImpl *m_currentTranslate;

	SVGLengthImpl *m_clip[4];

	QMap<QString, SVGElementImpl *> m_map;

	// Transformation provided by the 'parent' of the outermost svg element
	SVGMatrixImpl *m_rootParentScreenCTM;

	SVGMatrixImpl *m_localMatrix;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		X, Y, Width, Height, ContentScriptType, ContentStyleType, Viewport,
		PixelUnitToMillimeterX, PixelUnitToMillimeterY, ScreenPixelToMillimeterX, ScreenPixelToMillimeterY,
		UseCurrentView, CurrentScale, CurrentTranslate, OnUnload, OnError, OnResize, OnZoom, OnScroll,
		// Functions
		CreateSVGNumber, CreateSVGLength, CreateSVGAngle, CreateSVGPoint, CreateSVGMatrix, CreateSVGRect, CreateSVGTransform,
		CreateSVGTransformFromMatrix, SuspendRedraw, UnsuspendRedraw, UnsuspendRedrawAll, ForceRedraw,
		PauseAnimations, UnpauseAnimations, AnimationsPaused, GetCurrentTime, SetCurrentTime,
		GetIntersectionList, GetEnclosureList, CheckIntersection, CheckEnclosure,
		DeselectAll, GetElementById
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGSVGElementImpl, "svg")

}

KSVG_DEFINE_PROTOTYPE(SVGSVGElementImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGSVGElementImplProtoFunc, SVGSVGElementImpl)

#endif

// vim:ts=4:noet
