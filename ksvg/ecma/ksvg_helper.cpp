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

#include <kjs/ustring.h>
#include <kjs/identifier.h>

#include <dom/dom_string.h>

KJS::UString::UString(const DOM::DOMString &d)
{
	if(d.isNull())
	{
		attach(&Rep::null);
		return;
	}

	unsigned int len = d.length();
	KJS::UChar *dat = new UChar[len];
	memcpy(dat, d.unicode(), len * sizeof(UChar));
	rep = KJS::UString::Rep::create(dat, len);
}

KJS::UString::UString(const QString &d)
{
	unsigned int len = d.length();
	KJS::UChar *dat = new UChar[len];
	memcpy(dat, d.unicode(), len * sizeof(UChar));
	rep = KJS::UString::Rep::create(dat, len);
}

QString KJS::UString::qstring() const
{
	return QString(reinterpret_cast<QChar *>(const_cast<KJS::UChar *>(data())), size());
}

DOM::DOMString KJS::UString::string() const
{
	return DOM::DOMString(reinterpret_cast<QChar *>(const_cast<KJS::UChar *>(data())), size());
}

DOM::DOMString KJS::Identifier::string() const
{
	return DOM::DOMString((QChar*) data(), size());
}

QString KJS::Identifier::qstring() const
{
	return QString((QChar*) data(), size());
}

// vim:ts=4:noet
