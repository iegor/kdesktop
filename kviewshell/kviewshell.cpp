/*
 * Parts of this file are
 * Copyright 2003 Waldo Bastian <bastian@kde.org>
 *
 * These parts are free software; you can redistribute and/or modify
 * them under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
 */


#include <qfileinfo.h>
#include <qtimer.h>
#include <qregexp.h>

#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kaction.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmenubar.h>
#include <klibloader.h>
#include <kstdaction.h>
#include <kedittoolbar.h>
#include <kurldrag.h>
#include <kparts/partmanager.h>
#include <kmimetype.h>

#include <kprogress.h>
#include <qlabel.h>

#include <stdlib.h>
#include <unistd.h>

#include "kviewpart_iface.h"
#include <kstatusbar.h>
#include <kkeydialog.h>
#include "kviewshell.moc"


#define StatusBar_ID_PageNr 1
#define StatusBar_ID_PageSize 2
#define StatusBar_ID_Zoom 3


KViewShell::KViewShell(const QString& defaultMimeType)
  : KParts::MainWindow()
{
  // create the viewer part

  // Try to load
  KLibFactory *factory = KLibLoader::self()->factory("kviewerpart");
  if (factory) {
    if (defaultMimeType == QString::null)
    {
      view = (KViewPart_Iface*) factory->create(this, "kviewerpart", "KViewPart");
    }
    else
    {
      QStringList args;
      args << defaultMimeType;
      view = (KViewPart_Iface*) factory->create(this, "kviewerpart", "KViewPart", args);
    }
    if (!view)
      ::exit(-1);
  } else {
    KMessageBox::error(this, i18n("No viewing component found"));
    ::exit(-1);
  }

  setCentralWidget(view->widget());

  // file menu
  KStdAction::open(view, SLOT(slotFileOpen()), actionCollection());
  recent = KStdAction::openRecent (this, SLOT(openURL(const KURL &)), actionCollection());
  reloadAction = new KAction(i18n("Reload"), "reload", CTRL + Key_R, view, SLOT(reload()), actionCollection(), "reload");
  closeAction = KStdAction::close(this, SLOT(slotFileClose()), actionCollection());
  KStdAction::quit (this, SLOT(slotQuit()), actionCollection());

  connect(view, SIGNAL(fileOpened()), this, SLOT(addRecentFile()));

  // view menu
  fullScreenAction = KStdAction::fullScreen(this, SLOT(slotFullScreen()), actionCollection(), this, "fullscreen" );

  // settings menu
  createStandardStatusBarAction();

  setStandardToolBarMenuEnabled(true);

  KStdAction::keyBindings(this, SLOT(slotConfigureKeys()), actionCollection());
  KStdAction::configureToolbars(this, SLOT(slotEditToolbar()), actionCollection());

  // statusbar connects
  connect( view, SIGNAL( zoomChanged(const QString &) ), this,SLOT( slotChangeZoomText(const QString &) ) );
  connect( view, SIGNAL( pageChanged(const QString &) ), this,SLOT( slotChangePageText(const QString &) ) );
  connect( view, SIGNAL( sizeChanged(const QString &) ), this,SLOT( slotChangeSizeText(const QString &) ) );

  // Setup session management
  connect( this, SIGNAL( restoreDocument(const KURL &, int) ), view, SLOT( restoreDocument(const KURL &, int)));
  connect( this, SIGNAL( saveDocumentRestoreInfo(KConfig*) ), view, SLOT( saveDocumentRestoreInfo(KConfig*)));

  setXMLFile( "kviewshell.rc" );
  createGUI(view);
  readSettings();
  checkActions();
  setAcceptDrops(true);

  // If kviewshell is started when another instance of kviewshell runs
  // in fullscreen mode, the menubar is switched off by default, which
  // is a nasty surprise for the user: there is no way to access the
  // menus. To avoid such complications, we switch the menubar on
  // explicitly.
  menuBar()->show();

  // Add statusbar-widgets for zoom, pagenr and format
  statusBar()->insertItem("", StatusBar_ID_PageNr, 0, true);
  statusBar()->insertFixedItem("XXXX%", StatusBar_ID_Zoom, true);
  statusBar()->changeItem("", StatusBar_ID_Zoom);
  statusBar()->insertItem(view->pageSizeDescription(), StatusBar_ID_PageSize, 0, true);

  connect( view, SIGNAL(pluginChanged(KParts::Part*)), this, SLOT(createGUI(KParts::Part*)));
}


void KViewShell::checkActions()
{
  bool doc = !view->url().isEmpty();

  closeAction->setEnabled(doc);
  reloadAction->setEnabled(doc);
  fullScreenAction->setEnabled(doc);
}


KViewShell::~KViewShell()
{
  writeSettings();
  delete view;
}


void KViewShell::slotQuit()
{
  // If we are to quit the application while we operate in fullscreen
  // mode, we need to restore the visibility properties of the
  // statusbar, toolbar and the menus because these properties are
  // saved automatically ... and we don't want to start next time
  // without having menus.
  if (fullScreenAction->isChecked()) {
    kdDebug() << "Switching off fullscreen mode before quitting the application" << endl;
    showNormal();
    if (isStatusBarShownInNormalMode)
      statusBar()->show();
    if (isToolBarShownInNormalMode)
      toolBar()->show();
    menuBar()->show();
    view->slotSetFullPage(false); 
  }
  kapp->closeAllWindows();
  kapp->quit();
}


void KViewShell::readSettings()
{
  resize(600, 300); // default size if the config file specifies no size
  setAutoSaveSettings( "General" ); // apply mainwindow settings (size, toolbars, etc.)

  KConfig *config = kapp->config();
  config->setGroup("General");

  recent->loadEntries(config, "Recent Files");

  // Constant source of annoyance in KDVI < 1.0: the 'recent-files'
  // menu contains lots of files which don't exist (any longer). Thus,
  // we'll sort out the non-existent files here.
  QStringList items = recent->items();
  for ( QStringList::Iterator it = items.begin(); it != items.end(); ++it ) {
    KURL url(*it);
    if (url.isLocalFile()) {
      QFileInfo info(url.path());
      if (!info.exists())
        recent->removeURL(url);
    }
  }

}


void KViewShell::writeSettings()
{
  KConfig *config = kapp->config();
  config->setGroup( "General" );
  recent->saveEntries(config, "Recent Files");

  config->sync();
}


void KViewShell::saveProperties(KConfig* config)
{
  // the 'config' object points to the session managed
  // config file.  anything you write here will be available
  // later when this app is restored
  emit saveDocumentRestoreInfo(config);
}


void KViewShell::readProperties(KConfig* config)
{
  // the 'config' object points to the session managed
  // config file.  this function is automatically called whenever
  // the app is being restored.  read in here whatever you wrote
  // in 'saveProperties'
  if (view)
  {
    KURL url (config->readPathEntry("URL"));
    if (url.isValid())
      emit restoreDocument(url, config->readNumEntry("Page", 1));
  }
}


void KViewShell::addRecentFile()
{
  // Get the URL of the opened file from the kviewpart.
  KURL actualURL = view->url();
  // To store the URL in the list of recent files, we remove the
  // reference part.
  actualURL.setRef(QString::null);
  recent->addURL(actualURL);
  checkActions();
}

void KViewShell::openURL(const KURL& url)
{
  view->openURL(url);
}

void KViewShell::slotFullScreen()
{
  if (fullScreenAction->isChecked()) {
    // In fullscreen mode, menu- tool- and statusbar are hidden. Save
    // the visibility flags of these objects here, so that they can
    // later be properly restored when we switch back to normal mode,
    // or before we leave the application in slotQuit()
    isStatusBarShownInNormalMode = statusBar()->isShown();
    statusBar()->hide();
    isToolBarShownInNormalMode = toolBar()->isShown();
    toolBar()->hide();
    menuBar()->hide();
    view->slotSetFullPage(true); 

    // Go to fullscreen mode
    showFullScreen();

    KMessageBox::information(this, i18n("Use the Escape key to leave the fullscreen mode."), i18n("Entering Fullscreen Mode"), "leavingFullScreen");
  } else {
    showNormal();
    if (isStatusBarShownInNormalMode)
      statusBar()->show();
    if (isToolBarShownInNormalMode)
      toolBar()->show();
    menuBar()->show();
    view->slotSetFullPage(false); 
  }
}


void KViewShell::slotFileClose()
{
  view->closeURL_ask();

  checkActions();
}

void KViewShell::slotConfigureKeys()
{
  KKeyDialog dlg( true, this );

  dlg.insert( actionCollection() );
  dlg.insert( view->actionCollection() );

  dlg.configure();
}

void KViewShell::slotEditToolbar()
{
  saveMainWindowSettings( KGlobal::config(), autoSaveGroup() );
  KEditToolbar dlg(factory());
  connect( &dlg, SIGNAL( newToolbarConfig() ), SLOT( slotNewToolbarConfig() ) );
  dlg.exec();
}


void KViewShell::slotNewToolbarConfig()
{
  applyMainWindowSettings( KGlobal::config(), autoSaveGroup() );
}

void KViewShell::dragEnterEvent(QDragEnterEvent *event)
{
  if (KURLDrag::canDecode(event))
  {
    KURL::List urls;
    KURLDrag::decode(event, urls);
    if (!urls.isEmpty())
    {
      KURL url = urls.first();

      // Always try to open remote files
      if (!url.isLocalFile())
      {
        event->accept();
        return;
      }

      // For local files we only accept a drop, if we have a plugin for its
      // particular mimetype
      KMimeType::Ptr mimetype = KMimeType::findByURL(url);
      kdDebug() << "[dragEnterEvent] Dragged URL is of type " << mimetype->comment() << endl;

      // Safety check
      if (view)
      {
        QStringList mimetypeList = view->supportedMimeTypes();
        kdDebug() << "[dragEnterEvent] Supported mime types: " << mimetypeList << endl;

        for (QStringList::Iterator it = mimetypeList.begin(); it != mimetypeList.end(); ++it)
        {
          if (mimetype->is(*it))
          {
            kdDebug() << "[dragEnterEvent] Found matching mimetype: " << *it << endl;
            event->accept();
            return;
          }
        }
        kdDebug() << "[dragEnterEvent] no matching mimetype found" << endl;
      }
    }
    event->ignore();
  }
}


void KViewShell::dropEvent(QDropEvent *event)
{
  KURL::List urls;
  if (KURLDrag::decode(event, urls) && !urls.isEmpty())
    view->openURL(urls.first());
}


void KViewShell::keyPressEvent(QKeyEvent *event)
{
  // The Escape Key is used to return to normal mode from fullscreen
  // mode
  if ((event->key() == Qt::Key_Escape) && (fullScreenAction->isChecked())) {
    showNormal();
    return;
  }
  // If we can't use the key event, pass it on
  event->ignore();
}


void KViewShell::slotChangePageText(const QString &message)
{
  statusBar()->changeItem(" "+message+" ",StatusBar_ID_PageNr);
}


void KViewShell::slotChangeSizeText(const QString &message)
{
  statusBar()->changeItem(" "+message+" ",StatusBar_ID_PageSize);
}


void KViewShell::slotChangeZoomText(const QString &message)
{
  statusBar()->changeItem(" "+message+" ",StatusBar_ID_Zoom);
}
