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

  $Id: kbgplayer.h 465369 2005-09-29 14:33:08Z mueller $

*/

#ifndef __KBGPLAYER_H 
#define __KBGPLAYER_H  

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kplayer.h>
#include <qdatastream.h> 

class KGame;


/**
 *
 *
 */
class KBgPlayer : public KPlayer
{
    Q_OBJECT
    
public:

    KBgPlayer();
    KBgPlayer(KGame* game);

    virtual int rtti() const;
    
    virtual bool load(QDataStream &stream);
    virtual bool save(QDataStream &stream);

};

#endif // __KBGPLAYER_H 

