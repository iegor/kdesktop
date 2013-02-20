/* Automatically generated from impl/SVGLengthImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGLengthImpl__s_hashTableStrings[] = {
    "\0"
    "valueInSpecifiedUnits\0"
    "valueAsString\0"
    "unitType\0"
    "value\0"
};


static const struct HashEntry SVGLengthImpl__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 37, SVGLengthImpl::UnitType, DontDelete|ReadOnly, 0, 5 },
   { 23, SVGLengthImpl::ValueAsString, DontDelete, 0, -1 },
   { 1, SVGLengthImpl::ValueInSpecifiedUnits, DontDelete, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 46, SVGLengthImpl::Value, DontDelete, 0, -1 }
};

const struct HashTable SVGLengthImpl::s_hashTable = { 2, 6, SVGLengthImpl__s_hashTableEntries, 5, SVGLengthImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGLengthImplProto__s_hashTableStrings[] = {
    "\0"
    "convertToSpecifiedUnits\0"
    "newValueSpecifiedUnits\0"
};


static const struct HashEntry SVGLengthImplProto__s_hashTableEntries[] = {
   { 1, SVGLengthImpl::ConvertToSpecifiedUnits, DontDelete|Function, 1, -1 },
   { 25, SVGLengthImpl::NewValueSpecifiedUnits, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGLengthImplProto::s_hashTable = { 2, 3, SVGLengthImplProto__s_hashTableEntries, 3, SVGLengthImplProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGLengthImplConstructor__s_hashTableStrings[] = {
    "\0"
    "SVG_LENGTHTYPE_PERCENTAGE\0"
    "SVG_LENGTHTYPE_UNKNOWN\0"
    "SVG_LENGTHTYPE_NUMBER\0"
    "SVG_LENGTHTYPE_EMS\0"
    "SVG_LENGTHTYPE_EXS\0"
    "SVG_LENGTHTYPE_CM\0"
    "SVG_LENGTHTYPE_MM\0"
    "SVG_LENGTHTYPE_PC\0"
    "SVG_LENGTHTYPE_PT\0"
    "SVG_LENGTHTYPE_PX\0"
};


static const struct HashEntry SVGLengthImplConstructor__s_hashTableEntries[] = {
   { 1, KSVG::SVG_LENGTHTYPE_PERCENTAGE, DontDelete|ReadOnly, 0, -1 },
   { 72, KSVG::SVG_LENGTHTYPE_EMS, DontDelete|ReadOnly, 0, 11 },
   { 27, KSVG::SVG_LENGTHTYPE_UNKNOWN, DontDelete|ReadOnly, 0, 12 },
   { 128, KSVG::SVG_LENGTHTYPE_MM, DontDelete|ReadOnly, 0, -1 },
   { 110, KSVG::SVG_LENGTHTYPE_CM, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 182, KSVG::SVG_LENGTHTYPE_PX, DontDelete|ReadOnly, 0, -1 },
   { 146, KSVG::SVG_LENGTHTYPE_PC, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 50, KSVG::SVG_LENGTHTYPE_NUMBER, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 91, KSVG::SVG_LENGTHTYPE_EXS, DontDelete|ReadOnly, 0, -1 },
   { 164, KSVG::SVG_LENGTHTYPE_PT, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGLengthImplConstructor::s_hashTable = { 2, 13, SVGLengthImplConstructor__s_hashTableEntries, 11, SVGLengthImplConstructor__s_hashTableStrings};

} // namespace
