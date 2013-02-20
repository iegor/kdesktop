/*
** Copyright (C) 2000 Peter Putzer <putzer@kde.org>
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
#include "kbusymanager.h"

#include <qvaluestack.h>
#include <qapplication.h>

#include <kcursor.h>
#include <kapplication.h>

class KBusyManager::Private
{
public:
  Private()
    : Size (-1)
  {
  }
  
  ~Private()
  {
  }

  QValueStack<bool> Stack;
  int Size;
};

KBusyManager* KBusyManager::self ()
{
  static KBusyManager kb;

  return &kb;
}

bool KBusyManager::isBusy () const
{
  return d->Stack.top();
}

void KBusyManager::setBusy (bool val)
{
  d->Stack.push (val);
  d->Size++;

  if (val)
    QApplication::setOverrideCursor (KCursor::waitCursor());

  kapp->mainWidget()->setEnabled (!val);
}

void KBusyManager::restore ()
{
  if (d->Size < 1)
    return;

  bool oldBusy = d->Stack.pop();
  
  if (oldBusy)
    QApplication::restoreOverrideCursor();

  kapp->mainWidget()->setEnabled (!d->Stack.top());
}

KBusyManager::KBusyManager ()
  : d (new Private())
{
  setBusy (false);
}

KBusyManager::~KBusyManager ()
{
  delete d;
}
