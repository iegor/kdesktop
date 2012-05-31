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

#ifndef SVGImageElementImpl_H
#define SVGImageElementImpl_H

#include <qobject.h>

#include "ksvg_lookup.h"

#include "SVGTestsImpl.h"
#include "SVGShapeImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGTransformableImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

namespace KSVG
{

class KSVGPolygon;
class SVGDocumentImpl;
class SVGSVGElementImpl;
class SVGAnimatedLengthImpl;
class SVGColorProfileElementImpl;
class SVGAnimatedPreserveAspectRatioImpl;
class SVGImageElementImpl : public QObject,
							public SVGShapeImpl,
							public SVGURIReferenceImpl,
							public SVGTestsImpl,
							public SVGLangSpaceImpl,
							public SVGExternalResourcesRequiredImpl,
							public SVGStylableImpl,
							public SVGTransformableImpl
{
Q_OBJECT
public:
	SVGImageElementImpl(DOM::ElementImpl *);
	virtual ~SVGImageElementImpl();

	SVGAnimatedLengthImpl *x();
	SVGAnimatedLengthImpl *y();
	SVGAnimatedLengthImpl *width();
	SVGAnimatedLengthImpl *height();
	SVGAnimatedPreserveAspectRatioImpl *preserveAspectRatio() const;

	virtual void createItem(KSVGCanvas *c);
	virtual void removeItem(KSVGCanvas *c);

	virtual void setAttributes();

	virtual bool prepareMouseEvent(const QPoint &p, const QPoint &a, SVGMouseEventImpl *mev);

	void setImage(QImage *image);
	QImage *image() { return m_image; }

	QImage scaledImage();
	SVGMatrixImpl *imageMatrix();
	SVGMatrixImpl *scaledImageMatrix();

	SVGSVGElementImpl *svgImageRootElement() const { return m_svgRoot; }

	void applyColorProfile();
	static void applyColorProfile(SVGColorProfileElementImpl *profile, SVGImageElementImpl *image);

	QString fileName() const;

	virtual SVGRectImpl *getBBox();
	
	// Screen-space clipping shape
	KSVGPolygon clippingShape();

	void onScreenCTMUpdated();

	void setupSVGElement(SVGSVGElementImpl *svg);

private slots:
	void slotParsingFinished(bool error, const QString &errorDesc);
	void slotLoadingFinished();

private:
	SVGAnimatedLengthImpl *m_x;
	SVGAnimatedLengthImpl *m_y;
	SVGAnimatedLengthImpl *m_width;
	SVGAnimatedLengthImpl *m_height;

	QImage *m_image;

	bool m_colorProfileApplied;
	SVGAnimatedPreserveAspectRatioImpl *m_preserveAspectRatio;
	SVGColorProfileElementImpl *m_colorProfile;
	SVGSVGElementImpl *m_svgRoot;
	SVGDocumentImpl *m_doc;

public:
	KSVG_GET
	KSVG_BRIDGE
	KSVG_PUT

	enum
	{
		// Properties
		X, Y, Width, Height, PreserveAspectRatio, Href
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

struct ImageStreamMap
{
	QByteArray *data;
	SVGImageElementImpl *imageElement;
};

KSVG_REGISTER_ELEMENT(SVGImageElementImpl, "image")

}

#endif

// vim:ts=4:noet
