/****************************************************************************
 *    stationdatabase_test.cpp  -  Test Program for StationDatabase Class
 *                          -------------------
 *         begin                : Friday June 4 2004
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
#include <cstdlib>
using namespace std;

#include <qfile.h>
#include <qstring.h>
#include <qdatetime.h>
#include <krfcdate.h>

#include "stationdatabase.h"


int main()
{
	bool anyFailed = false;
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
	
	StationDatabase stationDb(path);
	
	cout << "Testing station name from ID with known station...";
	
	if (stationDb.stationNameFromID("KORD") == "Chicago, Chicago-O'Hare International Airport")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing station name from ID with unknown station...";
	
	if (stationDb.stationNameFromID("KXYZ") == "Unknown Station")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing station name from ID with known station already loaded...";
	
	// station should be cached this time.
	if (stationDb.stationNameFromID("KORD") == "Chicago, Chicago-O'Hare International Airport")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}

	cout << "Testing latitude from ID with known station...";
	
	if (stationDb.stationLatitudeFromID("KPWK") == "42-07-15N")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing latitude from ID with unknown station...";
	
	if (stationDb.stationLatitudeFromID("KZZZ") == "Unknown Station")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing latitude from ID with known station already loaded...";
	
	if (stationDb.stationLatitudeFromID("KPWK") == "42-07-15N")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing longitude from ID with known station...";
	
	if (stationDb.stationLongitudeFromID("KAAA") == "089-20-06W")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing longitude from ID with unknown station...";
	
	if (stationDb.stationLongitudeFromID("KZYY") == "Unknown Station")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing longitude from ID with known station already loaded...";
	
	if (stationDb.stationLongitudeFromID("KAAA") == "089-20-06W")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing country from ID with known station...";
	
	if (stationDb.stationCountryFromID("KUGN") == "United States")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing country from ID with unknown station...";
	
	if (stationDb.stationCountryFromID("KYYY") == "Unknown Station")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing country from ID with known station already loaded...";
	
	if (stationDb.stationCountryFromID("KUGN") == "United States")
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
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
