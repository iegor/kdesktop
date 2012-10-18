#ifndef __WACLUTTERBAR_H
#define __WACLUTTERBAR_H

#include <qpainter.h>
#include "waWidget.h"

class WaClutterbar : public WaWidget {
    Q_OBJECT

  public:
     WaClutterbar();
    ~WaClutterbar();

  public slots:
    void paintEvent(QPaintEvent *);
};

#endif
