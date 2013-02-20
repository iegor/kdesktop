/* Automatically generated from impl/SVGEcma.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGDOMNodeBridge__s_hashTableStrings[] = {
    "\0"
    "previousSibling\0"
    "ownerDocument\0"
    "namespaceURI\0"
    "nextSibling\0"
    "attributes\0"
    "childNodes\0"
    "firstChild\0"
    "parentNode\0"
    "lastChild\0"
    "localName\0"
    "nodeValue\0"
    "nodeName\0"
    "nodeType\0"
    "prefix\0"
};


static const struct HashEntry SVGDOMNodeBridge__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 67, SVGDOMNodeBridge::ChildNodes, DontDelete|ReadOnly, 0, -1 },
   { 100, SVGDOMNodeBridge::LastChild, DontDelete|ReadOnly, 0, -1 },
   { 89, SVGDOMNodeBridge::ParentNode, DontDelete|ReadOnly, 0, 18 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 139, SVGDOMNodeBridge::NodeType, DontDelete|ReadOnly, 0, 17 },
   { 130, SVGDOMNodeBridge::NodeName, DontDelete|ReadOnly, 0, 20 },
   { 17, SVGDOMNodeBridge::OwnerDocument, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 120, SVGDOMNodeBridge::NodeValue, DontDelete, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 78, SVGDOMNodeBridge::FirstChild, DontDelete|ReadOnly, 0, -1 },
   { 1, SVGDOMNodeBridge::PreviousSibling, DontDelete|ReadOnly, 0, 19 },
   { 44, SVGDOMNodeBridge::NextSibling, DontDelete|ReadOnly, 0, -1 },
   { 56, SVGDOMNodeBridge::Attributes, DontDelete|ReadOnly, 0, 22 },
   { 31, SVGDOMNodeBridge::NamespaceURI, DontDelete|ReadOnly, 0, 21 },
   { 148, SVGDOMNodeBridge::Prefix, DontDelete, 0, -1 },
   { 110, SVGDOMNodeBridge::LocalName, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGDOMNodeBridge::s_hashTable = { 2, 23, SVGDOMNodeBridge__s_hashTableEntries, 17, SVGDOMNodeBridge__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMNodeBridgeProto__s_hashTableStrings[] = {
    "\0"
    "removeEventListener\0"
    "getPreviousSibling\0"
    "addEventListener\0"
    "getOwnerDocument\0"
    "getNamespaceURI\0"
    "getNextSibling\0"
    "getAttributes\0"
    "getChildNodes\0"
    "getFirstChild\0"
    "getParentNode\0"
    "hasAttributes\0"
    "hasChildNodes\0"
    "getLastChild\0"
    "getLocalName\0"
    "getNodeValue\0"
    "insertBefore\0"
    "replaceChild\0"
    "appendChild\0"
    "getNodeName\0"
    "getNodeType\0"
    "isSupported\0"
    "removeChild\0"
    "cloneNode\0"
    "getPrefix\0"
    "normalize\0"
    "contains\0"
};


static const struct HashEntry SVGDOMNodeBridgeProto__s_hashTableEntries[] = {
   { 175, SVGDOMNodeBridge::HasChildNodes, DontDelete|Function, 0, -1 },
   { 215, SVGDOMNodeBridge::GetNodeValue, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 119, SVGDOMNodeBridge::GetChildNodes, DontDelete|Function, 0, -1 },
   { 1, SVGDOMNodeBridge::RemoveEventListener, DontDelete|Function, 3, -1 },
   { 0, 0, 0, 0, -1 },
   { 302, SVGDOMNodeBridge::RemoveChild, DontDelete|Function, 1, 37 },
   { 21, SVGDOMNodeBridge::GetPreviousSibling, DontDelete|Function, 0, -1 },
   { 228, SVGDOMNodeBridge::InsertBefore, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 254, SVGDOMNodeBridge::AppendChild, DontDelete|Function, 1, 36 },
   { 0, 0, 0, 0, -1 },
   { 161, SVGDOMNodeBridge::HasAttributes, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 133, SVGDOMNodeBridge::GetFirstChild, DontDelete|Function, 0, 32 },
   { 314, SVGDOMNodeBridge::CloneNode, DontDelete|Function, 1, 29 },
   { 57, SVGDOMNodeBridge::GetOwnerDocument, DontDelete|Function, 0, -1 },
   { 344, SVGDOMNodeBridge::Contains, DontDelete|Function, 1, 30 },
   { 147, SVGDOMNodeBridge::GetParentNode, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 40, SVGDOMNodeBridge::AddEventListener, DontDelete|Function, 3, -1 },
   { 290, SVGDOMNodeBridge::IsSupported, DontDelete|Function, 2, 31 },
   { 241, SVGDOMNodeBridge::ReplaceChild, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 334, SVGDOMNodeBridge::Normalize, DontDelete|Function, 0, 34 },
   { 266, SVGDOMNodeBridge::GetNodeName, DontDelete|Function, 0, -1 },
   { 278, SVGDOMNodeBridge::GetNodeType, DontDelete|Function, 0, 33 },
   { 189, SVGDOMNodeBridge::GetLastChild, DontDelete|Function, 0, 35 },
   { 90, SVGDOMNodeBridge::GetNextSibling, DontDelete|Function, 0, -1 },
   { 105, SVGDOMNodeBridge::GetAttributes, DontDelete|Function, 0, -1 },
   { 74, SVGDOMNodeBridge::GetNamespaceURI, DontDelete|Function, 0, -1 },
   { 324, SVGDOMNodeBridge::GetPrefix, DontDelete|Function, 0, -1 },
   { 202, SVGDOMNodeBridge::GetLocalName, DontDelete|Function, 0, -1 }
};

const struct HashTable SVGDOMNodeBridgeProto::s_hashTable = { 2, 38, SVGDOMNodeBridgeProto__s_hashTableEntries, 29, SVGDOMNodeBridgeProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMElementBridge__s_hashTableStrings[] = {
    "\0"
    "tagName\0"
};


static const struct HashEntry SVGDOMElementBridge__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 1, SVGDOMElementBridge::TagName, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGDOMElementBridge::s_hashTable = { 2, 2, SVGDOMElementBridge__s_hashTableEntries, 2, SVGDOMElementBridge__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMElementBridgeProto__s_hashTableStrings[] = {
    "\0"
    "getElementByTagNameNS\0"
    "getElementsByTagName\0"
    "removeAttributeNode\0"
    "getAttributeNodeNS\0"
    "setAttributeNodeNS\0"
    "removeAttributeNS\0"
    "getAttributeNode\0"
    "setAttributeNode\0"
    "removeAttribute\0"
    "getAttributeNS\0"
    "hasAttributeNS\0"
    "setAttributeNS\0"
    "getAttribute\0"
    "hasAttribute\0"
    "setAttribute\0"
};


static const struct HashEntry SVGDOMElementBridgeProto__s_hashTableEntries[] = {
   { 64, SVGDOMElementBridge::GetAttributeNodeNS, DontDelete|Function, 2, -1 },
   { 170, SVGDOMElementBridge::GetAttributeNS, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 44, SVGDOMElementBridge::RemoveAttributeNode, DontDelete|Function, 1, -1 },
   { 154, SVGDOMElementBridge::RemoveAttribute, DontDelete|Function, 1, 17 },
   { 241, SVGDOMElementBridge::SetAttribute, DontDelete|Function, 2, -1 },
   { 228, SVGDOMElementBridge::HasAttribute, DontDelete|Function, 1, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 120, SVGDOMElementBridge::GetAttributeNode, DontDelete|Function, 1, -1 },
   { 215, SVGDOMElementBridge::GetAttribute, DontDelete|Function, 1, -1 },
   { 1, SVGDOMElementBridge::GetElementsByTagNameNS, DontDelete|Function, 2, -1 },
   { 102, SVGDOMElementBridge::RemoveAttributeNS, DontDelete|Function, 2, 18 },
   { 200, SVGDOMElementBridge::SetAttributeNS, DontDelete|Function, 3, -1 },
   { 185, SVGDOMElementBridge::HasAttributeNS, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 23, SVGDOMElementBridge::GetElementsByTagName, DontDelete|Function, 1, -1 },
   { 137, SVGDOMElementBridge::SetAttributeNode, DontDelete|Function, 2, -1 },
   { 83, SVGDOMElementBridge::SetAttributeNodeNS, DontDelete|Function, 1, -1 }
};

const struct HashTable SVGDOMElementBridgeProto::s_hashTable = { 2, 19, SVGDOMElementBridgeProto__s_hashTableEntries, 17, SVGDOMElementBridgeProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMNodeListBridge__s_hashTableStrings[] = {
    "\0"
    "length\0"
};


static const struct HashEntry SVGDOMNodeListBridge__s_hashTableEntries[] = {
   { 1, SVGDOMNodeListBridge::Length, DontDelete, 0, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGDOMNodeListBridge::s_hashTable = { 2, 2, SVGDOMNodeListBridge__s_hashTableEntries, 2, SVGDOMNodeListBridge__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMNodeListBridgeProto__s_hashTableStrings[] = {
    "\0"
    "getLength\0"
    "item\0"
};


static const struct HashEntry SVGDOMNodeListBridgeProto__s_hashTableEntries[] = {
   { 1, SVGDOMNodeListBridge::GetLength, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 11, SVGDOMNodeListBridge::Item, DontDelete|Function, 1, -1 }
};

const struct HashTable SVGDOMNodeListBridgeProto::s_hashTable = { 2, 3, SVGDOMNodeListBridgeProto__s_hashTableEntries, 3, SVGDOMNodeListBridgeProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMCharacterDataBridge__s_hashTableStrings[] = {
    "\0"
    "length\0"
    "data\0"
};


static const struct HashEntry SVGDOMCharacterDataBridge__s_hashTableEntries[] = {
   { 1, SVGDOMCharacterDataBridge::Length, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 8, SVGDOMCharacterDataBridge::Data, DontDelete, 0, -1 }
};

const struct HashTable SVGDOMCharacterDataBridge::s_hashTable = { 2, 3, SVGDOMCharacterDataBridge__s_hashTableEntries, 3, SVGDOMCharacterDataBridge__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMCharacterDataBridgeProto__s_hashTableStrings[] = {
    "\0"
    "substringData\0"
    "replaceData\0"
    "appendData\0"
    "deleteData\0"
    "insertData\0"
    "getLength\0"
    "getData\0"
    "setData\0"
};


static const struct HashEntry SVGDOMCharacterDataBridgeProto__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 38, SVGDOMCharacterDataBridge::DeleteData, DontDelete|Function, 2, -1 },
   { 70, SVGDOMCharacterDataBridge::GetData, DontDelete|Function, 0, 12 },
   { 78, SVGDOMCharacterDataBridge::SetData, DontDelete|Function, 1, 11 },
   { 1, SVGDOMCharacterDataBridge::SubstringData, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 27, SVGDOMCharacterDataBridge::AppendData, DontDelete|Function, 1, -1 },
   { 15, SVGDOMCharacterDataBridge::ReplaceData, DontDelete|Function, 2, -1 },
   { 60, SVGDOMCharacterDataBridge::GetLength, DontDelete|Function, 0, -1 },
   { 49, SVGDOMCharacterDataBridge::InsertData, DontDelete|Function, 2, -1 }
};

const struct HashTable SVGDOMCharacterDataBridgeProto::s_hashTable = { 2, 13, SVGDOMCharacterDataBridgeProto__s_hashTableEntries, 11, SVGDOMCharacterDataBridgeProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMTextBridge__s_hashTableStrings[] = {
    "\0"
    "dummy\0"
};


static const struct HashEntry SVGDOMTextBridge__s_hashTableEntries[] = {
   { 1, SVGDOMTextBridge::Dummy, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGDOMTextBridge::s_hashTable = { 2, 2, SVGDOMTextBridge__s_hashTableEntries, 2, SVGDOMTextBridge__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMTextBridgeProto__s_hashTableStrings[] = {
    "\0"
    "splitText\0"
};


static const struct HashEntry SVGDOMTextBridgeProto__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 1, SVGDOMTextBridge::SplitText, DontDelete|Function, 1, -1 }
};

const struct HashTable SVGDOMTextBridgeProto::s_hashTable = { 2, 2, SVGDOMTextBridgeProto__s_hashTableEntries, 2, SVGDOMTextBridgeProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMDOMImplementationBridge__s_hashTableStrings[] = {
    "\0"
    "dummy\0"
};


static const struct HashEntry SVGDOMDOMImplementationBridge__s_hashTableEntries[] = {
   { 1, SVGDOMDOMImplementationBridge::Dummy, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGDOMDOMImplementationBridge::s_hashTable = { 2, 2, SVGDOMDOMImplementationBridge__s_hashTableEntries, 2, SVGDOMDOMImplementationBridge__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMDOMImplementationBridgeProto__s_hashTableStrings[] = {
    "\0"
    "hasFeature\0"
};


static const struct HashEntry SVGDOMDOMImplementationBridgeProto__s_hashTableEntries[] = {
   { 1, SVGDOMDOMImplementationBridge::HasFeature, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGDOMDOMImplementationBridgeProto::s_hashTable = { 2, 2, SVGDOMDOMImplementationBridgeProto__s_hashTableEntries, 2, SVGDOMDOMImplementationBridgeProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDOMDocumentFragmentBridge__s_hashTableStrings[] = {
    "\0"
    "dummy\0"
};


static const struct HashEntry SVGDOMDocumentFragmentBridge__s_hashTableEntries[] = {
   { 1, SVGDOMDocumentFragmentBridge::Dummy, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGDOMDocumentFragmentBridge::s_hashTable = { 2, 2, SVGDOMDocumentFragmentBridge__s_hashTableEntries, 2, SVGDOMDocumentFragmentBridge__s_hashTableStrings};

} // namespace
