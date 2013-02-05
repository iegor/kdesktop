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

#include <kurl.h>
#include <kdebug.h>
#include <kio/netaccess.h>

#include <qimage.h>

#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"

#include "SVGRenderingIntent.h"

#include "SVGAnimatedStringImpl.h"
#include "SVGColorProfileElementImpl.h"

using namespace KSVG;

#include "SVGColorProfileElementImpl.lut.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGColorProfileElementImpl::SVGColorProfileElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGURIReferenceImpl()
{
	KSVG_EMPTY_FLAGS

	m_loaded = false;

	// Spec: Default value 'auto', if not overwritten later
	m_renderingIntent = RENDERING_INTENT_AUTO;
}

SVGColorProfileElementImpl::~SVGColorProfileElementImpl()
{
	if(m_loaded)
		closeColorProfile();
}

/*
@namespace KSVG
@begin SVGColorProfileElementImpl::s_hashTable 5
 name	SVGColorProfileElementImpl::Name	DontDelete|ReadOnly
 href	SVGColorProfileElementImpl::Href	DontDelete|ReadOnly
 rendering-intent	SVGColorProfileElementImpl::RenderingIntent	DontDelete|ReadOnly
@end
*/

Value SVGColorProfileElementImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case Name:
			return String(m_name);
		case Href:
			return href()->cache(exec);
		case RenderingIntent:
			return Number(m_renderingIntent);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGColorProfileElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Name:
			m_name = value.toString(exec).string();
			ownerDoc()->rootElement()->addToIdMap(m_name.string(), this);
			break;
		case Href:
			href()->setBaseVal(value.toString(exec).string());
			break;
		case RenderingIntent:
		{
			QString compare = value.toString(exec).qstring().lower();

			if(compare == "perceptual")
				m_renderingIntent = RENDERING_INTENT_PERCEPTUAL;
			else if(compare == "relative-colorimetric")
				m_renderingIntent = RENDERING_INTENT_RELATIVE_COLORIMETRIC;
			else if(compare == "saturation")
				m_renderingIntent = RENDERING_INTENT_SATURATION;
			else if(compare == "absolute-colorimetric")
				m_renderingIntent = RENDERING_INTENT_ABSOLUTE_COLORIMETRIC;
			else
				m_renderingIntent = RENDERING_INTENT_AUTO;
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGColorProfileElementImpl::setLocal(const DOM::DOMString &local)
{
	m_local = local;
}

DOM::DOMString SVGColorProfileElementImpl::local() const
{
	return m_local;
}

void SVGColorProfileElementImpl::setName(const DOM::DOMString &name)
{
	m_name = name;
}

DOM::DOMString SVGColorProfileElementImpl::name() const
{
	return m_name;
}

void SVGColorProfileElementImpl::setRenderingIntent(unsigned short renderingIntent)
{
	m_renderingIntent = renderingIntent;
}

unsigned short SVGColorProfileElementImpl::renderingIntent() const
{
	return m_renderingIntent;
}

bool SVGColorProfileElementImpl::canLoad()
{
	QString open;
	bool temp;
	return canLoad(false, temp, open, true);
}

bool SVGColorProfileElementImpl::canLoad(bool remote, bool &tempFile, QString &open, bool verbose)
{
	KURL file;

	if(!KURL::isRelativeURL(href()->baseVal().string()))
		file = KURL(href()->baseVal().string());
	else
		file = KURL(ownerDoc()->baseUrl(), href()->baseVal().string());

	if(file.path().isEmpty())
	{
		if(verbose)
		    kdDebug() << "Couldn't load color profile " << file.path() << "!" << endl;

		return false;
	}

	if(file.isLocalFile())
	{
		open = file.path();

		if(!QFile::exists(open))
		{
			if(verbose)
				kdDebug() << "Couldn't load color profile " << file.path() << "! It does not exist." << endl;

			return false;
		}
	}
	else
	{
		if(remote)
		{
			if(KIO::NetAccess::download(file, open, 0))
				tempFile = true;
		}
	}

	return true;
}

bool SVGColorProfileElementImpl::loadColorProfile()
{
	QString open;
	bool tempFile = false;

	if(!canLoad(true, tempFile, open, false))
		return false;

	m_hInput = cmsOpenProfileFromFile(open.latin1(), "r");
	m_hOutput = cmsCreate_sRGBProfile();

	unsigned int dwIn = BYTES_SH(2) | CHANNELS_SH(cmsChannelsOf(m_inputColorSpace));
	unsigned int dwOut = BYTES_SH(2) | CHANNELS_SH(cmsChannelsOf(m_outputColorSpace));

	if(m_renderingIntent != RENDERING_INTENT_AUTO)
		m_hTrans = cmsCreateTransform(m_hInput, dwIn, m_hOutput, dwOut, m_renderingIntent - 2, cmsFLAGS_NOOPTIMIZE);
	else
		// this maybe stupid O_o. dkCmsTakeRenderingIntent -> cmsGetHeaderRenderingIntent
		m_hTrans = cmsCreateTransform(m_hInput, dwIn, m_hOutput, dwOut, cmsGetHeaderRenderingIntent(m_hInput), cmsFLAGS_NOOPTIMIZE);

	m_inputColorSpace = cmsGetColorSpace(m_hInput);
	m_outputColorSpace = cmsGetColorSpace(m_hOutput);
	m_loaded = true;

	if(tempFile)
		KIO::NetAccess::removeTempFile(open);

	return true;
}

void SVGColorProfileElementImpl::closeColorProfile()
{
	cmsDeleteTransform(m_hTrans);
	cmsCloseProfile(m_hInput);
}

QRgb SVGColorProfileElementImpl::correctPixel(float r, float g, float b)
{
	if(!m_loaded)
	{
		if(!loadColorProfile())
			return qRgb(0, 0, 0);
	}

	unsigned short input[cmsMAXCHANNELS], output[cmsMAXCHANNELS];

	input[0] = ((unsigned int) r) * 257;
	input[1] = ((unsigned int) g) * 257;
	input[2] = ((unsigned int) b) * 257;

	cmsDoTransform(m_hTrans, input, output, 1);

	if(m_outputColorSpace == cmsSigRgbData)
		return qRgb(output[0] / 257, output[1] / 257, output[2] / 257);

	return qRgb(0, 0, 0);
}

QImage *SVGColorProfileElementImpl::correctImage(QImage *input)
{
	if(!canLoad())
		return input;

	for(int y = 0; y < input->height(); y++)
	{
		for(int x = 0; x < input->width(); x++)
		{
			QRgb pixel = input->pixel(x, y);
			input->setPixel(x, y, correctPixel(qRed(pixel), qGreen(pixel), qBlue(pixel)));
		}
	}

	return input;
}

// vim:ts=4:noet
