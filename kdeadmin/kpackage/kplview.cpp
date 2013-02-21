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
#include <qpixmap.h>
#include <qptrstack.h>
#include <qheader.h>

#include <kdebug.h>
#include <klocale.h>

// kpackage.headers
#include "kpackage.h"
#include "pkgInterface.h"
#include "packageInfo.h"
#include "managementWidget.h"
#include "packageDisplay.h"
#include "kplview.h"
#include "options.h"

#define MPOS  1

KpTreeList::KpTreeList( QWidget *parent ) :
KListView (parent)
{
  markPkg = 0;
  setShowSortIndicator(true);

  QPtrStack<QString> stack();
  readTreeType();

  setFrameStyle(QFrame::Panel|QFrame::Sunken);
  setLineWidth(2);
  setItemMargin(2);
  addColumn(i18n("Package"));
  setColumnWidthMode(0,QListView::Manual);
  addColumn(i18n("Mark"));
  setColumnWidthMode(1,QListView::Manual);
  addColumn(i18n("Summary"));
  setColumnWidthMode(2,QListView::Manual);
  addColumn(i18n("Size"));
  setColumnWidthMode(3,QListView::Manual);
  addColumn(i18n("Version"));
  setColumnWidthMode(4,QListView::Manual);
  addColumn(i18n("Old Version"));
  setColumnWidthMode(5,QListView::Manual);
  
  //  setAllColumnsShowFocus(TRUE);
  setRootIsDecorated(TRUE);
  readTreeConfig();
  update();
  show();

}

void KpTreeList::clear()
{
  markPkg = 0;
  emit cleared();
  KListView::clear();
}

KpTreeListItem *KpTreeList::firstChild()
{
  return  (KpTreeListItem *)KListView::firstChild();
}

KpTreeListItem *KpTreeList::currentItem()
{
  return  (KpTreeListItem *)KListView::currentItem();
}

void KpTreeList::contentsMousePressEvent ( QMouseEvent * e )
{
  bool markUpdate = false;

  if (e->button() == LeftButton) {
    if (inMark(e->x())) {
      QPoint vp = contentsToViewport(e->pos());
      KpTreeListItem *i = ( KpTreeListItem *)itemAt( vp );
      if (i && i->childCount() == 0) {   
	if (e->state() == ShiftButton) {
	  if (i->childCount() == 0) {
	    i->setMark(true);
	    markUpdate = true;
	  }
	  KpTreeListItem *item = i;
	  while ((item = (KpTreeListItem *)item->itemAbove()) && !item->marked) {;}
	  if (item) {
	    item = i;
	    while ((item = (KpTreeListItem *)item->itemAbove()) && !item->marked) {
	      if (item->childCount() == 0) {
		item->setMark(true);
		markUpdate = true;
	      }
	    }
	  } else {
	    item = i;
	    while ((item = (KpTreeListItem *)item->itemBelow()) && !item->marked) {;}
	    if (item) {
	      item = i;
	      while ((item = (KpTreeListItem *)item->itemBelow()) && !item->marked) {
		if (item->childCount() == 0) {
		  item->setMark(true);
		  markUpdate = true;
		}
	      }
	    }
	  }
	} else {
	  i->toggleMark();
	  markUpdate = true;
	}
	setCurrentItem(i);
      } else 
	KListView::contentsMousePressEvent (e);
    } else 
      KListView::contentsMousePressEvent (e);
  }
  
  if (markUpdate)
     emit updateMarked();

}
 
bool KpTreeList::inMark(int x) {
  int st = 0;
  int i;

    
  QHeader* const thisHeader = header();
  int mpos = thisHeader->mapToIndex(MPOS);

  for (i = 0; i < mpos; i++)
    st += columnWidth(i);

   return (x >= st && x <= st + columnWidth(mpos));
}


///////////////////////////////////////////////////////////////////////////

void KpTreeList::expand()
{
  KpTreeListItem  *item = firstChild();

  if (!item)
    return;

  do {
      expandChild(item->firstChild());
  } while ((item = item->nextSibling()));
}

void  KpTreeList::expandChild(KpTreeListItem *it)
{
  do {
    if (it->childCount() > 0) {
      expandChild(it->firstChild());
      it->setVisible(true);
    }

  } while ((it = it->nextSibling()));
}

///////////////////////////////////////////////////////////////////////////

void KpTreeList::sweep(bool init)
{
  KpTreeListItem  *item = firstChild();

  if (!item)
    return;

  if (init) {
    expand();
  }

  do {
      sweepChild(item->firstChild());
  } while ((item = item->nextSibling()));
}

int  KpTreeList::sweepChild(KpTreeListItem *it)
{
  int ret, shown = 0;

  do {
    if (it->childCount() > 0) {
      ret = sweepChild(it->firstChild());
      //      kdDebug()<<"VV="<<it->text(0)<<" "<<it->isVisible()<<" " << ret << "\n";
      if (!ret) {
	it->setVisible(false);
      } else {
	if (it->isVisible())
	  shown += ret;
      }
    } else {
      //      kdDebug()<<"VV="<<it->text(0)<<" "<<it->isVisible()<<"\n";
      if (!it->info->display(treeType)) {
	it->setVisible(false);
      } else {
	if (it->isVisible())
	  shown++;
      }
    }
  } while ((it = it->nextSibling()));
  return shown;
}


///////////////////////////////////////////////////////////////////////////
void KpTreeList::clearMarked(KpTreeListItem *item)
{
  while (item) {
    if (item->childCount() > 0) {
      clearMarked(item->firstChild());
    }
    item->setMark(false);
    item = item->nextSibling();
  }
}

void KpTreeList::markAll(KpTreeListItem *item)
{
  while (item) {
    if (item->childCount() > 0) {
      markAll(item->firstChild());
    }
    else {
      if (item->info->display(treeType)) {
        item->setMark(true);
      }
    }
    item = item->nextSibling();
  }
}

void KpTreeList::findMarked(KpTreeListItem *item, QPtrList<KpTreeListItem> &selList)
{
  while (item) {
    if (item->childCount() > 0) {
      findMarked(item->firstChild(), selList);
    }
    if (item->marked) {
      selList.insert(0,item);
    }
    item = item->nextSibling();
  }
}

void KpTreeList::countMarked(KpTreeListItem *item, int &cntInstall, int &cntUnInstall)
{
  while (item) {
    if (item->childCount() > 0) {
      countMarked(item->firstChild(), cntInstall, cntUnInstall);
    }
    if (item->marked) {
      if (item->info->isInstallable())
	cntInstall++;
      else
	cntUnInstall++;
    }
    item = item->nextSibling();
  }
}

///////////////////////////////////////////////////////////////////////////
void KpTreeList::expandTree(KpTreeList *list)
{
 KpTreeListItem *item = list->firstChild();

  while (item) {
    if (item->childCount() > 0) {
      item->setOpen(TRUE);
      expandTree(item);
    }
  item = item->nextSibling();
  }
}


void KpTreeList::expandTree(KpTreeListItem *pitem)
{
  KpTreeListItem *item = pitem->firstChild();

  while (item) {
    if (item->childCount() > 0) {
      item->setOpen(TRUE);
      expandTree(item);
    }
    item = item->nextSibling();
  }
}

void KpTreeList::collapseTree(KpTreeList *list)
{
  KpTreeListItem *item = list->firstChild();

  while (item) {
    if (item->childCount() > 0) {
      collapseTree(item);
    }
  item = item->nextSibling();
  }
}

void KpTreeList::collapseTree(KpTreeListItem *pitem)
{
  int n = 0;
  KpTreeListItem *item = pitem->firstChild();

  while (item) {
    if (item->childCount() > 0) {
      n++;
      collapseTree(item);
    }
    item = item->nextSibling();
  };
  if (n)
    pitem->setOpen(TRUE);
  else
    pitem->setOpen(FALSE);
}

///////////////////////////////////////////////////////////////////////////
// A package has been highlighted in the list tree
void KpTreeList::packageHighlighted(QListViewItem *item, packageDisplayWidget *packageDisplay)
{
  KpTreeListItem *sel = (KpTreeListItem *)item;

  if (!sel || sel->childCount()) {
    packageDisplay->changePackage(0);
  } else if (sel) {
    if (!notPress) {
      int n = stack.at();
      int num = stack.count();
      for (int i = num - 1; i >= n; i--) {
	stack.remove(i);
      }
      kpkg->disableNext();
    }

    // Disable the tree list while we do this
    setEnabled(FALSE);

    // Tell everything that is interested to change packages
     packageDisplay->changePackage(sel->info);

    // Re-enable the treeList and uninstall button
    setEnabled(TRUE);
    setFocus();

    if (!notPress) {
      stack.append(sel);
    }
  }

  notPress = false;
}

KpTreeListItem *KpTreeList::search(const QString &str, const QString &head,
				KpTreeListItem  *start)
{
  KpTreeListItem  *item = firstChild();

  searchCitem = start;
  searchSkip = FALSE;
  searchSubstr = FALSE;
  searchStr = str;
  searchResult = 0;

  do {
    if (item->text(0) == head) {
      searchChild(item->firstChild());
      if (searchResult != 0)
	return searchResult;
    }
  } while ((item = item->nextSibling()));
  return 0;
}

KpTreeListItem *KpTreeList::search(const QString &str, bool subStr, bool wrap,
			     bool start=FALSE)
{
  if (!firstChild())
    return 0;

  if (start)
    searchCitem = 0;
  else
    searchCitem = currentItem();
  searchSkip = !wrap;
  searchSubstr = subStr;
  searchStr = str;
  searchResult = 0;

  searchChild(firstChild());

  return changePack(searchResult);
}

bool KpTreeList::searchChild(KpTreeListItem *it)
{
  do {
    if (!searchSkip) {
      QString s = it->text(0);
      //      kdDebug() << "s='" << s << "'='" <<  searchStr << "\n";
      if ((it->childCount() == 0) && (it->info->display(treeType)) &&
	  (searchSubstr ? s.contains(searchStr,FALSE) : s == searchStr)) {
	searchResult = it;
	return TRUE;
      }
    }

    if (searchCitem == it) {
      if (searchSkip) {
	searchSkip = FALSE;
      } else {
	return TRUE;
      }
    }

    if (it->childCount() > 0) {
      if (searchChild(it->firstChild()))
	return TRUE;
    }
  } while ((it = it->nextSibling()));
  return FALSE;
}

KpTreeListItem *KpTreeList::changePack(KpTreeListItem *searchResult, bool push)
{
  if (searchResult) {
    QListViewItem *i;

    i = searchResult;
    while ((i = i->parent())) {
      i->setOpen(TRUE);
    }
    if (push) {
      stack.append(searchResult);
      kpkg->enablePrevious();
    }

    notPress = true;
    setSelected(searchResult,TRUE);
    setCurrentItem(searchResult);
    ensureItemVisible(searchResult);
    return searchResult;
  } else {
    return 0;
  }
}

void KpTreeList::next()
{
  int n = stack.at();
  KpTreeListItem *s = stack.at(n + 1);
  if (s) {
    changePack(s, false);
  }
  if (n >= int(stack.count() - 2)) {
    kpkg->disableNext();
  }
  if (n >= 0) {
    kpkg->enablePrevious();
  }
}

void KpTreeList::previous()
{
  int n = stack.at();
  KpTreeListItem *s = stack.at(n-1);

  if (s) {
    changePack(s, false);
    kpkg->enableNext();
  }
  if (n <= 1) {
    kpkg->disablePrevious();
  }
  if (n < int(stack.count() - 2)) {
    kpkg->enableNext();
  }
}

void KpTreeList::stackRemove(KpTreeListItem *pack)
{
  int n = stack.find(pack);
  if (n >= 0) {
    if (n == 0) {
      kpkg->disablePrevious();
    }
    if (n >= int(stack.count() - 1)) {
      kpkg->disableNext();
    }
    stack.remove(pack);
  }
}

////////////////////////////////////////////////////////////////
void KpTreeList::writeTreeType()
{
  KConfig *config = kapp->config();

  config->setGroup("Kpackage");

  config->writeEntry("Package_Display",treeType);
}

void KpTreeList::readTreeType()
{
  KConfig *config = kapp->config();

  config->setGroup("Kpackage");

  treeType = config->readNumEntry("Package_Display",3);

}

//////////////////////////////////////////////////////////////////////////////

void KpTreeList::writeTreeConfig()
{
  saveLayout( kapp->config(), "Tree");
}

void KpTreeList::readTreeConfig()
{
  KConfig *config = kapp->config();

  restoreLayout(config, "Tree");

  config->setGroup("Tree");
  if (!config->hasKey("ColumnWidths")) {
    int i,n;
    int num[] = {185,37,180,54,95,95};

    QString colpos;
    for (i = 0; i < 6; i++) {
      colpos.setNum(i);
      n = config->readNumEntry(colpos,num[i]);
      setColumnWidth(i,n);
    }
  }
}


//////////////////////////////////////////////////////////////////////////////
KpTreeListItem::KpTreeListItem( QListViewItem *parent, packageInfo* pinfo,
                  const QPixmap& thePixmap,
                  QString label1, QString label2 ,
                  QString label3 , QString label4 ,
                  QString label5 , QString label6 ,
                  QString label7 , QString label8 
 
) :  QListViewItem(parent, label1, label2, label3, label4, label5,
                label6, label7, label8)
{
  info = pinfo;
  marked = false;
  setPixmap(0, thePixmap);
  if (!label2.isNull())
    setPixmap(1,info->interface->markUnInst);
}
 
KpTreeListItem::KpTreeListItem( KListView *parent, packageInfo* pinfo,
                  const QPixmap& thePixmap,
                  QString label1, QString label2 ,
                  QString label3 , QString label4 ,
                  QString label5 , QString label6 ,
                  QString label7 , QString label8 
) :  QListViewItem(parent, label1, label2, label3, label4, label5,
                label6, label7, label8)
{
  info = pinfo;
  marked = false;
  setPixmap(0, thePixmap);
  if (!label2.isNull())
    setPixmap(1,info->interface->markUnInst);
}

KpTreeListItem *KpTreeListItem::firstChild()
{
  return  (KpTreeListItem *)QListViewItem::firstChild();
}

KpTreeListItem *KpTreeListItem::nextSibling()
{
  return  (KpTreeListItem *)QListViewItem::nextSibling();
}

void KpTreeListItem::toggleMark() 
{
  marked = ! marked;
  if (marked)
    setPixmap(1,info->interface->markInst);
  else
    setPixmap(1,info->interface->markUnInst);
}

void KpTreeListItem::setMark(bool mark) 
{
  marked = mark;
  if (mark)
    setPixmap(1,info->interface->markInst);
  else {
    if (info)
      setPixmap(1,info->interface->markUnInst);
  }
}

void KpTreeListItem::hide() 
{
  setHeight(0);
}

void KpTreeListItem::show() 
{
  setup();
}

int KpTreeListItem::compare( QListViewItem *i, int col, bool ascending ) const
{ // Make sorting more certain
  if (col == 3) { // size column
    QString k, j;

    j = key( col, ascending );
    j = j.replace(' ','0');
    j = j.rightJustify(6,'0');

    k = i->key( col, ascending );
    k = k.replace(' ','0');
    k = k.rightJustify(6,'0');

    //    kdDebug() << k <<"=" << j << "\n";
    return j.compare(k);
  } else {
    return  QListViewItem::compare(i, col, ascending);
  }
}

#include "kplview.moc"
