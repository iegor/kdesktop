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
#ifndef FLAGS_H
#define FLAGS_H


#include <qstring.h>
#include <qcolor.h>
#include <qptrlist.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qbitmap.h>


#include <kconfig.h>


class Flag
{
public:
  
  Flag(double lo, double la, const QColor &col)
    : _lo(lo), _la(la), _col(col) {};

  double longitude() const { return _lo; };
  double latitude() const { return _la; };

  QColor color() const { return _col; };

  QString annotation() const { return _ann; };
  void setAnnotation(const QString &ann) { _ann = ann; };


private:

  double _lo, _la;

  QColor _col;

  QString _ann;

};


class FlagList
{
public:

  FlagList();

  void paint(QPainter *p, int w, int h, int offset);

  void addFlag(Flag *f);

  void removeNearestFlag(const QPoint &target, int w, int h, int offset);
  void removeAllFlags();

  void save(KConfig *config);
  void load(KConfig *config);
  

private:
 
  QPoint getPosition(double la, double lo, int w, int h, int offset); 

  QPtrList<Flag> _flags;

  QPixmap _flagPixmap;
  QBitmap _flagMask;

};


#endif
