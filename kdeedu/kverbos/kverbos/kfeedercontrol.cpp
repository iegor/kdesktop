/***************************************************************************
                          kfeedercontrol.cpp  -  description
                             -------------------
    begin                : Wed Apr 17 2002
    copyright            : (C) 2002 by Arnold Kraschinski
    email                : arnold.k67@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kfeedercontrol.h"

#include <kapplication.h>
#include <kstandarddirs.h>

KFeederControl::KFeederControl(int ID, bool splash, bool automatic)
{
	QString f = KStandardDirs::findExe("kfeeder");
	QString s;
	s.setNum(ID);
	childFeeder << f;
	if (splash)
		childFeeder << "-s" << "yes";
	else
		childFeeder << "-s" << "no";
	if (automatic)
		childFeeder << "-m" << "automatic";
	else
		childFeeder << "-m" << "triggered";
	if (!childFeeder.start())
		active = false;
	else
	{
		feederID.setNum(childFeeder.pid());
		feederID = "KFeeder-" + feederID;

		dcop = kapp->dcopClient();
		dcop->attach();
		appId = dcop->registerAs("KVerbos");	
		active = true;
	};
}

KFeederControl::~KFeederControl()
{
}

// resets the KFeeder
bool KFeederControl::reset()
{
	if (active)
	{
		QByteArray data;								// "raw support" for data
		QDataStream	arg(data, IO_WriteOnly);			// "container" provides
														// easy access to data
		int a_number = 1;
		arg << a_number;								// put information on the
														// "support" in the "container"
		lastResult = dcop->send(feederID,				// identify the recipient
										"kfeeder",		//hierarchically designate
														// the targeted object
										"reset()",		// signature of the method
														// that will handle sent data
										data);			// the data
	};
	return active && lastResult;
}

bool KFeederControl::addOne()
{
	if (active)
	{
		QByteArray data;								// "raw support" for data
		QDataStream	arg(data, IO_WriteOnly);			// "container" provides
														// easy access to data
		int a_number = 1;
		arg << a_number;								// put information on the
		// "support" in the "container"
		lastResult = dcop->send(feederID,				// identify the recipient
										"kfeeder",		//hierarchically designate
														// the targeted object
										"addOne()",		// signature of the method
														// that will handle sent data
										data);			// the data
	};
	return active && lastResult;
}

bool KFeederControl::addN(int n)
{
	if (active)
	{
		QByteArray data;								// "raw support" for data
		QDataStream	arg(data, IO_WriteOnly);			// "container" provides
														// easy access to data
		int a_number = n;
		arg << a_number;								// put information on the
		// "support" in the "container"
		lastResult = dcop->send(feederID,				// identify the recipient
										"kfeeder",		// hierarchically designate
														// the targeted object
										"addN(int)",	// signature of the method
														// that will handle sent data
										data);			// the data
	};
	return active && lastResult;
}

// sets the type of food movement. 'true' the food is moved automatically.
// 'false' it is necessary to trigger the food movement. */
bool KFeederControl::setAutomatic(bool a)
{
	if (active)
	{
		QByteArray data;								// "raw support" for data
		QDataStream	arg(data, IO_WriteOnly);			// "container" provides
														// easy access to data
		int num;
		if (a)
			num = 1;									// put information on the
		else
			num = 0;
		arg << num;
														// "support" in the "container"
		lastResult = dcop->send(feederID,				// identify the recipient
						"kfeeder",						//hierarchically designate
														// the targeted object
						"setAutomatic(int)",			// signature of the method
														// that will handle sent data
						data);							// the data
	};
	return active && lastResult;
}
