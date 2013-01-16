/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
// Author: Damyan Pepper
//
// This widget is used to provide the management mode of ksetup.
// There are two subwidgets; firstly a tree list showing all the
// currently installed packages and secondly a display of the currently
// selected package's properties.
//
// There are also some control buttons which allow the currently
// selected package to be uninstalled or verified.
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




#ifndef MANAGEMENTWIDGET_H
#define MANAGEMENTWIDGET_H

#include "../config.h"
// Standard Headers

// Qt Headers
#include <qframe.h>
#include <qpushbutton.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qhbox.h>

// KDE headers
#include <kaction.h>
#include <klistview.h>
#include <klistviewsearchline.h>

// ksetup headers
#include "packageInfo.h"
#include "kplview.h"

class packageDisplayWidget;
class packageInfo;
class QSplitter;
class KActionCollection;
class KToolBar;
class managementWidget;

class KpListViewSearchLine : public KListViewSearchLine
{
  Q_OBJECT

public:

  KpListViewSearchLine(QWidget *parent, KpTreeList *listView);

  ~KpListViewSearchLine();

  
  void updateSearch(const QString &s = QString::null);

private:
  KpTreeList *list;
};


class managementWidget : public QFrame
{
  Q_OBJECT

  ///////////// METHODS ------------------------------------------------------
public:
  managementWidget(QWidget *parent);
  // Constructor

  ~managementWidget();
  // Destructor

  KpTreeListItem *updatePackage(packageInfo *pki, bool install);
  // update package in treelist

  void readPSeparator();
  void writePSeparator();
  // config: position of panel seperator

 void doChangePackage(packageInfo *p);
  // emit change package

  KpTreeListItem *search(QString str, bool subStr, bool wrap,
		    bool start=FALSE);
protected:
  void resizeEvent(QResizeEvent *re);
  // This is called when the widget is resized

private:
  void setupWidgets();
  // This sets up the sub-widgets

  void setupInstButton();
  // Set button for inst or uninst

  void arrangeWidgets();
  // This arranges the widgets in the window (should be called after a
  // resize event)

  void setupMultButton(int &cntInstall, int &cntUnInstall);
  // Setup mult install/uninstall button appropriately


  ///////////// SLOTS   ------------------------------------------------------
 public slots:
 void collectData(bool refresh);
  // This collects data about all the packages installed.
  // The list tree is filled with this data.  Whenever something happens
  // that requires data to be (re)collected a signal connected to this slot
  // should be emitted.  This function can also be called directly.

  void rebuildListTree();
  // This rebuilds the list tree.  This would normally be called if the
  // data contained about the packages has been changed (e.g. a verification
  // failed / succeeded).

  void uninstallMultClicked();
  // This is called when uninstalling multiple packages

  void uninstallSingleClicked();
  // This is called when uninstalling a single package

  void installSingleClicked();
  // This is called when the install button has been clicked with single package

  void installMultClicked();
  // This is called when the install button has been clicked with multiple packages

  void setInstallAction(KAction *a) { install_action = a; }
  void setUninstallAction(KAction *a) { uninstall_action = a; }

 void packageHighlighted(QListViewItem *);
  // This is called when a package has been highlighted in the list tree

  void tabChanged(int);
  // treelist display tab changed

  void checkMarked();
  // Count marked packages that can be installed/uninstalled

 ///////////// SIGNALS ------------------------------------------------------


  ///////////// DATA ---------------------------------------------------------
private:

  QPushButton *linstButton,*luinstButton,*instButton,*uinstButton;
  // This button is used to (un)install the selected package

  packageDisplayWidget *packageDisplay;
  // This widget displays the package info / file-list

  QBoxLayout *top, *leftbox, *rightbox, *lbuttons, *rbuttons;
  // These are the geometry managers

  QFrame *leftpanel, *rightpanel;
  // frame to put QBox in

  QTabWidget *ltab;
  // tab between various treelist displays

  QSplitter *vPan;
  // veritcal panner between panels

  KToolBar *searchToolBar;

  QPtrList<KpTreeListItem> selList;
  // list for selected packages

  QString tType[4];
  // identifiers for tree display

public:
  QPtrList<packageInfo> *allPackages;
  // The list of packages

  QDict<packageInfo> *dirInstPackages;
  // maps installed package name to package

  QDict<packageInfo> *dirUninstPackages;
  // maps uninstalled package name to package

  QDict<packageInfo> *dirInfoPackages;
  // maps Info package name to package

  KpTreeList *treeList;
  // This is the tree list where all the packages / groups are displayed

  KpListViewSearchLine *searchLine;
  // Widget for search treeList

  KAction *install_action;
  KAction *uninstall_action;
};

#endif



