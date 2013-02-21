/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
// Author: Damyan Pepper
//         Toivo Pedaste
//
// See kpackage.h for more information.
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

#include <qdir.h>
#include <qlabel.h>
#include <qframe.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kprogress.h>
#include <kurl.h>
#include <kapplication.h>
#include <kaccel.h>
#include <kaction.h>
#include <klocale.h>
#include <kinputdialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kstdaction.h>
#include <kedittoolbar.h>
#include <kmimemagic.h>
#include <kurldrag.h>

#include "kpackage.h"
#include "managementWidget.h"
#include "pkgOptions.h"
#include "kio.h"
#include "findf.h"
#include "search.h"
#include "options.h"
#include "cache.h"

extern Opts *opts;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
KPKG::KPKG(KConfig *_config)
  : KMainWindow(0)
{
  kpackage = new KPACKAGE(_config, this);
  setCentralWidget(kpackage);

  config =  kapp->config();
  config->setGroup("Kpackage");

  kpackage->management->readPSeparator();

  // Get a nice default size
  resize(760,540);

  setupMenu();
  disableNext();
  disablePrevious();

  optiondialog = new Options(this);

  prop_restart = false;
  setAutoSaveSettings();
}

// Set up the menu

void KPKG::setupMenu()
{

  pack_open = KStdAction::open(kpackage, SLOT(fileOpen()),
			       actionCollection());

  recent = KStdAction::openRecent(this, SLOT(openRecent(const KURL&)),
				  actionCollection());
  recent->loadEntries( config );

  pack_find = new KAction( i18n("Find &Package..."), "find",
			   KStdAccel::shortcut(KStdAccel::Find), kpackage,
			   SLOT(find()), actionCollection(), "pack_find");

  pack_findf = new KAction( i18n("Find &File..."), "filefind",
			    0, kpackage,
			    SLOT(findf()), actionCollection(), "pack_findf");

  kpack_reload = new KAction( i18n("&Reload"), "reload",
			      KStdAccel::shortcut(KStdAccel::Reload), kpackage,
			      SLOT(reload()), actionCollection(), "kpack_reload");

  (void) KStdAction::quit(kpackage, SLOT(fileQuit()),
			  actionCollection());

  pack_prev = KStdAction::back(kpackage->management->treeList, SLOT(previous()),
			       actionCollection(),"pack_prev");

  pack_next = KStdAction::forward(kpackage->management->treeList, SLOT(next()),
				  actionCollection(),"pack_next");

  (void) (new KAction( i18n("&Expand Tree"), "ftout",
		       0, kpackage,
		       SLOT(expandTree()), actionCollection(), "kpack_expand"));

  (void) (new KAction( i18n("&Collapse Tree"), "ftin",
		       0, kpackage,
		       SLOT(collapseTree()), actionCollection(), "kpack_collapse"));

  (void) (new KAction( i18n("Clear &Marked"), QString::null,
		       0, kpackage,
		       SLOT(clearMarked()), actionCollection(), "kpack_clear"));

  (void) (new KAction( i18n("Mark &All"), QString::null,
		       0, kpackage,
		       SLOT(markAll()), actionCollection(), "kpack_markall"));

  pack_install = new KAction( i18n("&Install"), QString::null,
			      0, kpackage->management,
			      SLOT(installSingleClicked()), actionCollection(), "install_single");

  pack_install->setEnabled(false);
  kpackage->management->setInstallAction(pack_install);


  pack_uninstall = new KAction( i18n("&Uninstall"), QString::null,
				0, kpackage->management,
				SLOT(uninstallSingleClicked()), actionCollection(), "uninstall_single");

  pack_uninstall->setEnabled(false);
  kpackage->management->setUninstallAction(pack_uninstall);


  (void) (new KAction( i18n("&Install Marked"), QString::null,
		       0, kpackage->management,
		       SLOT(installMultClicked()), actionCollection(), "install_marked"));

  (void) (new KAction( i18n("&Uninstall Marked"), QString::null,
		       0, kpackage->management,
		       SLOT(uninstallMultClicked()), actionCollection(), "uninstall_marked"));

  setStandardToolBarMenuEnabled(true);

  KStdAction::configureToolbars( this, SLOT(configureToolBars()),
				 actionCollection());

  KStdAction::saveOptions( this, SLOT(saveSettings()), actionCollection());

  KStdAction::keyBindings( guiFactory(), SLOT(configureShortcuts()), actionCollection());

  (void) (new KAction( i18n("Configure &KPackage..."), "configure",
		       0, this,
		       SLOT(setOptions()), actionCollection(), "kpack_options"));

  (void) (new KAction( i18n("Clear Package &Folder Cache"), QString::null,
		       0, this,
		       SLOT(clearDCache()), actionCollection(), "clear_dcache"));

  (void) (new KAction( i18n("Clear &Package Cache"), QString::null,
		       0, this,
		       SLOT(clearPCache()), actionCollection(), "clear_pcache"));

  int i;
  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
      kpinterface[i]->makeMenu(actionCollection());
    }
  }

  //  urlList.setAutoDelete(TRUE);
  createGUI();
}

void KPKG::disableMenu()
{
  pack_open->setEnabled(false);
  pack_find->setEnabled(false);
  pack_findf->setEnabled(false);
  kpack_reload->setEnabled(false);
  recent->setEnabled(false);
}

void KPKG::enableMenu()
{
  pack_open->setEnabled(true);
  pack_find->setEnabled(true);
  pack_findf->setEnabled(true);
  kpack_reload->setEnabled(true);
  recent->setEnabled(true);
}

void KPKG::disableNext() {
  pack_next->setEnabled(false);
}

void KPKG::enableNext() {
  pack_next->setEnabled(true);
}

void KPKG::disablePrevious() {
  pack_prev->setEnabled(false);
}

void KPKG::enablePrevious() {
  pack_prev->setEnabled(true);
}
void KPKG::openRecent(const KURL& url){
  kpackage->openNetFile( url );
}

void KPKG::add_recent_file(const QString &newfile){

  KURL url = KURL(newfile);

  recent->addURL( url );
}

void KPKG::configureToolBars() {
  KEditToolbar dlg(actionCollection());
  connect(&dlg,SIGNAL(newToolbarConfig()),this,SLOT(slotNewToolbarConfig()));
  dlg.exec();
}

void KPKG::slotNewToolbarConfig() {
  createGUI();
}

void KPKG::writeSettings(){

  kpackage->management->writePSeparator();

  KConfig *config = kapp->config();

  config->setGroup("Kpackage");

  recent->saveEntries( config );

  kpackage->management->treeList->writeTreeConfig();
  kpackage->management->treeList->writeTreeType();

  config->sync();
}

void KPKG::setOptions(){
  optiondialog->restore();
}

void KPKG::saveSettings(){
  writeSettings();
}

void KPKG::clearPCache(){
  cacheObj::clearPCache();
}

void KPKG::clearDCache(){
  cacheObj::clearDCache();
}

void KPKG::saveProperties(KConfig *config )
{
    config->writePathEntry("Name", kpackage->save_url.url());
}


void KPKG::readProperties(KConfig *config)
{
    QString entry = config->readPathEntry("Name"); // no default
    if (entry.isNull())
	return;
    kpackage->openNetFiles(entry);
    prop_restart = true;
}

bool KPKG::queryClose() {
    kpackage->cleanUp();
    return true;
} 

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

KPACKAGE::KPACKAGE(KConfig *_config, QWidget *parent)
  : QWidget(parent)
{

  // Save copy of config
  config = _config;

  setAcceptDrops(true);
  setupModeWidgets();

  setupStatusBar();

  file_dialog = NULL;
  findialog = NULL;
  srchdialog = NULL;

}

// Destructor
KPACKAGE::~KPACKAGE()
{
  //  destroyModeWidgets();
  //  delete status;
  //  delete processProgress;
}

// resize event -- arrange the widgets
void KPACKAGE::resizeEvent(QResizeEvent *re)
{
  re = re;			// prevent warning
  arrangeWidgets();
}

// Set up the mode widgets
void KPACKAGE::setupModeWidgets()
{
  management = new managementWidget(this);

  for (int i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
      kpinterface[i]->uninstallation = new pkgOptionsU(kpinterface[i]);
      kpinterface[i]->installation = new pkgOptionsI(kpinterface[i]);
    }
  }
}

// destroy the mode widgets
void KPACKAGE::destroyModeWidgets()
{
  //  delete management;
  //  for (int i = 0; i < kpinterfaceN; i++) {
  //    if (kpinterface[i]) {
  //      delete kpinterface[i]->installation;
  //      delete kpinterface[i]->uninstallation;
  //    }
  //  }
}


// Set up the status bar
void KPACKAGE::setupStatusBar()
{
  statusbar = new QFrame(this);
  statusbar->setFrameStyle(QFrame::Raised | QFrame::Panel);
  processProgress = new KProgress(100,statusbar);
  processProgress->setTextEnabled(FALSE);

  status = new QLabel(i18n("Management Mode"), statusbar);
}

// Arrange the widgets nicely
void KPACKAGE::arrangeWidgets()
{
  int i;

  statusbar->resize(width(),20);
  statusbar->move(0,height()-20);
  status->resize((statusbar->width() / 4) * 3, 16);
  status->move(2,2);
  processProgress->resize(statusbar->width() / 4 - 4, 16);
  processProgress->move((statusbar->width() / 4) * 3 + 3, 2);

  management->resize(width(),height() - 20);

  for (i = 0; i < kpinterfaceN; i++)
    if (kpinterface[i]) {
      kpinterface[i]->installation->resize(width(),height() - 20);
    }
}

void KPACKAGE::setup()
{
  management->collectData(1);
}

void KPACKAGE::fileQuit()		// file->quit selected from menu
{
  cleanUp();

  KApplication::exit(0);	// exit the application
}

void KPACKAGE::cleanUp()		// file->quit selected from menu
{
  kpkg->writeSettings();
  if (opts->DCache >= Opts::SESSION) {
    cacheObj::clearDCache(); // clear dir caches if needed
  }
  if (opts->PCache >= Opts::SESSION) {
    cacheObj::clearPCache(); // clear package caches if needed
  }
}

void KPACKAGE::reload()
{
  kpackage->management->collectData(TRUE);
}

void KPACKAGE::fileOpen()		// file->quit selected from menu
{
    KFileDialog *box;

    box = getFileDialog(i18n("Select Package"));

    if( box->exec())
    {
        if(!box->selectedURL().isEmpty())
        {
            openNetFile( box->selectedURL() );
        }
    }
}

void KPACKAGE::clearMarked()
{
  management->treeList->clearMarked(management->treeList->firstChild());
}

void KPACKAGE::markAll()
{
  management->treeList->markAll(management->treeList->firstChild());
}

void KPACKAGE::expandTree()
{
  management->treeList->expandTree(management->treeList);
}

void KPACKAGE::collapseTree()
{
  management->treeList->collapseTree(management->treeList);
}

pkgInterface *KPACKAGE::pkType(const QString &fname)
{
  // Get the package information for this package
  char buf[51];
  int i;

  FILE *file= fopen(QFile::encodeName(fname),"r");
  if (file) {
    fgets(buf,sizeof(buf)-1,file);
    buf[50] = 0;

    // check enabled package handlers
    for (i = 0; i < kpinterfaceN; i++) {
      if (kpinterface[i]) {
	if (opts->handlePackage[i] && kpinterface[i]->isType(buf, fname)) {
	  fclose(file);
	  return kpinterface[i];
	}
      }
    }
    // check unenabled package handlers
    for (i = 0; i < kpinterfaceN; i++) {
      if (kpinterface[i]) {
	if (!opts->handlePackage[i] && kpinterface[i]->isType(buf, fname)) {
	  fclose(file);
	  return kpinterface[i];
	}
      }
    }
    fclose(file);
    KpMsgE(i18n("Unknown package type: %1").arg(fname),TRUE);
  } else {
    KpMsgE(i18n("File not found: %1").arg(fname),TRUE);
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////
int KPACKAGE::typeIndex(pkgInterface *type) {
  int i;
  for (i = 0; i < kpinterfaceN; i++) {
    if (type ==  kpinterface[i]) {
      return i;
    }
  }
  return -1;
}

void KPACKAGE::openNetFiles (const QStringList &urls, bool install )
{
  QStringList files;
  int i;
  int index;
  QPtrList<packageInfo> **lst = new QPtrList<packageInfo>*[kpinterfaceN];
  packageInfo *pk = 0;

  kdDebug() << "openNetFiles\n";

  for (QStringList::ConstIterator it = urls.begin(); it != urls.end(); ++it) {
    files.append(fetchNetFile(*it));
    kpkg->add_recent_file(*it);
  }

  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
      lst[i] = new QPtrList<packageInfo>;
    }
  }
  
  for (QStringList::Iterator t = files.begin(); t != files.end(); ++t) {
    pkgInterface *type = pkType(*t);
    index = typeIndex(type);
    if (index >= 0) {
      pk = type->getPackageInfo('u', *t, 0);
      if (pk) {
	pk->pkgFileIns(*t);
	lst[index]->insert(0,pk);
        }
      }
    }

  if (install)
    for (i = 0; i < kpinterfaceN; i++) {
      if (kpinterface[i]) {
	if ( lst[i]->count() > 0) {
	  kpinterface[i]->installation->setup(lst[i],kpinterface[i]->head);
	  if (kpinterface[i]->installation->exec()) {
	    for (packageInfo *inf = lst[i]->first(); inf != 0; inf = lst[i]->next()) {
	      kpackage->management->updatePackage(inf,TRUE);
	    }
	  }
	}
      }
    } else {
      if (pk) {
	KpTreeListItem *pt = pk->item;
	// NOT the best place for this CODE
	kpackage->management->tabChanged(Opts::ALL);
	if (pt)
	  kpackage->management->packageHighlighted(pt);
      }
    }

  // Dealloc memory
  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
	delete lst[i];
    }
  }
  delete [] lst;
}

void KPACKAGE::openNetFile(const KURL &url, bool install )
{
  openNetFiles(url.url(), install);
}

//    KMimeMagic *magic = KMimeMagic::self();
//    KMimeMagicResult *r = magic->findFileType(s);
    //    printf("r=%s\n",(r->mimeType()).data());



QString KPACKAGE::getFileName(const KURL & url, QString &cacheName )
{
  QString none  = "";
  QString fname = "";

  if ( !url.isValid() )  {
    KpMsgE(i18n("Malformed URL: %1").arg(url.url()),TRUE);
  } else {

    // Just a usual file ?
    if ( url.isLocalFile() ) {
      cacheName = url.path();
      fname = url.path();
    } else {

      QString tmpd = cacheObj::PDir();
      if (!tmpd.isEmpty()) {

	QString cacheFile = tmpd + url.fileName();

	cacheName = cacheFile;
	QFileInfo f(cacheFile);
	if (f.exists() && (opts->DCache != Opts::NEVER)) {
	  fname =  cacheFile;
	}
      }
    }
  }
  return fname;
}

bool KPACKAGE::isFileLocal( const KURL & url )
{
  QString cf;

  QString f = getFileName(url, cf);

  if (cf.isEmpty()) {
    return false;
  } else {
    if (!f.isEmpty()) {
      return true;
    } else {
      return false;
    }
  }
}

QString KPACKAGE::fetchNetFile( const KURL & url )
{

  QString cf;

  QString f = getFileName(url, cf);

  if (cf.isEmpty()) {
    return "";
  } else {

    if (!f.isEmpty()) {
      return f;
    } else {
      save_url = url;

      setStatus(i18n("Starting KIO"));

      Kio kio;

      if (kio.download(url, cf)) {
	setStatus(i18n("KIO finished"));
	QFileInfo fi(cf);
	if (!(fi.exists() && fi.size() > 0)) {
          unlink(QFile::encodeName(cf));
	  return "";
	} else {
          CacheList cl(fi.dirPath());
          cl.append(fi.fileName());
          cl.write();
	  return cf;
	}
      } else {
	setStatus(i18n("KIO failed"));
	return "";
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////
void KPACKAGE::fileOpenUrl(){

  bool ok;

  QString url = KInputDialog::getText( QString::null,
      i18n( "Open location:" ), save_url.prettyURL(), &ok, this );

  if ( ok )
    {
      kpkg->add_recent_file( url );
      openNetFile( url );
    }
}

void KPACKAGE::find(){
  if (srchdialog)
    srchdialog->show();
  else
    srchdialog = new Search(this, "find package");
}

void KPACKAGE::findf(){
  if (findialog)
    findialog->show();
  else
    findialog = new FindF(this);
}

KFileDialog* KPACKAGE::getFileDialog(const QString &captiontext)
{

  if(!file_dialog) {
    file_dialog = new KFileDialog(QDir::currentDirPath(), "",
				  this,"file_dialog",TRUE);
  }

  QString pat;
  for (int i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i] && opts->handlePackage[i]) {
      pat += kpinterface[i]->packagePattern;
      pat += " ";
    }
  }
  file_dialog->setFilter(pat);
  file_dialog->setCaption(captiontext);
  //  file_dialog->rereadDir();

  return file_dialog;
}

void KPACKAGE::dragEnterEvent(QDragEnterEvent* e)
{
  e->accept(KURLDrag::canDecode(e));
}

void KPACKAGE::dropEvent(QDropEvent *de) // something has been dropped
{
  KURL::List list;
  if (!KURLDrag::decode(de, list) || list.isEmpty())
     return;

  openNetFiles(list.toStringList());
}

void KPACKAGE::setStatus(const QString &s)	// set the text in the status bar
{
  status->setText(s);
  kapp->processEvents();	// refresh the screen
}

QString KPACKAGE::getStatus()	// get the text in the status bar
{
  if(status)
    return status->text();
  else
    return "";
}

void KPACKAGE::setPercent(int x)	// set the progress in the status bar
{
  processProgress->setValue(x);
  kapp->processEvents();	// refresh it
}

//////////////////////////////////////////////////////////////////////////////



#include "kpackage.moc"
