/*****************************************************************

Copyright (c) 2000-2001 the noatun authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <qapplication.h>

#include "kaboodle_factory.h"
#include "kaboodleapp.h"

void noMessageOutput(QtMsgType, const char *)
{
}

static KCmdLineOptions options[] =
{
	{ "+[URL]", I18N_NOOP("URL to open"), 0 },
#ifndef NDEBUG
	{ "qdebug", I18N_NOOP("Turn on Qt Debug output"), 0 },
#endif
	KCmdLineLastOption
};

int main(int argc, char **argv)
{
	KCmdLineArgs::init(argc, argv, Kaboodle::KaboodleFactory::aboutData());
	KCmdLineArgs::addCmdLineOptions(options);

#ifndef NDEBUG
	if(!KCmdLineArgs::parsedArgs()->isSet("qdebug"))
		qInstallMsgHandler(noMessageOutput);
#endif

	Kaboodle::KaboodleApp::addCmdLineOptions();
	Kaboodle::KaboodleApp app;

	return app.exec();
}
