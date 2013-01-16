/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
** This is the main widget for kpackage
** The whole widget is a DND drop zone where users can drop packages to
** be installed.
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




#ifndef KPACKAGE_H
#define KPACKAGE_H

#include "../config.h"

// KDE headers
#include <kurl.h>
#include <kmainwindow.h>
#include <kpPty.h>

class KFileDialog;
class KProgress;
class QFrame;
class KConfig;
class QLabel;
class Search;
class FindF;
class Options;
class pkgInterface;
class managementWidget;
class KAccel;
class QDropEevnt;
class KRecentFilesAction;
class KAction;
class kpRun;

#define kpinterfaceN 7

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class KPACKAGE : public QWidget
{
  Q_OBJECT

  ///////////// METHODS ------------------------------------------------------
public:
  KPACKAGE(KConfig *_config, QWidget *parent);
  // Constructor

  ~KPACKAGE();
  // Destructor

    enum { OPEN_READWRITE 	= 1,
	   OPEN_READONLY 	= 2,
	   OPEN_INSERT 		= 4 };

  void setStatus(const QString &s);
  // this sets the status bar's string to s

  void setPercent(int x);
  // this set the status bar's progress to x

  QString getStatus();
  // this gets the current status string on the status bar

  //  void setMode(int newmode, pkgInterface *type, int refresh);
  // This sets the mode to newmode and updates the display accordingly.

  void setup();

  pkgInterface *pkType(const QString &fname);
  // find type of package

  int typeIndex(pkgInterface *);
  // convert interface pointer to index
  
  void openNetFiles(const QStringList &urls, bool install=TRUE);
  void openNetFile(const KURL & url, bool install=TRUE);
  // open a file given a URL

  QString fetchNetFile(const KURL & url);
  // fetch a file given a URL

  static QString getFileName(const KURL & url, QString &cacheName);
  // return file name, if not local file cachename is name for cache entry

  static bool isFileLocal( const KURL & url );
  // true if URL refers to local or cached file

protected:
  void resizeEvent(QResizeEvent *re);
  // This is called when the widget is resized

  void dropEvent(QDropEvent *);
  // This is called when a URL has been dropped in the drop zone

  void dragEnterEvent(QDragEnterEvent* e);

private:

  void setupModeWidgets();
  // This sets up the mode widgets (ie management/installation widgets)

  void destroyModeWidgets();
  // This deletes the mode widgets (ie management/installation widgets)

  void setupStatusBar();
  // This sets up the status bar

  void arrangeWidgets();
  // This arranges the widgets in the window (should be called after a
  // resize event)

  KFileDialog* getFileDialog(const QString &captiontext);

  ///////////// SLOTS --------------------------------------------------------
public slots:

//  void modeFinished(int mode, pkgInterface *interface, int refresh);
  // This is called when the mode `mode' has finished.  KPACKAGE should
  // then change modes appropriately

  void fileOpen();
  // This is called when File->Open is selected from the menu

  void clearMarked();
  // clear package Marks

  void markAll();
  // mark all packages in the selected view

  void expandTree();
  void collapseTree();
  // expand and collapse file tree

  void fileOpenUrl();
  // menu item FIle->OpenUrl

  void find();
  // search for package

  void findf();
  // search for file in package

  void fileQuit();
  // This is called when File->Quit is selected from the menu

  void cleanUp();
  // Cleanup for Exit

  void reload();
  // reload file package infomation

 ///////////// SIGNALS ------------------------------------------------------

  ///////////// DATA ---------------------------------------------------------
public:

  enum { Management, Installation } ;
  // Widget modes

  KConfig *config;
  // pointer to kconfig object

  managementWidget *management;
  // management widget

  KURL save_url;
  // save the URL entered

  FindF *findialog;
  // find file dialog

private:
  int mode;
  // Widget mode

  // Menu item identifiers

  QFrame *statusbar;
  // the status bar

  KProgress *processProgress;
  // Progress bar for showing progress

  QLabel *status;
  // The actual status

  KFileDialog *file_dialog;
  /// If we load a file from the net this is the corresponding URL

  Search *srchdialog;
  // find package dialog

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class KPKG : public KMainWindow
{
  Q_OBJECT

  enum { Tback = 1,
	 Tforward = 2,
	 Tfileopen = 3,
	 Tftout = 4,
	 Tftin = 5,
	 Tfind = 6,
	 Tfindf = 7,
	 Treload = 8
  };

public:

  KPKG(KConfig *_config);
  // Constructor

  void add_recent_file(const QString &newfile);
  // keep list of files accessed

  void writeSettings();
  // write config settings

  void saveProperties(KConfig *config);
  void readProperties(KConfig *config);
  // save and read restart sstate

  void disableMenu();
  void enableMenu();
  // enable/deisable menu elements

  bool prop_restart;
  // indicates a restart from saved state

  Options *optiondialog;
  // Options dialog

  KConfig *config ;
  // Saved config information

  void disableNext();
  void enableNext();
  void disablePrevious();
  void enablePrevious();
  // Control next and previous commands


private:

  void setupMenu();
  // This sets up the menubar

  QStrList recent_files;

  KAction *pack_open;
  KAction *pack_find;
  KAction *pack_findf;
  KAction *kpack_reload;
  KAction *pack_prev;
  KAction *pack_next;
  KAction *pack_install;
  KAction *pack_uninstall;

  int  toolID, selectID;
  // refrences to  toolbar  and menu items

  bool hide_toolbar;
  // don't display toolbar

  KRecentFilesAction *recent;

public slots:

  void openRecent(const KURL& url);
  // opens file from list of recently opened ones

  void setOptions();
  // set options

  void saveSettings();
  // save config

  void configureToolBars();

  void clearPCache();
  // Clear package cache

  void clearDCache();
  // Clear directory cache

private slots:
  void slotNewToolbarConfig();

protected:
  bool queryClose ();
};

//////////////////////////////////////////////////////////////////////////////

extern KPKG *kpkg;
extern KPACKAGE *kpackage;
extern kpPty *kpty;
extern kpRun *kprun;
extern kpRun *kpstart;

extern QString hostName;

extern pkgInterface *kpinterface[];

void KpMsg(const QString &lab, const QString &msg, bool stop);
void KpMsgE(const QString &msg, bool stop = FALSE);
#endif

