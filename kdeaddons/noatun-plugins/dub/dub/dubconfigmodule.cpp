//
//
// C++ Implementation: cpp
//
// Description:
//
//
// Author: Eray Ozkural (exa) <erayo@cs.bilkent.edu.tr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <kurlrequester.h>
#include <kdebug.h>

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qradiobutton.h>

#include "dubconfigmodule.h"

DubConfigModule::DubConfigModule(QObject* parent)
  : CModule(i18n("Dub"), i18n("Folder-Based Playlist"), "noatun", parent)
    , playMode(oneDir)
    , playOrder(normal)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setAutoAdd(true);
  vbox->setSpacing( 0 );
  vbox->setMargin( 0 );

  prefs = new DubPrefs(this);
  prefs->mediaDirectory->setMode(KFile::Directory);

  reopen();
}

DubConfigModule::~DubConfigModule()
{
}

void DubConfigModule::save(void)
{
  KConfig* config(KGlobal::config());
  config->setGroup("DubPlaylist");

  mediaDirectory = prefs->mediaDirectory->url();

  QButton* selectedMode = prefs->playMode->selected();
  if (selectedMode==prefs->allFiles)
    playMode = DubConfigModule::allFiles;
  else
    if (selectedMode==prefs->recursiveDir)
      playMode = DubConfigModule::recursiveDir;
    else
      playMode = DubConfigModule::oneDir;

  QButton* selectedOrder = prefs->playOrder->selected();
  if (selectedOrder==prefs->shuffle)
    playOrder = DubConfigModule::shuffle;
  else
    if (selectedOrder==prefs->repeat)
      playOrder = DubConfigModule::repeat;
    else
      if (selectedOrder==prefs->single)
	playOrder = DubConfigModule::single;
      else
 	playOrder = DubConfigModule::normal;

  config->writePathEntry("MediaDirectory", mediaDirectory);
  config->writeEntry("PlayMode", (int) playMode);
  config->writeEntry("PlayOrder", (int) playOrder);
}

void DubConfigModule::reopen(void)
{
  kdDebug(90010) << "dubconfig: reopen" << endl;
  KConfig* config(KGlobal::config());
  config->setGroup("DubPlaylist");
  mediaDirectory = config->readPathEntry("MediaDirectory", "~/");
  kdDebug(90010) << "dub: media home is " << mediaDirectory << endl;
  playMode = (PlayMode) config->readNumEntry("PlayMode", DubConfigModule::oneDir);
  playOrder = (PlayOrder) config->readNumEntry("PlayOrder", DubConfigModule::normal);
  apply();
}

void DubConfigModule::apply()
{
  prefs->mediaDirectory->setURL(mediaDirectory);
  // what a horrible repetition, but it's not worthwhile doing any better ;)
  switch (playMode) {
  case allFiles:
    prefs->playMode->setButton(prefs->playMode->id(prefs->allFiles));
    break;
  case oneDir:
    prefs->playMode->setButton(prefs->playMode->id(prefs->oneDir));
    break;
  case recursiveDir:
    prefs->playMode->setButton(prefs->playMode->id(prefs->recursiveDir));
    break;
  }
  switch (playOrder) {
  case normal:
    prefs->playOrder->setButton(prefs->playOrder->id(prefs->normal));
    break;
  case shuffle:
    prefs->playOrder->setButton(prefs->playOrder->id(prefs->shuffle));
    break;
  case repeat:
    prefs->playOrder->setButton(prefs->playOrder->id(prefs->repeat));
    break;
  case single:
    prefs->playOrder->setButton(prefs->playOrder->id(prefs->single));
    break;
  }
}


#include "dubconfigmodule.moc"
