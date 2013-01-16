/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/

#include "../config.h"
#include <kpackage.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "utils.h"
#include "kplview.h"

void KpMsgE( const QString &msg, bool stop)
{
 KpMsg( "Error", msg,  stop);
}

void KpMsg(const QString &lab, const QString &msg, bool stop)
{
  if (stop)
    KMessageBox::sorry(kpkg, msg, lab);
  else
    KMessageBox::information(kpkg, msg, lab);
}

KpTreeListItem *findGroup(const QString &name, KpTreeListItem *search)
{
  if(!search)
    return NULL;

  do {
    if (name == search->text(0)) {
      return search;
    }
  } while( (search=search->nextSibling()) != NULL);

  return NULL;
}



