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

#ifndef SVGEcma_H
#define SVGEcma_H

#include <dom/dom_doc.h>
#include <dom/dom_node.h>
#include <dom/dom_text.h>
#include <dom/dom_element.h>

#include "ksvg_lookup.h"

namespace KSVG
{

// Access DOM::Node via js
class SVGDOMNodeBridge
{
public:
	SVGDOMNodeBridge(DOM::Node impl) : m_impl(impl) { KSVG_EMPTY_FLAGS }
	virtual ~SVGDOMNodeBridge() { }

	DOM::Node impl() const { return m_impl; }

	KSVG_BASECLASS_GET
	KSVG_PUT

	enum
	{
		// Properties
		NodeName, NodeValue, NodeType, ParentNode,
		ChildNodes, FirstChild, LastChild, PreviousSibling,
		NextSibling, Attributes, NamespaceURI, Prefix,
		LocalName, OwnerDocument,
		// Functions
		InsertBefore, ReplaceChild, RemoveChild, AppendChild,
		HasAttributes, HasChildNodes, CloneNode, Normalize,
		IsSupported, AddEventListener, RemoveEventListener, Contains,
		GetNodeName, GetNodeValue, GetNodeType, GetParentNode,
		GetChildNodes, GetFirstChild, GetLastChild, GetPreviousSibling,
		GetNextSibling, GetAttributes, GetNamespaceURI, GetPrefix,
		GetLocalName, GetOwnerDocument
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);

private:
	DOM::Node m_impl;
};

// Access DOM::Element via js
class SVGDOMElementBridge : public SVGDOMNodeBridge
{
public:
	SVGDOMElementBridge(DOM::Element impl) : SVGDOMNodeBridge(static_cast<DOM::Node>(impl)), m_impl(impl) { }
	~SVGDOMElementBridge() { }

	DOM::Element impl() const { return m_impl; }

	KSVG_GET
	KSVG_FORWARDPUT

	enum
	{
		// Properties
		TagName,
		// Functions
		GetAttribute, SetAttribute, RemoveAttribute, GetAttributeNode,
		SetAttributeNode, RemoveAttributeNode, GetElementsByTagName,
		GetAttributeNS, SetAttributeNS, RemoveAttributeNS, GetAttributeNodeNS,
		SetAttributeNodeNS, GetElementsByTagNameNS, HasAttribute, HasAttributeNS
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;

private:
	DOM::Element m_impl;
};

// Access DOM::NodeList via js
class SVGDOMNodeListBridge
{
public:
	SVGDOMNodeListBridge(DOM::NodeList impl) : m_impl(impl) { }
	~SVGDOMNodeListBridge() { }

	DOM::NodeList impl() const { return m_impl; }

	KSVG_GET

	enum
	{
		// Properties
		Length,
		// Functions
		GetLength, Item
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;

private:
	DOM::NodeList m_impl;
};

// Access DOM::CharacterData via js
class SVGDOMCharacterDataBridge : public SVGDOMNodeBridge
{
public:
	SVGDOMCharacterDataBridge(DOM::CharacterData impl) : SVGDOMNodeBridge(static_cast<DOM::Node>(impl)), m_impl(impl) { KSVG_EMPTY_FLAGS }
	~SVGDOMCharacterDataBridge() { }

	DOM::CharacterData impl() const { return m_impl; }

	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		Data, Length,
		// Functions
		GetData, SetData, GetLength,
		SubstringData, AppendData, InsertData, DeleteData, ReplaceData
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);

private:
	DOM::CharacterData m_impl;
};

// Access DOM::Text via js
class SVGDOMTextBridge : public SVGDOMCharacterDataBridge
{
public:
	SVGDOMTextBridge(DOM::Text impl) : SVGDOMCharacterDataBridge(static_cast<DOM::CharacterData>(impl)), m_impl(impl) { }
	~SVGDOMTextBridge() { }

	DOM::Text impl() const { return m_impl; }

	KSVG_GET
	KSVG_FORWARDPUT

	enum
	{
		// Properties
		Dummy,
		// Functions
		SplitText
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;

private:
	DOM::Text m_impl;
};

// Access DOM::DOMImplementation via js
class SVGDOMDOMImplementationBridge
{
public:
	SVGDOMDOMImplementationBridge(DOM::DOMImplementation impl) : m_impl(impl) { }
	~SVGDOMDOMImplementationBridge() { }

	DOM::DOMImplementation impl() const { return m_impl; }

	KSVG_GET
	KSVG_FORWARDPUT

	enum
	{
		// Properties
		Dummy,
		// Functions
		HasFeature
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;

private:
	DOM::DOMImplementation m_impl;
};

// Access DOM::DocumentFragment via js
class SVGDOMDocumentFragmentBridge : public SVGDOMNodeBridge
{
public:
	SVGDOMDocumentFragmentBridge(DOM::DocumentFragment impl) : SVGDOMNodeBridge(static_cast<DOM::Node>(impl)), m_impl(impl) { }
	~SVGDOMDocumentFragmentBridge() { }

	DOM::DocumentFragment impl() const { return m_impl; }

	KSVG_GET
	KSVG_FORWARDPUT

	enum
	{
		// Properties
		Dummy
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;

private:
	DOM::DocumentFragment m_impl;
};

}

KSVG_DEFINE_PROTOTYPE(SVGDOMNodeBridgeProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGDOMNodeBridgeProtoFunc, SVGDOMNodeBridge)

KSVG_DEFINE_PROTOTYPE(SVGDOMElementBridgeProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGDOMElementBridgeProtoFunc, SVGDOMElementBridge)

KSVG_DEFINE_PROTOTYPE(SVGDOMNodeListBridgeProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGDOMNodeListBridgeProtoFunc, SVGDOMNodeListBridge)

KSVG_DEFINE_PROTOTYPE(SVGDOMCharacterDataBridgeProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGDOMCharacterDataBridgeProtoFunc, SVGDOMCharacterDataBridge)

KSVG_DEFINE_PROTOTYPE(SVGDOMTextBridgeProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGDOMTextBridgeProtoFunc, SVGDOMTextBridge)

KSVG_DEFINE_PROTOTYPE(SVGDOMDOMImplementationBridgeProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGDOMDOMImplementationBridgeProtoFunc, SVGDOMDOMImplementationBridge)

#endif

// vim:ts=4:noet
