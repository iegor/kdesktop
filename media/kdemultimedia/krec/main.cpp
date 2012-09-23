/***************************************************************************
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

// Should stay here because I use it for Start-/Stop-Info (see below)
//#include <iostream>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "krecord.h"

#include <qobject.h>

using namespace std;

static const char description[] =
	I18N_NOOP("\
This is a recording tool for KDE.\n\
It uses aRts, just look at the audiomanager\n\
and you will find it there accepting sound\n\
for recording."
	);

static KCmdLineOptions options[] =
{
	KCmdLineLastOption
	// INSERT YOUR COMMANDLINE OPTIONS HERE
};

extern "C" KDE_EXPORT int kdemain( int argc, char* argv[] )
{
	//cout<<endl<<"Starting kRec..."<<endl;

	KAboutData aboutData( "krec", I18N_NOOP("KRec"),
		"0.5.1", description, KAboutData::License_GPL,
		"(c) 2002, 2003 Arnold Krille" );
	aboutData.addAuthor("Arnold Krille", I18N_NOOP( "Creator \nLook at the website www.arnoldarts.de \nfor other good stuff." ), "arnold@arnoldarts.de");
	aboutData.addCredit( "Matthias Kretz", I18N_NOOP( "Helped where he was asked" ), "kretz@kde.org" );
	aboutData.addCredit( "Stefan Asserhaell", I18N_NOOP( "Made some minor improvements" ) );
	aboutData.addCredit( "Stefan Asserhaell & Nikolas Zimmermann", I18N_NOOP( "They indirectly wrote the exports. At least I learned from their files and patches." ) );
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KApplication app;
	KRecord *w = new KRecord();
	w->show();
	app.setMainWidget(w);

	int quit = app.exec();
	//cout<<"Quitting with "<<quit<<endl;
	return quit;
}

// vim:sw=4:ts=4

