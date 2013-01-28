#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapplication.h>

#include "designer.h"

int main(int argc, char *argv[])
{
	KAboutData aboutData(
	           "atlantikdesigner",
	           I18N_NOOP("Atlantik Designer"), "1.1.0",
	           I18N_NOOP("Atlantik gameboard designer"),
	           KAboutData::License_GPL,
	           I18N_NOOP("(c) 2002 Jason Katz-Brown"),
	           0,
	           "http://www.unixcode.org/atlantik/"
	           );

	aboutData.addAuthor("Jason Katz-Brown", I18N_NOOP("main author"), "jason@katzbrown.com", "http://katzbrown.com/");
	aboutData.addAuthor("Rob Kaper", I18N_NOOP("libatlantikui"), "cap@capsi.com", "http://capsi.com/");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication kapplication;

	if (kapplication.isRestored())
		RESTORE(AtlanticDesigner)
	else
	{
		AtlanticDesigner *designer = new AtlanticDesigner;
		designer->show();
	}

	return kapplication.exec();
}

