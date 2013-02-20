/*
**
** Copyright (C) 1998-2001 by Matthias Hölzer-Klüpfel <hoelzer@kde.org>
**	Maintainence has ceased - send questions to kde-devel@kde.org
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
#ifndef KWW_applet_h
#define KWW_applet_h


#include <qstring.h>
#include <qpixmap.h>


#include <kpanelapplet.h>


class MapWidget;


class KWWApplet : public KPanelApplet
{
  Q_OBJECT
    
public:

  KWWApplet(const QString& configFile, Type t = Normal, int actions = 0,
     QWidget *parent = 0, const char *name = 0);
  ~KWWApplet();

  int widthForHeight(int height) const;
  int heightForWidth(int width) const;


protected:

  void mousePressEvent(QMouseEvent *ev);
  bool eventFilter(QObject *, QEvent *);


private:

  MapWidget *map;


};


#endif
