/* Automatically generated from impl/SVGColorImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGColorImpl__s_hashTableStrings[] = {
    "\0"
    "colorType\0"
    "ICCColor\0"
    "RGBColor\0"
};


static const struct HashEntry SVGColorImpl__s_hashTableEntries[] = {
   { 20, SVGColorImpl::RGBColor, DontDelete|ReadOnly, 0, -1 },
   { 1, SVGColorImpl::ColorType, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 11, SVGColorImpl::ICCColor, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGColorImpl::s_hashTable = { 2, 5, SVGColorImpl__s_hashTableEntries, 5, SVGColorImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGColorImplProto__s_hashTableStrings[] = {
    "\0"
    "setRGBColorICCColor\0"
    "setRGBColor\0"
    "setColor\0"
};


static const struct HashEntry SVGColorImplProto__s_hashTableEntries[] = {
   { 1, SVGColorImpl::SetRGBColorICCColor, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 21, SVGColorImpl::SetRGBColor, DontDelete|Function, 1, -1 },
   { 33, SVGColorImpl::SetColor, DontDelete|Function, 3, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGColorImplProto::s_hashTable = { 2, 5, SVGColorImplProto__s_hashTableEntries, 5, SVGColorImplProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGColorImplConstructor__s_hashTableStrings[] = {
    "\0"
    "SVG_COLORTYPE_RGBCOLOR_ICCCOLOR\0"
    "SVG_COLORTYPE_CURRENTCOLOR\0"
    "SVG_COLORTYPE_RGBCOLOR\0"
    "SVG_COLORTYPE_UNKNOWN\0"
};


static const struct HashEntry SVGColorImplConstructor__s_hashTableEntries[] = {
   { 83, KSVG::SVG_COLORTYPE_UNKNOWN, DontDelete|ReadOnly, 0, 6 },
   { 0, 0, 0, 0, -1 },
   { 60, KSVG::SVG_COLORTYPE_RGBCOLOR, DontDelete|ReadOnly, 0, 5 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 1, KSVG::SVG_COLORTYPE_RGBCOLOR_ICCCOLOR, DontDelete|ReadOnly, 0, -1 },
   { 33, KSVG::SVG_COLORTYPE_CURRENTCOLOR, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGColorImplConstructor::s_hashTable = { 2, 7, SVGColorImplConstructor__s_hashTableEntries, 5, SVGColorImplConstructor__s_hashTableStrings};

} // namespace
