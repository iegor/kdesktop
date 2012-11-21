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
#ifndef MAP_WIDGET_H
#define MAP_WIDGET_H


#include <time.h>


#include <qwidget.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qpoint.h>
#include <qtimer.h>


class QPopupMenu;


#include <kconfig.h>


class CityList;
class FlagList;


#include "maploader.h"


class MapWidget : public QWidget
{
  Q_OBJECT

public:

  MapWidget(bool applet=false, bool restore=false, QWidget *parent=0, const char *name=0);
  ~MapWidget();

  void setTheme(const QString &theme);
  void setTime(struct tm *time);
  void setIllumination(bool i);
  void setCities(bool c);
  void setFlags(bool f);
  void setSize(int w, int h);

  void save(KConfig *config);
  void load(KConfig *config);

  void updateBackground();

  QPixmap getPixmap();
  QPopupMenu* contextMenu() const { return _popup; }

  void paintContents(QPainter *p);
  QPixmap calculatePixmap();

signals:

  void addClockClicked(const QString &zone);
  void saveSettings();


protected slots:

  void timeout();
  void updateCityIndicator();

public slots:

  void about();

  void toggleIllumination();
  void toggleCities();
  void toggleFlags();

  void removeFlag();
  void removeAllFlags();

  void slotSaveSettings();


protected:

  void resizeEvent(QResizeEvent *ev);
  void paintEvent(QPaintEvent *ev);
  void mousePressEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  void enterEvent(QEvent *ev);
  void leaveEvent(QEvent *ev);


private slots:

  void themeSelected(int index);
  void addFlag(int index);
  void addClock();


private:

  void updateMap();
  QString cityTime(const QString &city);
  void showIndicator(const QPoint &pos);

  MapLoader _loader;

  QString _theme;

  QPixmap _pixmap;

  int gmt_position;

  time_t sec;

  QPopupMenu *_popup, *_themePopup, *_flagPopup;
  QPtrList<MapTheme> _themes;

  bool _illumination, _cities, _flags;
  int _illuminationID, _citiesID, _flagsID;

  CityList *_cityList;
  QLabel *_cityIndicator;
  QString _currentCity;

  FlagList *_flagList;
  QPoint _flagPos;

  bool _applet;
  QTimer m_timer;
  
  int _width, _height;
};


#endif
