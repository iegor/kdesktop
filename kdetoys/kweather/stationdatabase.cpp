//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: ian reinhart geiser <geiseri@yahoo.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "stationdatabase.h"

#include <qstringlist.h>
#include <qfile.h>
#include <kdebug.h>

class StationInfo
{
	public:
		QString cityName;
		QString country;
		QString longitude;
		QString latitude;
		StationInfo () {}
};

StationDatabase::StationDatabase(const QString path) : mPath(path)
{
}


StationDatabase::~StationDatabase()
{
}

bool StationDatabase::loadStation( const QString & stationID )
{
	QFile file( mPath );
	bool found = FALSE;

	if ( !file.open( IO_ReadOnly ) )
		return false;

	QTextStream stream( &file );
	stream.setEncoding( QTextStream::UnicodeUTF8 );
	QString line;
	while ( !stream.eof() )
	{
		line = stream.readLine(); // line of text excluding '\n'
		QStringList data = QStringList::split( ";", line, true );

		if ( data[ 0 ].stripWhiteSpace() == stationID )
		{
			StationInfo station;
			station.cityName = data[ 3 ].stripWhiteSpace();
			station.country = data[ 5 ].stripWhiteSpace();
			station.latitude = data[ 7 ].stripWhiteSpace();
			station.longitude = data[ 8 ].stripWhiteSpace();

			theDB.insert( data[ 0 ], station );
			found = TRUE;
			break;
		}
	}

	file.close();
	return found;
}

/*!
    \fn StationDatabase::stationNameFromID(const QString& id)
 */
QString StationDatabase::stationNameFromID( const QString & stationID )
{
	QString result;
	
	if ( theDB.find( stationID ) == theDB.end() )
	{
		if ( loadStation( stationID ) )
			result = theDB[ stationID ].cityName;
		else
			result = i18n( "Unknown Station" );
	}
	else
	{
		result = theDB[ stationID ].cityName;
	}

	return result;
}

/*!
    \fn StationDatabase::stationLongitudeFromID( const QString &stationID)
 */
QString StationDatabase::stationLongitudeFromID( const QString & stationID )
{
	QString result;
	
	if ( theDB.find( stationID ) == theDB.end() )
	{
		if ( loadStation( stationID ) )
			result = theDB[ stationID ].longitude;
		else
			result = i18n( "Unknown Station" );
	}
	else
	{
		result = theDB[ stationID ].longitude;
	}

	return result;
}

/*!
    \fn StationDatabase::stationLatitudeFromID(const QString &stationID)
 */
QString StationDatabase::stationLatitudeFromID( const QString & stationID )
{
	QString result;
	
	if ( theDB.find( stationID ) == theDB.end() )
	{
		if ( loadStation( stationID ) )
			result = theDB[ stationID ].latitude;
		else
			result = i18n( "Unknown Station" );
	}
	else
	{
		result = theDB[ stationID ].latitude;
	}

	return result;
}

/*!
    \fn StationDatabase::stationCountryFromID( const QString &stationID)
 */
QString StationDatabase::stationCountryFromID( const QString &stationID )
{
	QString result;
	
	if ( theDB.find( stationID ) == theDB.end() )
	{
		if ( loadStation( stationID ) )
			result = theDB[ stationID ].country;
		else
			result = i18n( "Unknown Station" );
	}
	else
	{
		result = theDB[ stationID ].country;
	}

	return result;
}

QString StationDatabase::stationIDfromName( const QString &name )
{
	QMap<QString,StationInfo>::Iterator itr = theDB.begin();
	for( ; itr != theDB.end(); ++itr)
	{
	  kdDebug() << "Checking " << itr.data().cityName << endl;
	  if( itr.data().cityName == name )
		return itr.key();
	}
	return "0000";
}
