/****************************************************************************
 *           metar_parser_test.cpp  -  MetarParser Test Program
 *
 *         Run all of the unit tests with "make check". Run just this 
 *         test with "make metar_parser_test; ./metar_parser_test"
 *                          -------------------
 *         begin                : Wednesday June 16 2004
 *         copyright            : (C) 2004 by John Ratke
 *         email                : jratke@comcast.net
 ****************************************************************************/

/****************************************************************************
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#include <iostream>
using namespace std;

#include <qdatetime.h>
#include <qfile.h>
#include <qstringlist.h>

#include <krfcdate.h>

#include "stationdatabase.h"
#include "metar_parser.h"

void test1();
void test2();

void displayWeatherInfo(const struct WeatherInfo &wi);
const char *getString(const QString &str);

int localUTCOffset = -300;

StationDatabase *stationDb;

bool anyFailed = false;

int main()
{
	bool found = false;
	
	// try the stations.dat file in the current directory first.
	QString path("stations.dat");
	
	if (QFile::exists(path))
	{
		found = true;
	}
	else
	{
		// If we couldn't find it, then maybe the build directory is not the
		// same as the source directory.  If that is the case, try to read it
		// from its location in the share/apps/kweatherservice directory, which
		// means that make install would have had to be done first before calling
		// make check so that the file will be there.
	
		QString kdeDirs(getenv("KDEDIR"));
	
		path = kdeDirs + "/share/apps/kweatherservice/stations.dat";
		if (QFile::exists(path))
		{
			found = true;
		}
	}
	
	if (found)
	{
		cout << "found stations.dat at " << path.latin1() << endl;
	}
	else
	{
		cout << "couldn't find stations.dat!!   try \"make install\" before \"make check\"" << endl;	
		return 1;
	}
	
	stationDb = new StationDatabase(path);

	test1();
	test2();

	delete stationDb;
	
	// If success, return 0, else return 1
	if (anyFailed)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void test1()
{
	QDate Date(2004, 6, 17);    // June 17th.
	QTime Time(21, 7);  // hours, minutes,   seconds and ms defaults to 0

	// Construct a MetarParser object for our tests.
	MetarParser parser( stationDb, KLocale::Imperial, Date, Time, localUTCOffset );

	struct WeatherInfo results = parser.processData("KUGN", "2004/06/18 00:55 KUGN 180055Z AUTO 04004KT 9SM SCT050 17/15 A3005 RMK AO2 SLP167 T01670150 TSNO");

	if (results.theWeather      == "cloudy2_night" &&
	    results.qsCoverList[0]  == "Scattered clouds at 5000 feet" &&
	    results.qsVisibility    == "9m" &&
	    results.qsPressure      == "30.05\" Hg" &&
	    results.qsTemperature   == "62.1°F" &&
	    results.qsDewPoint      == "59°F" &&
	    results.qsRelHumidity   == "89.7%" &&
	    results.qsWindSpeed     == "4 MPH" &&
	    results.qsWindChill.isNull() &&
	    results.qsHeatIndex.isNull() &&
	    results.qsWindDirection == "NE" &&
	    results.reportLocation  == "KUGN" &&
	    results.stationNeedsMaintenance == false)
	{	
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		displayWeatherInfo(results);
		anyFailed = true;
	}
}

void test2()
{
	QDate Date(2004, 6, 18);    // June 18th.
	QTime Time(18, 43);  // hours, minutes,   seconds and ms defaults to 0
	
	// Construct a MetarParser object for our tests.
	MetarParser parser( stationDb, KLocale::Imperial, Date, Time, localUTCOffset );
	
	struct WeatherInfo results = parser.processData("KUGN", "2004/06/18 22:55 KUGN 182255Z 29005KT 10SM BKN110 21/13 A3010 RMK AO2 SLP187 T02110133");

	if (results.theWeather      == "cloudy3" &&
	    results.qsCoverList[0]  == "Broken clouds at 11000 feet" &&
	    results.qsVisibility    == "10m" &&
	    results.qsPressure      == "30.10\" Hg" &&
	    results.qsTemperature   == "70°F" &&
	    results.qsDewPoint      == "55.9°F" &&
	    results.qsRelHumidity   == "61.1%" &&
	    results.qsWindSpeed     == "5 MPH" &&
	    results.qsWindChill.isNull() &&
	    results.qsHeatIndex.isNull() &&
	    results.qsWindDirection == "WNW" &&
	    results.reportLocation  == "KUGN" &&
	    results.stationNeedsMaintenance == false)
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		displayWeatherInfo(results);
		anyFailed = true;
	}
}

void displayWeatherInfo(const struct WeatherInfo &wi)
{
	cout << "theWeather: " << getString(wi.theWeather) << endl;
	cout << "clouds: "     << wi.clouds << endl;
	cout << "windMPH: "    << wi.windMPH << endl;
	cout << "tempC: "      << wi.tempC << endl;
	cout << "dewC: "       << wi.dewC << endl;
	cout << "heavy: "      << wi.heavy << endl;
	
	unsigned int i = 0;
	for ( QStringList::ConstIterator it = wi.qsCoverList.begin(); 
	      it != wi.qsCoverList.end(); 
	      ++it, i++ ) {
		cout << "qsCoverList[" << i << "]: " << (*it).latin1() << endl;
	}
	i = 0;
	for ( QStringList::ConstIterator it = wi.qsCurrentList.begin(); 
	      it != wi.qsCurrentList.end(); 
	      ++it, i++ ) {
		cout << "qsCurrentList[" << i << "]: " << (*it).latin1() << endl;
	}

	cout << "qsDate: "        << wi.qsDate.toString().latin1() << endl;
	cout << "qsPressure: "    << getString(wi.qsPressure) << endl;
	cout << "qsTemperature: " << getString(wi.qsTemperature) << endl;
	cout << "qsDewPoint: "    << getString(wi.qsDewPoint) << endl;
	cout << "qsRelHumidity: " << getString(wi.qsRelHumidity) << endl;
	cout << "qsTime: "        << wi.qsTime.toString().latin1() << endl;
	cout << "qsVisibility: "  << getString(wi.qsVisibility) << endl;
	cout << "qsWindSpeed: "   << getString(wi.qsWindSpeed) << endl;
	cout << "qsWindChill: "   << getString(wi.qsWindChill) << endl;
	cout << "qsHeatIndex: "   << getString(wi.qsHeatIndex) << endl;
	cout << "qsWindDirection: "   << getString(wi.qsWindDirection) << endl;
	cout << "reportLocation: "    << getString(wi.reportLocation) << endl;
	cout << "stationNeedsMaintenance: " << wi.stationNeedsMaintenance << endl;
}

static const char *nullString = "[null]";

const char *getString(const QString &str)
{
	if (str.isNull())
	{
		return nullString;
	}
	else
	{
		return str.latin1();
	}
}
