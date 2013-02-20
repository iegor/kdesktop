#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaboutdata.h>
#include "SVGTestWidget.h"

static KCmdLineOptions options[] =
{
	{ "+URL",   I18N_NOOP("SVG file to open"), 0 },
	KCmdLineLastOption
};

int main(int argc, char **argv)
{

	KAboutData *about = new KAboutData( "KSVG test app", I18N_NOOP("KSVG test"), "0.1", I18N_NOOP("KSVG standalone test app") );
	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions( options );
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->count() == 0)
	{
		args->usage();
		return 0;
	}
	KURL url = args->url(0);
	KApplication a( argc, argv );
	SVGTestWidget *w = new SVGTestWidget(url);
	a.setMainWidget(w);
	w->show();

	return a.exec();
}
