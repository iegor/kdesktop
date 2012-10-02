/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
                2001-2002 by Stanislav Visnovsky
                            <visnovsky@kde.org>

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

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */

#ifndef CATMANRESOURCE_H
#define CATMANRESOURCE_H

#include <kiconloader.h>

#define COL_NAME 0
#define COL_MARKER 1
#define COL_FUZZY 2
#define COL_UNTRANS 3
#define COL_TOTAL 4
#define COL_CVS_OR_SVN 5
#define COL_REVISION 6
#define COL_TRANSLATOR 7

#define ICON_OK UserIcon("ok",KIcon::DefaultState)
#define ICON_MISSING UserIcon("missing",KIcon::DefaultState)
#define ICON_NEEDWORK UserIcon("needwork",KIcon::DefaultState)
#define ICON_BROKEN UserIcon("broken",KIcon::DefaultState)
#define ICON_UPDATING SmallIcon("reload")
#define ICON_FLAG SmallIcon("flag")
#define ICON_FOLDER_CLOSED_OK SmallIcon("folder_green")
#define ICON_FOLDER_CLOSED_WORK SmallIcon("folder_red")
#define ICON_FOLDER_OPEN_OK SmallIcon("folder_green_open")
#define ICON_FOLDER_OPEN_WORK SmallIcon("folder_red_open")
#define ICON_ERROR UserIcon("error",KIcon::DefaultState)
#define ICON_NOFLAG UserIcon("noflag",KIcon::DefaultState)

// Needed for determining which actions should be enabled and which not.
#define NEEDS_PO       1
#define NEEDS_POT      2
#define NEEDS_MARK     4
#define NEEDS_DIR      8
#define NEEDS_PO_CVS  16
#define NEEDS_POT_CVS 32
#define NEEDS_PO_SVN  64
#define NEEDS_POT_SVN 128

#endif
