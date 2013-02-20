/*
    Copyright (C) 2001-2003 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option); any later version.

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

#include <qstringlist.h>

#include "SVGColor.h"

#include "SVGColorImpl.h"
#include "SVGNumberImpl.h"
#include "SVGICCColorImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGNumberListImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGColorProfileElementImpl.h"

using namespace KSVG;

#include "SVGColorImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_cacheimpl.h"

SVGColorImpl::SVGColorImpl(SVGElementImpl *object) : m_object(object)
{
	m_colorType = SVG_COLORTYPE_UNKNOWN;
	m_iccColor = 0;
}

SVGColorImpl::SVGColorImpl(const SVGColorImpl &other) : DOM::DomShared()
{
	(*this) = other;
}

SVGColorImpl::~SVGColorImpl()
{
	if(m_iccColor)
		m_iccColor->deref();
}

SVGColorImpl &SVGColorImpl::operator=(const SVGColorImpl &other)
{
	m_colorType = other.m_colorType;
	m_rgbColor = other.m_rgbColor;

	if(m_iccColor && other.m_iccColor)
		*m_iccColor = *(other.m_iccColor);

	return *this;
}

unsigned short SVGColorImpl::colorType() const
{
	return m_colorType;
}

DOM::RGBColor SVGColorImpl::rgbColor() const
{
	return m_rgbColor;
}

SVGICCColorImpl *SVGColorImpl::iccColor() const
{
	return m_iccColor;
}

void SVGColorImpl::setRGBColor(QColor color)
{
	m_colorType = SVG_COLORTYPE_RGBCOLOR;
	m_rgbColor = DOM::RGBColor(color.rgb());
}

void SVGColorImpl::setRGBColor(int r, int g, int b)
{
	m_colorType = SVG_COLORTYPE_RGBCOLOR;
	m_rgbColor = DOM::RGBColor(QColor(r, g, b).rgb());
}

void SVGColorImpl::setRGBColor(const DOM::DOMString &rgbColor)
{
	if(rgbColor == "aliceblue")
		setRGBColor(240, 248, 255);
	else if(rgbColor == "antiquewhite")
		setRGBColor(250, 235, 215);
	else if(rgbColor == "aqua")
		setRGBColor(0, 255, 255);
	else if(rgbColor == "aquamarine")
		setRGBColor(127, 255, 212);
	else if(rgbColor == "azure")
		setRGBColor(240, 255, 255);
	else if(rgbColor == "beige")
		setRGBColor(245, 245, 220);
	else if(rgbColor == "bisque")
		setRGBColor(255, 228, 196);
	else if(rgbColor == "black")
		setRGBColor(0, 0, 0);
	else if(rgbColor == "blanchedalmond")
		setRGBColor(255, 235, 205);
	else if(rgbColor == "blue")
		setRGBColor(0, 0, 255);
	else if(rgbColor == "blueviolet")
		setRGBColor(138, 43, 226);
	else if(rgbColor == "brown")
		setRGBColor(165, 42, 42);
	else if(rgbColor == "burlywood")
		setRGBColor(222, 184, 135);
	else if(rgbColor == "cadetblue")
		setRGBColor(95, 158, 160);
	else if(rgbColor == "chartreuse")
		setRGBColor(127, 255, 0);
	else if(rgbColor == "chocolate")
		setRGBColor(210, 105, 30);
	else if(rgbColor == "coral")
		setRGBColor(255, 127, 80);
	else if(rgbColor == "cornflowerblue")
		setRGBColor(100, 149, 237);
	else if(rgbColor == "cornsilk")
		setRGBColor(255, 248, 220);
	else if(rgbColor == "crimson")
		setRGBColor(220, 20, 60);
	else if(rgbColor == "cyan")
		setRGBColor(0, 255, 255);
	else if(rgbColor == "darkblue")
		setRGBColor(0, 0, 139);
	else if(rgbColor == "darkcyan")
		setRGBColor(0, 139, 139);
	else if(rgbColor == "darkgoldenrod")
		setRGBColor(184, 134, 11);
	else if(rgbColor == "darkgray")
		setRGBColor(169, 169, 169);
	else if(rgbColor == "darkgrey")
		setRGBColor(169, 169, 169);
	else if(rgbColor == "darkgreen")
		setRGBColor(0, 100, 0);
	else if(rgbColor == "darkkhaki")
		setRGBColor(189, 183, 107);
	else if(rgbColor == "darkmagenta")
		setRGBColor(139, 0, 139);
	else if(rgbColor == "darkolivegreen")
		setRGBColor(85, 107, 47);
	else if(rgbColor == "darkorange")
		setRGBColor(255, 140, 0);
	else if(rgbColor == "darkorchid")
		setRGBColor(153, 50, 204);
	else if(rgbColor == "darkred")
		setRGBColor(139, 0, 0);
	else if(rgbColor == "darksalmon")
		setRGBColor(233, 150, 122);
	else if(rgbColor == "darkseagreen")
		setRGBColor(143, 188, 143);
	else if(rgbColor == "darkslateblue")
		setRGBColor(72, 61, 139);
	else if(rgbColor == "darkslategray")
		setRGBColor(47, 79, 79);
	else if(rgbColor == "darkslategrey")
		setRGBColor(47, 79, 79);
	else if(rgbColor == "darkturquoise")
		setRGBColor(0, 206, 209);
	else if(rgbColor == "darkviolet")
		setRGBColor(148, 0, 211);
	else if(rgbColor == "deeppink")
		setRGBColor(255, 20, 147);
	else if(rgbColor == "deepskyblue")
		setRGBColor(0, 191, 255);
	else if(rgbColor == "dimgray")
		setRGBColor(105, 105, 105);
	else if(rgbColor == "dimgrey")
		setRGBColor(105, 105, 105);
	else if(rgbColor == "dodgerblue")
		setRGBColor(30, 144, 255);
	else if(rgbColor == "firebrick")
		setRGBColor(178, 34, 34);
	else if(rgbColor == "floralwhite")
		setRGBColor(255, 250, 240);
	else if(rgbColor == "forestgreen")
		setRGBColor(34, 139, 34);
	else if(rgbColor == "fuchsia")
		setRGBColor(255, 0, 255);
	else if(rgbColor == "gainsboro")
		setRGBColor(220, 220, 220);
	else if(rgbColor == "ghostwhite")
		setRGBColor(248, 248, 255);
	else if(rgbColor == "gold")
		setRGBColor(255, 215, 0);
	else if(rgbColor == "goldenrod")
		setRGBColor(218, 165, 32);
	else if(rgbColor == "gray")
		setRGBColor(128, 128, 128);
	else if(rgbColor == "grey")
		setRGBColor(128, 128, 128);
	else if(rgbColor == "green")
		setRGBColor(0, 128, 0);
	else if(rgbColor == "greenyellow")
		setRGBColor(173, 255, 47);
	else if(rgbColor == "honeydew")
		setRGBColor(240, 255, 240);
	else if(rgbColor == "hotpink")
		setRGBColor(255, 105, 180);
	else if(rgbColor == "indianred")
		setRGBColor(205, 92, 92);
	else if(rgbColor == "indigo")
		setRGBColor(75, 0, 130);
	else if(rgbColor == "ivory")
		setRGBColor(255, 255, 240);
	else if(rgbColor == "khaki")
		setRGBColor(240, 230, 140);
	else if(rgbColor == "lavender")
		setRGBColor(230, 230, 250);
	else if(rgbColor == "lavenderblush")
		setRGBColor(255, 240, 245);
	else if(rgbColor == "lawngreen")
		setRGBColor(124, 252, 0);
	else if(rgbColor == "lemonchiffon")
		setRGBColor(255, 250, 205);
	else if(rgbColor == "lightblue")
		setRGBColor(173, 216, 230);
	else if(rgbColor == "lightcoral")
		setRGBColor(240, 128, 128);
	else if(rgbColor == "lightcyan")
		setRGBColor(224, 255, 255);
	else if(rgbColor == "lightgoldenrodyellow")
		setRGBColor(250, 250, 210);
	else if(rgbColor == "lightgray")
		setRGBColor(211, 211, 211);
	else if(rgbColor == "lightgrey")
		setRGBColor(211, 211, 211);
	else if(rgbColor == "lightgreen")
		setRGBColor(144, 238, 144);
	else if(rgbColor == "lightpink")
		setRGBColor(255, 182, 193);
	else if(rgbColor == "lightsalmon")
		setRGBColor(255, 160, 122);
	else if(rgbColor == "lightseagreen")
		setRGBColor(32, 178, 170);
	else if(rgbColor == "lightskyblue")
		setRGBColor(135, 206, 250);
	else if(rgbColor == "lightslategray")
		setRGBColor(119, 136, 153);
	else if(rgbColor == "lightslategrey")
		setRGBColor(119, 136, 153);
	else if(rgbColor == "lightsteelblue")
		setRGBColor(176, 196, 222);
	else if(rgbColor == "lightyellow")
		setRGBColor(255, 255, 224);
	else if(rgbColor == "lime")
		setRGBColor(0, 255, 0);
	else if(rgbColor == "limegreen")
		setRGBColor(50, 205, 50);
	else if(rgbColor == "linen")
		setRGBColor(250, 240, 230);
	else if(rgbColor == "magenta")
		setRGBColor(255, 0, 255);
	else if(rgbColor == "maroon")
		setRGBColor(128, 0, 0);
	else if(rgbColor == "mediumaquamarine")
		setRGBColor(102, 205, 170);
	else if(rgbColor == "mediumblue")
		setRGBColor(0, 0, 205);
	else if(rgbColor == "mediumorchid")
		setRGBColor(186, 85, 211);
	else if(rgbColor == "mediumpurple")
		setRGBColor(147, 112, 219);
	else if(rgbColor == "mediumseagreen")
		setRGBColor(60, 179, 113);
	else if(rgbColor == "mediumslateblue")
		setRGBColor(123, 104, 238);
	else if(rgbColor == "mediumspringgreen")
		setRGBColor(0, 250, 154);
	else if(rgbColor == "mediumturquoise")
		setRGBColor(72, 209, 204);
	else if(rgbColor == "mediumvioletred")
		setRGBColor(199, 21, 133);
	else if(rgbColor == "midnightblue")
		setRGBColor(25, 25, 112);
	else if(rgbColor == "mintcream")
		setRGBColor(245, 255, 250);
	else if(rgbColor == "mistyrose")
		setRGBColor(255, 228, 225);
	else if(rgbColor == "moccasin")
		setRGBColor(255, 228, 181);
	else if(rgbColor == "navajowhite")
		setRGBColor(255, 222, 173);
	else if(rgbColor == "navy")
		setRGBColor(0, 0, 128);
	else if(rgbColor == "oldlace")
		setRGBColor(253, 245, 230);
	else if(rgbColor == "olive")
		setRGBColor(128, 128, 0);
	else if(rgbColor == "olivedrab")
		setRGBColor(107, 142, 35);
	else if(rgbColor == "orange")
		setRGBColor(255, 165, 0);
	else if(rgbColor == "orangered")
		setRGBColor(255, 69, 0);
	else if(rgbColor == "orchid")
		setRGBColor(218, 112, 214);
	else if(rgbColor == "palegoldenrod")
		setRGBColor(238, 232, 170);
	else if(rgbColor == "palegreen")
		setRGBColor(152, 251, 152);
	else if(rgbColor == "paleturquoise")
		setRGBColor(175, 238, 238);
	else if(rgbColor == "palevioletred")
		setRGBColor(219, 112, 147);
	else if(rgbColor == "papayawhip")
		setRGBColor(255, 239, 213);
	else if(rgbColor == "peachpuff")
		setRGBColor(255, 218, 185);
	else if(rgbColor == "peru")
		setRGBColor(205, 133, 63);
	else if(rgbColor == "pink")
		setRGBColor(255, 192, 203);
	else if(rgbColor == "plum")
		setRGBColor(221, 160, 221);
	else if(rgbColor == "powderblue")
		setRGBColor(176, 224, 230);
	else if(rgbColor == "purple")
		setRGBColor(128, 0, 128);
	else if(rgbColor == "red")
		setRGBColor(255, 0, 0);
	else if(rgbColor == "rosybrown")
		setRGBColor(188, 143, 143);
	else if(rgbColor == "royalblue")
		setRGBColor(65, 105, 225);
	else if(rgbColor == "saddlebrown")
		setRGBColor(139, 69, 19);
	else if(rgbColor == "salmon")
		setRGBColor(250, 128, 114);
	else if(rgbColor == "sandybrown")
		setRGBColor(244, 164, 96);
	else if(rgbColor == "seagreen")
		setRGBColor(46, 139, 87);
	else if(rgbColor == "seashell")
		setRGBColor(255, 245, 238);
	else if(rgbColor == "sienna")
		setRGBColor(160, 82, 45);
	else if(rgbColor == "silver")
		setRGBColor(192, 192, 192);
	else if(rgbColor == "skyblue")
		setRGBColor(135, 206, 235);
	else if(rgbColor == "slateblue")
		setRGBColor(106, 90, 205);
	else if(rgbColor == "slategray")
		setRGBColor(112, 128, 144);
	else if(rgbColor == "slategrey")
		setRGBColor(112, 128, 144);
	else if(rgbColor == "snow")
		setRGBColor(255, 250, 250);
	else if(rgbColor == "springgreen")
		setRGBColor(0, 255, 127);
	else if(rgbColor == "steelblue")
		setRGBColor(70, 130, 180);
	else if(rgbColor == "tan")
		setRGBColor(210, 180, 140);
	else if(rgbColor == "teal")
		setRGBColor(0, 128, 128);
	else if(rgbColor == "thistle")
		setRGBColor(216, 191, 216);
	else if(rgbColor == "tomato")
		setRGBColor(255, 99, 71);
	else if(rgbColor == "turquoise")
		setRGBColor(64, 224, 208);
	else if(rgbColor == "violet")
		setRGBColor(238, 130, 238);
	else if(rgbColor == "wheat")
		setRGBColor(245, 222, 179);
	else if(rgbColor == "white")
		setRGBColor(255, 255, 255);
	else if(rgbColor == "whitesmoke")
		setRGBColor(245, 245, 245);
	else if(rgbColor == "yellow")
		setRGBColor(255, 255, 0);
	else if(rgbColor == "yellowgreen")
		setRGBColor(154, 205, 50);
}

void SVGColorImpl::setRGBColorICCColor(const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor)
{
	QColor color;

	QString content = iccColor.string().right(iccColor.string().length() - 10);
	QString iccTarget = content.mid(0, content.find(','));

	QStringList colors = QStringList::split(',', content);
	QString r = colors[1];
	QString g = colors[2];
	QString b = colors[3].left(colors[3].length() - 1);

	iccTarget = SVGURIReferenceImpl::getTarget(iccTarget);

	SVGColorProfileElementImpl *handle = 0;
	if(m_object)
		handle = static_cast<SVGColorProfileElementImpl *>(dynamic_cast<SVGElementImpl *>(m_object)->ownerDoc()->rootElement()->getElementById(iccTarget));

	if(iccTarget.isEmpty() || !handle)
	{
		color.setNamedColor(rgbColor.string().stripWhiteSpace());
		setRGBColor(color);
	}
	else
	{
		color.setRgb(handle->correctPixel(r.toFloat() * 257, g.toFloat() * 257, b.toFloat() * 257));
		setRGBColor(color);

		m_colorType = SVG_COLORTYPE_RGBCOLOR_ICCCOLOR;

		if(!m_iccColor)
		{
			m_iccColor = new SVGICCColorImpl();
			m_iccColor->ref();
		}

		m_iccColor->setColorProfile(DOM::DOMString(content));

		SVGNumberImpl *rnumber = SVGSVGElementImpl::createSVGNumber();
		rnumber->setValue(r.toFloat());

		SVGNumberImpl *gnumber = SVGSVGElementImpl::createSVGNumber();
		gnumber->setValue(g.toFloat());

		SVGNumberImpl *bnumber = SVGSVGElementImpl::createSVGNumber();
		bnumber->setValue(b.toFloat());

		m_iccColor->colors()->clear();
		m_iccColor->colors()->appendItem(bnumber);
		m_iccColor->colors()->appendItem(gnumber);
		m_iccColor->colors()->appendItem(rnumber);
	}
}

void SVGColorImpl::setColor(unsigned short colorType, const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor)
{
	m_colorType = colorType;
	
	if(m_colorType == SVG_COLORTYPE_UNKNOWN || m_colorType == SVG_COLORTYPE_CURRENTCOLOR)
		return;

	setRGBColorICCColor(rgbColor, iccColor);
}

// Ecma stuff
/*
@namespace KSVG
@begin SVGColorImpl::s_hashTable 5
 colorType				SVGColorImpl::ColorType				DontDelete|ReadOnly
 RGBColor				SVGColorImpl::RGBColor				DontDelete|ReadOnly
 ICCColor				SVGColorImpl::ICCColor				DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGColorImplProto::s_hashTable 5
 setRGBColor			SVGColorImpl::SetRGBColor			DontDelete|Function 1
 setRGBColorICCColor	SVGColorImpl::SetRGBColorICCColor	DontDelete|Function 2
 setColor				SVGColorImpl::SetColor				DontDelete|Function 3
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGColor", SVGColorImplProto, SVGColorImplProtoFunc)

Value SVGColorImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case ColorType:
			return Number(colorType());
#ifdef __GNUC__
#warning FIXME bridge stuff
#endif
		case RGBColor:
			return Undefined();
		case ICCColor:		
			return m_iccColor->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGColorImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGColorImpl)

	switch(id)
	{
		case SVGColorImpl::SetRGBColor:
			obj->setRGBColor(args[0].toString(exec).string());
			break;
		case SVGColorImpl::SetRGBColorICCColor:
			obj->setRGBColorICCColor(args[0].toString(exec).string(), args[1].toString(exec).string());
			break;
		case SVGColorImpl::SetColor:
			obj->setColor(static_cast<unsigned short>(args[0].toNumber(exec)), args[1].toString(exec).string(), args[2].toString(exec).string());
			break;
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

/*
@namespace KSVG
@begin SVGColorImplConstructor::s_hashTable 5
 SVG_COLORTYPE_UNKNOWN				KSVG::SVG_COLORTYPE_UNKNOWN				DontDelete|ReadOnly
 SVG_COLORTYPE_RGBCOLOR				KSVG::SVG_COLORTYPE_RGBCOLOR			DontDelete|ReadOnly
 SVG_COLORTYPE_RGBCOLOR_ICCCOLOR	KSVG::SVG_COLORTYPE_RGBCOLOR_ICCCOLOR	DontDelete|ReadOnly
 SVG_COLORTYPE_CURRENTCOLOR			KSVG::SVG_COLORTYPE_CURRENTCOLOR		DontDelete|ReadOnly
@end
*/

Value SVGColorImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGColorImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGColorImplConstructor>(exec, "[[svgcolor.constructor]]");
}

// vim:ts=4:noet
