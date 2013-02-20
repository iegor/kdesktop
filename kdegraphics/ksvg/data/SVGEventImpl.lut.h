/* Automatically generated from impl/SVGEventImpl.cc using ../../kdelibs/kjs/create_hash_table. DO NOT EDIT ! */

using namespace KJS;

namespace KSVG {

static const char SVGEventImpl__s_hashTableStrings[] = {
    "\0"
    "currentTarget\0"
    "cancelable\0"
    "eventPhase\0"
    "timeStamp\0"
    "bubbles\0"
    "target\0"
    "type\0"
};


static const struct HashEntry SVGEventImpl__s_hashTableEntries[] = {
   { 1, SVGEventImpl::CurrentTarget, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 37, SVGEventImpl::TimeStamp, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 15, SVGEventImpl::Cancelable, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 55, SVGEventImpl::Target, DontDelete|ReadOnly, 0, 11 },
   { 62, SVGEventImpl::Type, DontDelete|ReadOnly, 0, -1 },
   { 26, SVGEventImpl::EventPhase, DontDelete|ReadOnly, 0, 12 },
   { 47, SVGEventImpl::Bubbles, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGEventImpl::s_hashTable = { 2, 13, SVGEventImpl__s_hashTableEntries, 11, SVGEventImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGEventImplProto__s_hashTableStrings[] = {
    "\0"
    "getCurrentTarget\0"
    "stopPropagation\0"
    "getCurrentNode\0"
    "preventDefault\0"
    "getCancelable\0"
    "getEventphase\0"
    "getTimeStamp\0"
    "getBubbles\0"
    "getTarget\0"
    "initEvent\0"
    "getType\0"
};


static const struct HashEntry SVGEventImplProto__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 92, SVGEventImpl::GetTimeStamp, DontDelete|Function, 0, 17 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 34, SVGEventImpl::GetCurrentNode, DontDelete|Function, 0, 14 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 105, SVGEventImpl::GetBubbles, DontDelete|Function, 0, -1 },
   { 136, SVGEventImpl::GetType, DontDelete|Function, 0, 13 },
   { 78, SVGEventImpl::GetEventPhase, DontDelete|Function, 0, 15 },
   { 116, SVGEventImpl::GetTarget, DontDelete|Function, 0, 16 },
   { 1, SVGEventImpl::GetCurrentTarget, DontDelete|Function, 0, -1 },
   { 64, SVGEventImpl::GetCancelable, DontDelete|Function, 0, -1 },
   { 18, SVGEventImpl::StopPropagation, DontDelete|Function, 0, -1 },
   { 49, SVGEventImpl::PreventDefault, DontDelete|Function, 0, -1 },
   { 126, SVGEventImpl::InitEvent, DontDelete|Function, 3, -1 }
};

const struct HashTable SVGEventImplProto::s_hashTable = { 2, 18, SVGEventImplProto__s_hashTableEntries, 13, SVGEventImplProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGUIEventImpl__s_hashTableStrings[] = {
    "\0"
    "detail\0"
    "view\0"
};


static const struct HashEntry SVGUIEventImpl__s_hashTableEntries[] = {
   { 1, SVGUIEventImpl::Detail, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 8, SVGUIEventImpl::View, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGUIEventImpl::s_hashTable = { 2, 3, SVGUIEventImpl__s_hashTableEntries, 3, SVGUIEventImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGUIEventImplProto__s_hashTableStrings[] = {
    "\0"
    "initUIEvent\0"
    "getDetail\0"
    "getView\0"
};


static const struct HashEntry SVGUIEventImplProto__s_hashTableEntries[] = {
   { 13, SVGUIEventImpl::GetDetail, DontDelete|Function, 0, -1 },
   { 23, SVGUIEventImpl::GetView, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 1, SVGUIEventImpl::InitUIEvent, DontDelete|Function, 5, -1 },
   { 0, 0, 0, 0, -1 }
};

const struct HashTable SVGUIEventImplProto::s_hashTable = { 2, 5, SVGUIEventImplProto__s_hashTableEntries, 5, SVGUIEventImplProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGKeyEventImpl__s_hashTableStrings[] = {
    "\0"
    "outputString\0"
    "virtKeyVal\0"
    "charCode\0"
    "keyCode\0"
    "keyVal\0"
};


static const struct HashEntry SVGKeyEventImpl__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 34, SVGKeyEventImpl::KeyVal, DontDelete|ReadOnly, 0, -1 },
   { 25, SVGKeyEventImpl::KeyVal, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 42, SVGKeyEventImpl::KeyVal, DontDelete|ReadOnly, 0, 7 },
   { 14, SVGKeyEventImpl::VirtKeyVal, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 1, SVGKeyEventImpl::OutputString, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGKeyEventImpl::s_hashTable = { 2, 8, SVGKeyEventImpl__s_hashTableEntries, 7, SVGKeyEventImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGKeyEventImplProto__s_hashTableStrings[] = {
    "\0"
    "checkModifier\0"
    "getCharCode\0"
    "getKeyCode\0"
    "getKeyVal\0"
};


static const struct HashEntry SVGKeyEventImplProto__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 1, SVGKeyEventImpl::CheckModifier, DontDelete|Function, 1, 7 },
   { 15, SVGKeyEventImpl::GetKeyVal, DontDelete|Function, 0, 8 },
   { 0, 0, 0, 0, -1 },
   { 38, SVGKeyEventImpl::GetKeyVal, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 27, SVGKeyEventImpl::GetKeyVal, DontDelete|Function, 0, -1 },
   { 15, SVGKeyEventImpl::GetCharCode, DontDelete|Function, 0, -1 }
};

const struct HashTable SVGKeyEventImplProto::s_hashTable = { 2, 9, SVGKeyEventImplProto__s_hashTableEntries, 7, SVGKeyEventImplProto__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGMouseEventImpl__s_hashTableStrings[] = {
    "\0"
    "relatedTarget\0"
    "shiftKey\0"
    "clientX\0"
    "clientY\0"
    "ctrlKey\0"
    "metaKey\0"
    "screenX\0"
    "screenY\0"
    "altKey\0"
    "button\0"
};


static const struct HashEntry SVGMouseEventImpl__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 24, SVGMouseEventImpl::ClientX, DontDelete|ReadOnly, 0, -1 },
   { 56, SVGMouseEventImpl::ScreenX, DontDelete|ReadOnly, 0, 11 },
   { 64, SVGMouseEventImpl::ScreenY, DontDelete|ReadOnly, 0, 12 },
   { 0, 0, 0, 0, -1 },
   { 48, SVGMouseEventImpl::MetaKey, DontDelete|ReadOnly, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 40, SVGMouseEventImpl::CtrlKey, DontDelete|ReadOnly, 0, 14 },
   { 0, 0, 0, 0, -1 },
   { 1, SVGMouseEventImpl::RelatedTarget, DontDelete|ReadOnly, 0, -1 },
   { 32, SVGMouseEventImpl::ClientY, DontDelete|ReadOnly, 0, 13 },
   { 15, SVGMouseEventImpl::ShiftKey, DontDelete|ReadOnly, 0, -1 },
   { 72, SVGMouseEventImpl::AltKey, DontDelete|ReadOnly, 0, -1 },
   { 79, SVGMouseEventImpl::Button, DontDelete|ReadOnly, 0, -1 }
};

const struct HashTable SVGMouseEventImpl::s_hashTable = { 2, 15, SVGMouseEventImpl__s_hashTableEntries, 11, SVGMouseEventImpl__s_hashTableStrings};

} // namespace

using namespace KJS;

namespace KSVG {

static const char SVGMouseEventImplProto__s_hashTableStrings[] = {
    "\0"
    "getRelatedTarget\0"
    "initMouseEvent\0"
    "getShiftKey\0"
    "getClientX\0"
    "getClientY\0"
    "getCtrlKey\0"
    "getMetaKey\0"
    "getScreenX\0"
    "getScreenY\0"
    "getAltKey\0"
    "getButton\0"
};


static const struct HashEntry SVGMouseEventImplProto__s_hashTableEntries[] = {
   { 0, 0, 0, 0, -1 },
   { 45, SVGMouseEventImpl::GetClientX, DontDelete|Function, 0, -1 },
   { 89, SVGMouseEventImpl::GetScreenX, DontDelete|Function, 0, 13 },
   { 100, SVGMouseEventImpl::GetScreenY, DontDelete|Function, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 78, SVGMouseEventImpl::GetMetaKey, DontDelete|Function, 0, 15 },
   { 67, SVGMouseEventImpl::GetCtrlKey, DontDelete|Function, 0, -1 },
   { 33, SVGMouseEventImpl::GetShiftKey, DontDelete|Function, 0, 14 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 0, 0, 0, 0, -1 },
   { 56, SVGMouseEventImpl::GetClientY, DontDelete|Function, 0, 16 },
   { 111, SVGMouseEventImpl::GetAltKey, DontDelete|Function, 0, -1 },
   { 121, SVGMouseEventImpl::GetButton, DontDelete|Function, 0, -1 },
   { 1, SVGMouseEventImpl::GetRelatedTarget, DontDelete|Function, 0, 17 },
   { 18, SVGMouseEventImpl::InitMouseEvent, DontDelete|Function, 15, -1 }
};

const struct HashTable SVGMouseEventImplProto::s_hashTable = { 2, 18, SVGMouseEventImplProto__s_hashTableEntries, 13, SVGMouseEventImplProto__s_hashTableStrings};

} // namespace
