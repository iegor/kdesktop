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

#ifndef SVGColorProfileElementImpl_H
#define SVGColorProfileElementImpl_H

#include <dom/dom_string.h>

// Provided by configure checks....
#include <config.h>
#undef QT_VERSION // Needed for 1.08 *grml*
#include LCMS_HEADER

#include "SVGElementImpl.h"
#include "SVGURIReferenceImpl.h"

#include "ksvg_lookup.h"

class QImage;

namespace KSVG
{

class SVGColorProfileElementImpl : public SVGElementImpl,
								   public SVGURIReferenceImpl
{
public:
	SVGColorProfileElementImpl(DOM::ElementImpl *);
	virtual ~SVGColorProfileElementImpl();

	void setLocal(const DOM::DOMString &local);
	DOM::DOMString local() const;

	void setName(const DOM::DOMString &name);
	DOM::DOMString name() const;

	void setRenderingIntent(unsigned short renderingIntent);
	unsigned short renderingIntent() const;

	QImage *correctImage(QImage *input);
	QRgb correctPixel(float r, float g, float b);

private:
	bool loadColorProfile();
	void closeColorProfile();

	bool canLoad();
	bool canLoad(bool remote, bool &tempFile, QString &open, bool verbose);

	DOM::DOMString m_local;
	DOM::DOMString m_name;
	unsigned short m_renderingIntent;

	bool m_loaded;

	cmsHPROFILE m_hInput, m_hOutput;
	cmsHTRANSFORM m_hTrans;
	int m_intent;

	cmsColorSpaceSignature m_inputColorSpace, m_outputColorSpace;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		Name, Href, RenderingIntent
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGColorProfileElementImpl, "color-profile")

}

#endif

// vim:ts=4:noet
