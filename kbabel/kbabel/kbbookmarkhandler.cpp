/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002 by Marco Wegner <mail@marcowegner.de>

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


#include <qpopupmenu.h>
#include <qregexp.h>
#include <qstring.h>

#include "kbbookmarkhandler.h"


// implementation of KBabelBookmark
KBabelBookmark::KBabelBookmark(int msgindex, QString msgtext)
{
  _msgindex = msgindex;
  if (msgtext.length() > 32) {
    msgtext.truncate(32);
    msgtext.append("...");
  }

  // insert one '&' before every consecutive group of ampersands to keep the
  // first of these from acting either as accelerator or mask in the menu
  QRegExp rx("&+");
  int pos = msgtext.find(rx);
  while (pos >= 0) {
    msgtext.insert(pos, '&');
    pos = msgtext.find(rx, pos + rx.matchedLength() + 1);
  }

  _msgtext = msgtext;
}

int KBabelBookmark::msgindex() const
{
  return _msgindex;
}

QString KBabelBookmark::msgtext() const
{
  return _msgtext;
}


// implementation of KBabelBookmarkHandler
KBabelBookmarkHandler::KBabelBookmarkHandler(QPopupMenu* menu)
{
  _menu = menu;
  _list.setAutoDelete(true);
}

void KBabelBookmarkHandler::addBookmark(KBabelBookmark* b)
{
  // check if a bookmark to the current msgid exists already
  QPtrListIterator<KBabelBookmark> it(_list);
  KBabelBookmark* temp;

  while((temp = it.current()) != 0) {
    ++it;
    if (temp->msgindex() == b->msgindex()) {
      // gotcha
      delete b;
      return;
    }
  }

  // if it's okay then add the bookmark
  _list.append(b);
  _menu->insertItem(QString("#%1 - %2").arg(b->msgindex()).arg(b->msgtext()),
    this, SIGNAL(signalBookmarkSelected(int)), 0, b->msgindex());
}

void KBabelBookmarkHandler::addBookmark(int msgindex, QString msgtext)
{
  addBookmark(new KBabelBookmark(msgindex, msgtext));
}

void KBabelBookmarkHandler::setBookmarks(QPtrList<KBabelBookmark> list)
{
  QPtrListIterator<KBabelBookmark> it(list);
  KBabelBookmark* temp;

  while((temp = it.current()) != 0) {
    ++it;
    addBookmark(temp->msgindex(), temp->msgtext()); // make deep copy
  }
}

QPtrList<KBabelBookmark> KBabelBookmarkHandler::bookmarks() const
{
  return _list;
}

void KBabelBookmarkHandler::slotClearBookmarks()
{
  while (!_list.isEmpty()) {
    KBabelBookmark* b = _list.first();
    _menu->removeItem(b->msgindex());
    _list.remove(b);
  }
}

#include "kbbookmarkhandler.moc"
