/* Automatically generated from impl/SVGMatrixImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGMatrixImpl__s_hashTableStrings[] = {
    "\0"
    "a\0"
    "b\0"
    "c\0"
    "d\0"
    "e\0"
    "f\0"
};


static const struct HashEntry SVGMatrixImpl__s_hashTableEntries[] = {
   { 3, SVGMatrixImpl::B, DontDelete, 0, -1 },
   { 5, SVGMatrixImpl::C, DontDelete, 0, -1 },
   { 7, SVGMatrixImpl::D, DontDelete, 0, -1 },
   { 9, SVGMatrixImpl::E, DontDelete, 0, -1 },
   { 11, SVGMatrixImpl::F, DontDelete, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 1, SVGMatrixImpl::A, DontDelete, 0, -1 }
};

const struct HashTable SVGMatrixImpl::s_hashTable = { 2, 7, SVGMatrixImpl__s_hashTableEntries, 7, SVGMatrixImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGMatrixImplProto__s_hashTableStrings[] = {
    "\0"
    "rotateFromVector\0"
    "scaleNonUniform\0"
    "translate\0"
    "multiply\0"
    "inverse\0"
    "rotate\0"
    "flipX\0"
    "flipY\0"
    "scale\0"
    "skewX\0"
    "skewY\0"
};


static const struct HashEntry SVGMatrixImplProto__s_hashTableEntries[] = {
   { 80, SVGMatrixImpl::Scale, DontDelete|Function, 1, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 61, SVGMatrixImpl::Rotate, DontDelete|Function, 1, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 18, SVGMatrixImpl::ScaleNonUniform, DontDelete|Function, 2, 14 },
   { 1, SVGMatrixImpl::RotateFromVector, DontDelete|Function, 2, 15 },
   { 53, SVGMatrixImpl::Inverse, DontDelete|Function, 0, 16 },
   { 92, SVGMatrixImpl::SkewY, DontDelete|Function, 1, -1 },
   { 44, SVGMatrixImpl::Multiply, DontDelete|Function, 1, 13 },
   { 34, SVGMatrixImpl::Translate, DontDelete|Function, 2, -1 },
   { 68, SVGMatrixImpl::FlipX, DontDelete|Function, 0, -1 },
   { 74, SVGMatrixImpl::FlipY, DontDelete|Function, 0, -1 },
   { 86, SVGMatrixImpl::SkewX, DontDelete|Function, 1, -1 }
};

const struct HashTable SVGMatrixImplProto::s_hashTable = { 2, 17, SVGMatrixImplProto__s_hashTableEntries, 13, SVGMatrixImplProto__s_hashTableStrings};

} // namespace
