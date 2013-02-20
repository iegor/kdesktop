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
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qpainter.h>


#include <kglobal.h>
#include <kstandarddirs.h>


#include "cities.h"


CityList::CityList()
{
  readCityLists();
}

CityList::~CityList()
{
  _cities.setAutoDelete(true);
  _cities.clear();
}

void CityList::readCityLists()
{
  QStringList lists = KGlobal::dirs()->findAllResources("data", "kworldclock/*.tab");
  for (QStringList::Iterator it = lists.begin(); it != lists.end(); ++it)
    readCityList(*it);
}


double coordinate(QString c)
{
  int neg;
  int d=0, m=0, s=0;

  neg = c.left(1) == "-";
  c.remove(0,1);

  switch (c.length())
    {
    case 4:
      d = c.left(2).toInt();
      m = c.mid(2).toInt();
      break;
    case 5:
      d = c.left(3).toInt();
      m = c.mid(3).toInt();
      break;
    case 6:
      d = c.left(2).toInt();
      m = c.mid(2,2).toInt();
      s = c.right(2).toInt();
      break;
    case 7:
      d = c.left(3).toInt();
      m = c.mid(3,2).toInt();
      s = c.right(2).toInt();
      break;
    default:
      break;
    }

  if (neg)
    return - (d + m/60.0 + s/3600.0);
  else
    return d + m/60.0 + s/3600.0;
}


void CityList::readCityList(const QString &fname)
{
  QFile f(fname);

  if (f.open(IO_ReadOnly))
    {
      QTextStream is(&f);

      QString line;
      QStringList tags;
      QRegExp coord("[+-]\\d+[+-]\\d+");
      QRegExp name("[^\\s]+/[^\\s]+");
      int pos;
      while (!is.eof())
	{
	  line = is.readLine().stripWhiteSpace();
	  if (line.isEmpty() || line.left(1) == "#")
	    continue;

	  QString c, n;
	  
	  pos = coord.search(line, 0);
	  if (pos >= 0)
	    c = line.mid(pos, coord.matchedLength());
	  
	  pos = name.search(line, pos);
	  if (pos > 0)
	    n = line.mid(pos, name.matchedLength()).stripWhiteSpace();

	  if (!c.isEmpty() && !n.isEmpty())
	    {
	      double la, lo;
	      pos = c.find("+", 1);
	      if (pos < 0)
		pos = c.find("-", 1);
	      if (pos > 0)
		{
		  la = coordinate(c.left(pos));
		  lo = coordinate(c.mid(pos));
		  _cities.append(new City(n.latin1(), la, lo));
		}
	    }
	}

      f.close();
    }
}


QPoint CityList::getPosition(double la, double lo, int w, int h, int offset)
{
  int x = (int)((double)w * (180.0 + lo) / 360.0);
  int y = (int)((double)h * (90.0 - la) / 180.0);
  x = (x + offset + w/2) % w;

  return QPoint(x,y);
}


void CityList::paint(QPainter *p, int width, int height, int offset)
{
  p->setPen(Qt::black);

  QPtrListIterator<City> it(_cities);
  for ( ; it.current(); ++it)
    {
      QPoint pos = getPosition(it.current()->latitude(), it.current()->longitude(), width, height, offset);

      if (width > 100)
	p->drawEllipse(pos.x(), pos.y(), 3,3);
      else
	p->drawPoint(pos);
    }
}


City *CityList::getNearestCity(int w, int h, int offset, int x, int y, QPoint &where)
{
  City *result = 0;
  double dist = 1.0e10;
  
  QPtrListIterator<City> it(_cities);
  for ( ; it.current(); ++it)
    {
      QPoint pos = getPosition(it.current()->latitude(), it.current()->longitude(), w, h, offset);
  
      double d = (pos.x()-x)*(pos.x()-x) + (pos.y()-y)*(pos.y()-y);
      if (d < dist)
	{
	  dist = d;
          where = pos;
          result = it.current();
	}       
    }

  return result;
}


QStringList CityList::timezones()
{
  QStringList r;

  QPtrListIterator<City> it(_cities);
  for ( ; it.current(); ++it)
    r << it.current()->name();
  r.sort();

  return r;
}
