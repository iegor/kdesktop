/* Automatically generated from impl/SVGNumberListImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGNumberListImpl__s_hashTableStrings[] = {
    "\0"
    "numberOfItems\0"
};


static const struct HashEntry SVGNumberListImpl__s_hashTableEntries[] = {
   { 1, SVGListDefs::NumberOfItems, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGNumberListImpl::s_hashTable = { 2, 2, SVGNumberListImpl__s_hashTableEntries, 2, SVGNumberListImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGNumberListImplProto__s_hashTableStrings[] = {
    "\0"
    "insertItemBefore\0"
    "replaceItem\0"
    "appendItem\0"
    "initialize\0"
    "removeItem\0"
    "getItem\0"
    "clear\0"
};


static const struct HashEntry SVGNumberListImplProto__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 71, SVGListDefs::Clear, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 63, SVGListDefs::GetItem, DontDelete|Function, 1, -1 },
   { 1, SVGListDefs::InsertItemBefore, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 41, SVGListDefs::Initialize, DontDelete|Function, 1, -1 },
   { 52, SVGListDefs::RemoveItem, DontDelete|Function, 1, 11 },
   { 18, SVGListDefs::ReplaceItem, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 30, SVGListDefs::AppendItem, DontDelete|Function, 1, -1 }
};

const struct HashTable SVGNumberListImplProto::s_hashTable = { 2, 12, SVGNumberListImplProto__s_hashTableEntries, 11, SVGNumberListImplProto__s_hashTableStrings};

} // namespace
