/*
    This file is part of KWeather.
    Copyright (c) 2004 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SERVICECONFIGWIDGET_H
#define SERVICECONFIGWIDGET_H

#include <serviceconfigdata.h>

class QListViewItem;
class WeatherService_stub;

class ServiceConfigWidget : public wsPrefs
{
  Q_OBJECT

  public:
    ServiceConfigWidget( QWidget *parent, const char *name = 0 );
    virtual ~ServiceConfigWidget();

  signals:
    void changed( bool );

  public slots:
    void addStation();
    void removeStation();
    void updateStations();
    void exitWeatherService();

    void selectionChanged( QListViewItem* );
    void modified();

  private:
    void initGUI();
    void loadLocations();
    bool dcopActive();
    void scanStations();

    QMap<QString, QString> mStationMap;
    WeatherService_stub *mService;
};

#endif
