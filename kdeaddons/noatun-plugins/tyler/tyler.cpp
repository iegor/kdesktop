// Copyright (C) 2001 Charles Samuels <charles@kde.org>
// Copyright (C) 2001 Neil Stevens <neil@qualityassistant.com>

#include "tyler.h"

#include <noatun/conversion.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <noatun/app.h>
#include <kdebug.h>

extern "C" Plugin *create_plugin()
{
	KGlobal::locale()->insertCatalogue("tyler");
	return new Tyler();
}

const int Tyler::bufferSize = 512;

Tyler::Tyler()
	: QObject()
	, StereoScope(25)
	, Plugin()
{
	setSamples(bufferSize*2);
	mBuffer = new char[bufferSize * 16 * 2];
}

Tyler::~Tyler()
{
	delete [] mBuffer;
}

void Tyler::init()
{
	process << KStandardDirs::findExe("noatuntyler.bin");
	connect(&process, SIGNAL(processExited(KProcess *)), this, SLOT(processExited(KProcess *)));

	// Note that process.start() will fail if findExe fails, so there's no real need
	// for two separate checks.
	if(!process.start(KProcess::NotifyOnExit, (KProcess::Communication)(KProcess::Stdin | KProcess::Stdout)))
	{
		KMessageBox::error(0, i18n("Unable to start noatuntyler.bin. Check your installation."));
		unload();
	}
	else
		start();

}

void Tyler::scopeEvent(float *left, float *right, int size)
{
	if(process.isRunning())
	{
		Conversion::convertMonoFloatTo16le((unsigned long)size, right,
		                                      (unsigned char*)mBuffer);

		Conversion::convertMonoFloatTo16le((unsigned long)size, left,
		                                      ((unsigned char*)mBuffer)+bufferSize*2);
		process.writeStdin((char *)mBuffer, bufferSize*2*2);
	}
}

void Tyler::processExited(KProcess *)
{
	unload();
}

#include "tyler.moc"
