/***************************************************************************
    begin                : Tue Dec 18 17:47:22 GMT 2001
    copyright            : (C) 2001-2004 by George Wright
    email                : gwright@kde.org
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

#include "klatin.h"

static const char description[] =
	I18N_NOOP("KLatin - a program to help revise Latin");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


static KCmdLineOptions options[] =
{
	KCmdLineLastOption
};

int main(int argc, char *argv[])
{

	KAboutData aboutData( "klatin", I18N_NOOP("KLatin"),
	"0.9", description, KAboutData::License_GPL,
	"(C) 2001-2004, George Wright", 0, "http://edu.kde.org/klatin", "submit@bugs.kde.org");

	aboutData.addAuthor("George Wright",I18N_NOOP("Author and Maintainer"), "gwright@kde.org");
	aboutData.addCredit("Anne-Marie Mahfouf",I18N_NOOP("Bug fixes, documentation"), "annma@kde.org");
	aboutData.addCredit("Neil Stevens",I18N_NOOP("Vocabulary shuffling code"), "neil@hakubi.us");
	
	aboutData.addCredit("Danny Allen",I18N_NOOP("SVG icon"), "danny@dannyallen.co.uk");
	aboutData.addCredit("Pino Toscano",I18N_NOOP("Bug fixes and code improvement, Italian Vocabulary Data Translation"), "toscano.pino@tiscali.it");
	aboutData.addCredit("Mikolaj Machowski",I18N_NOOP("Polish Vocabulary Data Translation"), "mikmach@wp.pl");

	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KApplication a;
	
	KLatin *klatin = new KLatin();
	
	if (a.isRestored()) {
		RESTORE(KLatin);
	} else {
		a.setMainWidget(klatin);
		klatin->show();
	}
	
	return a.exec();
}
