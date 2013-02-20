/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue May  1 02:59:33 BST 2001
    copyright            : (C) 2001 by Richard Moore
    email                : rich@kde.org
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
#include <klocale.h>

#include "spy.h"

static const char description[] =
	I18N_NOOP("Spy");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  KCmdLineLastOption
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

  KAboutData aboutData( "spy", I18N_NOOP("Spy"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2001, Richard Moore", 0, 0, "rich@kde.org");
  aboutData.addAuthor("Richard Moore",0, "rich@kde.org");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication a;
  Spy *spy = new Spy();
  a.setMainWidget(spy);

  spy->show();

  return a.exec();
}
