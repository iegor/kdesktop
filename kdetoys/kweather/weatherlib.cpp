/***************************************************************************
			weatherlib.cpp  -  description
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

#include "config.h"
#include <qfile.h>
#include <qdatetime.h>
#include <qtextstream.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <math.h>
#include <krfcdate.h>
#include <kio/job.h>
#include <kurl.h>
#include <knotifyclient.h>
#include <ktempfile.h>
#include <unistd.h>
#include <kapplication.h>
#include <kpassivepopup.h>

#include "metar_parser.h"
#include "stationdatabase.h"
#include "sun.h"
#include "weatherlib.h"

#include "weatherlib.moc"


class WeatherLib::Data
{
	public:
		Data();
		~Data(){ if ( target ) delete target; }

		void clear();

		/** The current weather state outside */
		struct WeatherInfo wi;
		QDateTime age;
		KTempFile *target;
		bool downloading;
		bool updated;
		KIO::Job *job;
};

WeatherLib::Data::Data()
		: target( 0 ), job( 0 )
{
	clear();
}

void WeatherLib::Data::clear()
{
	age = QDateTime::currentDateTime();
	downloading = false;
	updated = false;
	job = 0;

	if ( target )
	{
		delete target;
		target = 0;
	}
}

WeatherLib::WeatherLib(StationDatabase *stationDB, QObject *parent, const char *name)
	: QObject (parent, name)
{
	KGlobal::locale()->insertCatalogue("kweather");
	
	m_StationDb    = stationDB;
	
	data.setAutoDelete( true );
}

WeatherLib::~WeatherLib()
{
}

void WeatherLib::slotCopyDone(KIO::Job* job)
{
	kdDebug(12006) << "Copy done..." << endl;
	if( job->error())
	{
		kdDebug(12006) << "Error code: " << job->error() << endl;
		//job->showErrorDialog(0L);
		if ((job->error() == KIO::ERR_COULD_NOT_CONNECT || job->error() == KIO::ERR_UNKNOWN_HOST)
		    && !hostDown)
		{
			hostDown= true;
      // no need to show a passive popup here, as below all stations will show "dunno" icon
		}
	}
	// Find the job
	QDictIterator<Data> it( data );
	Data *d = 0L;
	for( ; it.current(); ++it )
	{
		kdDebug(12006) << "Searching for job..." << endl;
		if(it.current()->job == job)
		{
			d = it.current();
			d->downloading = false;
			if( !job->error() )
			{
				kdDebug( 12006) << "Reading: " << d->target->name() << endl;
				QFile file( d->target->name() );
				file.open( IO_ReadOnly );
				QTextStream *t = new QTextStream( &file );
				//QTextStream *t = d->target->textStream();
				if( t )
				{
					QString s = QString::null;
					while ( !t->eof() )
					{
						s += " " + t->readLine();
					}

					if ( !s.isEmpty() )
					{
						kdDebug( 12006 ) << "Parse: " << s << endl;
						MetarParser parser(m_StationDb, KGlobal::locale()->measureSystem());
						d->wi = parser.processData(d->wi.reportLocation, s);
						d->age = QDateTime::currentDateTime().addSecs(1800);
						emit fileUpdate(d->wi.reportLocation);
						d->updated = true;
					}
					else
					{
						// File error
						kdDebug( 12006 ) << "File empty error..." << endl;
						KPassivePopup::message( i18n("KWeather Error!"),
						i18n("The temp file %1 was empty.").arg(d->target->name()),  0L,"error" );
						d->updated = false;
					}
				}
				else
				{
					// File error
					kdDebug( 12006 ) << "File read error..." << endl;
					KPassivePopup::message( i18n("KWeather Error!"),
				i18n("Could not read the temp file %1.").arg(d->target->name()),  0L,"error" );
					d->updated = false;
				}
				delete d->target;
				d->target = 0L;
				d->job = 0L;

			}
			else if( job->error()  == KIO::ERR_DOES_NOT_EXIST)
			{
				data.remove(d->wi.reportLocation);
				kdDebug( 12006 ) << "Bad station data so i am going to remove it" << endl;
				KPassivePopup::message( i18n("KWeather Error!"),
				i18n("The requested station does not exist."),  0L,"error" );
			}
			else if(job->error() == KIO::ERR_COULD_NOT_CONNECT ||
				job->error() == KIO::ERR_UNKNOWN_HOST)
			{
				kdDebug( 12006 ) << "Offline now..." << endl;
				d->clear();
				d->wi.theWeather = "dunno";
				d->wi.qsCurrentList.append(i18n("The network is currently offline..."));
				d->wi.qsCurrentList.append(i18n("Please update later."));
				emit fileUpdate(d->wi.reportLocation);
			}
			else
			{
				kdDebug( 12006 ) << "Duh?..." << endl;
			}

		}
	}
}

void WeatherLib::getData(Data *d, bool force /* try even if host was down last time*/)
{
	if(!d->downloading && (force || !hostDown) )
	{
		d->downloading = true;
		d->updated = false;
		QString u = "http://weather.noaa.gov/pub/data/observations/metar/stations/";
		u += d->wi.reportLocation.upper().stripWhiteSpace();
		u += ".TXT";

		d->target = new KTempFile(QString::null, "-weather");
		d->target->setAutoDelete(true);
		d->target->file();

		KURL url(u);
		KURL local(d->target->name());

		d->job = KIO::file_copy( url, local, -1, true, false, false);
		d->job->addMetaData("cache", "reload"); // Make sure to get fresh info
		connect( d->job, SIGNAL( result( KIO::Job *)),
			SLOT(slotCopyDone(KIO::Job *)));
		kdDebug( 12006 ) << "Copying " << url.prettyURL() << " to "
			<< local.prettyURL() << endl;
		emit fileUpdating(d->wi.reportLocation);
	}
}

WeatherLib::Data* WeatherLib::findData(const QString &stationID)
{
	Data *d = data[stationID];
	if (!d)
	{
		d = new Data();
		d->wi.reportLocation = stationID;
		d->wi.theWeather = "dunno";
		d->wi.qsCurrentList.append( i18n( "Retrieving weather data..." ) );
		data.insert(stationID, d);
		getData(d);
	}

	return d;
}

QString WeatherLib::temperature(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsTemperature;
}

QString WeatherLib::pressure(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsPressure;
}

QString WeatherLib::wind(const QString &stationID){
	Data *d = findData(stationID);
	return (d->wi.qsWindSpeed + " " + d->wi.qsWindDirection);
}

/**  */
QString WeatherLib::dewPoint(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsDewPoint;
}

QString WeatherLib::relHumidity(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsRelHumidity;
}

QString WeatherLib::heatIndex(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsHeatIndex;
}

QString WeatherLib::windChill(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsWindChill;
}

QString WeatherLib::iconName(const QString &stationID){

	QString result("dunno");
		
	// isEmpty is true for null or 0 length strings
	if ( !stationID.isEmpty() )
	{
		Data *d = findData(stationID);
		result = d->wi.theWeather;
	}
	
	return result;
}

QString WeatherLib::date(const QString &stationID){
	Data *d = findData(stationID);

  if ( ! d->wi.qsDate.isValid() )
    return "";
  else
  {
    QDateTime gmtDateTime(d->wi.qsDate, d->wi.qsTime);
    QDateTime localDateTime = gmtDateTime.addSecs(KRFCDate::localUTCOffset() * 60);
    return KGlobal::locale()->formatDateTime(localDateTime, false, false);
  }
}

/** Returns the current cover */
QStringList WeatherLib::cover(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsCoverList;
}

/** return the visibility */
QString WeatherLib::visibility(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsVisibility;
}

/** return the weather text */
QStringList WeatherLib::weather(const QString &stationID){
	Data *d = findData(stationID);
	return d->wi.qsCurrentList;
}

bool WeatherLib::stationNeedsMaintenance(const QString &stationID)
{
	Data *d = findData(stationID);
	return d->wi.stationNeedsMaintenance;
}

void WeatherLib::update(const QString &stationID)
{
	// Only grab new data if its more than 50 minutes old
	Data *d = findData(stationID);

	QDateTime timeout = QDateTime::currentDateTime();

	kdDebug (12006) << "Current Time: " << KGlobal::locale()->formatDateTime(timeout, false, false) <<
			" Update at: " << KGlobal::locale()->formatDateTime(d->age, false, false) << endl;
	if( timeout >= d->age || !d->updated)
		getData(d, true /* try even if host was down last time */);
	else
		emit fileUpdate(d->wi.reportLocation);
}

QStringList WeatherLib::stations()
{
	QStringList l;
	QDictIterator<Data> it( data );
	for( ; it.current(); ++it )
		l += it.currentKey();
	return l;
}

void WeatherLib::forceUpdate(const QString &stationID)
{
	hostDown = false; // we want to show error message if host is still down
	Data *d = findData(stationID);
	getData( d );
}

void WeatherLib::remove(const QString &stationID)
{
	data.remove(stationID);
	emit stationRemoved(stationID);
}
