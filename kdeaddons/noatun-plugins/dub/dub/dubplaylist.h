// -*-c++-*-
//
// C++ Interface dubplaylist.h
//
// Description:
//
//
// Author: Eray Ozkural (exa), (C) 2001
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef DUBPLAYLIST_H
#define DUBPLAYLIST_H

class Dub;
class DubConfigModule;

#include <kfileitem.h>
#include <noatun/playlist.h>
#include <noatun/plugin.h>

#include "dubplaylistitem.h"

/**Playlist class. Implements the noatun playlist interface
  *@author Eray Ozkural (exa)
  */

class DubPlaylist : public Playlist, public Plugin  {
public:

  DubPlaylist();
  ~DubPlaylist();

  virtual void reset();

  virtual void clear();
  virtual void addFile(const KURL&, bool play=false);
  /**
  * Cycle everthing through forward
  **/
  virtual PlaylistItem next();
  /**
  * return the one that might/should be playing now
  **/
  virtual PlaylistItem current();
  /**
  * Cycle through backwards
  **/
  virtual PlaylistItem previous();
  virtual PlaylistItem getFirst() const;
  virtual PlaylistItem getAfter(const PlaylistItem &item) const;

  virtual bool listVisible() const;
  virtual void init();
  virtual Playlist* playlist();

  virtual void setCurrent(const PlaylistItem &play);

public slots:
  virtual void showList();
  virtual void hideList();
  virtual void setCurrent(const KFileItem* file, bool play = false);
  void updateCurrent();
//  virtual void remove(const PlaylistItem&);
//  virtual void sort();

  DubConfigModule *dubconfig;

private:
  Dub *dub;
  PlaylistItem currentItem;
  bool visible;
};

#endif
