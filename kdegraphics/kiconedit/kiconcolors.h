/*
    kiconedit - a small graphics drawing program for creating KDE icons
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/  

#ifndef __KDRAWCOLORS_H__
#define __KDRAWCOLORS_H__

#include <qpopupmenu.h>

#include <kcolordialog.h>

#include "kcolorgrid.h"
#include "utils.h"

class KDrawColors : public KColorGrid
{
  Q_OBJECT
public:
  KDrawColors(QWidget *parent);

  //bool hasColor(uint);

signals:
  void newColor(uint);

protected:
  virtual void paintCell( QPainter*, int, int );
  virtual void mouseReleaseEvent(QMouseEvent*);

  int selected;
};

class KCustomColors : public KDrawColors
{
  Q_OBJECT
public:
  KCustomColors(QWidget *parent);
  ~KCustomColors();

  void addColor(uint);
  void clear();

protected:
  virtual void mouseDoubleClickEvent(QMouseEvent*);
  int getFreeCell();
  void freeAllCells();

protected:
  bool *freecells;
  QPopupMenu *popup;
};

class KSysColors : public KDrawColors
{
  Q_OBJECT
public:

  KSysColors(QWidget *parent);

};



#endif //__KDRAWCOLORS_H__



