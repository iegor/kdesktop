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

#ifndef SVGScriptElementImpl_H
#define SVGScriptElementImpl_H

#include <dom/dom_string.h>

#include <kio/jobclasses.h>

#include <qobject.h>

#include "SVGElementImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGScriptElementImpl : public QObject,
							 public SVGElementImpl,
							 public SVGURIReferenceImpl,
							 public SVGExternalResourcesRequiredImpl
{
Q_OBJECT
public:
	SVGScriptElementImpl(DOM::ElementImpl *);
	virtual ~SVGScriptElementImpl();

	void setType(const DOM::DOMString &type);
	DOM::DOMString type() const;

	virtual void setAttributes();

	bool canExecuteScript();
	bool executeScript(DOM::Node node);

	static bool executeScript(DOM::Node node, SVGDocumentImpl *document, const QString &text);

private slots:
	void slotData(KIO::Job *, const QByteArray &);
	void slotResult(KIO::Job *);

private:
	KIO::TransferJob *m_job;
	QByteArray m_data;
	QString m_text;
	bool m_added;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		Type
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGScriptElementImpl, "script")

}

#endif

// vim:ts=4:noet
