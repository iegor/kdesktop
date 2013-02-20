/* 
   Copyright (c) 2001 Malte Starostik <malte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// $Id: svgcreator.cpp 719657 2007-10-01 15:47:33Z lunakl $

#include <time.h>

#include <qpixmap.h>
#include <qimage.h>

#include "KSVGCanvas.h"
#include "CanvasFactory.h"

#include <kapplication.h>

#include "SVGSVGElementImpl.h"
#include "SVGDocumentImpl.h"

#include "svgcreator.moc"

extern "C"
{
    ThumbCreator *new_creator()
    {
        return new SVGCreator;
    }
}

SVGCreator::SVGCreator()
{
}

SVGCreator::~SVGCreator()
{
}

bool SVGCreator::create(const QString &path, int width, int height, QImage &img)
{
	KSVG::SVGDocumentImpl *doc = new KSVG::SVGDocumentImpl(false, true);
	doc->ref();
       
	QPixmap pix(width, height);
	pix.fill(Qt::white);

	KSVG::KSVGCanvas *c = KSVG::CanvasFactory::self()->loadCanvas(width, height);
	c->setup(&pix, &pix);

	doc->attach(c);
	connect(doc, SIGNAL(finishedRendering()), SLOT(slotFinished()));
	doc->open( KURL::fromPathOrURL(path));

	m_finished = false;

	while(!m_finished)
		kapp->processOneEvent();

	doc->deref();

	img = pix.convertToImage();

	delete c;

	return true;
}

void SVGCreator::slotFinished()
{
	m_finished = true;
}

ThumbCreator::Flags SVGCreator::flags() const
{
	return DrawFrame;
}

// vim: ts=4 sw=4 noet
