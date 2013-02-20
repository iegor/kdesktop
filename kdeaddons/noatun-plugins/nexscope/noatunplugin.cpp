#include "nex.h"
#include "noatunplugin.h"

#include <kprocess.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>

extern "C"
{
Plugin *create_plugin()
{
	KGlobal::locale()->insertCatalogue("nexscope");
	return new NexPlugin();
}
}


NexPlugin::NexPlugin()
{
	connect(&process, SIGNAL(processExited(KProcess *)), this, SLOT(processExited(KProcess *)));
}

NexPlugin::~NexPlugin()
{
	process.kill();
}

void NexPlugin::init()
{
	process << KStandardDirs::findExe("nexscope.bin");

	// Note that process.start() will fail if findExe fails, so there's no real need
	// for two separate checks.
	if(!process.start(KProcess::NotifyOnExit, (KProcess::Communication)(KProcess::Stdin | KProcess::Stdout)))
	{
		KMessageBox::error(0, i18n("Unable to start noatunNex. Check your installation."));
		unload();
	}

}

void NexPlugin::processExited(KProcess *)
{
	unload();
}

#include  "noatunplugin.moc"

