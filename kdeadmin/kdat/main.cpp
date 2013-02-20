// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <qfile.h>
#include <qdir.h>

#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kcrash.h>
#include <kmessagebox.h>

#include <signal.h>

#include "kdat.h"
#include "KDatMainWindow.h"

static const char description[] = 
	I18N_NOOP("tar-based DAT archiver for KDE");

/* in ErrorHandler.cpp */
void error_handler(int err_sig);

int main( int argc, char** argv )
{
  KAboutData aboutData( "kdat", I18N_NOOP("KDat"), 
    KDAT_VERSION, description, KAboutData::License_GPL, 
    "(c) 1999-2000, Sean Vyain; 2001-2002 Lawrence Widman");

  /* 2002-01-28 LEW: so we can dump core if we want to */
  // KCrash::setCrashHandler(0);  // this is supposed to work, but it doesn't
#ifdef DEBUG
  {
    char *newarg;
    if( ( newarg = (char *)malloc( strlen("--nocrashhandler") + 1 ) ) == NULL )
      {
	KMessageBox::sorry(NULL, i18n("Can't allocate memory in kdat"));
        exit(1);
      }
    strcpy( newarg, "--nocrashhandler" );
    argv[ argc ] = newarg;
    argc++;
  }
  {
    int i;
    for(i=0; i<argc; i++){
      printf("Arg %d: %s\n", i, argv[i]);
    }
  }
#endif /* DEBUG */
  /* 2002-01-28 LEW */

  KCmdLineArgs::init( argc, argv, &aboutData );
  aboutData.addAuthor( "Lawrence Widman", 0, "kdat@cardiothink.com");
//  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  KApplication app;
  
  app.setMainWidget( KDatMainWindow::getInstance() );

  /* set up error handler so we don't crash without notice */
  signal(SIGHUP,  error_handler);
  signal(SIGINT,  error_handler);
  signal(SIGFPE,  error_handler);
  signal(SIGSEGV, error_handler);
  signal(SIGTERM, error_handler);   

  if ( app.isRestored() && KDatMainWindow::canBeRestored( 1 ) ) {
    KDatMainWindow::getInstance()->restore( 1 );
  } else {
    KDatMainWindow::getInstance()->show();
  }

  return app.exec();
}

