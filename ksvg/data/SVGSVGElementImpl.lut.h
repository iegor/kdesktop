/* Automatically generated from impl/SVGSVGElementImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGSVGElementImpl__s_hashTableStrings[] = {
    "\0"
    "screenPixelToMillimeterX\0"
    "screenPixelToMillimeterY\0"
    "pixelUnitToMillimeterX\0"
    "pixelUnitToMillimeterY\0"
    "contentScriptType\0"
    "contentStyleType\0"
    "currentTranslate\0"
    "useCurrentView\0"
    "currentScale\0"
    "onresize\0"
    "onscroll\0"
    "onunload\0"
    "viewport\0"
    "onerror\0"
    "height\0"
    "onzoom\0"
    "width\0"
    "x\0"
    "y\0"
};


static const struct HashEntry SVGSVGElementImpl__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 186, SVGSVGElementImpl::OnScroll, DontDelete, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 241, SVGSVGElementImpl::X, DontDelete|ReadOnly, 0, 27 },
   { 243, SVGSVGElementImpl::Y, DontDelete|ReadOnly, 0, -1 },
   { 228, SVGSVGElementImpl::OnZoom, DontDelete, 0, -1 },
   { 115, SVGSVGElementImpl::ContentStyleType, DontDelete, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 149, SVGSVGElementImpl::UseCurrentView, DontDelete, 0, 24 },
   { 221, SVGSVGElementImpl::Height, DontDelete|ReadOnly, 0, 23 },
   { 74, SVGSVGElementImpl::PixelUnitToMillimeterY, DontDelete|ReadOnly, 0, 25 },
   { 0, 0, 0, 0, -1 },
   { 235, SVGSVGElementImpl::Width, DontDelete|ReadOnly, 0, -1 },
   { 97, SVGSVGElementImpl::ContentScriptType, DontDelete, 0, 26 },
   { 164, SVGSVGElementImpl::CurrentScale, DontDelete, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 1, SVGSVGElementImpl::ScreenPixelToMillimeterX, DontDelete|ReadOnly, 0, -1 },
   { 26, SVGSVGElementImpl::ScreenPixelToMillimeterY, DontDelete|ReadOnly, 0, -1 },
   { 204, SVGSVGElementImpl::Viewport, DontDelete|ReadOnly, 0, -1 },
   { 51, SVGSVGElementImpl::PixelUnitToMillimeterX, DontDelete|ReadOnly, 0, -1 },
   { 132, SVGSVGElementImpl::CurrentTranslate, DontDelete|ReadOnly, 0, -1 },
   { 195, SVGSVGElementImpl::OnUnload, DontDelete, 0, -1 },
   { 213, SVGSVGElementImpl::OnError, DontDelete, 0, -1 },
   { 177, SVGSVGElementImpl::OnResize, DontDelete, 0, -1 }
};

const struct HashTable SVGSVGElementImpl::s_hashTable = { 2, 28, SVGSVGElementImpl__s_hashTableEntries, 23, SVGSVGElementImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGSVGElementImplProto__s_hashTableStrings[] = {
    "\0"
    "createSVGTransformFromMatrix\0"
    "getIntersectionList\0"
    "createSVGTransform\0"
    "unsuspendRedrawAll\0"
    "checkIntersection\0"
    "unpauseAnimations\0"
    "animationsPaused\0"
    "getEnclosureList\0"
    "createSVGLength\0"
    "createSVGMatrix\0"
    "createSVGNumber\0"
    "unsuspendRedraw\0"
    "checkEnclosure\0"
    "createSVGAngle\0"
    "createSVGPoint\0"
    "getCurrentTime\0"
    "getElementById\0"
    "setCurrentTime\0"
    "createSVGRect\0"
    "deselectAll\0"
    "forceRedraw\0"
};


static const struct HashEntry SVGSVGElementImplProto__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 30, SVGSVGElementImpl::GetIntersectionList, DontDelete|Function, 2, 34 },
   { 124, SVGSVGElementImpl::AnimationsPaused, DontDelete|Function, 0, -1 },
   { 222, SVGSVGElementImpl::CheckEnclosure, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 190, SVGSVGElementImpl::CreateSVGNumber, DontDelete|Function, 0, 30 },
   { 0, 0, 0, 0, -1 },
   { 267, SVGSVGElementImpl::GetCurrentTime, DontDelete|Function, 0, -1 },
   { 338, SVGSVGElementImpl::ForceRedraw, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 88, SVGSVGElementImpl::CheckIntersection, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 106, SVGSVGElementImpl::UnpauseAnimations, DontDelete|Function, 0, -1 },
   { 1, SVGSVGElementImpl::CreateSVGTransformFromMatrix, DontDelete|Function, 1, 29 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 174, SVGSVGElementImpl::CreateSVGMatrix, DontDelete|Function, 0, -1 },
   { 312, SVGSVGElementImpl::CreateSVGRect, DontDelete|Function, 0, 31 },
   { 208, SVGSVGElementImpl::SuspendRedraw, DontDelete|Function, 1, 32 },
   { 237, SVGSVGElementImpl::CreateSVGAngle, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 141, SVGSVGElementImpl::GetEnclosureList, DontDelete|Function, 2, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 50, SVGSVGElementImpl::CreateSVGTransform, DontDelete|Function, 0, -1 },
   { 252, SVGSVGElementImpl::CreateSVGPoint, DontDelete|Function, 0, -1 },
   { 158, SVGSVGElementImpl::CreateSVGLength, DontDelete|Function, 0, -1 },
   { 206, SVGSVGElementImpl::UnsuspendRedraw, DontDelete|Function, 1, -1 },
   { 69, SVGSVGElementImpl::UnsuspendRedrawAll, DontDelete|Function, 0, -1 },
   { 108, SVGSVGElementImpl::PauseAnimations, DontDelete|Function, 0, -1 },
   { 297, SVGSVGElementImpl::SetCurrentTime, DontDelete|Function, 1, 33 },
   { 326, SVGSVGElementImpl::DeselectAll, DontDelete|Function, 0, -1 },
   { 282, SVGSVGElementImpl::GetElementById, DontDelete|Function, 1, -1 }
};

const struct HashTable SVGSVGElementImplProto::s_hashTable = { 2, 35, SVGSVGElementImplProto__s_hashTableEntries, 29, SVGSVGElementImplProto__s_hashTableStrings};

} // namespace
