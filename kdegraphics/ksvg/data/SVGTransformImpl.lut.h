/* Automatically generated from impl/SVGTransformImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGTransformImpl__s_hashTableStrings[] = {
    "\0"
    "matrix\0"
    "angle\0"
    "type\0"
};


static const struct HashEntry SVGTransformImpl__s_hashTableEntries[] = {
   { 14, SVGTransformImpl::Type, DontDelete|ReadOnly, 0, -1 },
   { 1, SVGTransformImpl::Matrix, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 8, SVGTransformImpl::Angle, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGTransformImpl::s_hashTable = { 2, 5, SVGTransformImpl__s_hashTableEntries, 5, SVGTransformImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGTransformImplProto__s_hashTableStrings[] = {
    "\0"
    "setTranslate\0"
    "setMatrix\0"
    "setRotate\0"
    "setScale\0"
    "setSkewX\0"
    "setSkewY\0"
};


static const struct HashEntry SVGTransformImplProto__s_hashTableEntries[] = {
   { 1, SVGTransformImpl::SetTranslate, DontDelete|Function, 2, -1 },
   { 34, SVGTransformImpl::SetScale, DontDelete|Function, 2, -1 },
   { 14, SVGTransformImpl::SetMatrix, DontDelete|Function, 1, -1 },
   { 24, SVGTransformImpl::SetRotate, DontDelete|Function, 3, -1 },
   { 43, SVGTransformImpl::SetSkewX, DontDelete|Function, 1, -1 },
   { 52, SVGTransformImpl::SetSkewY, DontDelete|Function, 1, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGTransformImplProto::s_hashTable = { 2, 7, SVGTransformImplProto__s_hashTableEntries, 7, SVGTransformImplProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGTransformImplConstructor__s_hashTableStrings[] = {
    "\0"
    "SVG_TRANSFORM_TRANSLATE\0"
    "SVG_TRANSFORM_UNKNOWN\0"
    "SVG_TRANSFORM_MATRIX\0"
    "SVG_TRANSFORM_ROTATE\0"
    "SVG_TRANSFORM_SCALE\0"
    "SVG_TRANSFORM_SKEWX\0"
    "SVG_TRANSFORM_SKEWY\0"
};


static const struct HashEntry SVGTransformImplConstructor__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 1, KSVG::SVG_TRANSFORM_TRANSLATE, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 109, KSVG::SVG_TRANSFORM_SKEWX, DontDelete|ReadOnly, 0, -1 },
   { 47, KSVG::SVG_TRANSFORM_MATRIX, DontDelete|ReadOnly, 0, 11 },
   { 89, KSVG::SVG_TRANSFORM_SCALE, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 25, KSVG::SVG_TRANSFORM_UNKNOWN, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 68, KSVG::SVG_TRANSFORM_ROTATE, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 129, KSVG::SVG_TRANSFORM_SKEWY, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGTransformImplConstructor::s_hashTable = { 2, 12, SVGTransformImplConstructor__s_hashTableEntries, 11, SVGTransformImplConstructor__s_hashTableStrings};

} // namespace
