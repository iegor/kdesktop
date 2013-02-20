/* Synaescope - a pretty noatun visualization (based on P. Harrison's Synaesthesia)
   Copyright (C) 1997 Paul Francis Harrison <pfh@yoyo.cc.monash.edu.au>
                 2001 Charles Samuels <charles@kde.org>
   Copyright (C) 2001 Neil Stevens <multivac@fcmail.com>

	this file is X11 source
 */

#include <noatun/conversion.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <qframe.h>
#include <qregexp.h>
#include <noatun/app.h>
#include "syna.h"
#include "synaescope.h"

extern "C" Plugin *create_plugin()
{
	KGlobal::locale()->insertCatalogue("tippecanoe");
	return new SynaeScope();
}

SynaeScope::SynaeScope()
	: QWidget(0, 0, WType_TopLevel | WStyle_DialogBorder | WRepaintNoErase | WMouseNoMask)
	, StereoScope(50)
	, Plugin()
{
	setSamples(bufferSize);
	mBuffer = new char[bufferSize * 16 * 2];
	setFixedSize(320, 240);
	setCaption(i18n("Tippecanoe - Noatun"));
	setIcon(SmallIcon("noatun"));
	show();
	embed = new QXEmbed(this);
	embed->move(0,0);
	embed->resize(320, 240);
	embed->show();
	embed->setFocus();
}

SynaeScope::~SynaeScope()
{
	connect(&process, SIGNAL(processExited(KProcess *)), this, SLOT(processExited(KProcess *)));
	napp->pluginMenuRemove(pluginMenuItem);
}

void SynaeScope::init()
{
	connect(&process, SIGNAL(receivedStdout(KProcess *, char *, int)), this, SLOT(read(KProcess *, char *, int)));
	pluginMenuItem = napp->pluginMenuAdd(i18n("Toggle Tippecanoe"), this, SLOT(toggle(void)));

	process << KStandardDirs::findExe("noatuntippecanoe.bin");

	// Note that process.start() will fail if findExe fails, so there's no real need
	// for two separate checks.
	if(!process.start(KProcess::NotifyOnExit, (KProcess::Communication)(KProcess::Stdin | KProcess::Stdout)))
	{
		KMessageBox::error(0, i18n("Unable to start noatuntippecanoe.bin. Check your installation."));
		unload();
	}
	else
		start();

}

void SynaeScope::scopeEvent(float *left, float *right, int size)
{
	if(!isHidden() && process.isRunning())
	{
		Conversion::convertStereo2FloatToI16le((unsigned long)size, left, 
		                                       right, (unsigned char*)mBuffer);

		process.writeStdin((char *)mBuffer, bufferSize*2);
	}
}

void SynaeScope::read(KProcess *, char *buf, int)
{
	QString num = QString::fromLatin1(buf);
	num = num.left(num.find(QRegExp("\\s")));
	id = num.toInt();
	embed->embed(id);
}

void SynaeScope::processExited(KProcess *)
{
	unload();
}

void SynaeScope::toggle(void)
{
	if(isHidden())
		show();
	else
		hide();
}

#include "synaescope.moc"
