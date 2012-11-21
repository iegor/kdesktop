#include <kuniqueapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <dcopclient.h>
#include "weatherservice.h"

int main (int argc, char *argv[])
{
	KLocale::setMainCatalogue("kdelibs");
	KAboutData aboutdata("KWeatherService", I18N_NOOP("KDE"),
				"0.8", I18N_NOOP("KWeather DCOP Service"),
				KAboutData::License_GPL, "(C) 2002, Ian Reinhart Geiser");
	aboutdata.addAuthor("Ian Reinhart Geiser",I18N_NOOP("Developer"),"geiseri@kde.org");
	aboutdata.addAuthor("Nadeem Hasan",I18N_NOOP("Developer"),"nhasan@kde.org");

	KCmdLineArgs::init( argc, argv, &aboutdata );
	// KCmdLineArgs::addCmdLineOptions( options );
	KUniqueApplication::addCmdLineOptions();

	if (!KUniqueApplication::start())
	{
		kdDebug() << "dcopservice is already running!" << endl;
		return (0);
	}

	KUniqueApplication app;
	// This app is started automatically, no need for session management
	app.disableSessionManagement();
	kdDebug() << "starting dcopservice " << endl;
	new WeatherService(&app, "WeatherService");
	return app.exec();

}
