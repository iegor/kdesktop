/***************************************************************************
                          weatherlib.h  -  description
                             -------------------
    begin                : Wed Jul 5 2000
    copyright            : (C) 2000 by Ian Reinhart Geiser
    email                : geiseri@msoe.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WEATHERLIB_H
#define WEATHERLIB_H
#include <qobject.h>
#include <qstring.h>
#include <qregexp.h>
#include <qdict.h>

namespace KIO
{
	class Job;
}

class StationDatabase;

class WeatherLib : public QObject
{
	Q_OBJECT
	
	public:
		class Data;

		WeatherLib(StationDatabase *stationDB, QObject *parent =0L, const char *name =0L);
		virtual ~WeatherLib();

		QString temperature(const QString &stationID);
		QString dewPoint(const QString &stationID);
		QString relHumidity(const QString &stationID);
		QString heatIndex(const QString &stationID);
		QString windChill(const QString &stationID);
		QString wind(const QString &stationID);
		QString pressure(const QString &stationID);
		QString iconName(const QString &stationID);
		QString date(const QString &stationID);
		QStringList weather(const QString &stationID);
		QString visibility(const QString &stationID);
		QStringList cover(const QString &stationID);
		bool stationNeedsMaintenance(const QString &stationID);
		
		QStringList stations();
		bool isNight(const QString &stationID) const;
		
		void update(const QString &stationID);
		void forceUpdate(const QString &stationID);
		void remove(const QString &stationID);
		
	signals:
		void fileUpdating(const QString &stationID);
		void fileUpdate(const QString &stationID);
		void stationRemoved(const QString &stationID);

	private slots:
		void slotCopyDone(KIO::Job*);

	private:
		Data* findData(const QString &stationID);
		void clearData(Data *d);
		void getData(Data *d, bool force = false);
		void processData(const QString &metar, Data *d);

		StationDatabase *m_StationDb;

		QDict<Data> data;
		bool fileDownloaded;
		bool hostDown;
};

#endif
