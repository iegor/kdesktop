#include <fstream>
#include <qfile.h>

#include "waColor.h"

WaColor *colorScheme = NULL;

WaColor::WaColor(QString filename) {
    int r, g, b;
    char comma;

    skinColors[0].setRgb(0, 0, 0);
    skinColors[1].setRgb(24, 33, 41);
    skinColors[2].setRgb(239, 49, 16);
    skinColors[3].setRgb(206, 41, 16);
    skinColors[4].setRgb(214, 90, 0);
    skinColors[5].setRgb(214, 102, 0);
    skinColors[6].setRgb(214, 115, 0);
    skinColors[7].setRgb(198, 123, 8);
    skinColors[8].setRgb(222, 165, 24);
    skinColors[9].setRgb(214, 181, 33);
    skinColors[10].setRgb(189, 222, 41);
    skinColors[11].setRgb(148, 222, 33);
    skinColors[12].setRgb(41, 206, 16);
    skinColors[13].setRgb(50, 190, 16);
    skinColors[14].setRgb(57, 181, 16);
    skinColors[15].setRgb(49, 156, 8);
    skinColors[16].setRgb(41, 148, 0);
    skinColors[17].setRgb(24, 132, 8);
    skinColors[18].setRgb(255, 255, 255);
    skinColors[19].setRgb(214, 214, 222);
    skinColors[20].setRgb(181, 189, 189);
    skinColors[21].setRgb(160, 170, 175);
    skinColors[22].setRgb(148, 156, 165);
    skinColors[23].setRgb(150, 150, 150);

    if (filename.isEmpty()) {
      return;
    }

    std::ifstream viscolor(QFile::encodeName(filename));

    if (!viscolor)
        return;

    for (int index = 0;index < 24;index++) {
        viscolor >> r;
	viscolor >> std::ws;
	viscolor >> comma;
	viscolor >> std::ws;
	viscolor >> g;
	viscolor >> std::ws;
	viscolor >> comma;
	viscolor >> std::ws;
	viscolor >> b;

        while(1) {
            char c;

            if (!viscolor.get(c))
                return;

            if (c == '\n')
                break;
        }

        skinColors[index].setRgb(r, g, b);
    }

}

WaColor::~WaColor() {
}
