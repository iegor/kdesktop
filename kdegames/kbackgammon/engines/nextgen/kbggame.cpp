/* Yo Emacs, this -*- C++ -*-

  Copyright (C) 1999-2001 Jens Hoefkens
  jens@hoefkens.com

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  $Id: kbggame.cpp 465369 2005-09-29 14:33:08Z mueller $

*/

#include "kbggame.moc"
#include "kbggame.h"

#include <kplayer.h>

#include <iostream>

/*
 * Constructor
 */
KBgGame::KBgGame(int cookie, QObject *parent)
    : KGame(cookie, parent)
{
    // do nothing...
}

bool KBgGame::playerInput(QDataStream &msg,KPlayer *player)
{
    Q_INT32 move;
    msg >>  move;
    cerr << "  Player " << player->id() << " moved to " << move << endl;
    return true;
}

