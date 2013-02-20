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

#ifndef MAIN_H
#define MAIN_H


/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include <kuniqueapplication.h>
#include <kmainwindow.h>


class MapWidget;
class ZoneClockPanel;


class WorldWideWatch : public KMainWindow
{
  Q_OBJECT

public:

  WorldWideWatch(bool restore=false, QWidget *parent=0, const char *name=0);

  void save(KConfig *config);
  void load(KConfig *load);


protected:

  void readProperties(KConfig *sc) { load(sc); };
  void saveProperties(KConfig *sc) { save(sc); };


private slots:

  void doSave();


private:

  MapWidget *_map;
  ZoneClockPanel *_clocks;
  
};


class WatchApplication : public KUniqueApplication
{
  Q_OBJECT

public:

  WatchApplication() : KUniqueApplication() {};

  int newInstance();

  void dumpMap();

};
#endif

