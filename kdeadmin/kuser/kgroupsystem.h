/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Maintained by Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#ifndef _KU_GROUPSYSTEM_H_
#define _KU_GROUPSYSTEM_H_

#include <sys/types.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>

#include "kgroup.h"

class KGroupSystem : public KU::KGroups {
public:
  KGroupSystem( KUserPrefsBase *cfg );
  virtual ~KGroupSystem();

  virtual bool reload();
  virtual bool dbcommit() { return true; }
};

#endif // _KU_GROUPSYSTEM_H_

