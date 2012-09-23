// -*- C++ -*-
/***************************************************************************
 *   Copyright (C) 2005 by Stefan Kebekus                                  *
 *   kebekus@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#ifndef _bookmarks_h_
#define _bookmarks_h_

#include "anchor.h"

#include <qptrlist.h>
#include <qstring.h>


/** \brief Bookmark

This class represents a bookmark. It contains the bookmark text, an
anchor that represents the target of the bookmark, and a list of
subordinate bookmarks.

@warning The list of subordinate bookmarks owns its entries,
i.e. autoDelete() is set to true. Thus, extra care must be taken when
copying bookmarks. Otherwise, a segfault could result.
*/

class Bookmark {
 public:
  /** Default constructor.

  Constructs an invalid bookmark with an empty bookMarkText and an
  invalid position
  */
  Bookmark() {subordinateBookmarks.setAutoDelete(true);}
  
  /** Constructs a bookmark with given a text and anchor

  @param text name of the bookmark, as it appears in the GUI

  @param pos an Anchor that specifies the position of the bookmark
  */
  Bookmark(const QString &text, const Anchor &pos) {
    Bookmark();
    bookmarkText = text;
    position = pos;
  }
  
  /** Convenience funtion, behaves as   Bookmark(const QString &text, const Anchor &pos)

  @param text name of the bookmark, as it appears in the GUI

  @param page number of the where the bookmark lives. Recall the
  convention that '0' is an invalid page, the first page is '1'

  @param distance_from_top position on the page, distance from
  top of the page
  */
  Bookmark(const QString& text, const PageNumber& page, const Length& distance_from_top) {
    Bookmark();
    bookmarkText = text;
    position.page = page;
    position.distance_from_top = distance_from_top;
  }
  
  /** Label of the bookmark as it will appear in the GUI to the user,
      such as "Section 1", "Theorem 12", etc. */
  QString bookmarkText;
  
  /** The anchor that specifies the position of the bookmark */
  Anchor position;
  
  /** List of subordinate bookmarks

   Bookmarks generally come in trees, and some bookmarks can have
   subordinate bookmarks. For instance, the bookmark that
   respresents a chapter heading would have headings of subsections
   as subordinate bookmarks. These are contained in this list.
  
   @warning This list owns its entries, e.g. autoDelete() is set to
   true. Thus, extra care must be taken when copying this
   list. Otherwise, a segfault could result.
  */
  QPtrList<Bookmark> subordinateBookmarks;
  
  // Returns true if the bookmark is valid, false otherwise. A
  // bookmark is valid if the text is not empty and the anchor is
  // valid.
  bool isValid() const {return (!bookmarkText.isEmpty()) && position.isValid();}
};


#endif
