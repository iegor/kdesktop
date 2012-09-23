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


#ifndef KBBOOKMARKHANDLER_H
#define KBBOOKMARKHANDLER_H

#include <qobject.h>
#include <qptrlist.h>

class QPopupMenu;
class QString;


/**
 * Simple class representing a bookmark in KBabel.
 * Stored in the class are the msgindex (msgid #) and part of its text.
 */
class KBabelBookmark
{
  public:
    /**
     * Constructor.
     * The msgtext will be truncated automatically when creating the bookmark.
     *
     * @param msgindex the index of the bookmarked msgid.
     * @param msgtext the msgid.
     */
    KBabelBookmark(int msgindex, QString msgtext);

    /**
     * Return the index of the msgid.
     *
     * @return the index of the bookmarked msgid.
     */
    int msgindex() const;
    /**
     * Return the msgid.
     *
     * @return the msgid.
     */
    QString msgtext() const;

  private:
    /**
     * The classes' own copy of the msgindex.
     */
    int _msgindex;
    /**
     * The classes' own copy of the msgid.
     */
    QString _msgtext;
};


/**
 * Simple class for managing bookmarks in KBabel.
 */
class KBabelBookmarkHandler : public QObject
{
  Q_OBJECT

  public:
    /**
     * Constructor.
     *
     * @param menu the pointer to the menu where the bookmarks will be
     *  displayed.
     */
    KBabelBookmarkHandler(QPopupMenu* menu);

    /**
     * Add a bookmark.
     * The bookmark is added to the internal list as well as to the menu.
     *
     * @param b the bookmark to be added.
     */
    void addBookmark(KBabelBookmark* b);
    /**
     * Add a bookmark.
     * Overloaded member. The bookmark will first be created from msgindex
     * and msgtext.
     *
     * @param msgindex the index of the bookmark's msgid.
     * @param msgtext the msgindex of the bookmark to be added.
     */
    void addBookmark(int msgindex, QString msgtext);
    /**
     * Provide the handler with a list of bookmarks.
     * Especially useful when creating a new view of the KBabel window.
     *
     * @param list the list to be copied.
     */
    void setBookmarks(QPtrList<KBabelBookmark> list);
    /**
     * Return the list of bookmarks for a new view of the KBabel window.
     *
     * @return the internal list of bookmarks
     */
    QPtrList<KBabelBookmark> bookmarks() const;

  public slots:
    /**
     * Clear all bookmarks.
     */
    void slotClearBookmarks();

  signals:
    /**
     * This signal is emitted if one of the bookmarks was activated.
     * The signal contains the msgindex of the bookmark.
     */
    void signalBookmarkSelected(int);

  private:
    /**
     * The pointer to the menu.
     */
    QPopupMenu* _menu;
    /**
     * The internal list for storing the bookmarks.
     */
    QPtrList<KBabelBookmark> _list;
};

#endif // KBBOOKMARKHANDLER_H
