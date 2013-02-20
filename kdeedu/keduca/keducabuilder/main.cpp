/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : mié may 23 20:36:15 CEST 2001
    copyright            : (C) 2001 by Javier Campos
    email                : javi@asyris.org
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
#include <kapplication.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include <qfileinfo.h>
#include "keducabuilder.h"

static const char description[] = I18N_NOOP("Form-based tests and exams builder");
static const char version[]     = "1.2.0";

static KCmdLineOptions options[] =
{
    { "+file",          I18N_NOOP("File to load"), 0 },
    KCmdLineLastOption
};

int main(int argc, char *argv[])
{
    KAboutData aboutData( "keduca", I18N_NOOP("KEducaBuilder"),
                          version, description, KAboutData::License_GPL,
                          "(c) 2001, Javier Campos", 0, "http://keduca.sourceforge.net", "submit@bugs.kde.org");
    aboutData.addAuthor("Javier Campos",I18N_NOOP("Original Author"), "javi@asyris.org");
    aboutData.addAuthor("Klas Kalass",I18N_NOOP("Maintainer 2002-2004"), "klas@kde.org");
    aboutData.addAuthor("Anne-Marie Mahfouf",I18N_NOOP("Various fixes and cleanups"), "annma@kde.org");
    aboutData.addCredit("Nenad Grujicic", I18N_NOOP("Icons"), "mengele@linuxo.org");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

    KApplication a;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    // start as KEduca Builder
    KEducaBuilder *keducaEditor = new KEducaBuilder();
    a.setMainWidget(keducaEditor);
    keducaEditor->show();

    if ( args->count() > 0 ){
        if( QFileInfo( KURL(args->url(0)).url() ).extension() == "edugallery" )
          keducaEditor->initGallery( args->url(0) );
          else
          keducaEditor->slotFileOpenURL( args->url(0) );
    }else
        keducaEditor->startChooser();


    args->clear();

    return a.exec();
}
