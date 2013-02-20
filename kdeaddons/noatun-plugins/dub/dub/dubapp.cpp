//
//
// C++ Implementation for module: DubApp
//
// Description:
//
//
// Author: Eray (exa) Ozkural, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

// include files for QT
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>

// include files for KDE
#include <kiconloader.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <noatun/playlist.h>

// application specific includes
#include "dubapp.h"
#include "dubapp.moc"
#include "dubview.h"

#define ID_STATUS_MSG 1

DubApp::DubApp(QWidget* parent, const char* name)
  : KMainWindow(parent, name)
{
  kdDebug(90010) << "cons dubapp" << endl;
  config=kapp->config();

  // call inits to invoke all other construction parts
  initStatusBar();
  initActions();
  initView();
  setAutoSaveSettings();
}

DubApp::~DubApp()
{
  kdDebug(90010) << "destruct dubapp" << endl;
}

void DubApp::initActions()
{
  fileClose = KStdAction::close(this, SLOT(close()), actionCollection());
  setStandardToolBarMenuEnabled(true);
  createStandardStatusBarAction();

  fileClose->setStatusText(i18n("Close playlist window"));

  // use the absolute path to your dubui.rc file for testing purpose in createGUI();
  createGUI();
}


void DubApp::initStatusBar()
{
  // status bar
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG);
}

void DubApp::initView()
{
  // create the main widget here that is managed by KTMainWindow's view-region and
  // connect the widget to your document to display document contents.

  view = new DubView(this);
  setCentralWidget(view);
  setCaption("Dub Playlist",false);
}

// slots

void DubApp::slotStatusMsg(const QString &text)
{
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

// events

void DubApp::closeEvent(QCloseEvent*)
{
  hide();
}
