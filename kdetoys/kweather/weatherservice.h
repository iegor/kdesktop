/* This file is part of the KDE project
   Copyright (C) 2001-2004 Ian Geiser <geiseri@kde.org>
   Copyright (C) 2002-2004 Nadeem Hasan <nhasan@kde.org>
   Copyright (C) 2003-2004 John Ratke

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _WEATHER_SERVICE
#define _WEATHER_SERVICE

#include <qstringlist.h>
#include <dcopobject.h>
#include <qpixmap.h>

class WeatherLib;
class StationDatabase;

class WeatherService : public QObject, public DCOPObject
{
	Q_OBJECT
	K_DCOP

	private:
		WeatherLib  *m_weatherLib;

	private slots:
		void updated(const QString &stationID);
		void updating(const QString &stationID);
		void slotStationRemoved(const QString &stationID);

	public:
		WeatherService(QObject *parent, const char *name);
		~WeatherService();

	k_dcop_signals:
		void fileUpdating(QString);
		void fileUpdate(QString);
		void stationRemoved(QString);

	k_dcop:
		QString temperature(const QString &stationID);
		QString dewPoint(const QString &stationID);
		QString relativeHumidity(const QString &stationID);
		QString heatIndex(const QString &stationID);
		QString windChill(const QString &stationID);
		QString wind(const QString &stationID);
		QString pressure(const QString &stationID);
		QPixmap currentIcon(const QString &stationID);
		QPixmap icon(const QString &stationID);
		QString currentIconString(const QString &stationID);
		QString iconFileName(const QString &stationID);
		QString date(const QString &stationID);
		QString visibility(const QString &stationID);
		QStringList cover(const QString &stationID);
		QStringList weather(const QString &stationID);
		bool stationNeedsMaintenance(const QString &stationID);

		QString stationName(const QString &stationID);
		QString stationCountry(const QString &stationID);
		QString longitude(const QString &stationID);
		QString latitude(const QString &stationID);

		QStringList findStations(float lon, float lat);

		QString sunRiseTime(const QString &stationID);
		QString sunSetTime(const QString &stationID);
		QString civilTwilightStart(const QString &stationID);
		QString civilTwilightEnd(const QString &stationID);

		void update(const QString &stationID);
		void updateAll();
		void forceUpdate(const QString &stationID);
		void removeStation(const QString &stationID);
		void addStation(const QString &stationID);
		QStringList listStations();
		QString stationCode( const QString &stationName );
		
		void exit();

  private:
		enum TimeType
		{
			RISE = 0,
			SET,
			CIVIL_START,
			CIVIL_END
		};
	  
		void saveSettings();
		QString getTime(const QString &stationID, TimeType timeType);
		StationDatabase *stationDB;
};
#endif
