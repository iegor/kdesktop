//
//
// C++ Implementation: dubplaylist.cpp
//
// Description:
//
//
// Author: Eray Ozkural (exa) <erayo@cs.bilkent.edu.tr>, (C) 2001
//
// Copyright: See COPYING file that comes with this distribution
//
//

//#include dubplaylist.cpp

#include <kmessagebox.h>
#include <kdebug.h>
#include <noatun/playlist.h>
#include <klocale.h>

#include "dub.h"
#include "dubconfigmodule.h"

#include "dubplaylist.h"

// plugin interface
extern "C" Plugin *create_plugin()
{
  KGlobal::locale()->insertCatalogue("dub");
  DubPlaylist* dub = new DubPlaylist;
  return dub;
}

/** Construct playlist object */
DubPlaylist::DubPlaylist()
 : Playlist(0, "Dub Playlist")
 , dub(0)
 , visible(false)
{
  kdDebug(90010) << "dub: cons playlist" << endl;
}

DubPlaylist::~DubPlaylist(){
  kdDebug(90010) << "dub: destruct playlist " << endl;
  // destroy app
  delete dub;
}

void DubPlaylist::reset() {
  kdDebug(90010) << "dub: reset" << endl;
}

void DubPlaylist::clear() {
  kdDebug(90010) << "dub: clear" << endl;
}

void DubPlaylist::addFile(const KURL&, bool play) {
  KMessageBox::information(0, i18n("Adding files not supported yet, see configuration"));
}

PlaylistItem DubPlaylist::next() {
  kdDebug(90010) << "dub: next" << endl;
  dub->selectNextFile();
  updateCurrent();
  return currentItem;
}

PlaylistItem DubPlaylist::current() {
  if (!currentItem.isNull())
    kdDebug(90010) << "dub: current item:" << currentItem.data()->url().prettyURL() << endl;
  return currentItem;
}

PlaylistItem DubPlaylist::previous() {
  kdDebug(90010) << "dub: previous" << endl;
  dub->selectPreviousFile();
  updateCurrent();
  return currentItem;
}

PlaylistItem DubPlaylist::getFirst() const {
  kdDebug(90010) << "dub: getFirst" << endl;
  const KFileItem* first = dub->queryFirstFile();
  if (first) {
    kdDebug(90010) << "dub: first " << first->url() << endl;
    DubPlaylistItem* firstData = new DubPlaylistItem(*first);
    return PlaylistItem(firstData);
  }
  else
    return 0;
}

PlaylistItem DubPlaylist::getAfter(const PlaylistItem &item) const {
  kdDebug(90010) << "dub: getAfter" << endl;
  return 0;
}

bool DubPlaylist::listVisible() const {
  kdDebug(90010) << "dub: listVisible" << endl;
  return visible;
}

void DubPlaylist::init() {
  kdDebug(90010) << "dub: init" << endl;
  dubconfig = new DubConfigModule(this); // we pass this around to dub app
  dub = new Dub(this);
  kdDebug(90010) << "dub: init: test playlist() " << endl;
  playlist();
}

void DubPlaylist::showList() {
  kdDebug(90010) << "dub: showList" << endl;
  visible = true;
  Q_ASSERT(dub);
  dub->show();
}

void DubPlaylist::hideList() {
  kdDebug(90010) << "dub: hideList" << endl;
  visible = false;
  Q_ASSERT(dub);
  dub->hide();
}

//void DubPlaylist::remove(const PlaylistItem&) {
//  KMessageBox::information(0, "Removing files not supported yet, see configuration");
//}

//void DubPlaylist::sort() {
//  kdDebug(90010) << "sort" << endl;
//}

Playlist* DubPlaylist::playlist() {
  kdDebug(90010) << "dub: playlist pointer " << this << endl;
  return static_cast<Playlist*>(this);
}

void DubPlaylist::setCurrent(const KFileItem* file, bool play) {
  Q_ASSERT(file);
  currentItem = new DubPlaylistItem(*file);
  if (play)
    emit playCurrent();
}

void DubPlaylist::setCurrent(const PlaylistItem &play)
{
  currentItem=play;
  emit playCurrent();
}

/** No descriptions */
void DubPlaylist::updateCurrent() {
  KFileItem* active = dub->getActiveFile();
  if ( active ) {
    currentItem = new DubPlaylistItem(*active);
    emit playCurrent();
  }
}
