/***************************************************************************
			metar_parser.h  -  Metar Parser
			-------------------
begin                : Wed June 7 2004
copyright            : (C) 2004 by John Ratke
email                : jratke@comcast.net
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef METAR_PARSER_H
#define METAR_PARSER_H

#include <kglobal.h>
#include <klocale.h>
#include <krfcdate.h>

#include <qdatetime.h>
#include <qregexp.h>
#include <qstringlist.h>

class StationDatabase;

struct WeatherInfo
{
	/** The current weather state outside */
	QString theWeather;
	int clouds;
	float windMPH;
	float tempC;
	float dewC;
	bool heavy;
	QStringList qsCoverList;
	QStringList qsCurrentList;
	QDate qsDate;
	QString qsPressure;
	QString qsTemperature;
	QString qsDewPoint;
	QString qsRelHumidity;
	QTime qsTime;
	QString qsVisibility;
	QString qsWindSpeed;
	QString qsWindChill;
	QString qsHeatIndex;
	QString qsWindDirection;
	QString reportLocation;
	bool stationNeedsMaintenance;
};


class MetarParser
{
	public:
		MetarParser(StationDatabase *stationDB,
			    KLocale::MeasureSystem units = KLocale::Imperial,
			    QDate date = QDate::currentDate(),
			    QTime time = QTime::currentTime(), 
			    unsigned int localUTCOffset = KRFCDate::localUTCOffset());

		/* 
		 * Process a METAR string (the second parameter) and return a WeatherInfo struct 
		 *
		 * The first parameter is the station ICAO code, which is needed to match
		 * the code present in the METAR string in order to parse the rest of the 
		 * data correctly.  But the station code is also used to lookup the station 
		 * latitude and longitude to calculate the sunrise and sunset time to see if 
		 * the day or night icon should be used.
		 */
		struct WeatherInfo processData(const QString &stationID, const QString &metar);

	private:
		bool parseCover(const QString &s);
		bool parseCurrent(const QString &s);
		bool parseTemperature(const QString &s);
		bool parseTemperatureTenths(const QString &s);
		void calcTemperatureVariables();
		void removeTrailingDotZero(QString &string);
		bool parseDate(const QString &s);
		bool parseTime(const QString &s);
		bool parseVisibility(QStringList::ConstIterator it);
		bool parsePressure( const QString &s );
		QString parseWindDirection(const unsigned int direction);
		bool parseWindSpeed(const QString &s);
		bool parseStationNeedsMaintenance(const QString &s);
		void calcCurrentIcon();
		void calcWindChill();
		bool isNight(const QString &stationID) const;
		QString iconName( const QString &icon ) const;

		/*
		 * Reset the internal WeatherInfo struct of the class so that
		 * processing can be run again.  (processData can be run again)
		 */
		void reset();

		StationDatabase* const m_stationDb;
		const KLocale::MeasureSystem m_units;
		const QDate m_date;
		const QTime m_time;
		const unsigned int m_localUTCOffset;
		
		struct WeatherInfo weatherInfo;
		
		QRegExp CoverRegExp;
		QRegExp CurrentRegExp;
		QRegExp WindRegExp;
		QRegExp VisRegExp;
		QRegExp VisFracRegExp;
		QRegExp TempRegExp;
		QRegExp TimeRegExp;
		QRegExp DateRegExp;
		QRegExp PressRegExp;
		QRegExp TempTenRegExp;
};

#endif
