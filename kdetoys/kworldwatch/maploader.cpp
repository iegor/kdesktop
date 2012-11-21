/*
**
** Copyright (C) 1998-2001 by Matthias Hölzer-Klüpfel <hoelzer@kde.org>
**	Maintainence has ceased - send questions to kde-devel@kde.org.
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include <time.h>

#include <qvaluelist.h>
#include <qdir.h>
#include <qimage.h>
#include <qpainter.h>
#include <qtl.h>
#include <qstringlist.h>


#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <kimageeffect.h>


#include "astro.h"
#include "maploader.h"


QPtrList<MapTheme> MapLoader::themes()
{
  QPtrList<MapTheme> result;

  QStringList files = KGlobal::dirs()->findAllResources("data", "kworldclock/maps/*/*.desktop");
  for (QStringList::Iterator it=files.begin(); it != files.end(); ++it)
    {
      KDesktopFile conf(*it);
      conf.setGroup("Theme");
      result.append(new MapTheme(conf.readName(), conf.readEntry("Theme")));
    }

  return result;
}


QStringList MapLoader::maps(const QString &theme)
{
  return KGlobal::dirs()->findAllResources("data", QString("kworldclock/maps/%1/*.jpg").arg(theme));
}


void MapLoader::load(unsigned int width, const QString &theme, unsigned int height, float opacity)
{
  // find the maps available
  QValueList<uint> sizes;
  QStringList files = maps(theme);
  for (uint i=0; i<files.count(); ++i)
    {
      QString f = files[i];
      int pos = f.findRev("/");
      if (pos >= 0)
        f = f.mid(pos+1);
      pos = f.findRev(".");
      if (pos >= 0)
        f = f.left(pos);
      sizes.append(f.toInt());
    }
  qHeapSort(sizes);

  // find the closest (bigger) size
  uint size=0;
  for (uint i=0; i<sizes.count(); ++i)
    if (sizes[i] >= width)
      {
	size = sizes[i];
	break;
      }
  
  QImage image;
  if (size == 0)
    {
      image = QImage(locate("data", "kworldclock/maps/depths/800.jpg"));
      size = 800;
    }
  else  
    image = QImage(locate("data", QString("kworldclock/maps/%1/%2.jpg").arg(theme).arg(size)));

  if (height == 0)
    height = width/2;

  if ((image.width() != (int)width) || (image.height() != (int)height))
    image = image.smoothScale(width, height);

  // convert to light map
  _light.convertFromImage(image);

  // calculate dark map
  _dark.convertFromImage(KImageEffect::blend(Qt::black, image, opacity));
}


QBitmap MapLoader::darkMask(int width, int height)
{
  time_t t;
  struct tm *tmp;
  double jt, sunra, sundec, sunrv, sunlong;
  short *wtab;

  QBitmap illuMask(width, height);
 
  // calculate the position of the sun
  t = time(NULL);
  tmp = gmtime(&t);
  jt = jtime(tmp);
  sunpos(jt,FALSE, &sunra, &sundec, &sunrv, &sunlong);

  int sec = tmp->tm_hour*60*60 + tmp->tm_min*60 + tmp->tm_sec;
  int gmt_position = width * sec / 86400; // note: greenwich is in the middle!

  // calculate the illuminated area
  wtab = new short[height];
  projillum(wtab,width,height,sundec);
 
  // draw illumination
  illuMask.fill(Qt::black);
  QPainter p;
  p.begin(&illuMask);
 
  int start, stop;
  int middle = width - gmt_position;
  for (int y=0; y<height; y++)
    if (wtab[y]>0)
      {
	start = middle - wtab[y];
	stop = middle + wtab[y];
	if (start < 0)
	  {
	    p.drawLine(0,y,stop,y);
	    p.drawLine(width+start,y,width,y);
	  }
	else
	  if (stop > width)
	    {
	      p.drawLine(start,y,width,y);
	      p.drawLine(0,y,stop-width,y);
	    }
	  else
	    p.drawLine(start,y,stop,y);
      }
  p.end();
  delete [] wtab;
  return illuMask;
}

