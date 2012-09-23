/* Automatically generated from impl/SVGDocumentImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGDocumentImpl__s_hashTableStrings[] = {
    "\0"
    "documentElement\0"
    "implementation\0"
    "rootElement\0"
    "referrer\0"
    "doctype\0"
    "domain\0"
    "title\0"
    "URL\0"
};


static const struct HashEntry SVGDocumentImpl__s_hashTableEntries[] = {
   { 74, SVGDocumentImpl::Url, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 61, SVGDocumentImpl::Domain, DontDelete|ReadOnly, 0, 10 },
   { 0, 0, 0, 0, -1 },
   { 53, SVGDocumentImpl::DocType, DontDelete|ReadOnly, 0, -1 },
   { 32, SVGDocumentImpl::RootElement, DontDelete|ReadOnly, 0, -1 },
   { 68, SVGDocumentImpl::Title, DontDelete|ReadOnly, 0, 9 },
   { 17, SVGDocumentImpl::Implementation, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 44, SVGDocumentImpl::Referrer, DontDelete|ReadOnly, 0, -1 },
   { 1, SVGDocumentImpl::DocumentElement, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGDocumentImpl::s_hashTable = { 2, 11, SVGDocumentImpl__s_hashTableEntries, 9, SVGDocumentImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGDocumentImplProto__s_hashTableStrings[] = {
    "\0"
    "getElementsByTagNameNS\0"
    "getElementsByTagName\0"
    "createElementNS\0"
    "createTextNode\0"
    "getElementById\0"
    "createElement\0"
};


static const struct HashEntry SVGDocumentImplProto__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 76, SVGDocumentImpl::GetElementById, DontDelete|Function, 1, -1 },
   { 0, 0, 0, 0, -1 },
   { 24, SVGDocumentImpl::GetElementsByTagName, DontDelete|Function, 1, 8 },
   { 61, SVGDocumentImpl::CreateTextNode, DontDelete|Function, 1, -1 },
   { 91, SVGDocumentImpl::CreateElement, DontDelete|Function, 1, 7 },
   { 0, 0, 0, 0, -1 },
   { 45, SVGDocumentImpl::CreateElementNS, DontDelete|Function, 2, -1 },
   { 1, SVGDocumentImpl::GetElementsByTagNameNS, DontDelete|Function, 2, -1 }
};

const struct HashTable SVGDocumentImplProto::s_hashTable = { 2, 9, SVGDocumentImplProto__s_hashTableEntries, 7, SVGDocumentImplProto__s_hashTableStrings};

} // namespace
