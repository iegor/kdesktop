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

#include <qobject.h>

#include <kdebug.h>
#include <kstaticdeleter.h>

#include "SVGDocument.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "DocumentFactory.h"

using namespace KSVG;

namespace KSVG
{
	class DocumentFactory::Private
	{
	public:
		Private() { m_docs.setAutoDelete(true); }
		~Private() { m_docs.clear(); }
	
		void setup(bool bFit) { m_docs.append(new SVGDocumentImpl(false /* animations */, bFit)); }
		SVGDocumentImpl *doc() const { return m_docs.current(); }

	private:
		QPtrList<SVGDocumentImpl> m_docs;
	};
}

static KStaticDeleter<DocumentFactory> s_deleter;
static DocumentFactory *s_factory = 0;

DocumentFactory::DocumentFactory() : m_d(new Private())
{
}

DocumentFactory::~DocumentFactory()
{
	delete m_d;
}

DocumentFactory *DocumentFactory::self()
{
	if(!s_factory)
		s_deleter.setObject(s_factory, new DocumentFactory());
	return s_factory;
}

SVGDocument *DocumentFactory::requestDocument(QObject *notifyObject, const char *notifySlot) const
{
	SVGDocumentImpl *impl = requestDocumentImpl(false);
	QObject::connect(impl, SIGNAL(finishedParsing(bool, const QString &)), notifyObject, notifySlot);
	
	return new SVGDocument(impl);
}

bool DocumentFactory::startParsing(SVGDocument *document, const KURL &url)
{
	if(!document || !document->handle())
		return false;

	return reinterpret_cast<SVGDocumentImpl *>(document->handle())->open(url);
}

bool DocumentFactory::attachCanvas(KSVGCanvas *canvas, SVGDocument *document)
{
	if(!canvas || !document || !document->handle())
		return false;
		
	SVGDocumentImpl *docImpl = reinterpret_cast<SVGDocumentImpl *>(document->handle());

	if(docImpl)
	{
		docImpl->attach(canvas);
		return true;
	}

	return false;
}	

SVGDocumentImpl *DocumentFactory::requestDocumentImpl(bool bFit) const
{
	m_d->setup(bFit);

	SVGDocumentImpl *impl = m_d->doc();
	impl->ref();
		
	return impl;
}

// vim:ts=4:noet
