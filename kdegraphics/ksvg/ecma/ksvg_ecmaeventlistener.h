/*
    Copyright (C) 2002-2003 KSVG Team
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

#ifndef KSVGEcmaEventListener_H
#define KSVGEcmaEventListener_H

#include "SVGEventImpl.h"

namespace KJS
{
	class Object;
}

class KSVGEcmaEventListener : public KSVG::SVGEventListener
{
public:
	KSVGEcmaEventListener(KJS::Object _listener, QString _type, KSVGEcma *_ecma);
	virtual ~KSVGEcmaEventListener();

	virtual void handleEvent(KSVG::SVGEventImpl *evt);
	virtual DOM::DOMString eventListenerType();

	QString type() { return m_type; }

	void forbidRemove();
	
private:
	KSVGEcma *m_ecma;
	QString m_type;
	bool m_remove;

	KJS::Object m_listener;
};

#endif

// vim:ts=4:noet
