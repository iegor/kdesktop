#include "waClutterbar.h"
#include "waClutterbar.moc"
WaClutterbar::WaClutterbar() : WaWidget(_WA_MAPPING_CLUTTERBAR) {
}

WaClutterbar::~WaClutterbar() {
}

void WaClutterbar::paintEvent(QPaintEvent *) {
    paintPixmap(_WA_SKIN_CLUTTERBAR_DISABLED);
}
