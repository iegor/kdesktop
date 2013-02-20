#include <config.h>

#include <dcopclient.h>
#include <dcopref.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kurl.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <qdir.h>

#include <stdlib.h>

#include "kviewshell.h"


static KCmdLineOptions options[] =
{
  { "u", 0, 0},
  { "unique", I18N_NOOP("Check if the file is loaded in another kviewshell.\nIf it is, bring up the other kviewshell. Otherwise, load the file."), 0 },
  { "m", 0, 0},
  { "mimetype <mimetype>", I18N_NOOP("Loads a plugin which supports files of type <mimetype>,\nif one is installed."), 0 },
  { "g", 0, 0},
  { "goto <pagenumber>", I18N_NOOP("Navigate to this page"), 0 },
  { "+file(s)", I18N_NOOP("Files to load"), 0 },
  KCmdLineLastOption
};


static const char description[] = I18N_NOOP("Generic framework for viewer applications");


int main(int argc, char **argv)
{
  KAboutData about ("kviewshell", I18N_NOOP("KViewShell"), "0.6",
		    description, KAboutData::License_GPL,
		    "(C) 2000, Matthias Hoelzer-Kluepfel\n"
                    "(C) 2004-2005, Wilfried Huss",
		    I18N_NOOP("Displays various document formats. "
		    "Based on original code from KGhostView."));
  about.addAuthor ("Wilfried Huss", I18N_NOOP("Current Maintainer"),
		   "Wilfried.Huss@gmx.at");
  about.addAuthor ("Matthias Hoelzer-Kluepfel", I18N_NOOP("Framework"),
		   "mhk@caldera.de");
  about.addAuthor ("David Sweet",
		   I18N_NOOP("KGhostView Maintainer"),
		   "dsweet@kde.org",
		   "http://www.chaos.umd.edu/~dsweet");
  about.addAuthor ("Mark Donohoe",
		   I18N_NOOP("KGhostView Author"));
  about.addAuthor ("Markku Hihnala",
		   I18N_NOOP("Navigation widgets"));
  about.addAuthor ("David Faure",
		   I18N_NOOP("Basis for shell"));
  about.addAuthor ("Daniel Duley",
		   I18N_NOOP("Port to KParts"));
  about.addAuthor ("Espen Sand",
		   I18N_NOOP("Dialog boxes"));
  about.addAuthor ("Stefan Kebekus",
		   I18N_NOOP("DCOP-Interface, major improvements"),
		   "kebekus@kde.org");

  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions( options ); // Add my own options.
  KApplication app;

  // see if we are starting with session management
  if (app.isRestored())
  {
    RESTORE(KViewShell);
  }
  else
  {
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->isSet("unique")){
      // With --unique, we need one argument.
      if (args->count() < 1) {
        args->usage();
        exit(-1);
      }

      // Find the fully qualified file name of the file we are
      // loading. Complain, if we are given a URL which does not point
      // to a local file.
      KURL url(args->url(0));

      if (!args->url(0).isValid()) {
        kdError(1223) << QString(I18N_NOOP("The URL %1 is not well-formed.")).arg(args->arg(0)) << endl;
        return -1;
      }

      if (!args->url(0).isLocalFile()) {
        kdError(1223) << QString(I18N_NOOP("The URL %1 does not point to a local file. You can only specify local "
                   "files if you are using the '--unique' option.")).arg(args->arg(0)) << endl;
        return -1;
      }

      QString qualPath = QFileInfo(args->url(0).path()).absFilePath();

      app.dcopClient()->attach();
      QCString id = app.dcopClient()->registerAs("unique-kviewshell");
      if (id.isNull())
        kdError(1223) << "There was an error using dcopClient()->registerAs()." << endl;
      QCStringList apps = app.dcopClient()->registeredApplications();
      for ( QCStringList::Iterator it = apps.begin(); it != apps.end(); ++it )
      {
        if ((*it).find("kviewshell") == 0) 
        {
          QByteArray data, replyData;
          QCString replyType;
          QDataStream arg(data, IO_WriteOnly);
          bool result;
          arg << qualPath.stripWhiteSpace();
          if (!app.dcopClient()->call( *it, "kmultipage", "is_file_loaded(QString)", data, replyType, replyData))
            kdError(1223) << "There was an error using DCOP." << endl;
          else
          {
            QDataStream reply(replyData, IO_ReadOnly);
            if (replyType == "bool")
            {
              reply >> result;
              if (result == true)
              {
                if (app.dcopClient()->send(*it, "kmultipage", "jumpToReference(QString)", args->url(0).ref()) == true)
                {
                  app.dcopClient()->detach();
                  return 0;
                }
              }
            }
            else
            {
              kdError(1223) << "The DCOP function 'doIt' returned an unexpected type of reply!";
            }
          }
        }
      }
    }
    app.dcopClient()->registerAs("kviewshell");
    KViewShell* shell;

    if (args->isSet("mimetype"))
    {
      shell = new KViewShell(args->getOption("mimetype"));
    }
    else if (args->count() > 0)
    {
      // If a url is given, we try to load a matching KViewShell plugin,
      // so we don't have to load the empty plugin first.
      KMimeType::Ptr mimetype = KMimeType::findByURL(args->url(0));
      shell = new KViewShell(mimetype->name());
    }
    else
    {
      // Load the empty plugin
      shell = new KViewShell();
    }

    // Show the main window before a file is loaded. This gives visual
    // feedback to the user and (hopefully) reduces the perceived
    // startup time.
    shell->show();
    app.processEvents();

    if ( args->count() > 0 )
    {
      KURL url = args->url(0);
      if (!url.hasRef() && args->isSet("goto"))
      {
        // If the url doesn't already has a reference part, add the
        // argument of --goto to the url as reference, to make the
        // KViewShell jump to this page.
        QString reference = args->getOption("goto");
        url.setHTMLRef(reference);
      }
      shell->openURL(url);
    }
  }

  return app.exec();
}
