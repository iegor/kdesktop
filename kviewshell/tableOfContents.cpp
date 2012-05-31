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

#include <config.h>

#include <klocale.h>
#include <kdebug.h>

#include "tableOfContents.h"
#include "bookmark.h"
#include "kvsprefs.h"


TocItem::TocItem(TocItem* parent)
  : KListViewItem(parent)
{
}

TocItem::TocItem(QListView* parent)
  : KListViewItem(parent)
{
}


TableOfContents::TableOfContents(QWidget* parent)
  : KListView(parent)
{
  addColumn(i18n("Topic"));
  addColumn(i18n("Page"));

  setSorting(-1);
  setRootIsDecorated(true);
  setSelectionMode(QListView::NoSelection);
  setResizeMode(AllColumns);
  setColumnWidthMode(0, Manual);
  setColumnWidthMode(1, Manual);
  setFullWidth(true);

  readSettings();

  connect(this, SIGNAL(executed(QListViewItem*)), this, SLOT(itemClicked(QListViewItem*)));
}

TableOfContents::~TableOfContents()
{
  writeSettings();
}

void TableOfContents::writeSettings()
{
  saveLayout(KVSPrefs::self()->config(), "tocLayout");
}

void TableOfContents::readSettings()
{
  restoreLayout(KVSPrefs::self()->config(), "tocLayout");
}

void TableOfContents::setContents(const QPtrList<Bookmark>& bookmarks)
{
  clear();
  addItems(bookmarks);
}

void TableOfContents::addItems(const QPtrList<Bookmark>& _bookmarks, TocItem* parent)
{
  kdDebug(1223) << "TableOfContents::setContents()" << endl;
  if (_bookmarks.isEmpty())
    return;
  kdDebug(1223) << "Bookmarks are not empty" << endl;

  // Why isn't QPtrList const-correct?
  QPtrList<Bookmark> bookmarks = _bookmarks;
  for (Bookmark* current = bookmarks.last(); current; current = bookmarks.prev() ) {
    TocItem* item;
    if (!parent)
      item = new TocItem(this);
    else
      item = new TocItem(parent);
    
    item->setText(0, current->bookmarkText);
    if (current->position.page != 0)
      item->setText(1, QString().setNum(current->position.page));
    else
      item->setText(1, "--");
    item->setAnchor(current->position);
    
    if (!current->subordinateBookmarks.isEmpty())
      addItems(current->subordinateBookmarks, item);
  }
 }

void TableOfContents::itemClicked(QListViewItem* _item)
{
  TocItem* item = static_cast<TocItem*>(_item);

  Anchor destination = item->getAnchor();

  emit gotoPage(destination);
}

#include "tableOfContents.moc"
