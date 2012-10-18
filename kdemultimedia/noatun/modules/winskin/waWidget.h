#ifndef _WAWIDGET_H
#define _WAWIDGET_H

#include <qwidget.h>
#include "waSkinModel.h"

class WaWidget : public QWidget {
    Q_OBJECT
public:
    WaWidget(int mapping);
    virtual ~WaWidget();

    QSize sizeHint();

    void paintPixmap(int wa_mapping);
    void paintPixmap(int wa_mapping, int number);

    void paintPixmap(int wa_mapping, int x, int y);
    void paintPixmap(int wa_mapping, int number, int x, int y);
 
    void paintBackground() { WaSkinModel::instance()->paintBackgroundTo(mapping, this, 0, 0); }

protected:
    void mousePressEvent(QMouseEvent *);
    int mapping;

private slots:
    void skinChanged();
};

#endif
