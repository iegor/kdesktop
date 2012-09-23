/* Automatically generated from impl/SVGTextContentElementImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGTextContentElementImpl__s_hashTableStrings[] = {
    "\0"
    "lengthAdjust\0"
    "textLength\0"
};


static const struct HashEntry SVGTextContentElementImpl__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 14, SVGTextContentElementImpl::TextLength, DontDelete|ReadOnly, 0, 3 },
   { 0, 0, 0, 0, -1 },
   { 1, SVGTextContentElementImpl::LengthAdjust, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGTextContentElementImpl::s_hashTable = { 2, 4, SVGTextContentElementImpl__s_hashTableEntries, 3, SVGTextContentElementImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGTextContentElementImplProto__s_hashTableStrings[] = {
    "\0"
    "getStartPositionOfChar\0"
    "getComputedTextLength\0"
    "getCharNumAtPosition\0"
    "getEndPositionOfChar\0"
    "getSubStringLength\0"
    "getRotationOfChar\0"
    "getNumberOfChars\0"
    "getExtentOfChar\0"
    "selectSubString\0"
};


static const struct HashEntry SVGTextContentElementImplProto__s_hashTableEntries[] = {
   { 88, SVGTextContentElementImpl::GetSubStringLength, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 67, SVGTextContentElementImpl::GetEndPositionOfChar, DontDelete|Function, 1, -1 },
   { 0, 0, 0, 0, -1 },
   { 107, SVGTextContentElementImpl::GetRotationOfChar, DontDelete|Function, 1, -1 },
   { 46, SVGTextContentElementImpl::GetCharNumAtPosition, DontDelete|Function, 1, -1 },
   { 24, SVGTextContentElementImpl::GetComputedTextLength, DontDelete|Function, 0, -1 },
   { 1, SVGTextContentElementImpl::GetStartPositionOfChar, DontDelete|Function, 1, 11 },
   { 142, SVGTextContentElementImpl::GetExtentOfChar, DontDelete|Function, 1, -1 },
   { 125, SVGTextContentElementImpl::GetNumberOfChars, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 158, SVGTextContentElementImpl::SelectSubString, DontDelete|Function, 2, -1 }
};

const struct HashTable SVGTextContentElementImplProto::s_hashTable = { 2, 12, SVGTextContentElementImplProto__s_hashTableEntries, 11, SVGTextContentElementImplProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGTextContentElementImplConstructor__s_hashTableStrings[] = {
    "\0"
    "LENGTHADJUST_SPACINGANDGLYPHS\0"
    "LENGTHADJUST_SPACING\0"
    "LENGTHADJUST_UNKNOWN\0"
};


static const struct HashEntry SVGTextContentElementImplConstructor__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 31, KSVG::LENGTHADJUST_SPACING, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 1, KSVG::LENGTHADJUST_SPACINGANDGLYPHS, DontDelete|ReadOnly, 0, -1 },
   { 52, KSVG::LENGTHADJUST_UNKNOWN, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGTextContentElementImplConstructor::s_hashTable = { 2, 5, SVGTextContentElementImplConstructor__s_hashTableEntries, 5, SVGTextContentElementImplConstructor__s_hashTableStrings};

} // namespace
