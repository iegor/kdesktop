/* Synaescope - a pretty noatun visualization (based on P. Harrison's Synaesthesia)
   Copyright (C) 1997 Paul Francis Harrison <pfh@yoyo.cc.monash.edu.au>
                 2001 Charles Samuels <charles@kde.org>

	this file is X11 source
 */

#include "synaescope.h"
#include "cmodule.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>

extern "C"
{
	Plugin *create_plugin()
	{
		KGlobal::locale()->insertCatalogue("synaescope");
		return new SynaeScope();
	}
}

SynaeScope::SynaeScope() : Plugin(), scopeExePath(0)
{
	kdDebug(66666) << k_funcinfo << endl;
	restarting=false;
	connect(&process, SIGNAL(processExited(KProcess *)),
		this, SLOT(processExited(KProcess *)));
	connect(&process, SIGNAL(receivedStdout(KProcess *,char *,int)),
		this, SLOT(receivedStdout(KProcess *,char *,int)));
	connect(&process, SIGNAL(receivedStderr(KProcess *,char *,int)),
		this, SLOT(receivedStderr(KProcess *,char *,int)));
}

SynaeScope::~SynaeScope()
{
	kdDebug(66666) << k_funcinfo << endl;
	process.kill();
}

void SynaeScope::init()
{
	kdDebug(66666) << k_funcinfo << endl;
	mPrefs = new SynaePrefs(this);
	mPrefs->reopen();
	mPrefs->save();
	connect(mPrefs, SIGNAL(configChanged()), this, SLOT(readConfig()));

	scopeExePath = KStandardDirs::findExe("noatunsynaescope.bin");
	if (scopeExePath.isEmpty())
	{
		KMessageBox::error(0, i18n("Unable to locate noatunsynaescope.bin in your path. Check your installation."));
		unload();
	}

	process << scopeExePath;

	runScope();
}

void SynaeScope::runScope()
{
	kdDebug(66666) << k_funcinfo << endl;
	if(!process.start(KProcess::NotifyOnExit, (KProcess::Communication)(KProcess::Stdin | KProcess::Stdout)))
	{
		KMessageBox::error(0, i18n("Unable to start noatunsynaescope. Check your installation."));
		unload();
	}
}


void SynaeScope::readConfig()
{
	kdDebug(66666) << k_funcinfo << endl;
	if (!process.isRunning())
		return;

	restarting=true;
	process.kill();
}

void SynaeScope::processExited(KProcess *)
{
	kdDebug(66666) << k_funcinfo << endl;
	if(restarting)
	{
		restarting=false;
		runScope();
	}
	else
	{
		unload();
	}
}

void SynaeScope::receivedStdout(KProcess *, char *buf, int len)
{
	QCString debugString(buf,len);
	kdDebug(66666) << k_funcinfo << debugString << endl;
}

void SynaeScope::receivedStderr(KProcess *, char *buf, int len)
{
	QCString debugString(buf,len);
	kdDebug(66666) << k_funcinfo << debugString << endl;
}

#include  "synaescope.moc"
