/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue Jul  9 23:14:22 EDT 2002
    copyright            : (C) 2002 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kdebug.h>

#include "ksig.h"
#include "siggenerator.h"

static const char description[] = I18N_NOOP("KSig");
	
static KCmdLineOptions options[] =
{
    { "random", I18N_NOOP("Display a random signature"), 0},
    { "daily", I18N_NOOP("Display a signature for the day"), 0},
    KCmdLineLastOption
};

int main(int argc, char *argv[])
{

    KAboutData aboutData("ksig", I18N_NOOP("KSig"),
			 "1.1", description, KAboutData::License_GPL,
			 "(c) 2002, Scott Wheeler", 0, 0, "wheeler@kde.org");

    aboutData.addAuthor("Scott Wheeler", 0, "wheeler@kde.org");
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(options);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if(args->isSet("random") || args->isSet("daily")) {
	KApplication a(false, false);
	SigGenerator generator;

	QTextStream output(stdout, IO_WriteOnly);
	if(args->isSet("random"))
	    output << generator.random();
	else if(args->isSet("daily"))
	    output << generator.daily();

	return(0);
    }
    else {
	KApplication a;

	KSig *ksig = new KSig();
	a.setMainWidget(ksig);
	ksig->show();
	return a.exec();
    }
}
