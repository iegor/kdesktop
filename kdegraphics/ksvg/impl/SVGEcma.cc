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

#include <typeinfo>

#include <kdebug.h>

#include <dom/dom_exception.h>

#include "CanvasItem.h"
#include "KSVGCanvas.h"

#include "SVGEcma.h"
#include "SVGShapeImpl.h"
#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"

using namespace KSVG;

#include "SVGEcma.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_ecmaeventlistener.h"
#include "ksvg_window.h"
#include "ksvg_ecma.h"

// SVGDOMNodeBridge

/*
@namespace KSVG
@begin SVGDOMNodeBridge::s_hashTable 17
 nodeName			SVGDOMNodeBridge::NodeName			DontDelete|ReadOnly
 nodeValue			SVGDOMNodeBridge::NodeValue			DontDelete
 nodeType			SVGDOMNodeBridge::NodeType			DontDelete|ReadOnly
 parentNode			SVGDOMNodeBridge::ParentNode		DontDelete|ReadOnly
 childNodes			SVGDOMNodeBridge::ChildNodes		DontDelete|ReadOnly
 firstChild			SVGDOMNodeBridge::FirstChild		DontDelete|ReadOnly
 lastChild			SVGDOMNodeBridge::LastChild			DontDelete|ReadOnly
 previousSibling	SVGDOMNodeBridge::PreviousSibling	DontDelete|ReadOnly
 nextSibling		SVGDOMNodeBridge::NextSibling		DontDelete|ReadOnly
 attributes			SVGDOMNodeBridge::Attributes		DontDelete|ReadOnly
 namespaceURI		SVGDOMNodeBridge::NamespaceURI		DontDelete|ReadOnly
 prefix				SVGDOMNodeBridge::Prefix			DontDelete
 localName			SVGDOMNodeBridge::LocalName			DontDelete|ReadOnly
 ownerDocument		SVGDOMNodeBridge::OwnerDocument		DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGDOMNodeBridgeProto::s_hashTable 29
 insertBefore			SVGDOMNodeBridge::InsertBefore			DontDelete|Function 2
 replaceChild			SVGDOMNodeBridge::ReplaceChild			DontDelete|Function 2
 removeChild			SVGDOMNodeBridge::RemoveChild			DontDelete|Function 1
 appendChild			SVGDOMNodeBridge::AppendChild			DontDelete|Function 1
 hasAttributes			SVGDOMNodeBridge::HasAttributes			DontDelete|Function 0
 hasChildNodes			SVGDOMNodeBridge::HasChildNodes			DontDelete|Function 0
 cloneNode				SVGDOMNodeBridge::CloneNode				DontDelete|Function 1
 normalize				SVGDOMNodeBridge::Normalize				DontDelete|Function 0
 isSupported			SVGDOMNodeBridge::IsSupported			DontDelete|Function 2
 addEventListener		SVGDOMNodeBridge::AddEventListener		DontDelete|Function 3
 removeEventListener	SVGDOMNodeBridge::RemoveEventListener	DontDelete|Function 3
 contains				SVGDOMNodeBridge::Contains				DontDelete|Function 1
 getNodeName			SVGDOMNodeBridge::GetNodeName			DontDelete|Function 0
 getNodeValue			SVGDOMNodeBridge::GetNodeValue			DontDelete|Function 0
 getNodeType			SVGDOMNodeBridge::GetNodeType			DontDelete|Function 0
 getParentNode			SVGDOMNodeBridge::GetParentNode			DontDelete|Function 0
 getChildNodes			SVGDOMNodeBridge::GetChildNodes			DontDelete|Function 0
 getFirstChild			SVGDOMNodeBridge::GetFirstChild			DontDelete|Function 0
 getLastChild			SVGDOMNodeBridge::GetLastChild			DontDelete|Function 0
 getPreviousSibling		SVGDOMNodeBridge::GetPreviousSibling	DontDelete|Function 0
 getNextSibling			SVGDOMNodeBridge::GetNextSibling		DontDelete|Function 0
 getAttributes			SVGDOMNodeBridge::GetAttributes			DontDelete|Function 0
 getNamespaceURI		SVGDOMNodeBridge::GetNamespaceURI		DontDelete|Function 0
 getPrefix				SVGDOMNodeBridge::GetPrefix				DontDelete|Function 0
 getLocalName			SVGDOMNodeBridge::GetLocalName			DontDelete|Function 0
 getOwnerDocument		SVGDOMNodeBridge::GetOwnerDocument		DontDelete|Function 0
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("DOMNode", SVGDOMNodeBridgeProto, SVGDOMNodeBridgeProtoFunc)

Value SVGDOMNodeBridge::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case NodeName:
			return getString(m_impl.nodeName());
		case NodeValue:
			return getString(m_impl.nodeValue());
		case NodeType:
			return Number(m_impl.nodeType());
		case ParentNode:
			return getDOMNode(exec, m_impl.parentNode());
		case ChildNodes:
			return (new SVGDOMNodeListBridge(m_impl.childNodes()))->cache(exec);
		case FirstChild:
			return getDOMNode(exec, m_impl.firstChild());
		case LastChild:
			return getDOMNode(exec, m_impl.lastChild());
		case PreviousSibling:
			return getDOMNode(exec, m_impl.previousSibling());
		case NextSibling:
			return getDOMNode(exec, m_impl.nextSibling());
//		case Attributes: // TODO
		case NamespaceURI:
			return getString(m_impl.namespaceURI());
		case Prefix:
			return getString(m_impl.prefix());
		case LocalName:
			return getString(m_impl.localName());
		case OwnerDocument:
			return getDOMNode(exec, m_impl.ownerDocument());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGDOMNodeBridge::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case NodeValue:
			m_impl.setNodeValue(value.toString(exec).string());
			break;
		case Prefix:
			m_impl.setPrefix(value.toString(exec).string());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// Special variantion to update the <text> element,
// triggered by one of the child nodes
void updateTextItem(ExecState *exec, const DOM::Node node)
{
	DOM::Node parent;
	while(!(parent = node.parentNode()).isNull())
	{
		DOM::DOMString name = parent.nodeName();
		if(name == "text" || name == "tspan" || name == "tref")
		{
			SVGHelperImpl::updateItem(exec, parent);
			break;
		}
	}
}

// Remove item from canvas
void removeItem(ExecState *exec, DOM::Node &node)
{
	// Get document
	SVGDocumentImpl *doc = KSVG::Window::retrieveActive(exec)->doc();

	// Update canvas
	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(doc->getElementFromHandle(node.handle()));
	if(shape && shape->item())
		doc->canvas()->removeItem(shape->item());
}

// parseXML + getURL() need all these 5 functions to work properly
void correctHandles(SVGElementImpl *main, DOM::Node &node)
{
	DOM::Element old(node.handle());
	DOM::Element *replace = static_cast<DOM::Element *>(main->ownerDoc()->getElementFromHandle(node.handle()));

	if(replace && node.nodeType() == DOM::Node::ELEMENT_NODE)
		*replace = old;

	if(node.hasChildNodes())
	{
		for(DOM::Node iterate = node.firstChild(); !iterate.isNull(); iterate = iterate.nextSibling())
			correctHandles(main, iterate);
	}
}
						
void integrateTree(SVGElementImpl *main, DOM::Node &node, DOM::Node &newNode, SVGElementImpl *obj, SVGDocumentImpl *doc)
{
	if(!obj)
		return;

	// Add to global element dicts
	doc->addToElemDict(newNode.handle(), obj);
	doc->addToElemDict(node.handle(), obj);

	if(node.hasChildNodes())
	{
		DOM::Node iterate2 = newNode.firstChild();
		for(DOM::Node iterate = node.firstChild(); !iterate.isNull(); iterate = iterate.nextSibling())
		{
			integrateTree(main, iterate, iterate2, obj->ownerDoc()->getElementFromHandle(iterate2.handle()), doc);
			iterate2 = iterate2.nextSibling();
		}
	}
}

void correctDocument(SVGElementImpl *main, DOM::Node &node, SVGElementImpl *obj, SVGDocumentImpl *doc)
{
	if(!obj)
		return;

	// Correct document
	obj->setOwnerDoc(main->ownerDoc());
	
	// Correct rootElement
	if(!obj->ownerSVGElement())
		obj->setOwnerSVGElement(main->ownerSVGElement());

	// Correct viewportElement
	if(!obj->viewportElement())
		obj->setViewportElement(main->viewportElement());

	// Properly (re-)register events in the current active document
	obj->setupEventListeners(main->ownerDoc(), doc);

	if(node.hasChildNodes())
	{
		for(DOM::Node iterate = node.firstChild(); !iterate.isNull(); iterate = iterate.nextSibling())
			correctDocument(main, iterate, doc->getElementFromHandle(iterate.handle()), doc);	
	}
}

void registerAdditional(ExecState *exec, SVGDocumentImpl *doc, DOM::Node node)
{
	// Register ID in rootElement!
	SVGElementImpl *resultElement = doc->getElementFromHandle(node.handle());
	if(resultElement && resultElement->hasAttribute("id"))
		doc->rootElement()->addToIdMap(resultElement->getAttribute("id").string(), resultElement);

	if(node.hasChildNodes())
	{
		for(DOM::Node iterate = node.firstChild(); !iterate.isNull(); iterate = iterate.nextSibling())
			registerAdditional(exec, doc, iterate);
	}
}

Value appendHelper(ExecState *exec, DOM::Node node, DOM::Node newNode)
{
	// This is quite tricky code by me (Niko)
	// Don't even try to modify it.
	if(!(node.ownerDocument() == newNode.ownerDocument()))
	{
		// Get document
		SVGDocumentImpl *doc = KSVG::Window::retrieveActive(exec)->doc();

		// Detect ownerDoc() of newNode
		SVGDocumentImpl *newDoc = doc->getDocumentFromHandle(newNode.ownerDocument().handle());

		// Get some SVGElementImpl's
		SVGElementImpl *nodeElement = doc->getElementFromHandle(node.handle());
		SVGElementImpl *newNodeElement = newDoc->getElementFromHandle(newNode.handle());

		// Import node into document
		DOM::Node result = doc->importNode(newNode, true);

		// Associate the imported node 'result' with the
		// 'newNodeElement' which belongs to 'newDoc'
		integrateTree(nodeElement, result, newNode, newNodeElement, doc);

		// Correct handles in SVG* elements
		correctHandles(nodeElement, result);

		// Correct ownerDoc() etc..
		correctDocument(nodeElement, newNode, newNodeElement, newDoc);

		// Register ID in global map
		registerAdditional(exec, doc, result);

		// Recalc style
		newNodeElement->setAttributes();

		// Append + create + update element
		Value retVal = getDOMNode(exec, node.appendChild(result));

		doc->syncCachedMatrices();
		newNodeElement->createItem(doc->canvas());
		SVGHelperImpl::updateItem(exec, *newNodeElement);

		return retVal;
	}
	else
	{
		Value retVal = getDOMNode(exec, node.appendChild(newNode));

		// Get document
		SVGDocumentImpl *doc = KSVG::Window::retrieveActive(exec)->doc();
		doc->syncCachedMatrices();

		// Get some SVGElementImpl's
		SVGElementImpl *nodeElement = doc->getElementFromHandle(newNode.handle());
		// TODO : extra check needed to see if the new elements parent is already appended
		// in the doc. Not really nice, should be some other way? (Rob)
		if(nodeElement && !nodeElement->parentNode().parentNode().isNull())
		{
			nodeElement->setAttributes(true);
			nodeElement->createItem();
			SVGHelperImpl::updateItem(exec, newNode);	
		}

		return retVal;
	}
}

Value SVGDOMNodeBridgeProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGDOMNodeBridge)
	DOM::Node node = obj->impl();

	switch(id)
	{
		case SVGDOMNodeBridge::InsertBefore:
		{
			DOM::Node newChild = toNode(args[0]);
			DOM::Node beforeChild = toNode(args[1]);
			Value retVal = getDOMNode(exec, node.insertBefore(newChild, beforeChild));
			
			// Get document
			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();

			// Get some SVGElementImpl's
			SVGShapeImpl *newShape = dynamic_cast<SVGShapeImpl *>(doc->getElementFromHandle(newChild.handle()));
			SVGElementImpl *newElement = doc->getElementFromHandle(newChild.handle());
			SVGShapeImpl *beforeElement = dynamic_cast<SVGShapeImpl *>(doc->getElementFromHandle(beforeChild.handle()));
			if(newShape && beforeElement && beforeElement->item())
			{
				int z = beforeElement->item()->zIndex();
				newElement->createItem();
				doc->canvas()->insert(newShape->item(), z);
			}
			SVGHelperImpl::updateItem(exec, newChild);

			return retVal;
		}
		case SVGDOMNodeBridge::ReplaceChild:
		{
			DOM::Node newChild = toNode(args[0]);
			Value retVal = getDOMNode(exec, node.replaceChild(newChild, toNode(args[1])));
			SVGHelperImpl::updateItem(exec, newChild);
			return retVal;
		}
		case SVGDOMNodeBridge::RemoveChild:
		{
			DOM::Node remove = toNode(args[0]);
			if(remove.isNull())
				return Undefined();

			// New removeChild logic:
			// - remove from DOM tree
			// - delete element (also deletes it's child element's)
			removeItem(exec, remove);
			Value retVal = getDOMNode(exec, node.removeChild(remove));
			return retVal;
		}
		case SVGDOMNodeBridge::AppendChild:
			return appendHelper(exec, node, toNode(args[0]));
		case SVGDOMNodeBridge::HasAttributes:
		{
			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();
			SVGElementImpl *element = doc->getElementFromHandle(node.handle());

			if(!element)
				return Undefined();

			return Boolean(element->hasAttributes());
		}
		case SVGDOMNodeBridge::HasChildNodes:
			return Boolean(node.hasChildNodes());
		case SVGDOMNodeBridge::CloneNode:
		{
			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();
			SVGElementImpl *element = doc->getElementFromHandle(node.handle());
			SVGElementImpl *newElement = element->cloneNode(args[0].toBoolean(exec));

			return getDOMNode(exec, *newElement);
		}
		case SVGDOMNodeBridge::Normalize:
		{
			node.normalize();
			return Undefined();
		}
		case SVGDOMNodeBridge::IsSupported:
			return Boolean(node.isSupported(args[0].toString(exec).string(), args[1].toString(exec).string()));
		case SVGDOMNodeBridge::AddEventListener:
		{
			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();
			SVGElementImpl *element = doc->getElementFromHandle(node.handle());

			if(element)
			{
				SVGEvent::EventId eventId = SVGEvent::typeToId(args[0].toString(exec).string());
				if(eventId != SVGEvent::UNKNOWN_EVENT)
					element->setEventListener(eventId, new KSVGEcmaEventListener(Object::dynamicCast(args[1]), QString::null, doc->ecmaEngine()));
			}
			return Undefined();
		}
		case SVGDOMNodeBridge::RemoveEventListener:
		{
			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();
			SVGElementImpl *element = doc->getElementFromHandle(node.handle());

			if(element)
			{
				SVGEvent::EventId eventId = SVGEvent::typeToId(args[0].toString(exec).string());
				if(eventId != SVGEvent::UNKNOWN_EVENT)
					element->removeEventListener((int) eventId);
			}
			return Undefined();
		}
//		case SVGDOMNodeBridge::Contains: // TODO
		case SVGDOMNodeBridge::GetNodeName:
			return getString(node.nodeName());
		case SVGDOMNodeBridge::GetNodeValue:
			return getString(node.nodeValue());
		case SVGDOMNodeBridge::GetNodeType:
			return Number(node.nodeType());
		case SVGDOMNodeBridge::GetParentNode:
			return getDOMNode(exec, node.parentNode());
		case SVGDOMNodeBridge::GetChildNodes:
			return (new SVGDOMNodeListBridge(node.childNodes()))->cache(exec);
		case SVGDOMNodeBridge::GetFirstChild:
			return getDOMNode(exec, node.firstChild());
		case SVGDOMNodeBridge::GetLastChild:
			return getDOMNode(exec, node.lastChild());
		case SVGDOMNodeBridge::GetPreviousSibling:
			return getDOMNode(exec, node.previousSibling());
		case SVGDOMNodeBridge::GetNextSibling:
			return getDOMNode(exec, node.nextSibling());
//		case SVGDOMNodeBridge::GetAttributes: // TODO
		case SVGDOMNodeBridge::GetNamespaceURI:
			return getString(node.namespaceURI());
		case SVGDOMNodeBridge::GetPrefix:
			return getString(node.prefix());
		case SVGDOMNodeBridge::GetLocalName:
			return getString(node.localName());
		case SVGDOMNodeBridge::GetOwnerDocument:
			return getDOMNode(exec, node.ownerDocument());
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// SVGDOMElementBridge

/*
@namespace KSVG
@begin SVGDOMElementBridge::s_hashTable 2
 tagName	SVGDOMElementBridge::TagName	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGDOMElementBridgeProto::s_hashTable 17
 getAttribute			SVGDOMElementBridge::GetAttribute			DontDelete|Function 1
 setAttribute			SVGDOMElementBridge::SetAttribute			DontDelete|Function 2
 removeAttribute		SVGDOMElementBridge::RemoveAttribute		DontDelete|Function 1
 getAttributeNode		SVGDOMElementBridge::GetAttributeNode		DontDelete|Function 1
 setAttributeNode		SVGDOMElementBridge::SetAttributeNode		DontDelete|Function 2
 removeAttributeNode	SVGDOMElementBridge::RemoveAttributeNode	DontDelete|Function 1
 getElementsByTagName	SVGDOMElementBridge::GetElementsByTagName	DontDelete|Function 1
 hasAttribute			SVGDOMElementBridge::HasAttribute			DontDelete|Function 1
 getAttributeNS			SVGDOMElementBridge::GetAttributeNS			DontDelete|Function 2
 setAttributeNS			SVGDOMElementBridge::SetAttributeNS			DontDelete|Function 3
 removeAttributeNS		SVGDOMElementBridge::RemoveAttributeNS		DontDelete|Function 2
 getAttributeNodeNS		SVGDOMElementBridge::GetAttributeNodeNS		DontDelete|Function 2
 setAttributeNodeNS		SVGDOMElementBridge::SetAttributeNodeNS		DontDelete|Function 1
 getElementByTagNameNS	SVGDOMElementBridge::GetElementsByTagNameNS	DontDelete|Function 2
 hasAttributeNS			SVGDOMElementBridge::HasAttributeNS			DontDelete|Function 2
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("DOMElement", SVGDOMElementBridgeProto, SVGDOMElementBridgeProtoFunc)

Value SVGDOMElementBridge::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case TagName:
			return getString(m_impl.tagName());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGDOMElementBridgeProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGDOMElementBridge)
	DOM::Element elem = obj->impl();

	switch(id)
	{
		case SVGDOMElementBridge::GetAttribute:
		{
			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();
			SVGElementImpl *element = doc->getElementFromHandle(elem.handle());
			if(element)
				return String(element->getAttribute(args[0].toString(exec).string()));
			else
				return Undefined();
		}
		case SVGDOMElementBridge::SetAttribute:
		{
			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();
			SVGElementImpl *element = doc->getElementFromHandle(elem.handle());
			if(element)
			{
				element->setAttribute(args[0].toString(exec).string(), args[1].toString(exec).string());
				element->setAttributeInternal(args[0].toString(exec).string(), args[1].toString(exec).string());

				SVGHelperImpl::updateItem(exec, elem);
			}

			return Undefined();
		}
		case SVGDOMElementBridge::RemoveAttribute:
		{
			elem.removeAttribute(args[0].toString(exec).string());
			return Undefined();
		}
		case SVGDOMElementBridge::GetAttributeNode:
			return getDOMNode(exec, elem.getAttributeNode(args[0].toString(exec).string()));
		case SVGDOMElementBridge::SetAttributeNode: // TODO: Correct?
			return getDOMNode(exec, elem.setAttributeNode(toNode(args[0])));
		case SVGDOMElementBridge::RemoveAttributeNode: // TODO: Correct?
			return getDOMNode(exec, elem.removeAttributeNode(toNode(args[0])));
		case SVGDOMElementBridge::GetElementsByTagName:
			return (new SVGDOMNodeListBridge(elem.getElementsByTagName(args[0].toString(exec).string())))->cache(exec);
		case SVGDOMElementBridge::GetAttributeNS:
		{
			// This just skips NS! (Rob)
			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();
			SVGElementImpl *element = doc->getElementFromHandle(elem.handle());
			if(element)
				return String(element->getAttribute(args[1].toString(exec).string()));
			else
				return Undefined();
		}
		case SVGDOMElementBridge::SetAttributeNS:
		{
			// For now, we strip the NS part (Rob)
			DOM::DOMString attr = args[1].toString(exec).string();
			int pos = attr.string().find(':');
			if(pos > -1)
				attr = attr.string().mid(pos + 1);

			SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();
			SVGElementImpl *element = doc->getElementFromHandle(elem.handle());
			if(element)
			{
				element->setAttribute(attr.string(), args[2].toString(exec).string());
				element->setAttributeInternal(attr.string(), args[2].toString(exec).string());

				SVGHelperImpl::updateItem(exec, *element);
			}

			return Undefined();
		}
		case SVGDOMElementBridge::RemoveAttributeNS:
		{
			elem.removeAttributeNS(args[0].toString(exec).string(), args[1].toString(exec).string());
			return Undefined();
		}
		case SVGDOMElementBridge::GetAttributeNodeNS:
			return getDOMNode(exec, elem.getAttributeNodeNS(args[0].toString(exec).string(), args[1].toString(exec).string()));
		case SVGDOMElementBridge::SetAttributeNodeNS: // TODO: Correct?
			return getDOMNode(exec, elem.setAttributeNodeNS(toNode(args[0])));
		case SVGDOMElementBridge::GetElementsByTagNameNS:
			return (new SVGDOMNodeListBridge(elem.getElementsByTagNameNS(args[0].toString(exec).string(), args[1].toString(exec).string())))->cache(exec);
		case SVGDOMElementBridge::HasAttribute:
			return Boolean(elem.hasAttribute(args[0].toString(exec).string()));
		case SVGDOMElementBridge::HasAttributeNS:
			return Boolean(elem.hasAttributeNS(args[0].toString(exec).string(), args[1].toString(exec).string()));
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}


// SVGDOMNodeListBridge

/*
@namespace KSVG
@begin SVGDOMNodeListBridge::s_hashTable 2
 length	SVGDOMNodeListBridge::Length	DontDelete
@end
@namespace KSVG
@begin SVGDOMNodeListBridgeProto::s_hashTable 3
 getLength	SVGDOMNodeListBridge::GetLength	DontDelete|Function 0
 item		SVGDOMNodeListBridge::Item		DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("DOMNodeList", SVGDOMNodeListBridgeProto, SVGDOMNodeListBridgeProtoFunc)

Value SVGDOMNodeListBridge::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case Length:
			return Number(m_impl.length());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGDOMNodeListBridgeProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGDOMNodeListBridge)
	DOM::NodeList nodeList = obj->impl();

	switch(id)
	{
		case SVGDOMNodeListBridge::GetLength:
			return Number(nodeList.length());
		case SVGDOMNodeListBridge::Item:
			return getDOMNode(exec, nodeList.item((unsigned long)args[0].toNumber(exec)));
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// SVGDOMCharacterDataBridge

/*
@namespace KSVG
@begin SVGDOMCharacterDataBridge::s_hashTable 3
 data	SVGDOMCharacterDataBridge::Data	DontDelete
 length	SVGDOMCharacterDataBridge::Length	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGDOMCharacterDataBridgeProto::s_hashTable 11
 getData		SVGDOMCharacterDataBridge::GetData			DontDelete|Function 0
 setData		SVGDOMCharacterDataBridge::SetData			DontDelete|Function 1
 getLength		SVGDOMCharacterDataBridge::GetLength		DontDelete|Function 0
 substringData	SVGDOMCharacterDataBridge::SubstringData	DontDelete|Function 2
 appendData		SVGDOMCharacterDataBridge::AppendData		DontDelete|Function 1
 insertData		SVGDOMCharacterDataBridge::InsertData		DontDelete|Function 2
 deleteData		SVGDOMCharacterDataBridge::DeleteData		DontDelete|Function 2
 replaceData	SVGDOMCharacterDataBridge::ReplaceData		DontDelete|Function 2
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("DOMCharacterData", SVGDOMCharacterDataBridgeProto, SVGDOMCharacterDataBridgeProtoFunc)

Value SVGDOMCharacterDataBridge::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case Data:
			return String(m_impl.data());
		case Length:
			return Number(m_impl.length());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGDOMCharacterDataBridge::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case Data:
			m_impl.setData(value.toString(exec).string());
			updateTextItem(exec, m_impl);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

Value SVGDOMCharacterDataBridgeProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGDOMCharacterDataBridge)
	DOM::CharacterData node = obj->impl();

	switch(id)
	{
		case SVGDOMCharacterDataBridge::GetData:
			return String(node.data());
		case SVGDOMCharacterDataBridge::SetData:
			node.setData(args[0].toString(exec).string());
			updateTextItem(exec, node);
			return Undefined();
		case SVGDOMCharacterDataBridge::GetLength:
			return Number(node.length());
		case SVGDOMCharacterDataBridge::SubstringData:
		{
			DOM::DOMString ret = node.substringData(args[0].toInteger(exec), args[1].toInteger(exec));
			updateTextItem(exec, node);
			return String(ret);
		}
		case SVGDOMCharacterDataBridge::AppendData:
			node.appendData(args[0].toString(exec).string());
			updateTextItem(exec, node);
			return Undefined();
		case SVGDOMCharacterDataBridge::InsertData:
			node.insertData(args[0].toInteger(exec), args[1].toString(exec).string());
			updateTextItem(exec, node);
			return Undefined();
		case SVGDOMCharacterDataBridge::DeleteData:
			node.deleteData(args[0].toInteger(exec), args[1].toInteger(exec));
			updateTextItem(exec, node);
			return Undefined();
		case SVGDOMCharacterDataBridge::ReplaceData:
			node.replaceData(args[0].toInteger(exec), args[1].toInteger(exec), args[2].toString(exec).string());
			updateTextItem(exec, node);
			return Undefined();
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// SVGDOMTextBridge

/*
@namespace KSVG
@begin SVGDOMTextBridge::s_hashTable 2
 dummy	SVGDOMTextBridge::Dummy	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGDOMTextBridgeProto::s_hashTable 2
 splitText	SVGDOMTextBridge::SplitText	DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("DOMText", SVGDOMTextBridgeProto, SVGDOMTextBridgeProtoFunc)

Value SVGDOMTextBridge::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGDOMTextBridgeProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGDOMTextBridge)
	DOM::Text node = obj->impl();

	switch(id)
	{
		case SVGDOMTextBridge::SplitText:
			return getDOMNode(exec, node.splitText(args[0].toInteger(exec)));
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// SVGDOMDOMImplementationBridge

/*
@namespace KSVG
@begin SVGDOMDOMImplementationBridge::s_hashTable 2
 dummy	SVGDOMDOMImplementationBridge::Dummy	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGDOMDOMImplementationBridgeProto::s_hashTable 2
 hasFeature	SVGDOMDOMImplementationBridge::HasFeature	DontDelete|Function 2
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("DOMDOMImplementation", SVGDOMDOMImplementationBridgeProto, SVGDOMDOMImplementationBridgeProtoFunc)

Value SVGDOMDOMImplementationBridge::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGDOMDOMImplementationBridgeProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGDOMDOMImplementationBridge)
	DOM::DOMImplementation node = obj->impl();

	switch(id)
	{
		case SVGDOMDOMImplementationBridge::HasFeature:
			return Boolean(node.hasFeature(args[0].toString(exec).string(), args[1].toString(exec).string()));
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// SVGDOMDocumentFragmentBridge

/*
@namespace KSVG
@begin SVGDOMDocumentFragmentBridge::s_hashTable 2
 dummy	SVGDOMDocumentFragmentBridge::Dummy	DontDelete|ReadOnly
@end
*/

Value SVGDOMDocumentFragmentBridge::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

// vim:ts=4:noet
