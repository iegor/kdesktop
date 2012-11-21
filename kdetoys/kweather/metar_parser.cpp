/***************************************************************************
			metar_parser.cpp  -  Metar Parser
			Based on code originally in weatherlib.cpp.
			-------------------
begin                : Wed June 7 2004
copyright            : (C) 2004 by John Ratke
                     : (C) 2002-2004 Nadeem Hasan <nhasan@kde.org>
                     : (C) 2002-2004 Ian Geiser <geiseri@kde.org>
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

#include "config.h"

#include <qdatetime.h>
#include <kdebug.h>
#include <math.h>

#include "metar_parser.h"
#include "stationdatabase.h"
#include "sun.h"

// Temperature conversion macros
#define TEMP_C_TO_F(x) ( ((x) * 9/5) + 32 )
#define TEMP_F_TO_C(x) ( ((x) - 32) * 5/9 )

MetarParser::MetarParser(StationDatabase *stationDB,
			 KLocale::MeasureSystem units,
			 QDate date,
			 QTime time,
			 unsigned int localUTCOffset) : 
	m_stationDb(stationDB), m_units(units), m_date(date), m_time(time), m_localUTCOffset(localUTCOffset)
{
	CoverRegExp    = QRegExp("^(FEW|SCT|BKN|OVC|SKC|CLR|CAVOK)([0-9]{3})?(?:TCU|CB)?$");
	CurrentRegExp  = QRegExp("^(\\+|-|VC)?([A-Z]{2,4})$");
	WindRegExp     = QRegExp("^([0-9]{3}|VRB)([0-9]{2,3})(?:G([0-9]{2,3}))?(KT|KMH|MPS)$");
	VisRegExp      = QRegExp("^([0-9]{1,2})SM$");
	VisFracRegExp  = QRegExp("^1/(2|4)SM$");
	TempRegExp     = QRegExp("^(M)?([0-9]{2})/(?:(M)?([0-9]{2}))?$");
	TimeRegExp     = QRegExp("^([0-9]{2}:[0-9]{2})$");
	DateRegExp     = QRegExp("^([0-9]{4}/[0-9]{2}/[0-9]{2})$");
	PressRegExp    = QRegExp("^([AQ])([0-9]{4})$");
	TempTenRegExp  = QRegExp("^T([01][0-9]{3})([01][0-9]{3})$");
}

void MetarParser::reset()
{
	// Initialize the WeatherInfo structure
	weatherInfo.theWeather = QString::null;
	weatherInfo.clouds = 0;
	weatherInfo.windMPH = 0;
	weatherInfo.tempC = 0;
	weatherInfo.dewC  = 0;
	weatherInfo.heavy = false;
	weatherInfo.qsCoverList.clear();
	weatherInfo.qsCurrentList.clear();
	weatherInfo.qsDate = m_date;
	weatherInfo.qsTime = m_time;
	weatherInfo.qsPressure = QString::null;
	weatherInfo.qsTemperature = QString::null;
	weatherInfo.qsDewPoint = QString::null;
	weatherInfo.qsRelHumidity = QString::null;
	weatherInfo.qsVisibility = QString::null;
	weatherInfo.qsWindSpeed = QString::null;
	weatherInfo.qsWindChill = QString::null;
	weatherInfo.qsHeatIndex = QString::null;
	weatherInfo.qsWindDirection = QString::null;
	weatherInfo.stationNeedsMaintenance = false;
}

struct WeatherInfo MetarParser::processData(const QString &stationID, const QString &metar)
{
	reset();
	
	weatherInfo.reportLocation = stationID;
	
	kdDebug(12006) << "Processing data: " << metar << endl;

	// Split at whitespace into tokens
	QStringList dataList = QStringList::split(QRegExp("\\s+"), metar);
	bool found = false;
	bool beforeRemark = true;

	for (QStringList::ConstIterator it = dataList.begin();
	     it != dataList.end(); ++it)
	{
		// Don't try to parse the ICAO location code
		if ((!found) && (*it == weatherInfo.reportLocation.upper().stripWhiteSpace()))
		{
			found = true;
			continue;
		}

		kdDebug(12006) << "Processing Token: " << *it << endl;
		
		if (*it == "RMK")
		{
			beforeRemark = false;
			continue;
		}
		
		if (beforeRemark)
		{
			if (parseDate(*it))
				continue;
			if (parseTime(*it))
				continue;
			if (parseWindSpeed(*it))
				continue;
			if (parseVisibility(it))  // Note, pass in iterator.
				continue;
			if (parseTemperature(*it))
				continue;
			if (parsePressure(*it))
				continue;
			if (parseCover(*it))
				continue;
			if (parseCurrent(*it))
				continue;
		}
		else
		{
			if (parseTemperatureTenths(*it))
				continue;
			if (parseStationNeedsMaintenance(*it))
				continue;
		}
	}

	calcTemperatureVariables();
	calcWindChill();
	calcCurrentIcon();
	
	return weatherInfo;
}

/** Parse the current cover type */
bool MetarParser::parseCover(const QString &s)
{
	if (CoverRegExp.search(s) > -1)
	{
		kdDebug(12006) << "Cover: " << CoverRegExp.capturedTexts().join("-")
				<< endl;

		QString sCode = CoverRegExp.cap(1);
		float height = CoverRegExp.cap(2).toFloat();  // initially in 100's of feet
		QString sClouds;
		QString skycondition;

		height *= 100;
		if (m_units == KLocale::Metric)
		{
			height = height * 0.3048;
			// using plural i18n form for proper translations
			sClouds = i18n("1 meter", "%n meters", (int)height);
		}
		else
		{
			// using plural i18n form for proper translations
			sClouds = i18n("1 foot", "%n feet", (int)height);
		}

		if (sCode == "FEW")
		{
			skycondition = i18n( "Few clouds at %1" ).arg(sClouds);
			weatherInfo.clouds += 2;
		}
		else if (sCode == "SCT")
		{
			skycondition = i18n( "Scattered clouds at %1" ).arg(sClouds);
			weatherInfo.clouds += 4;
		}
		else if (sCode == "BKN")
		{
			skycondition = i18n( "Broken clouds at %1" ).arg(sClouds);
			weatherInfo.clouds += 8;
		}
		else if (sCode == "OVC")
		{
			skycondition = i18n( "Overcast clouds at %1" ).arg(sClouds);
			weatherInfo.clouds += 64;
		}
		else if ((sCode == "CLR") || (sCode == "SKC") || (sCode == "CAVOK"))
		{
			skycondition = i18n("Clear skies");
			weatherInfo.clouds = 0;
		}

		kdDebug(12006) << "*** Clouds: " << weatherInfo.clouds << endl;
		weatherInfo.qsCoverList << skycondition;

		return true;
	}
	return false;
}

/** Parse the current weather conditions */
bool MetarParser::parseCurrent(const QString &s)
{
	if (CurrentRegExp.search(s) > -1)
	{
		QString sIntensity = CurrentRegExp.cap(1);
		QString sCode = CurrentRegExp.cap(2);
		QString intensity, descriptor, phenomena, currentWeather;

		kdDebug(12006) << "Current: " << CurrentRegExp.capturedTexts().join("-") << endl;

		// Decode the intensity
		if (sIntensity == "+")
		{
			intensity = i18n("Heavy");
			weatherInfo.heavy = true;
		}
		else if (sIntensity == "-")
		{
			intensity = i18n("Light");
			weatherInfo.heavy = false;
		}

		// Decode the descriptor
		if (sCode.contains("MI"))
			descriptor = i18n("Shallow");
		else if (sCode.contains("PR"))
			descriptor = i18n("Partial");
		else if (sCode.contains("BC"))
			descriptor = i18n("Patches");
		else if (sCode.contains("DR"))
			descriptor = i18n("Low Drifting");
		else if (sCode.contains("BL"))
			descriptor = i18n("Blowing");
		else if (sCode.contains("SH"))
		{
			descriptor = i18n("Showers");
			weatherInfo.theWeather = "shower";
		}
		else if (sCode.contains("TS"))
		{
			descriptor = i18n("Thunder Storm");
			weatherInfo.theWeather = "tstorm";
		}
		else if (sCode.contains("FZ"))
		{
			descriptor = i18n("Freezing");
		}

		// Decode weather phenomena
		if (sCode.contains("DZ"))
		{
			phenomena = i18n("Drizzle");
			weatherInfo.theWeather = "light_rain";
		}
		else if (sCode.contains("RA"))
		{
			phenomena = i18n("Rain");
			weatherInfo.theWeather = "shower";
		}
		else if (sCode.contains("SN"))
		{
			phenomena = i18n("Snow");
			weatherInfo.theWeather = "snow";
		}
		else if (sCode.contains("SG"))
		{
			phenomena = i18n("Snow Grains");
			weatherInfo.theWeather = "snow4";
		}
		else if (sCode.contains("IC"))
		{
			phenomena = i18n("Ice Crystals");
			weatherInfo.theWeather = "hail";
		}
		else if (sCode.contains("PE"))
		{
			phenomena = i18n("Ice Pellets");
			weatherInfo.theWeather = "hail";
		}
		else if (s.contains("GR"))
		{
			phenomena = i18n("Hail");
			weatherInfo.theWeather = "hail";
		}
		else if (sCode.contains("GS"))
		{
			phenomena = i18n("Small Hail Pellets");
			weatherInfo.theWeather = "hail";
		}
		else if (s.contains("UP"))
		{
			phenomena = i18n("Unknown Precipitation");
			weatherInfo.theWeather = iconName("shower1");
		}
		else if (sCode.contains("BR"))
		{
			phenomena = i18n("Mist");
			// Mist has lower priority than say rain or snow
			if ( weatherInfo.theWeather.isEmpty() )
			{
				weatherInfo.theWeather = "mist";
			}
		}
		else if (sCode.contains("FG"))
		{
			phenomena = i18n("Fog");
			// Fog has lower priority than say rain or snow
			if ( weatherInfo.theWeather.isEmpty() )
			{
				weatherInfo.theWeather = "fog";
			}
		}
		else if (sCode.contains("FU"))
			phenomena = i18n("Smoke");
		else if (sCode.contains("VA"))
			phenomena = i18n("Volcanic Ash");
		else if (sCode.contains("DU"))
			phenomena = i18n("Widespread Dust");
		else if (sCode.contains("SA"))
			phenomena = i18n("Sand");
		else if (sCode.contains("HZ"))
			phenomena = i18n("Haze");
		else if (sCode.contains("PY"))
			phenomena = i18n("Spray");
		else if (sCode.contains("PO"))
			phenomena = i18n("Dust/Sand Swirls");
		else if (sCode.contains("SQ"))
			phenomena = i18n("Sudden Winds");
		else if (sCode.contains("FC"))
		{
			if (sIntensity == "+")
				currentWeather = i18n("Tornado");
			else
				phenomena = i18n("Funnel Cloud");
		}
		else if (sCode.contains("SS"))
			phenomena = i18n("Sand Storm");
		else if (sCode.contains("DS"))
			phenomena = i18n("Dust Storm");
		
		if (currentWeather.isEmpty()) currentWeather = i18n("%1 is the intensity, %2 is the descriptor and %3 is the phenomena", "%1 %2 %3").arg(intensity).arg(descriptor).arg(phenomena);

		if (!currentWeather.isEmpty())
			weatherInfo.qsCurrentList << currentWeather;

		return true;
	}
	return false;
}

/** Parse out the current temperature */
bool MetarParser::parseTemperature(const QString &s)
{
	if (TempRegExp.search(s) > -1)
	{
		kdDebug(12006) << "Temp: " << TempRegExp.capturedTexts().join("-")
				<< endl;

		float fTemp = TempRegExp.cap(2).toFloat();
		if (TempRegExp.cap(1) == "M" && fTemp != 0 )
			fTemp *= -1;
		float fDew = TempRegExp.cap(4).toFloat();
		if (TempRegExp.cap(3) == "M" && fDew != 0 )
			fDew *= -1;
		
		weatherInfo.tempC = fTemp;
		weatherInfo.dewC = fDew;
		return true;
	}
	return false;
}

bool MetarParser::parseTemperatureTenths(const QString &s)
{
	if (TempTenRegExp.search(s) > -1)
	{
		kdDebug(12006) << "Temp Tenths: " << TempTenRegExp.capturedTexts().join("-")
				<< endl;
		
		float temperature = TempTenRegExp.cap( 1 ).toFloat() / 10;
		float dewPoint    = TempTenRegExp.cap( 2 ).toFloat() / 10;

		if ( temperature >= 100 )
		{
			temperature -= 100;
			temperature *= -1;
		}
		if ( dewPoint >= 100 )
		{
			dewPoint -= 100;
			dewPoint *= -1;
		}

		weatherInfo.tempC = temperature;
		weatherInfo.dewC = dewPoint;
		
		return true;
	}
	return false;
}

void MetarParser::calcTemperatureVariables()
{
#define E(t) ::pow(10, 7.5*t/(237.7+t))
	float fRelHumidity = E(weatherInfo.dewC)/E(weatherInfo.tempC) * 100;
        if (fRelHumidity > 100.0) fRelHumidity = 100.0; 

	weatherInfo.qsRelHumidity.sprintf("%.1f", fRelHumidity);
	removeTrailingDotZero(weatherInfo.qsRelHumidity);
	weatherInfo.qsRelHumidity += "%";

	float fHeatIndex = 0;
	float tempF = TEMP_C_TO_F(weatherInfo.tempC);
	if (tempF >= 80)
	{
#define SQR(a)  ((a)*(a))
		fHeatIndex = -42.379 + (2.04901523*tempF)
			+ (10.14333127*fRelHumidity)
			- (0.22475541*tempF*fRelHumidity)
			- (0.00683783*SQR(tempF))
			- (0.05481717*SQR(fRelHumidity))
			+ (0.00122874*SQR(tempF)*fRelHumidity)
			+ (0.00085282*tempF*SQR(fRelHumidity))
			- (0.00000199*SQR(tempF)*SQR(fRelHumidity));

		if ( fHeatIndex <= tempF )
			fHeatIndex = 0;
	}

	QString unit;
	if (m_units == KLocale::Metric)
	{
		unit = i18n("°C");
		weatherInfo.qsTemperature.sprintf("%.1f", weatherInfo.tempC);
		weatherInfo.qsDewPoint.sprintf("%.1f", weatherInfo.dewC);
		if (fHeatIndex >= 80)
			weatherInfo.qsHeatIndex.sprintf("%.1f", TEMP_F_TO_C(fHeatIndex));
	}
	else
	{
		unit = i18n("°F");
		weatherInfo.qsTemperature.sprintf("%.1f", tempF);
		weatherInfo.qsDewPoint.sprintf("%.1f", TEMP_C_TO_F(weatherInfo.dewC));
		if (fHeatIndex >= 80)
			weatherInfo.qsHeatIndex.sprintf("%.1f", (fHeatIndex));
	}
	
	removeTrailingDotZero(weatherInfo.qsTemperature);
	removeTrailingDotZero(weatherInfo.qsDewPoint);
	removeTrailingDotZero(weatherInfo.qsHeatIndex);
	
	weatherInfo.qsTemperature += unit;
	weatherInfo.qsDewPoint += unit;
	if (!weatherInfo.qsHeatIndex.isEmpty())
		weatherInfo.qsHeatIndex += unit;
}

void MetarParser::removeTrailingDotZero(QString &string)
{
	if ( string.right( 2 ) == ".0" )
	{
		string = string.left( string.length() - 2 );
	}
}

/** Parse out the current date. */
bool MetarParser::parseDate(const QString &s)
{
	if (DateRegExp.search(s) > -1)
	{
		kdDebug(12006) << "Date: " << DateRegExp.capturedTexts().join("-")
				<< endl;
		QString dateString = DateRegExp.cap(1);
		QString day, month, year;

		day = dateString.mid(8,2);
		month = dateString.mid(5,2);
		year = dateString.mid(0,4);

		QDate theDate(year.toInt(), month.toInt(), day.toInt());


		weatherInfo.qsDate = theDate;
		return true;
	}
	return false;
}

/** Parse out the current time. */
bool MetarParser::parseTime(const QString &s)
{
	if (TimeRegExp.search(s) > -1)
	{
		kdDebug(12006) << "Time: " << TimeRegExp.capturedTexts().join("-")
				<< endl;

		QString hour, minute, dateString;

		dateString = TimeRegExp.cap(1);
		hour = dateString.mid(0,2);
		minute = dateString.mid(3,2);
		QTime theTime(hour.toInt(), minute.toInt());

		weatherInfo.qsTime = theTime;
		return true;
	}
	return false;
}

/** Parse out the current visibility */
bool MetarParser::parseVisibility(QStringList::ConstIterator it)
{
	float fVisibility = 0;
	
	if (VisRegExp.search(*it) > -1)
	{
		fVisibility = VisRegExp.cap(1).toFloat();

		kdDebug(12006) << "Visibility: " << VisRegExp.capturedTexts().join("-")
				<< endl;

	}
	else if (VisFracRegExp.search(*it) > -1)
	{
		// got a fractional visibility, go back to previous string in the list
		// and get the whole part.
		fVisibility = (*(it--)).toFloat();
		// shouldn't be necessary?  
		//it++;
		fVisibility += ( 1 / VisFracRegExp.cap(1).toFloat() );
	}
	
	if (fVisibility > 0)
	{
		if (m_units == KLocale::Metric)
		{
			fVisibility *= 1.6;
			weatherInfo.qsVisibility.setNum(fVisibility);
			weatherInfo.qsVisibility += i18n("km");
		}
		else
		{
			weatherInfo.qsVisibility.setNum(fVisibility);
			weatherInfo.qsVisibility += i18n("m");
		}
		return true;
	}
	else
	{
		return false;
	}
}

/** Parse out the current pressure. */
bool MetarParser::parsePressure( const QString &s)
{
	if (PressRegExp.search(s) > -1)
	{
		QString type = PressRegExp.cap(1);
		float fPressure = PressRegExp.cap(2).toFloat();

		kdDebug(12006) << "Pressure: " << PressRegExp.capturedTexts().join("-")
				<< endl;

		if (m_units == KLocale::Metric)
		{
			if (type == "A")
				fPressure *= (33.8639/100);
			weatherInfo.qsPressure.setNum(fPressure, 'f', 0);
			weatherInfo.qsPressure += i18n(" hPa");
		}
		else
		{
			if (type == "Q")
				fPressure /= 33.8639;
			else
				fPressure /= 100;
			weatherInfo.qsPressure.setNum(fPressure, 'f', 2);
			weatherInfo.qsPressure += i18n("\" Hg");
		}
		return true;
	}
	return false;
}

struct wind_info
{
	unsigned int   number;
	QString        name;
};

static const struct wind_info wind_direction[] = 
{
	{ 0,   i18n("N")   },   // North is 0 to 11, and so on
	{ 12,  i18n("NNE") },
	{ 33,  i18n("NE")  },
	{ 57,  i18n("ENE") },
	{ 79,  i18n("E")   }, 
	{ 102, i18n("ESE") },
	{ 124, i18n("SE")  },
	{ 147, i18n("SSE") },
	{ 169, i18n("S")   },
	{ 192, i18n("SSW") },
	{ 214, i18n("SW")  },
	{ 237, i18n("WSW") },
	{ 259, i18n("W")   },
	{ 282, i18n("WNW") },
	{ 304, i18n("NW")  },
	{ 327, i18n("NNW") },
	{ 349, i18n("N")   },
	{ 360, i18n("N")   }
};


QString MetarParser::parseWindDirection(const unsigned int direction)
{
	unsigned int i = 0;
	
	for (i = 0; i < (sizeof(wind_direction) / sizeof(wind_info)) - 1; i++)
	{
		if (direction >= wind_direction[i].number &&
		    direction < wind_direction[i + 1].number)
		{
			break;
		}
	}
	
	return wind_direction[i].name;
}

/** Parse the wind speed */
bool MetarParser::parseWindSpeed(const QString &s)
{
	if (WindRegExp.search(s) > -1)
	{
		unsigned int direction = WindRegExp.cap(1).toInt();
		float windSpeed = WindRegExp.cap(2).toFloat();
		float gustSpeed = WindRegExp.cap(3).toFloat();
		QString sWindUnit = WindRegExp.cap(4);

		kdDebug(12006) << "Wind: " << WindRegExp.capturedTexts().join("-")
				<< endl;

		if (m_units == KLocale::Metric)
		{
			if (sWindUnit == "KT")
			{
				windSpeed = (windSpeed * 3.6 / 1.94);
				gustSpeed = (gustSpeed * 3.6 / 1.94);
			}
			else if (sWindUnit == "MPS")
			{
				windSpeed = (windSpeed * 3.6);
				gustSpeed = (gustSpeed * 3.6);
			}
			weatherInfo.windMPH = (windSpeed / 1.61);
			weatherInfo.qsWindSpeed = i18n("1 km/h", "%n km/h", (int) windSpeed);
		}
		else
		{
			if (sWindUnit == "KT")
			{
				windSpeed = (windSpeed * 2.24 / 1.94);
				gustSpeed = (gustSpeed * 2.24 / 1.94);
			}
			else if (sWindUnit == "KMH")
			{
				windSpeed = (windSpeed / 1.61);
				gustSpeed = (gustSpeed / 1.61);
			}
			else if (sWindUnit == "MPS")
			{
				windSpeed = (windSpeed * 2.24);
				gustSpeed = (gustSpeed * 2.24);
			}
			weatherInfo.windMPH = windSpeed;
			weatherInfo.qsWindSpeed = i18n("1 MPH", "%n MPH", (int) windSpeed);
		}

		if (gustSpeed >= 1)
		{
			if (m_units == KLocale::Metric)
			{
				weatherInfo.qsCurrentList << i18n("Wind gusts up to 1 km/h", 
			     "Wind gusts up to %n km/h", (int) gustSpeed);
			}
			else
			{
				weatherInfo.qsCurrentList << i18n("Wind gusts up to 1 MPH", 
			     "Wind gusts up to %n MPH", (int) gustSpeed);
			}
		}

		if ((WindRegExp.cap(1) != "VRB") && (windSpeed >= 1))
		{
			weatherInfo.qsWindDirection = parseWindDirection(direction);
		}
		return true;
	}
	return false;
}

bool MetarParser::parseStationNeedsMaintenance(const QString &s)
{
	if (s == "$")
	{
		weatherInfo.stationNeedsMaintenance = true;
		kdDebug(12006) << "Station Needs Maintenance" << endl;
		return true;
	}
	
	return false;
}

void MetarParser::calcCurrentIcon()
{
	// Default to overcast clouds
	if ( weatherInfo.clouds == -1 )
		weatherInfo.clouds = 64;

	if (weatherInfo.theWeather.isEmpty())
	{
		if (weatherInfo.clouds == 0)
			weatherInfo.theWeather = iconName("sunny");
		else if (weatherInfo.clouds > 0 && weatherInfo.clouds <= 2)
			weatherInfo.theWeather = iconName("cloudy1");
		else if ( weatherInfo.clouds > 2 && weatherInfo.clouds <= 4)
			weatherInfo.theWeather = iconName("cloudy2");
		else if ( weatherInfo.clouds > 4 && weatherInfo.clouds <= 8)
			weatherInfo.theWeather = iconName("cloudy3");
		else if ( weatherInfo.clouds > 8 && weatherInfo.clouds < 63)
			weatherInfo.theWeather = iconName( "cloudy4" );
		else
			weatherInfo.theWeather = "cloudy5";
	}
	else if (weatherInfo.theWeather == "tstorm")
	{
		if ( weatherInfo.heavy )
			weatherInfo.clouds = 30;

		if (weatherInfo.clouds >= 0 && weatherInfo.clouds <= 10)
			weatherInfo.theWeather = iconName("tstorm1");
		else if ( weatherInfo.clouds > 10 && weatherInfo.clouds <= 20)
			weatherInfo.theWeather = iconName("tstorm2");
		else
			weatherInfo.theWeather = "tstorm3";
	}
	else if (weatherInfo.theWeather == "shower")
	{
		if ( weatherInfo.heavy )
			weatherInfo.clouds = 30;

		if (weatherInfo.clouds >= 0 && weatherInfo.clouds <= 10)
			weatherInfo.theWeather = iconName("shower1");
		else if ( weatherInfo.clouds > 10 && weatherInfo.clouds <= 20)
			weatherInfo.theWeather = iconName("shower2");
		else
			weatherInfo.theWeather = "shower3";
	}
	else if (weatherInfo.theWeather == "snow")
	{
		if ( weatherInfo.heavy )
			weatherInfo.clouds = 30;

		if (weatherInfo.clouds >= 0 && weatherInfo.clouds <= 8)
			weatherInfo.theWeather = iconName("snow1");
		else if ( weatherInfo.clouds > 8 && weatherInfo.clouds <= 16)
			weatherInfo.theWeather = iconName("snow2");
		else if (weatherInfo.clouds > 16 && weatherInfo.clouds <= 24)
			weatherInfo.theWeather = iconName("snow3");
		else
			weatherInfo.theWeather = "snow5";
	}
	else if (isNight(weatherInfo.reportLocation) && weatherInfo.theWeather == "mist")
		weatherInfo.theWeather = "mist_night";
	else if (isNight(weatherInfo.reportLocation) && weatherInfo.theWeather == "fog")
		weatherInfo.theWeather = "fog_night";
	else if ( weatherInfo.theWeather == "mist" || weatherInfo.theWeather == "fog" )
	{
		if ( weatherInfo.clouds >= 63 )
			weatherInfo.theWeather = "cloudy5";
	}

	kdDebug(12006) << "Clouds: " << weatherInfo.clouds << ", Icon: "
			<< weatherInfo.theWeather << endl;
}

void MetarParser::calcWindChill()
{
	float windChill = 35.74 + (0.6215 * TEMP_C_TO_F(weatherInfo.tempC))
			- (35.75 * ::pow(weatherInfo.windMPH, 0.16))
			+ (0.4275 * TEMP_C_TO_F(weatherInfo.tempC) * ::pow(weatherInfo.windMPH, 0.16));

	kdDebug(12006) << "Wind Chill: " << windChill << endl;

	if (windChill < 48)
	{
		if (m_units == KLocale::Metric)
		{
			weatherInfo.qsWindChill.setNum(TEMP_F_TO_C(windChill), 'f', 1);
			removeTrailingDotZero(weatherInfo.qsWindChill);
			weatherInfo.qsWindChill += i18n("°C");
		}
		else
		{
			weatherInfo.qsWindChill.setNum(windChill, 'f', 1);
			removeTrailingDotZero(weatherInfo.qsWindChill);
			weatherInfo.qsWindChill += i18n("°F");
		}
	}
}

bool MetarParser::isNight(const QString &stationID) const
{
	QString upperStationID = stationID.upper();
	QString latitude  = m_stationDb->stationLatitudeFromID(upperStationID);
	QString longitude = m_stationDb->stationLongitudeFromID(upperStationID);

	if ( latitude.compare(  i18n("Unknown Station" ) ) == 0  ||
	     longitude.compare( i18n("Unknown Station" ) ) == 0 )
	{
		return false;
	}
	else
	{
		Sun theSun( latitude, longitude , m_date, m_localUTCOffset );
		
		QTime currently = m_time;

		QTime civilStart = theSun.computeCivilTwilightStart();
		QTime civilEnd   = theSun.computeCivilTwilightEnd();

		kdDebug (12006) << "station, current, lat, lon, start, end, offset: " << 
				upperStationID << " " << currently << " " << latitude << " " << 
				longitude << " " << civilStart << " " << civilEnd << " " <<
				m_localUTCOffset << endl;

		if (civilStart != civilEnd)
		{
			if (civilEnd < civilStart)
				/* Handle daylight past midnight in local time     */
				/* for weather stations located at other timezones */
				return (currently < civilStart && currently > civilEnd);
			else
				return (currently < civilStart || currently > civilEnd);
		}
		else
		{
			// Midnight Sun & Polar Night - In summer, the Sun is always 
			// over the horizon line ... so use latitude & today date to 
			// set isNight() value. 
			return ((m_date.daysInYear() >= 80 || m_date.daysInYear() <= 264) && latitude.contains("S")); 
		}
	}
}

QString MetarParser::iconName( const QString &icon ) const
{
	QString _iconName = icon;

	if ( isNight( weatherInfo.reportLocation ) )
		_iconName += "_night";

	return _iconName;
}

