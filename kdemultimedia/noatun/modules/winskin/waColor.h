

#ifndef WACOLOR_H
#define WACOLOR_H

#include <qcolor.h>
#include <qstring.h>

#define INDEX_BACKGROUND_COLOR	0
#define INDEX_GRID_COLOR	1
#define INDEX_SPEC_BASE		2
#define INDEX_OSC_BASE		18
#define INDEX_PEAKS		23

class WaColor {
public:
    WaColor(QString filename);
    ~WaColor();

    QColor skinColors[24];
};

extern WaColor *colorScheme;

#endif

