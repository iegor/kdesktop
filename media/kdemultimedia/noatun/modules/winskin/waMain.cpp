#include "waMain.h"

WaMain::WaMain() : WaWidget(_WA_MAPPING_MAIN) {    
}

WaMain::~WaMain() {
}

void WaMain::paintEvent(QPaintEvent *) {
    paintPixmap(_WA_SKIN_MAIN);
}

#include "waMain.moc"
