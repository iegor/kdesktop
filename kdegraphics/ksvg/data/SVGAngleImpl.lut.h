/* Automatically generated from impl/SVGAngleImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGAngleImpl__s_hashTableStrings[] = {
    "\0"
    "valueInSpecifiedUnits\0"
    "valueAsString\0"
    "unitType\0"
    "value\0"
};


static const struct HashEntry SVGAngleImpl__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 46, SVGAngleImpl::Value, DontDelete, 0, 5 },
   { 23, SVGAngleImpl::ValueAsString, DontDelete, 0, -1 },
   { 1, SVGAngleImpl::ValueInSpecifiedUnits, DontDelete, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 37, SVGAngleImpl::UnitType, DontDelete, 0, -1 }
};

const struct HashTable SVGAngleImpl::s_hashTable = { 2, 6, SVGAngleImpl__s_hashTableEntries, 5, SVGAngleImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGAngleImplProto__s_hashTableStrings[] = {
    "\0"
    "convertToSpecifiedUnits\0"
    "newValueSpecifiedUnits\0"
};


static const struct HashEntry SVGAngleImplProto__s_hashTableEntries[] = {
   { 1, SVGAngleImpl::ConvertToSpecifiedUnits, DontDelete|Function, 1, -1 },
   { 25, SVGAngleImpl::NewValueSpecifiedUnits, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGAngleImplProto::s_hashTable = { 2, 3, SVGAngleImplProto__s_hashTableEntries, 3, SVGAngleImplProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGAngleImplConstructor__s_hashTableStrings[] = {
    "\0"
    "SVG_ANGLETYPE_UNSPECIFIED\0"
    "SVG_ANGLETYPE_UNKNOWN\0"
    "SVG_ANGLETYPE_GRAD\0"
    "SVG_ANGLETYPE_DEG\0"
    "SVG_ANGLETYPE_RAD\0"
};


static const struct HashEntry SVGAngleImplConstructor__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 1, KSVG::SVG_ANGLETYPE_UNSPECIFIED, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 86, KSVG::SVG_ANGLETYPE_RAD, DontDelete|ReadOnly, 0, 7 },
   { 49, KSVG::SVG_ANGLETYPE_GRAD, DontDelete|ReadOnly, 0, -1 },
   { 27, KSVG::SVG_ANGLETYPE_UNKNOWN, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 68, KSVG::SVG_ANGLETYPE_DEG, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGAngleImplConstructor::s_hashTable = { 2, 8, SVGAngleImplConstructor__s_hashTableEntries, 7, SVGAngleImplConstructor__s_hashTableStrings};

} // namespace
