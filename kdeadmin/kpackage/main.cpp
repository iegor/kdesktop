/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
// Author: Damyan Pepper
// Author: Toivo Pedaste
//
// This is the entry point to the program
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/

#include <stdlib.h>

#include "../config.h"
#include "kpackage.h"

#include <qfile.h>

#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <options.h>

#include <kaboutdata.h>
#include <kdebug.h>

#include <kpTerm.h>
#include "debInterface.h"
#include "debAptInterface.h"
#include "debDpkgInterface.h"
#include "kissInterface.h"
#include "slackInterface.h"
#include "fbsdInterface.h"
#include "rpmInterface.h"
#include "gentooInterface.h"


static const char description[] =
	I18N_NOOP("KDE Package installer");

static KCmdLineOptions options[] =
{
   { "remote ", I18N_NOOP("Remote host for Debian APT, via SSH"), 0 },
   { "r ", 0, 0},
   { "+[Package]", I18N_NOOP("Package to install"), 0 },
   KCmdLineLastOption
};

// Globals
KPKG *kpkg;
KPACKAGE *kpackage;
kpPty *kpty;
kpRun *kprun;
kpRun *kpstart;
Opts *opts;

QString hostName;

pkgInterface *kpinterface[kpinterfaceN];
 
int main(int argc, char **argv)
{
  KAboutData aboutData( "kpackage", I18N_NOOP("KPackage"), 
			VERSION, description, KAboutData::License_GPL, 
			//    VERSION, description, 0, 
			"(c) 1999-2001, Toivo Pedaste");
  KCmdLineArgs::init( argc, argv, &aboutData );
  aboutData.addAuthor( "Toivo Pedaste",0, "toivo@ucs.uwa.edu.au");
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  KApplication app;
  
  kpkg = 0;
  int j = 0;

// Make sure PATH has the right directories in it
  QCString path = getenv("PATH");
  path = "PATH=" + path + ":/sbin:/usr/sbin:/usr/local/bin";
  putenv(strdup(path.data()));
	 
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  QString remoteHost = args->getOption("remote");

  if (remoteHost.isEmpty() || remoteHost == "localhost") {
    hostName = "";
  } else {
    hostName = remoteHost;
  }

  kpty = new kpPty();
  kprun = new kpRun();
  kpstart = new kpRun();

  for (int i = 0; i < kpinterfaceN; i++) {
    kpinterface[i] = 0;
  }

  kdDebug() << "Hostname=" << hostName << "\n";
  opts = new Opts(hostName);

  kpinterface[j++] = new DEBDPKG();
  kpinterface[j++] = new DEBAPT();
  kpinterface[j++] = new KISS();
  kpinterface[j++] = new RPM();
  kpinterface[j++] = new fbsdInterface();
  kpinterface[j++] = new SLACK(); // Also catched BSD packages...
  kpinterface[j++] = new Gentoo();
  
  opts->readLaterSettings();

  if ( app.isRestored() ) {
    if (KPKG::canBeRestored(1)) {
      kpkg =  new KPKG(app.config());  
      kpkg->restore(1);
    }
  } else {
    // Create the main widget and show it
    kpkg = new KPKG(app.config());
    kpkg->show();
  }
  kpkg->setCaption(hostName);

  if (args->count()) {		// an argument has been given
    QStringList files;
    for(int i = 0; i < args->count(); i++) {
      files.append(args->url(i).url());
    }
    kpackage->openNetFiles(files, FALSE); 
  }  else {	
    if (!kpkg->prop_restart)
      kpackage->setup();
  }
  
  args->clear();

  int r = app.exec();		// execute the application

  return r;			// return the result
}

