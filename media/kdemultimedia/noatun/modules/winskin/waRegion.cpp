/*
  Winamp Skin
  Copyright (C) 2002  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include <ksimpleconfig.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qregexp.h>

#include "waSkinModel.h"
#include "waSkins.h"
#include "waRegion.h"

WaRegion *windowRegion = NULL;

// Hack around case-insensitivity in Winamp INI files by searching for common capitializations
// Needs to be replaced with an all-custom loader after 3.0
const char *pointListNames[] = {"PointList", "pointlist", "Pointlist", "pointList", "POINTLIST", 0};
const char *numPointsNames[] = {"NumPoints", "numpoints", "Numpoints", "numPoints", "NUMPOINTS", 0};

WaRegion::WaRegion(QString filename) {
    // Load the region file, which happens to be in KConfig format
    KSimpleConfig regionFile(filename, true);

    // Clear our variables by default
    window_mask = 0;
    shade_mask = 0;

    // Make the new bitmaps, default window size
    window_mask = new QBitmap(WaSkinModel::instance()->getMapGeometry(_WA_MAPPING_MAIN).size(), true);  
    shade_mask = new QBitmap(WaSkinModel::instance()->getMapGeometry(_WA_MAPPING_TITLE).size(), true);  

    // Load the normal window mask data
    regionFile.setGroup("Normal");
    
    QValueList<int> num_points;
    for (int x = 0;numPointsNames[x];x++) {
        if (regionFile.hasKey(numPointsNames[x]))
            num_points = parseList(regionFile.readEntry(numPointsNames[x]));
    }

    QValueList<int> point_list;
    for (int x = 0;pointListNames[x];x++) {
        if (regionFile.hasKey(pointListNames[x]))
            point_list = parseList(regionFile.readEntry(pointListNames[x]));
    }

    // Now build the mask
    buildPixmap(num_points, point_list, window_mask);

    // Load the windowshade mask data
    regionFile.setGroup("WindowShade");
   
    num_points = parseList(regionFile.readEntry("NumPoints"));
    point_list = parseList(regionFile.readEntry("PointList"));

    // Now build the mask
    buildPixmap(num_points, point_list, shade_mask);
}

WaRegion::~WaRegion() {
   delete window_mask;
   delete shade_mask;
}

void WaRegion::buildPixmap(const QValueList<int> &num_points_list, const QValueList<int> &points_list, QBitmap *dest) {
    if (!num_points_list.count()) {
        dest->fill(Qt::color1);
        return;
    }

    QValueList<int>::const_iterator points = points_list.begin();

    QPainter p(dest);

    // Coordinates in REGION.TXT can go one pixel beyond the window size
    QBitmap bm(dest->width()+1,dest->height()+1,true);
    QPainter bmp(&bm);

    bmp.setBrush(Qt::color1);
    bmp.setPen(Qt::NoPen); // The polygon border itself should not be part of the visible window

    // Go over each "region" in the file
    for (QValueList<int>::const_iterator num_points = num_points_list.begin();num_points != num_points_list.end();num_points++) {
        // Make a new point array
        QPointArray point_array(*num_points);

        // Populate it
        for (int i = 0;i < *num_points;i++) {
            int x = (*points++);
            int y = (*points++);

            point_array.setPoint(i, x, y);
        }

        // Now draw it as a filled polygon on the mask
        bmp.drawPolygon(point_array);
    }

    p.drawPixmap(0,0,bm,0,0,dest->width(),dest->height());
}


// The winamp list format is absolutely insane, it will accept either
// commas or whitespace as the delimiter. This function deals with
// that.
QValueList<int> WaRegion::parseList(const QString &list) const {
    QValueList<int> temp_list;

    if (list.isEmpty())
        return temp_list;

    QStringList open=QStringList::split(QRegExp("[,\\s]+"), list);
    for (QStringList::Iterator i=open.begin(); i != open.end(); ++i)
		temp_list.append((*i).toInt());
	
	return temp_list;
}
