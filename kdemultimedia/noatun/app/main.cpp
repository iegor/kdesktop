#include <noatun/app.h>
//#include "joinhandler.h"
//#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] = I18N_NOOP("The Fusion of Frequencies");
static const char version[]     = NOATUN_VERSION;

static KCmdLineOptions options[] =
{
	{ "+[URL]",          I18N_NOOP("Files/URLs to open"),     0 },
	KCmdLineLastOption
};

extern "C" KDE_EXPORT int kdemain(int argc, char **argv)
{
	KAboutData aboutData("noatun", I18N_NOOP("Noatun"),
	                     version, description, KAboutData::License_BSD,
	                     I18N_NOOP("(c) 2000-2004, The Noatun Developers"),
	                     0, "http://noatun.kde.org");
	aboutData.addAuthor("Charles Samuels", I18N_NOOP("Noatun Developer"),
	                    "charles@kde.org");
	aboutData.addAuthor("Stefan Westerfeld", I18N_NOOP("Patron of the aRts"),
	                    "stefan@space.twc.de");
	aboutData.addAuthor("Martin Vogt", I18N_NOOP("MPEG Codec and OGG Vorbis Support"),
	                    "mvogt@rhrk.uni-kl.de");
	aboutData.addAuthor("Malte Starostik", I18N_NOOP("Infrared Control Support and HTML playlist export"),
	                    "malte@kde.org");
	aboutData.addAuthor("Nikolas Zimmermann", I18N_NOOP("HTML playlist export and Plugin System"),
	                    "wildfox@kde.org");
	aboutData.addAuthor("Stefan Schimanski", I18N_NOOP("Kaiman Skin Support"),
	                    "1Stein@gmx.de");
	aboutData.addAuthor("Stefan Gehn", I18N_NOOP("Extended K-Jöfol Skin Support, EXTM3U playlist loading"),
	                    "metz@gehn.net");

	aboutData.addCredit("Bill Huey", I18N_NOOP("Special help with the equalizer"));

	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions(options);
	NoatunApp::addCmdLineOptions();

	NoatunApp app;
	return app.exec();
}

