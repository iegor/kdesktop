/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Fre Dez 14 19:28:09 CET 2001
    copyright            : (C) 2001 by Arnold Kraschinski
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

#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "kverbos.h"

static const char description[] =
	I18N_NOOP("KVerbos");

static KCmdLineOptions options[] =
{
  { "+[File]", I18N_NOOP("File to open"), 0 },
  KCmdLineLastOption
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};



int main(int argc, char *argv[])
{

	KAboutData aboutData( "kverbos", I18N_NOOP("KVerbos"),
		VERSION, description, KAboutData::License_GPL,
		"(c) 2001, Arnold Kraschinski", 0, 0, "arnold.k67@gmx.de");
	aboutData.addAuthor("Arnold Kraschinski",0, "arnold.k67@gmx.de");
	aboutData.addCredit("David Vignoni", I18N_NOOP("svg icon"), "david80v@tin.it");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KApplication app;

	if (app.isRestored())
	{
		RESTORE(KVerbosApp);
	}
	else
	{
		KVerbosApp *kverbos = new KVerbosApp();
		kverbos->show();

		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

		if (args->count())
		{
			kverbos->openDocumentFile(args->url(0));
		}
		else
		{
			// Meiner Meinung nach geh�t die folgende Zeile nicht hier hin, weil hier das
			// Programm durchl�ft, wenn kein Dateiname ber die Kommandozeile eingegeben
			// worden ist.
			// kverbos->openDocumentFile();
		}
		args->clear();
	}

	return app.exec();
}
