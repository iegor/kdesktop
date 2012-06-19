/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2003 by Stanislav Visnovsky
			    <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA  02111-1307, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than  
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
		  
**************************************************************************** */
#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcursor.h>
#include <klocale.h>
#include <kapplication.h>
#include <kwin.h>

#include <qtimer.h>

#include "kbabeldict.h"
#include "kbabelsplash.h"
#include <version.h>

class KBabelDictApplication : public KApplication
{
public:
    KBabelDictApplication();
    ~KBabelDictApplication();

private:
    KBabelDict *topLevel;
};

KBabelDictApplication::KBabelDictApplication()
    : KApplication()
    , topLevel(0)
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    
    QTimer timer;
    QWidget *splash=0;
    bool showSplash=args->isSet("splash");
    if(showSplash)
    {
        timer.start(2000,true);
        KApplication::setOverrideCursor(KCursor::waitCursor());
        splash = new KBabelSplash();
	splash->show();
    }
	
    topLevel = new KBabelDict();

    while(timer.isActive()) // let the user admire the splash screen ;-)
        processEvents();

	setTopWidget(topLevel);
    topLevel->show();

    if(KBabelSplash::instance)
    {
        delete KBabelSplash::instance;
    }
    if(showSplash)
    {
        KApplication::restoreOverrideCursor();
    }

    QObject::connect( topLevel, SIGNAL( destroyed() ),
	     this, SLOT( quit() ) );
}

KBabelDictApplication::~KBabelDictApplication()
{
    delete(topLevel);
}

static KCmdLineOptions options[] =
{
   {"nosplash",I18N_NOOP("Disable splashscreen at startup"),0},
   KCmdLineLastOption
};


int main(int argc, char **argv)
{
    KLocale::setMainCatalogue("kbabel");
    
    KAboutData about("kbabeldict",I18N_NOOP("KBabel - Dictionary"),VERSION,
       I18N_NOOP("A dictionary for translators"),KAboutData::License_GPL,
       I18N_NOOP("(c) 2000,2001,2002,2003 The KBabeldict developers"),0,"http://kbabel.kde.org");

    about.addAuthor("Matthias Kiefer",I18N_NOOP("Original author"),"kiefer@kde.org");
    about.addAuthor("Stanislav Visnovsky",I18N_NOOP("Current maintainer"),"visnovsky@kde.org");

    about.setTranslator( I18N_NOOP("_: NAME OF TRANSLATORS\nYour names")
	, I18N_NOOP("_: EMAIL OF TRANSLATORS\nYour emails"));

    // Initialize command line args
    KCmdLineArgs::init(argc, argv, &about);

    // Tell which options are supported
    KCmdLineArgs::addCmdLineOptions( options );

    // Add options from other components
    KApplication::addCmdLineOptions();
    
    KBabelDictApplication app;

    return app.exec();
}
