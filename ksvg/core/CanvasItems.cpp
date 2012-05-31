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
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "KSVGCanvas.h"
#include "KSVGHelper.h"
#include "KSVGTextChunk.h"

#include "CanvasItems.h"

#include "SVGMatrixImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGPathElementImpl.h"
#include "SVGMarkerElementImpl.h"
#include "SVGTSpanElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGAnimatedLengthListImpl.h"
#include "SVGAnimatedEnumerationImpl.h"

#include <Glyph.h>
#include <Converter.h>
#include <Font.h>

#include <kdebug.h>

using namespace KSVG;

CanvasText::CanvasText(SVGTextElementImpl *text) : CanvasItem(), m_text(text)
{
}

CanvasText::~CanvasText()
{
}

void CanvasText::handleTSpan(KSVGCanvas *canvas, const SVGMatrixImpl *screenCTM, int &curx, int &cury, int &endx, int &endy, SVGElementImpl *element, KSVGTextChunk *textChunk, T2P::BezierPath *bpath)
{
	SVGTSpanElementImpl *tspan = dynamic_cast<SVGTSpanElementImpl *>(element);
	if(!tspan)
		return;

	if(!tspan->text().isEmpty() || element->nodeName() == "tref")
	{
		if((KSVG_TOKEN_NOT_PARSED_ELEMENT(SVGTextPositioningElementImpl::X, tspan) && KSVG_TOKEN_NOT_PARSED_ELEMENT(SVGTextPositioningElementImpl::Y, tspan)))// && !bpath)
			textChunk->addText(tspan->text(), tspan);
		else
		{
			// new absolute value for next textChunk, render old one
			if(textChunk->count() > 0)
			{
				createGlyphs(textChunk, canvas, screenCTM, curx, cury, curx, cury, bpath);
				textChunk->clear();
			}

			int usex, usey;
			bool bMultipleX = false;
			bool bMultipleY = false;

			if(tspan->x()->baseVal()->numberOfItems() == 0)
			{
				usex = curx;

				if(tspan->dx()->baseVal()->numberOfItems() > 0)
					usex += int(tspan->dx()->baseVal()->getItem(0)->value());
			}
			else
			{
				if(tspan->x()->baseVal()->numberOfItems() > 1)
					bMultipleX = true;

				usex = int(tspan->x()->baseVal()->getItem(0)->value());
			}

			if(tspan->y()->baseVal()->numberOfItems() == 0)
			{
				usey = cury;

				if(tspan->dy()->baseVal()->numberOfItems() > 0)
					usey += int(tspan->dy()->baseVal()->getItem(0)->value());
			}
			else
			{
				if(tspan->y()->baseVal()->numberOfItems() > 1)
					bMultipleY = true;

				usey = int(tspan->y()->baseVal()->getItem(0)->value());
			}

			QString text = tspan->text();
			if(!text.isEmpty())
			{
				T2P::GlyphLayoutParams *params = tspan->layoutParams();

				if(bMultipleX || bMultipleY)
				{
					for(unsigned int i = 0; i < text.length(); i++)
					{
						if(bMultipleX && i < tspan->x()->baseVal()->numberOfItems())
							usex = int(tspan->x()->baseVal()->getItem(i)->value());
						if(bMultipleY && i < tspan->y()->baseVal()->numberOfItems())
							usey = int(tspan->y()->baseVal()->getItem(i)->value());

						textChunk->addText(QString(text.at(i)), tspan);
						createGlyphs(textChunk, canvas, screenCTM, usex, usey, endx, endy, bpath);
						textChunk->clear();

						if(!params->tb())
							usex += endx;
						else
							usey += endy;
					}
				}
				else
				{
					textChunk->addText(text, tspan);
					//createGlyphs(textChunk, canvas, screenCTM, usex, usey, endx, endy, bpath);
					//textChunk->clear();
				}

				curx = usex;
				cury = usey;

				if(!params->tb())
					curx += endx;
				else
					cury += endy;

				delete params;
			}
		}
	}

	DOM::Node node = (tspan->getTextDirection() == LTR) ? tspan->firstChild() : tspan->lastChild();

	bool tspanFound = false;
	for(; !node.isNull(); node = ((tspan->getTextDirection() == LTR) ? node.nextSibling() : node.previousSibling()))
	{
		SVGElementImpl *element = m_text->ownerDoc()->getElementFromHandle(node.handle());
		if(node.nodeType() == DOM::Node::TEXT_NODE)
		{
 			if(tspanFound)
			{
				DOM::Text text = node;
				QString temp = text.data().string();
				textChunk->addText(temp, tspan);
			}
		}
		else if(element->nodeName() == "tspan" || element->nodeName() == "tref")
		{
			tspanFound = true;
			handleTSpan(canvas, screenCTM, curx, cury, endx, endy, element, textChunk, 0);
		}
	}

}

KSVGTextChunk *CanvasText::createTextChunk(KSVGCanvas *canvas, const SVGMatrixImpl *screenCTM, int &curx, int &cury, int &endx, int &endy)
{
	KSVGTextChunk *textChunk = new KSVGTextChunk();

	SVGLengthImpl *length = m_text->x()->baseVal()->getItem(0);
	if(length)
		curx = int(length->value());

	length = m_text->y()->baseVal()->getItem(0);
	if(length)
		cury = int(length->value());

	// Otherwhise some js scripts which require a child, don't work (Niko)
	if(!m_text->hasChildNodes())
	{
		DOM::Text impl = static_cast<DOM::Document *>(m_text->ownerDoc())->createTextNode(DOM::DOMString(""));
		m_text->appendChild(impl);
	}
	else
	{
		DOM::Node node = (m_text->getTextDirection() == LTR) ? m_text->firstChild() : m_text->lastChild();

		for(; !node.isNull(); node = ((m_text->getTextDirection() == LTR) ? node.nextSibling() : node.previousSibling()))
		{
			if(node.nodeType() == DOM::Node::TEXT_NODE)
			{
				DOM::Text text = node;
				QString temp = text.data().string();

				if(!temp.isEmpty())
				{
					if(m_text->getTextDirection() != LTR)
					{
						QString convert = temp;

						for(int i = temp.length(); i > 0; i--)
							convert[temp.length() - i] = temp[i - 1];

						temp = convert;
					}

					textChunk->addText(temp, m_text);
				}
			}
			else
			{
				SVGElementImpl *element = m_text->ownerDoc()->getElementFromHandle(node.handle());
				if(element->nodeName() == "textPath")
				{
					// new absolute value for next textChunk, render old one
					if(textChunk->count() > 0)
					{
						createGlyphs(textChunk, canvas, screenCTM, curx, cury, curx, cury);
						textChunk->clear();
					}
					
					SVGTextPathElementImpl *tpath = dynamic_cast<SVGTextPathElementImpl *>(element);
					QString target = SVGURIReferenceImpl::getTarget(tpath->href()->baseVal().string());
					SVGPathElementImpl *path = dynamic_cast<SVGPathElementImpl *>(tpath->ownerSVGElement()->getElementById(target));

					T2P::BezierPath *bpath = 0;
					if(path && path->item())
						bpath = tpath->ownerDoc()->canvas()->toBezierPath(path->item());

					DOM::Node iterate = tpath->firstChild();
					for(; !iterate.isNull(); iterate = iterate.nextSibling())
					{
						if(iterate.nodeType() == DOM::Node::TEXT_NODE)
						{
							DOM::Text text = iterate;
							QString temp = text.data().string();

							if(!temp.isEmpty())
								textChunk->addText(temp, tpath);
						}
						else
						{
							kdDebug() << "FOUND TSPAN IN TEXTPATH! BPATH:" << bpath <<endl;

							SVGElementImpl *itelement = m_text->ownerDoc()->getElementFromHandle(iterate.handle());
							handleTSpan(canvas, screenCTM, curx, cury, endx, endy, itelement, textChunk, bpath);
						}
					}

					if(textChunk->count() > 0)
					{
						int usex = 0, usey = 0;
						createGlyphs(textChunk, canvas, screenCTM, usex, usey, endx, endy, bpath);
						textChunk->clear();

						curx = usex;
						cury = usey;

						T2P::GlyphLayoutParams *params = tpath->layoutParams();

						if(!params->tb())
							curx += endx;
						else
							cury += endy;

						delete params;
					}
				}
				else if(element->nodeName() == "tspan" || element->nodeName() == "tref")
					handleTSpan(canvas, screenCTM, curx, cury, endx, endy, element, textChunk, 0);
			}
		}
	}

	return textChunk;
}

void CanvasText::createGlyphs(KSVGTextChunk *textChunk, KSVGCanvas *canvas, const SVGMatrixImpl *screenCTM, int curx, int cury, int &endx, int &endy, T2P::BezierPath *bpath) const
{
	double _curx = double(curx);
	QMemArray<double> _cury(1);
	_cury[0] = double(cury);

	T2P::GlyphLayoutParams *params = m_text->layoutParams();
	SVGTextPositioningElementImpl *tp = textChunk->getTextElement(0);
	SVGTextContentElementImpl *tc = textChunk->getTextContentElement(0);
	SVGTextContentElementImpl *tc0 = tc;

	T2P::SharedFont font;
	QString text;
	QPtrList<T2P::GlyphSet> glyphs;
	glyphs.setAutoDelete(true);

	double pathAdvance = 0;
	SVGTextPathElementImpl *tpath = dynamic_cast<SVGTextPathElementImpl *>(tc0);
	if(tpath)
		pathAdvance = tpath->startOffset()->baseVal()->value();
	double pathLength = bpath ? bpath->length() : 0;
	double pathDy = 0;
	for(unsigned int i = 0; i < textChunk->count(); i++)
	{
		tp = textChunk->getTextElement(i);
		tc = textChunk->getTextContentElement(i);

		if(tp && tp->dx()->baseVal()->numberOfItems() > 0)
			if(bpath)
				pathAdvance += tp->dx()->baseVal()->getItem(0)->value() / pathLength;
			else
				_curx += tp->dx()->baseVal()->getItem(0)->value();
		_cury[i] += (tp && tp->dy()->baseVal()->numberOfItems() > 0) ? tp->dy()->baseVal()->getItem(0)->value() : 0;

		SVGMatrixImpl *tempMatrix = SVGSVGElementImpl::createSVGMatrix();
		tempMatrix->translate(_curx, _cury[i]);

		text = textChunk->getText(i);
		if(i != textChunk->count() - 1)
			text += QChar(' ');

		if(!canvas->fontContext()->ready())
			canvas->fontContext()->init();

		font = canvas->fontContext()->requestFont(canvas->fontVisualParams(tc));

		if(!font)
			break;

		double addLetterSpacing = 0;

		// Apply affine corrections, through lengthAdjust + textLength
		if(tp && tp->textLength()->baseVal()->value() != -1)
		{
			// #1 Measure text
			SVGTextElementImpl *textElement = dynamic_cast<SVGTextElementImpl *>(tp);
			const SVGMatrixImpl *ctm = textElement->screenCTM();

			T2P::Affine affine;
			{
				SVGMatrixImpl *temp = SVGSVGElementImpl::createSVGMatrix();

				temp->multiply(ctm);
				temp->translate(_curx, _cury[0]);

				KSVGHelper::matrixToAffine(temp, affine);

				temp->deref();
			}

			T2P::GlyphSet *measure = canvas->fontContext()->calcString(font.get(), text.ucs2(), text.length(), affine, params, bpath);
		
			// Free bpath's
			measure->set().clear();

			// #2 Calculate textLength
			double textLength = tp->textLength()->baseVal()->value();

			// #3 Apply the spacing
			if(tp->lengthAdjust()->baseVal() == LENGTHADJUST_SPACINGANDGLYPHS)
				tempMatrix->scaleNonUniform((textLength * ctm->a()) / measure->width(), 1);
			else if(tp->lengthAdjust()->baseVal() == LENGTHADJUST_SPACING)
				addLetterSpacing = ((textLength - (measure->width() / ctm->a())) / text.length());

			// #4 cleanup
			delete measure;
		}

		{
			T2P::GlyphLayoutParams *params = tc->layoutParams();
			params->setLetterSpacing(params->letterSpacing() + addLetterSpacing);
			if(bpath)
			{
				params->setTextPathStartOffset(pathAdvance);
				if(tp && tp->dy()->baseVal()->numberOfItems() > 0)
					pathDy += tp->dy()->baseVal()->getItem(0)->value();
				QString shift = QString("%1%%").arg((pathDy / font->fontParams()->size()) * -100.0);
				params->setBaselineShift(shift.latin1());
			}

			T2P::Affine affine;
			KSVGHelper::matrixToAffine(tempMatrix, affine);
			tempMatrix->deref();

			T2P::GlyphSet *glyph = canvas->fontContext()->calcString(font.get(), text.ucs2(), text.length(), affine, params, bpath);
			if(bpath)
				pathAdvance += double(glyph->width()) / pathLength;
			_curx += (params->tb() ? 0 : glyph->xpen());
			_cury.resize(i + 2);
			_cury[i + 1] = _cury[i] + (params->tb() ? glyph->ypen() : 0);
			if(!glyph)
				break;
			else
				glyphs.append(glyph);

			delete params;
		}
	}

	// Calculate text-anchor
	double anchor = 0;

	// anchor == "start" is the default here (Rob)
	if(tc->getTextAnchor() == TAMIDDLE)
	{
		if(!params->tb())
			anchor = ((_curx - curx) + 1) / 2;
		else
			anchor = ((_cury[textChunk->count()] - cury) + 1) / 2;
	}
	else if(tc->getTextAnchor() == TAEND)
	{
		if(!params->tb())
			anchor = (_curx - curx);
		else
			anchor = (_cury[textChunk->count()] - cury);
	}

	// Render all glyphs of the text chunk
	// Take first glyphset
	T2P::GlyphSet *glyph = glyphs.at(0);
	if(!glyph)
		return;

	// Draw 'text-decoration'
	// TODO: Currently just ignore text-decoration on vertical layouts, is that correct?
	// Underline and overline have to be drawn before the glyphs are rendered
	if(tc0->getTextDecoration() & UNDERLINE && !params->tb())
		addTextDecoration(tc0, (curx - anchor), (cury + (glyph->underlinePosition() - glyph->pixelBaseline())),
		_curx - curx, glyph->underlineThickness());
	if(tc0->getTextDecoration() & OVERLINE && !params->tb())
		addTextDecoration(tc0, (curx - anchor), (cury + (glyph->overlinePosition() - glyph->pixelBaseline())),
		_curx - curx, glyph->underlineThickness());

	for(unsigned int j = 0; j < glyphs.count(); j++)
	{
		glyph = glyphs.at(j);
		SVGTextContentElementImpl *style = textChunk->getTextContentElement(j);

		// Draw 'text-decoration'
		// TODO: Currently just ignore text-decoration on vertical layouts, is that correct?
		// Underline and overline have to be drawn before the glyphs are rendered
		if(style->getAttribute("text-decoration") == "underline" && !params->tb())
			addTextDecoration(style, glyph->bboxX() - anchor, (cury + (glyph->underlinePosition() - glyph->pixelBaseline())),
						glyph->width(), glyph->underlineThickness());
		else if(style->getAttribute("text-decoration") == "overline"  && !params->tb())
			addTextDecoration(style, glyph->bboxX() - anchor, (cury + (glyph->overlinePosition() - glyph->pixelBaseline())),
						glyph->width(), glyph->underlineThickness());

		renderCallback(style, screenCTM, glyph, params, anchor);

		// Clear GlyphAffinePair's
		for(std::vector<T2P::GlyphAffinePair *>::iterator it = glyph->set().begin(); it != glyph->set().end(); ++it)
		{
			T2P::GlyphAffinePair *glyphAffine = *it;
			delete glyphAffine;
		}

		glyph->set().clear();

		// Draw 'line-through' text decoration
		// Line-through has to be drawn after the glyphs are rendered
		if(style->getAttribute("text-decoration") == "line-through" && !params->tb())
			addTextDecoration(style, glyph->bboxX() - anchor, (cury + (glyph->strikeThroughPosition() - glyph->pixelBaseline())), glyph->width(), glyph->underlineThickness());

	}

	endx = glyph->bboxX() + glyph->width();
	endy = int(_cury[glyphs.count() - 1]);

	// Draw 'line-through' text decoration
	// Line-through has to be drawn after the glyphs are rendered
	if(tc0->getTextDecoration() & LINE_THROUGH && !params->tb())
		addTextDecoration(tc0, (curx - anchor), (cury + (glyph->strikeThroughPosition() - glyph->pixelBaseline())), _curx - curx, glyph->underlineThickness());

	delete params;
}

// #####

void MarkerHelper::doMarker(SVGShapeImpl *shape, SVGStylableImpl *style, double x, double y, double angle, const QString &markerId)
{
	SVGMarkerElementImpl *marker = dynamic_cast<SVGMarkerElementImpl *>(shape->ownerSVGElement()->getElementById(markerId));
	if(marker)
		marker->draw(shape, x, y, style->getStrokeWidth()->baseVal()->value(), angle);
}

void MarkerHelper::doStartMarker(SVGShapeImpl *shape, SVGStylableImpl *style, double x, double y, double angle)
{
	doMarker(shape, style, x, y, angle, style->getStartMarker());
}

void MarkerHelper::doMidMarker(SVGShapeImpl *shape, SVGStylableImpl *style, double x, double y, double angle)
{
	doMarker(shape, style, x, y, angle, style->getMidMarker());
}

void MarkerHelper::doEndMarker(SVGShapeImpl *shape, SVGStylableImpl *style, double x, double y, double angle)
{
	doMarker(shape, style, x, y, angle, style->getEndMarker());
}

// vim:ts=4:noet
