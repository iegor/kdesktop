/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : zo mrt 18 17:12:24 CET 2001
    copyright            : (C) 2001 by Martijn Klingens
    email                : klingens@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <dcopclient.h>

#include "gui/kbbmainwindow.h"
#include "bugsystem.h"
#include "kbbprefs.h"

static const char description[] =
    I18N_NOOP("KBugBuster");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE

static const KCmdLineOptions options[] =
{
  {"d", 0, 0},
  {"disconnected", I18N_NOOP("Start in disconnected mode"), 0},
  {"pkg", 0, 0},
  {"package <pkg>", I18N_NOOP("Start with the buglist for <package>"), 0},
  {"bug <br>", I18N_NOOP("Start with bug report <br>"), 0},
  KCmdLineLastOption
};

int main(int argc, char *argv[])
{
    KAboutData aboutData( "kbugbuster", I18N_NOOP( "KBugBuster" ),
        VERSION, description, KAboutData::License_GPL,
        I18N_NOOP("(c) 2001,2002,2003 the KBugBuster authors") );
    aboutData.addAuthor( "Martijn Klingens", 0, "klingens@kde.org" );
    aboutData.addAuthor( "Cornelius Schumacher", 0, "schumacher@kde.org" );
    aboutData.addAuthor( "Simon Hausmann", 0, "hausmann@kde.org" );
    aboutData.addAuthor( "David Faure", 0, "faure@kde.org" );
    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;

    app.dcopClient()->attach();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("disconnected")) {
        BugSystem::self()->setDisconnected( true );
    }

    if ( app.isRestored() )
    {
        RESTORE( KBBMainWindow );
    }
    else 
    {
        KBBMainWindow *mainWin = new KBBMainWindow(args->getOption("package"), args->getOption("bug"));

        // Since all background jobs remaing running after closing the
        // main window we force a quit here
        QObject::connect( &app, SIGNAL( lastWindowClosed() ),
                          &app, SLOT( quit() ) );
        mainWin->show();
        return app.exec();
    }
}

/* vim: set et ts=4 sw=4 softtabstop=4: */
