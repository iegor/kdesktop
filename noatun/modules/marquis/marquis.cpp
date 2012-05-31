// marquis.cpp
//
// Copyright (C) 2000 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <noatun/pluginloader.h>
#include "marquis.h"
#include <noatun/player.h>
#include <noatun/engine.h>
#include <kaction.h>

static int getPlayStatus( Player *player )
{
	if ( player->isPlaying() )
		return 1;
	if ( player->isPaused() )
		return 2;
	return 0;
}

static void setPlayStatus( Player *player, int status )
{
	switch( status )
	{
		default:
		case 0:
			player->stop();
			break;
		case 1:
			player->play();
			break;
		case 2:
			if ( !player->isPaused() )
			{
				player->play();
				player->playpause();
			}
			break;
	}
}


Marquis::Marquis()
	: KMainWindow(0, "Marquis")
	, SessionManagement()
{
	hide();
	kdDebug(66666) << k_funcinfo << "number of members == " << memberList->count() << endl;

// for testing: uncomment this and use
// dcop `dcop | grep noatun` Marquis activateAction dynamicRestore
// and dynamicSave accordingly.
//	(void) new KAction("Restore", 0, this, SLOT( dynamicRestore() ), actionCollection(), "dynamicRestore" );
// 	(void) new KAction("Save", 0, this, SLOT( dynamicSave() ), actionCollection(), "dynamicSave" );
	connect( napp, SIGNAL( saveYourself() ), SLOT( dynamicSave() ));
}

Marquis::~Marquis()
{
}

void Marquis::restore(void)
{
	kdDebug(66666) << k_funcinfo << endl;
	dynamicRestore();
}

// unload every window, and save the config as QStringList of those loaded
void Marquis::saveSessionConfig(KConfig *c)
{
	kdDebug(66666) << k_funcinfo << endl;

	Player *player = napp->player();
	c->writeEntry("Volume", player->volume());
	c->writeEntry("Loop Style", player->loopStyle());
	if ( napp->playlist() )
		c->writeEntry("Playlist Visible", napp->playlist()->listVisible());

	if ( !player->current().isNull() )
	{
		KURL songURL = player->current().url();
		songURL.setPass( QString::null ); // don't save passwords
		c->writePathEntry("Current Song", songURL.url());
	}
	else
		c->writePathEntry("Current Song", QString::null);

	c->writeEntry("Current Position", player->getTime());
	c->writeEntry("PlayStatus", getPlayStatus( player ));

	// borrowed from Plugin config dialog
	QStringList specList;

	QValueList<NoatunLibraryInfo> loaded = napp->libraryLoader()->loaded();
	for( QValueList<NoatunLibraryInfo>::Iterator i = loaded.begin(); i != loaded.end(); ++i)
	{
		if(!specList.contains((*i).specfile)
		   && napp->libraryLoader()->isLoaded((*i).specfile)
		   && (*i).specfile != "marquis.plugin")
		{
			specList += (*i).specfile;
			napp->libraryLoader()->remove((*i).specfile, false);
		}
	}

	c->writeEntry("Loaded Plugins", specList);
}

// get the list of loaded plugins from the config, and load them
void Marquis::readSessionConfig(KConfig *c)
{
	Player *player = napp->player();

	c->setGroup(QString::null);

	// First set volume, then load modules, some module could start
	// playback and that would be with 100% volume!
	player->setVolume( c->readNumEntry("Volume", 100) );
	//player->setVolume( 0 );
	player->loop( c->readNumEntry("Loop Style", (int) Player::None) );

	QStringList list = c->readListEntry("Loaded Plugins");
	/*
	kdDebug(66666) << "Marquis::readSessionConfig()" << endl;
	for(QStringList::ConstIterator i=list.begin(); i!=list.end(); ++i)
		kdDebug(66666) << *i << endl;
	kdDebug(66666) << "Marquis::readSessionConfig() there we go" << endl;
	*/
	napp->libraryLoader()->loadAll(list);

	if (!napp->playlist())
	{
		KMessageBox::error(0,i18n("No playlist plugin was found. " \
			"Please make sure that Noatun was installed correctly."));
		napp->quit();
		return;
	}

	if ( c->readBoolEntry( "Playlist Visible", false ))
		napp->playlist()->showList();

	napp->player()->engine()->setInitialized();
	// TODO: restore the playback state (also save it ;)
}

void Marquis::dynamicSave()
{
	KConfig config( "marquisrc" );
	saveSessionConfig( &config );
}

void Marquis::dynamicRestore()
{
	KConfig config( "marquisrc" );
	readSessionConfig( &config );
}

#include "marquis.moc"
