/****************************************************************************
 *              sun_test.cpp  -  Sun Rise and Set Test Program
 *                          -------------------
 *         begin                : Tuesday June 2 2004
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

#include <qstring.h>
#include <qdatetime.h>
#include <krfcdate.h>

#include "sun.h"


int main()
{
	bool anyFailed = false;
	
	QString KUGN_Latitude("42-25N");
	QString KUGN_Longitude("087-52W");
	QDate Date(2004, 6, 1);    // June 1st.
	int localUTCOffset = -300;
	
	// Construct a sun object for our tests.
	Sun theSun( KUGN_Latitude, KUGN_Longitude, Date, localUTCOffset );
	
	QTime civilStart = theSun.computeCivilTwilightStart();
	QTime civilEnd   = theSun.computeCivilTwilightEnd();
	
	cout << "Testing Civil Twilight Calculations...";
	// Start should be 04:42:39    End should be 20:56:06
	if (civilStart.hour() == 4 && civilStart.minute() == 42 && civilStart.second() == 39 &&
	    civilEnd.hour() == 20 && civilEnd.minute() == 56 && civilEnd.second() == 06)
	{
		cout << "passed" << endl;
	}
	else
	{
		cout << "failed" << endl;
		anyFailed = true;
	}
	
	cout << "Testing Rise and Set Time Calculations...";
	
	QTime rise = theSun.computeRiseTime();
	QTime set  = theSun.computeSetTime();
	
	// Rise should be 05:16:35   Set should be 20:22:10
	if (rise.hour() == 5 && rise.minute() == 16 && rise.second() == 35 &&
	    set.hour() == 20 && set.minute() == 22 && set.second() == 10)
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

