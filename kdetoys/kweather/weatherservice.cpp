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

#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include "weatherlib.h"
#include "weatherservice.h"
#include "stationdatabase.h"
#include "sun.h"

WeatherService::WeatherService(QObject *parent, const char *name) : QObject (parent, name),  DCOPObject("WeatherService")
{
	kdDebug(12006) << "Starting new service... " << endl;

	stationDB = new StationDatabase();

	m_weatherLib = new WeatherLib(stationDB, this, "WeatherLib");
	connect(m_weatherLib, SIGNAL(fileUpdating( const QString&)),
			SLOT(updating( const QString&)));
	connect(m_weatherLib, SIGNAL(fileUpdate( const QString&)),
			SLOT(updated( const QString&)));
	connect(m_weatherLib, SIGNAL(stationRemoved(const QString&)),
			SLOT(slotStationRemoved(const QString&)));

	KConfig *conf = kapp->config();

	conf->setGroup("WEATHERSTATIONS");
	QStringList stations =conf->readListEntry("stations");
        QStringList::Iterator it = stations.begin();
	for ( ; it != stations.end(); ++it )
		m_weatherLib->update(*it);
}

WeatherService::~WeatherService()
{
	kdDebug(12006) << "Going away... " << endl;
	// Don't need to call saveSettings() because WeatherService::exit
        // already does that. Plus it wouldn't work here anyway because
        // we can't get the config after calling kapp->quit();
	delete stationDB;
}

void WeatherService::updated(const QString &stationID)
{
	kdDebug(12006) << "Sending update for " << stationID << endl;
	emit fileUpdate( stationID );
}

void WeatherService::updateAll()
{
	kdDebug(12006) << "Sending for all" << endl;
	QStringList stations =  m_weatherLib->stations();
	QStringList::ConstIterator end(stations.end());
	for  ( QStringList::ConstIterator it = stations.begin(); it != end; ++it ) {
		update(*it);
    	}

}

void WeatherService::updating(const QString &stationID)
{
	kdDebug(12006) << "Sending updating for " << stationID << endl;
	emit fileUpdating( stationID );
}

void WeatherService::slotStationRemoved(const QString &stationID)
{
  kdDebug(12006) << "Sending stationRemoved for " << stationID << endl;
  emit stationRemoved( stationID );
}

QString WeatherService::temperature(const QString &stationID)
{
	kdDebug (12006) << "Returning " << stationID << endl;
	return m_weatherLib->temperature(stationID);
}

QString WeatherService::dewPoint(const QString &stationID)
{
	return m_weatherLib->dewPoint(stationID);
}

QString WeatherService::relativeHumidity(const QString &stationID)
{
	return m_weatherLib->relHumidity(stationID);
}

QString WeatherService::heatIndex(const QString &stationID)
{
	return m_weatherLib->heatIndex(stationID);
}

QString WeatherService::windChill(const QString &stationID)
{
	return m_weatherLib->windChill(stationID);
}

QString WeatherService::wind(const QString &stationID)
{
	return m_weatherLib->wind(stationID);
}

QString WeatherService::pressure(const QString &stationID)
{
	return m_weatherLib->pressure(stationID);
}

QPixmap WeatherService::currentIcon(const QString &stationID)
{
    return icon( stationID );
}

QPixmap WeatherService::icon(const QString &stationID)
{
	kdDebug(12006) << "Get the current weather icon.." << endl;
	QString icon  = iconFileName(stationID);
	QPixmap theIcon = QPixmap(icon);
	return theIcon;
}

QString WeatherService::currentIconString(const QString &stationID)
{
	return m_weatherLib->iconName(stationID);
}

QString WeatherService::iconFileName(const QString &stationID)
{
	QString icon  = m_weatherLib->iconName(stationID);
	icon = locate( "data", "kweather/" + icon + ".png" );
	return icon;
}

QString WeatherService::date(const QString &stationID)
{
	return m_weatherLib->date(stationID);
}

QString WeatherService::visibility(const QString &stationID)
{
	return m_weatherLib->visibility(stationID);
}

QStringList WeatherService::cover(const QString &stationID)
{
	return m_weatherLib->cover(stationID);
}

QStringList WeatherService::weather(const QString &stationID)
{
	return m_weatherLib->weather(stationID);
}

bool WeatherService::stationNeedsMaintenance(const QString &stationID)
{
	return m_weatherLib->stationNeedsMaintenance(stationID);
}

void WeatherService::update(const QString &stationID)
{
	m_weatherLib->update(stationID);
}

void WeatherService::forceUpdate(const QString &stationID)
{
	m_weatherLib->forceUpdate(stationID);
}

void WeatherService::removeStation(const QString &stationID)
{
	m_weatherLib->remove(stationID);
	saveSettings();
}

void WeatherService::addStation(const QString &stationID)
{
	m_weatherLib->update(stationID);
	saveSettings();
}

void WeatherService::exit()
{
	saveSettings();
	kapp->quit();
}

QStringList WeatherService::listStations()
{
	return m_weatherLib->stations();
}

void WeatherService::saveSettings()
{
	KConfig *conf = kapp->config();
	conf->setGroup("WEATHERSTATIONS");
	conf->writeEntry( "stations", m_weatherLib->stations());
	conf->sync();
}

QString WeatherService::stationName(const QString &stationID)
{
	if ( stationDB )
	{
		QString upperStationID = stationID.upper();
		return stationDB->stationNameFromID(upperStationID);
	}
	else
		return stationID;
}
QString WeatherService::stationCode( const QString &stationName )
{
	if ( stationDB )
	{
		return stationDB->stationIDfromName(stationName);
	}
	else
		return stationName;
}

QString WeatherService::stationCountry(const QString &stationID)
{

	if ( stationDB )
	{
		QString upperStationID = stationID.upper();
		return stationDB->stationCountryFromID(upperStationID);
	}
	else
		return stationID;
}
QString WeatherService::longitude(const QString &stationID)
{
	if ( stationDB )
	{
		QString upperStationID = stationID.upper();
		return stationDB->stationLongitudeFromID(upperStationID);
	}
	else
		return "None";
}
QString WeatherService::latitude(const QString &stationID)
{
	if ( stationDB )
	{
		QString upperStationID = stationID.upper();
		return stationDB->stationLatitudeFromID(upperStationID);
	}
	else
		return "None";
}

QStringList WeatherService::findStations(float /*lon*/, float /*lat*/)
{
	QStringList stationList;
	stationList << "KMKE" << "KPNE" << "KTPW";
	return stationList;
}

QString WeatherService::getTime(const QString &stationID, TimeType timeType)
{
	QString upperStationID = stationID.upper();

	QString latitude  = stationDB->stationLatitudeFromID(upperStationID);
	QString longitude = stationDB->stationLongitudeFromID(upperStationID);

	if ( latitude.compare(  i18n("Unknown Station" ) ) == 0  ||
	     longitude.compare( i18n("Unknown Station" ) ) == 0 )
	{
		return i18n( "Unknown Station" );
	}
	else
	{
		Sun theSun( latitude, longitude );
		
		QTime time;
		switch ( timeType )
		{
			case RISE:
				time = theSun.computeRiseTime();
				break;
			case SET:
				time = theSun.computeSetTime();
				break;
			case CIVIL_START:
				time = theSun.computeCivilTwilightStart();
				break;
			case CIVIL_END:
				time = theSun.computeCivilTwilightEnd();
				break;
		}

		kdDebug(12006) << "station, lat, lon, time: " << upperStationID << " " <<
			latitude << " " << longitude << " " << time << endl;

		return KGlobal::locale()->formatTime(time);
	}
}

QString WeatherService::sunRiseTime(const QString &stationID)
{
	return getTime(stationID, RISE);
}

QString WeatherService::sunSetTime(const QString &stationID)
{
	return getTime(stationID, SET);
}

QString WeatherService::civilTwilightStart(const QString &stationID)
{
	return getTime(stationID, CIVIL_START);
}

QString WeatherService::civilTwilightEnd(const QString &stationID)
{
	return getTime(stationID, CIVIL_END);
}



#include "weatherservice.moc"
