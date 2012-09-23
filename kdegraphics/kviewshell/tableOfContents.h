// -*- C++ -*-
/***************************************************************************
 *   Copyright (C) 2005 by Wilfried Huss                                   *
 *   Wilfried.Huss@gmx.at                                                  *
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

#ifndef TABLEOFCONTENTS_H
#define TABLEOFCONTENTS_H

#include "anchor.h"

#include <klistview.h>

class Bookmark;


class TocItem : public KListViewItem
{
public:
  TocItem(TocItem* parent);
  TocItem(QListView* parent);

  void setAnchor(const Anchor & _anchor) { anchor = _anchor; }
  Anchor getAnchor() { return anchor; }

private:
  Anchor anchor;
};


class TableOfContents : public KListView
{
Q_OBJECT

public:
  TableOfContents(QWidget* parent);
  virtual ~TableOfContents();

  void setContents(const QPtrList<Bookmark>& bookmarks);

  void writeSettings();
  void readSettings();

signals:
  void gotoPage(const Anchor&);

private:
  void addItems(const QPtrList<Bookmark>& bookmarks, TocItem* parent = 0);

private slots:
  void itemClicked(QListViewItem*);
};

#endif
