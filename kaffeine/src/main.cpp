/*
 * main.cpp
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kuniqueapplication.h>

#include "kaffeine.h"
#include "version.h"

#ifndef HAVE_XCB
/* including the Xlib header pollutes namespace too much --pfister */
extern "C" int XInitThreads();
#endif

class KaffeineApp : public KUniqueApplication
{
public:
	KaffeineApp() : kaffeine(NULL) { }

	~KaffeineApp()
	{
		delete kaffeine;
	}

private:
	Kaffeine *kaffeine;

	int newInstance();

	KaffeineApp(const KaffeineApp &);
	KaffeineApp &operator=(const KaffeineApp &);
};

int KaffeineApp::newInstance()
{
	if (kaffeine)
		kaffeine->updateArgs();
	else
		kaffeine = new Kaffeine();

	return 0;
}

int main(int argc, char *argv[])
{
	KAboutData aboutData("kaffeine", I18N_NOOP("Kaffeine Player"), KAFFEINE_VERSION,
		I18N_NOOP("A media player for KDE 3. Can use multiple backends for playback, default (and recommended) is xine."),
		KAboutData::License_GPL_V2, I18N_NOOP("Copyright (C) 2003-2007, The Kaffeine Authors"), 0,
		"http://kaffeine.sourceforge.net");

	aboutData.addAuthor("Christophe Thommeret", I18N_NOOP("Current maintainer"), "hftom@free.fr");
	aboutData.addAuthor("Christoph Pfister", I18N_NOOP("Developer"), "christophpfister@gmail.com");
	aboutData.addAuthor("Jürgen Kofler", I18N_NOOP("Original author"), "kaffeine@gmx.net");

	/* FIXME: what about the other contributors listed in CREDITS? --pfister */

	aboutData.addCredit("Devin J. Heitmuelle", I18N_NOOP("ATSC scanning."), "devin.heitmueller@gmail.com");
	aboutData.addCredit("Eldon Tyrell", I18N_NOOP("DVB patches."), "dr.e.tyrell@gmail.com");
	aboutData.addCredit("Michael Hoertnagl", I18N_NOOP("Various patches."), "mtron@a1.net");
	aboutData.addCredit("Ricardo Manuel Santos Rodrigues", I18N_NOOP("Various patches."), "madinfo@cadaval.net");
	aboutData.addCredit("Christopher Martin", I18N_NOOP("Various patches."), "christopher.martin@utoronto.ca");
	aboutData.addCredit("Ben Jackson", I18N_NOOP("DVB OSD browsing patch."), "benj@puremourning.co.uk");
	aboutData.addCredit("Rainer Wirtz", I18N_NOOP("DVB categories patches."), "rainer.wirtz@gmx.de");
	aboutData.addCredit("Dieter Zander", I18N_NOOP("Logo for Kaffeine 0.8 and other artwork."), "dieter-mz@online.de");
	aboutData.addCredit("Anders Ellenshøj Andersen", I18N_NOOP("Logo animation for Kaffeine 0.5"), "andersa@ellenshoej.dk");
	aboutData.addCredit("Assaf Gillat", I18N_NOOP("Alternate encoding for meta tags. Many patches."), "gillata@gmail.com");
	aboutData.addCredit("Miguel Freitas", I18N_NOOP("xine post plugin handling. Many patches."), "miguel@cetuc.puc-rio.br");
	aboutData.addCredit("Giorgos Gousios", I18N_NOOP("Subtitle file import."), "gousiosg@cs.man.ac.uk");
	aboutData.addCredit("Michael Rolf", I18N_NOOP("M3U import. Testing."), "mi.rolf@gmx.net");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(cmdLineOptions);

	if (!KaffeineApp::start())
		return 0;

#ifndef HAVE_XCB
	/*
	 * XInitThreads() should the first call to xlib in multithreaded X programs -
	 * but because of a bug in some xfree versions that can freeze at startup,
	 * we also call XInitThreads() in kxinewidget.cpp
	 *
	 * kaffeinepart & konqueror: We call XInitThreads() in kxinewidget.cpp, so the part is
	 * not stable with buggy X implementations.
	 *
	 * FIXME: deadline for this workaround is 2007-03-16 --pfister
	 */

#ifndef XINIT_WKRND /* configure flag --with-xinit-workaround not set */
	flush(kdDebug() << "if kaffeine hangs here run 'configure --with-xinit-workaround' and recompile / reinstall ...");
	XInitThreads();
	kdDebug() << " ok\n";
#endif
#endif

	KaffeineApp a;
	return a.exec();
}
