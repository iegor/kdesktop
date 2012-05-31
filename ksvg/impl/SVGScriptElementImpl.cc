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

#include <kio/job.h>
#include <kfilterdev.h>
#include <qbuffer.h>

#include "SVGDocumentImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGScriptElementImpl.moc"

using namespace KSVG;

#include "SVGScriptElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGScriptElementImpl::SVGScriptElementImpl(DOM::ElementImpl *impl) : QObject(), SVGElementImpl(impl), SVGURIReferenceImpl(), SVGExternalResourcesRequiredImpl()
{
	KSVG_EMPTY_FLAGS

	m_job = 0;
	m_added = false;
}

SVGScriptElementImpl::~SVGScriptElementImpl()
{
}

void SVGScriptElementImpl::setType(const DOM::DOMString &type)
{
	setAttribute("type", type);
}

DOM::DOMString SVGScriptElementImpl::type() const
{
	return getAttribute("type");
}

void SVGScriptElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: provide a default type
	if(KSVG_TOKEN_NOT_PARSED(Type))
		KSVG_SET_ALT_ATTRIBUTE(Type, "text/ecmascript")

	// Remote downloading
	QString href = m_href->baseVal().string();

	if(!href.isEmpty())
	{
		KURL url(ownerDoc()->baseUrl(), href);

		if(m_job == 0)
			m_job = KIO::get(url, false, false);

		connect(m_job, SIGNAL(data(KIO::Job *, const QByteArray &)), this, SLOT(slotData(KIO::Job *, const QByteArray &)));
		connect(m_job, SIGNAL(result(KIO::Job *)), this, SLOT(slotResult(KIO::Job *)));
	}
}

void SVGScriptElementImpl::slotData(KIO::Job *, const QByteArray &data)
{
	QDataStream dataStream(m_data, IO_WriteOnly | IO_Append);
	dataStream.writeRawBytes(data.data(), data.size());
}

void SVGScriptElementImpl::slotResult(KIO::Job *)
{
	m_job = 0;

	// Append a NULL terminator so we don't die
	m_data.resize(m_data.size() + 1);
	m_data[m_data.size() - 1] = '\0';

	QBuffer buf(m_data);
	QIODevice *dev = KFilterDev::device(&buf, "application/x-gzip", false);
	QByteArray contents;
	if(dev->open(IO_ReadOnly))
		contents = dev->readAll();
	delete dev;
	m_text = QString::fromUtf8(contents.data());

	m_data.resize(0);
}

bool SVGScriptElementImpl::canExecuteScript()
{
	if(!m_added)
	{
		m_added = true;
		m_text += collectText();
	}

	if(m_text.isEmpty() && !getAttribute("href").isNull() && !getAttribute("href").string().isEmpty())
		return false;

	return true;
}

bool SVGScriptElementImpl::executeScript(DOM::Node node)
{
	return SVGScriptElementImpl::executeScript(node, ownerDoc(), m_text);
}

bool SVGScriptElementImpl::executeScript(DOM::Node node, SVGDocumentImpl *document, const QString &text)
{
#ifdef KJS_VERBOSE
    kdDebug(6070) << "SVGScriptElementImpl::executeScript n=" << node.nodeName().string().latin1() << "(" << (node.isNull() ? 0 : node.nodeType()) << ") " << text << endl;
#endif
	KSVGEcma *ecmaEngine = document->ecmaEngine();

	if(!ecmaEngine->initialized())
		ecmaEngine->setup();

	KJS::Value thisNode = node.isNull() ? ecmaEngine->globalObject() : getDOMNode(ecmaEngine->globalExec(), node);

	KJS::UString code(text);
	KJS::Completion comp = ecmaEngine->evaluate(code, thisNode);

	// TODO: If that's needed find a better solution which 
	// doesn't cause endless loops if the func, specified in
	// onerror="..." isn't yet available
	// onerror support
//	if(comp.complType() == Throw)
//		document->rootElement()->dispatchEvent(SVGEventImpl::ERROR_EVENT, false, false);		

	return (comp.complType() == KJS::Normal) || (comp.complType() == KJS::ReturnValue);
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGScriptElementImpl::s_hashTable 2
 type	SVGScriptElementImpl::Type  DontDelete
@end
*/

Value SVGScriptElementImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case Type:
			return String(type());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGScriptElementImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case Type:
			setType(value.toString(exec).string());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
