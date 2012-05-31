/*
    Copyright (C) 2003 KSVG Team
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

#ifndef DocumentFactory_H
#define DocumentFactory_H

#include <kurl.h>
#include <qobject.h>

namespace KSVG
{

class KSVGCanvas;
class SVGDocument;
class SVGDocumentImpl;
class DocumentFactory
{
public:
	DocumentFactory();
	~DocumentFactory();

	static DocumentFactory *self();

	// Creates a document and connects the parsingFinished() signal to the notifySlot...
	SVGDocument *requestDocument(QObject *notifyObject, const char *notifySlot) const;

	// Loads 'url' and emits parsingFinisihed() signal, when done
	bool startParsing(SVGDocument *document, const KURL &url);

	// Attaches the a canvas to the document, that is ksvg specific code
	bool attachCanvas(KSVGCanvas *canvas, SVGDocument *document);

	// Internal use only - external coders don't have the
	// possibility to use SVGDocumentImpl anyway
	SVGDocumentImpl *requestDocumentImpl(bool bFit) const;
	
private:
	class Private;
	Private *m_d;
};

}

#endif

// vim:ts=4:noet
