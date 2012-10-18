#include "waWidget.h"
#include "waSkinModel.h"
#include "noatun/stdaction.h"
#include "waSkin.h"

WaWidget::WaWidget(int _mapping) : QWidget(WaSkin::instance()) {
    mapping = _mapping;
    setBackgroundMode(NoBackground);
    connect (WaSkinModel::instance(), SIGNAL(skinChanged()), this, SLOT(skinChanged()));
}

WaWidget::~WaWidget() {
}

void WaWidget::paintPixmap(int pixmap_mapping) {
    if (pixmap_mapping != -1) 
        WaSkinModel::instance()->bltTo(pixmap_mapping, this, 0, 0);
     else
        WaSkinModel::instance()->paintBackgroundTo(mapping, this, 0, 0);
}

void WaWidget::paintPixmap(int pixmap_mapping, int x, int y) {
    if (pixmap_mapping != -1) 
        WaSkinModel::instance()->bltTo(pixmap_mapping, this, x, y);
     else
        WaSkinModel::instance()->paintBackgroundTo(mapping, this, x, y);
}

void WaWidget::paintPixmap(int pixmap_mapping, int argument) {
    if (pixmap_mapping != -1) 
        WaSkinModel::instance()->bltTo(pixmap_mapping, this, 0, 0, argument);
     else
        WaSkinModel::instance()->paintBackgroundTo(mapping, this, 0, 0);

}

void WaWidget::paintPixmap(int pixmap_mapping, int argument, int x, int y) {
    if (pixmap_mapping != -1) 
        WaSkinModel::instance()->bltTo(pixmap_mapping, this, x, y, argument);
    else
        WaSkinModel::instance()->paintBackgroundTo(mapping, this, x, y);
}

QSize WaWidget::sizeHint() {
    return WaSkinModel::instance()->getMapGeometry(mapping).size();
}

void WaWidget::mousePressEvent(QMouseEvent *e) {
    if (e->button() == RightButton)
        NoatunStdAction::ContextMenu::showContextMenu();
}

void WaWidget::skinChanged() {
    setGeometry(WaSkinModel::instance()->getMapGeometry(mapping));
    update();
}

#include <waWidget.moc>
