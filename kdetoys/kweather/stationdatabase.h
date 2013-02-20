//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: ian reinhart geiser <geiseri@yahoo.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef STATIONDATABASE_H
#define STATIONDATABASE_H

#include <qmap.h>
#include <qstring.h>

#include <klocale.h>
#include <kstandarddirs.h>

/**
This is the main database for mapping METAR codes to Station information.

@author ian reinhart geiser
*/

class StationInfo;

class StationDatabase
{
public:
    StationDatabase(const QString path = locate("data", "kweatherservice/stations.dat"));
    ~StationDatabase();

    QString stationNameFromID(const QString& id);

    QString stationLongitudeFromID( const QString &stationID);

    QString stationLatitudeFromID(const QString &stationID);

    QString stationCountryFromID( const QString &stationID);
    
    QString stationIDfromName( const QString &name );

private:
    QMap<QString, StationInfo> theDB;

    bool loadStation( const QString & stationID );

    const QString mPath;
};

#endif
