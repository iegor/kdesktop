/***************************************************************************
                         main.cpp  -  description
                            -------------------
   begin                : Fre Nov 16 14:52:33 CET 2001
   copyright            : (C) 2001 by Matthias Messmer &
                                      Carsten Niehaus &
                                      Robert Gogolok
   email                : bmlmessmer@web.de &
                          cniehaus@gmx.de &
                          mail@robert-gogolok.de
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// KDE includes
#include <kcmdlineargs.h>
#include <kaboutdata.h>

// local includes
#include "kpercentage.h"

static const char description[] =
    I18N_NOOP( "A program to improve your skills in calculating with percentages" );

static KCmdLineOptions options[] =
    {
        KCmdLineLastOption
    };

int main( int argc, char *argv[] )
{
    KAboutData aboutData
    (
        "kpercentage",
        I18N_NOOP( "KPercentage" ),
        KPERCENTAGE_VERSION, description, KAboutData::License_GPL,
        "(c) 2001-2007, Matthias Messmer",
        0, 0,
        "submit@bugs.kde.org"
    );
    aboutData.addAuthor( "Matthias Messmer", I18N_NOOP( "coding, coding and coding" ), "bmlmessmer@web.de" );
    aboutData.addAuthor( "Carsten Niehaus", I18N_NOOP( "CVS, coding and sed-script" ), "cniehaus@gmx.de" );
    aboutData.addAuthor( "Robert Gogolok", I18N_NOOP( "CVS, coding and Makefile stuff" ), "mail@robert-gogolok.de" );
    aboutData.addCredit( "Elhay Achiam", I18N_NOOP( "Pixmaps" ), "e_achiam@netvision.net.il" );
    aboutData.addCredit( "Danny Allen", I18N_NOOP( "Pixmaps" ), "danny@dannyallen.co.uk" );
    aboutData.addCredit( "Jonathan Drews", I18N_NOOP( "Spelling and Language" ), "j.e.drews@att.net" );
    aboutData.addCredit( "Primoz Anzur", I18N_NOOP( "Cleaning and bugfixing code" ), "zerokode@gmx.net" );
    aboutData.addCredit("David Vignoni", I18N_NOOP("SVG icon"), "david80v@tin.it");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

    KApplication a;
    KPercentage *kpercentage = new KPercentage( "kpercentage" );
    a.setMainWidget( kpercentage );

    /*
     * no window is shown here, cause constructor of KPercentage shows
     * splash screen first, then the main window is shown.
     */

    return a.exec();
}
