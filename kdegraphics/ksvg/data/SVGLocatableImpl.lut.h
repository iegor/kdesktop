/* Automatically generated from impl/SVGLocatableImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGLocatableImpl__s_hashTableStrings[] = {
    "\0"
    "farthestViewportElement\0"
    "nearestViewportElement\0"
};


static const struct HashEntry SVGLocatableImpl__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 25, SVGLocatableImpl::NearestViewportElement, DontDelete, 0, 3 },
   { 0, 0, 0, 0, -1 },
   { 1, SVGLocatableImpl::FarthestViewportElement, DontDelete, 0, -1 }
};

const struct HashTable SVGLocatableImpl::s_hashTable = { 2, 4, SVGLocatableImpl__s_hashTableEntries, 3, SVGLocatableImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGLocatableImplProto__s_hashTableStrings[] = {
    "\0"
    "getTransformToElement\0"
    "getScreenCTM\0"
    "getBBox\0"
    "getCTM\0"
};


static const struct HashEntry SVGLocatableImplProto__s_hashTableEntries[] = {
   { 1, SVGLocatableImpl::GetTransformToElement, DontDelete|Function, 1, -1 },
   { 23, SVGLocatableImpl::GetScreenCTM, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 36, SVGLocatableImpl::GetBBox, DontDelete|Function, 0, 5 },
   { 0, 0, 0, 0, -1 },
   { 44, SVGLocatableImpl::GetCTM, DontDelete|Function, 0, -1 }
};

const struct HashTable SVGLocatableImplProto::s_hashTable = { 2, 6, SVGLocatableImplProto__s_hashTableEntries, 5, SVGLocatableImplProto__s_hashTableStrings};

} // namespace
