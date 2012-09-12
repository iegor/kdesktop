/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapp.h>
#include <dcopclient.h>

#include "katapult.h"

static const char description[] =
    I18N_NOOP("A plugin-driven utility that can launch applications, bookmarks, or anything else that you can find a plugin for.");

static const char version[] = "0.3.2.1";

static KCmdLineOptions options[] =
{
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
	KAboutData about("katapult", I18N_NOOP("Katapult"), version, description,
			 KAboutData::License_GPL, I18N_NOOP("(C) until 2005, Joe Ferris\n(C) 2005, the Katapult development team"), 0, 0, "dev@thekatapult.org.uk");
	about.addAuthor( "Martin Meredith" , I18N_NOOP("Project Manager"), "mez@thekatapult.org.uk");
	about.addAuthor( "Bastian Holst", I18N_NOOP("Lead Developer"), "oculus@thekatapult.org.uk" );
	about.addAuthor( "Tobi Vollebregt", I18N_NOOP("Developer"), "tobi@thekatapult.org.uk" );
	about.addAuthor( "Lee Olson", I18N_NOOP("Graphics"), "clearbeast@gmail.com" );
	about.addAuthor( "Jean-Remy Falleri", 0, "jr.falleri@gmail.com" );
	about.addAuthor( "Jonathan Riddell", 0, "jriddell@ubuntu.com" );
	about.addAuthor( "Joe Ferris", I18N_NOOP("Original Author"), "jferris@optimistictech.com" );
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app;
	
	if(kapp->dcopClient()->isApplicationRegistered("katapult")) {
		kapp->dcopClient()->send("katapult", "Katapult", "showLauncher()", QByteArray());
		app.setStartupId("");
		return(0);
	}
	
	Katapult mainWin;
	
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	
	app.setMainWidget( &mainWin );
	
	args->clear();
	
	app.setStartupId("");
	
	return app.exec();
}

