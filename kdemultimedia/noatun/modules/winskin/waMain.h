#ifndef __WAMAIN_H
#define __WAMAIN_H

#include "waWidget.h"

class WaMain : WaWidget {
    Q_OBJECT
public:
    WaMain();
    ~WaMain();

protected:
    void paintEvent(QPaintEvent *);
};

#endif
