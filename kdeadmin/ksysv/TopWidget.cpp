/*
    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997-2000 Peter Putzer
                            putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/****************************************************************
**
** KSysV
** Toplevel Widget
**
****************************************************************/

#include <ctype.h>

#include <qpopupmenu.h>
#include <qkeycode.h>
#include <qmessagebox.h>
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qmultilineedit.h>
#include <qdatetime.h>
#include <kprinter.h>
#include <qpaintdevicemetrics.h>
#include <qbuttongroup.h>
#include <qclipboard.h>
#include <qtooltip.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qregexp.h>
#include <qdatastream.h>
#include <qpixmapcache.h>
#include <qtextview.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qwhatsthis.h>
#include <qsimplerichtext.h>

#include <kresolver.h>
#include <ktoolbar.h>
#include <kkeydialog.h>
#include <kmenubar.h>
#include <kcompletion.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kglobal.h>
#include <khelpmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstdaccel.h>
#include <kedittoolbar.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kaboutdata.h>
#include <kcursor.h>

#include "kbusymanager.h"
#include "ServiceDlg.h"
#include "PreferencesDialog.h"
#include "ActionList.h"
#include "ksv_core.h"
#include "ksv_conf.h"
#include "RunlevelAuthIcon.h"
#include "ksvdraglist.h"
#include "Data.h"
#include "ksvdrag.h"
#include "OldView.h"
#include "IOCore.h"
#include "TopWidget.h"
#include <qlabel.h>
#include <kstatusbar.h>

namespace Status
{
  enum {
    Changed, Checklist, Writable
  };
} // namespace Status

KSVTopLevel::KSVTopLevel()
  : KMainWindow(0, 0L, WStyle_ContextHelp|WDestructiveClose),
    mConfig(KSVConfig::self()),
	mView (0L),
    mPreferences (0L),

	mEditUndo (0L), mEditCut (0L), mEditCopy (0L), mEditPaste (0L),
    mEditProperties (0L),

	mFileRevert (0L), mFileLoad (0L), mFilePrint (0L), mFilePrintLog (0L),
    mFileSave (0L), mFileSaveAs (0L), mFileSaveLog(0L), mFileQuit (0L),

	mToolsStartService (0L), mToolsStopService (0L),
    mToolsRestartService (0L), mToolsEditService (0L),

    mOptionsToggleLog (0L),

    mOpenWith (0L), mOpenDefault (0L),

    mUndoList (new ActionList (this, "UndoList")),
	mRedoList (new ActionList (this, "RedoList")),
	mStartDlg (new ServiceDlg (i18n("Start Service"),
							   i18n("&Choose which service to start:"),
							   this)),
	mStopDlg (new ServiceDlg (i18n("Stop Service"),
							  i18n("&Choose which service to stop:"),
							  this)),
	mRestartDlg (new ServiceDlg (i18n("Restart Service"),
								 i18n("&Choose which service to restart:"),
								 this)),
	mEditDlg (new ServiceDlg (i18n("Edit Service"),
							  i18n("&Choose which service to edit:"),
							  this)),
    mVisible (new QCheckBox*[ksv::runlevelNumber])
{
  setCaption(false);

  initStatusBar(); // order dependency
  initActions(); // order dependency
  mView = new KSVContent (mOpenWith->popupMenu(), this, "Content"); // order dependency
  initTools(); // order dependency

  setCentralWidget(mView);

  // ensure that the statusbar gets updated correctly
  connect (mView, SIGNAL(sigRun(const QString&)), this, SLOT(slotUpdateRunning(const QString&)));
  connect (mView, SIGNAL(sigStop()), statusBar(), SLOT(clear()));
  connect (mView, SIGNAL(cannotGenerateNumber()), this, SLOT(catchCannotGenerateNumber()));
  connect (mView, SIGNAL(undoAction(KSVAction*)), this, SLOT(pushUndoAction(KSVAction*)));
  connect (mView, SIGNAL(logChanged()), this, SLOT(enableLogActions()));

  // cut & copy
  connect (mView, SIGNAL (newOrigin()),
		   this, SLOT (dispatchEdit()));

  // undo
  connect (mUndoList, SIGNAL(empty()), this, SLOT(disableUndo()));
  connect (mUndoList, SIGNAL(filled()), this, SLOT(enableUndo()));

  // and redo
  connect (mRedoList, SIGNAL(empty()), this, SLOT(disableRedo()));
  connect (mRedoList, SIGNAL(filled()), this, SLOT(enableRedo()));

  // paste
  connect (kapp->clipboard(), SIGNAL (dataChanged()),
		   this, SLOT (dispatchEdit()));

  // init mView according to saved preferences
  slotReadConfig();
  initView();
  setMinimumSize(600,400);

  // restore size and position
  move(mConfig->position()); // doesnt seem to work while unmapped
  setAutoSaveSettings();

  // start watching the directories
  mAuth->setCheckEnabled(true);

  dispatchEdit(); // disable cut & copy on startup
}

KSVTopLevel::~KSVTopLevel()
{
  mUndoList->clear();
  mRedoList->clear();

  delete[] mVisible;
}

void KSVTopLevel::initTools()
{
  connect (mStartDlg, SIGNAL(doAction(const QString&)),
		   mView, SLOT(startService(const QString&)));
  connect (mStopDlg, SIGNAL(doAction(const QString&)),
		   mView, SLOT(editService(const QString&)));
  connect (mRestartDlg, SIGNAL(doAction(const QString&)),
		   mView, SLOT(restartService(const QString&)));
  connect (mEditDlg, SIGNAL(doAction(const QString&)),
		   mView, SLOT(editService(const QString&)));

  connect (mStartDlg, SIGNAL (display (bool)),
		   this, SLOT (dispatchStartService (bool)));
  connect (mStopDlg, SIGNAL (display (bool)),
		   this, SLOT (dispatchStopService (bool)));
  connect (mRestartDlg, SIGNAL (display (bool)),
		   this, SLOT (dispatchRestartService (bool)));
  connect (mEditDlg, SIGNAL (display (bool)),
		   this, SLOT (dispatchEditService (bool)));

}

void KSVTopLevel::initActions ()
{
  KActionCollection* coll = actionCollection();

  // setup File menu
  mFileRevert = KStdAction::revert (this, SLOT (slotClearChanges()), coll);
  mFileRevert->setText (i18n("Re&vert Configuration"));

  mFileLoad = KStdAction::open (this, SLOT (load()), coll);
  mFileLoad->setText (i18n ("&Open..."));

  mFileSave = KStdAction::save(this, SLOT(slotAcceptChanges()), coll);
  mFileSave->setText (i18n("&Save Configuration"));

  mFileSaveAs = KStdAction::saveAs (this, SLOT (saveAs ()), coll);

  mFileSaveLog = KStdAction::save (this, SLOT(slotSaveLog()), coll, "ksysv_save_log");
  mFileSaveLog->setText (i18n("Save &Log..."));
  mFileSaveLog->setShortcut (Key_L+CTRL);
  mFileSaveLog->setEnabled (false);

  // disabled due to complexity
  //  mFilePrint = KStdAction::print (this, SLOT (print()), coll);

  mFilePrintLog = KStdAction::print(this, SLOT(printLog()), coll, "ksysv_print_log");
  mFilePrintLog->setText( i18n("&Print Log..."));
  mFilePrintLog->setEnabled (false);

  mFileQuit = KStdAction::quit(this, SLOT(close()), coll);

  // setup Edit menu
  mEditUndo = KStdAction::undo(this, SLOT(editUndo()), coll);
  mEditUndo->setEnabled (false);
  mEditRedo = KStdAction::redo(this, SLOT(editRedo()), coll);
  mEditUndo->setEnabled (false);
  mEditCut = KStdAction::cut(this, SLOT(editCut()), coll);
  mEditCopy = KStdAction::copy(this, SLOT(editCopy()), coll);
  mEditPaste = KStdAction::paste(this, SLOT(editPaste()), coll);
  mPasteAppend = KStdAction::paste (this, SLOT (pasteAppend()),
									coll, "ksysv_paste_append");

  mEditProperties = new KAction (i18n("P&roperties"), 0,
                                 this, SLOT(properties()),
                                 coll, "ksysv_properties");

  mOpenDefault = new KAction (i18n ("&Open"), 0,
                              this, SLOT (editService()),
                              coll, "ksysv_open_service");

  mOpenWith = new KActionMenu (i18n ("Open &With"), coll, "ksysv_open_with");

  // setup Settings menu
  createStandardStatusBarAction();
  setStandardToolBarMenuEnabled(true);
  KStdAction::keyBindings (this, SLOT(configureKeys()), coll);
  KStdAction::configureToolbars (this, SLOT(configureToolbars()), coll);
  KStdAction::saveOptions(this, SLOT(saveOptions()), coll);
  KStdAction::preferences(this, SLOT(slotShowConfig()), coll);
  mOptionsToggleLog = new KToggleAction (i18n("Show &Log"), "toggle_log", 0,
										 this, SLOT (toggleLog()),
										 coll, "ksysv_toggle_log");
  mOptionsToggleLog->setCheckedState(i18n("Hide &Log"));

  // setup Tools menu
  mToolsStartService = new KToggleAction (i18n("&Start Service..."), "ksysv_start", 0,
                                          mStartDlg, SLOT (toggle()),
                                          coll, "ksysv_start_service");

  mToolsStopService = new KToggleAction (i18n("&Stop Service..."), "ksysv_stop", 0,
                                         mStopDlg, SLOT (toggle()),
                                         coll, "ksysv_stop_service");

  mToolsRestartService = new KToggleAction (i18n("&Restart Service..."), 0,
                                            mRestartDlg, SLOT (toggle()),
                                            coll, "ksysv_restart_service");

  mToolsEditService = new KToggleAction (i18n("&Edit Service..."), 0,
                                         mEditDlg, SLOT (toggle()),
                                         coll, "ksysv_edit_service");

  createGUI(xmlFile());
}

//
bool KSVTopLevel::queryExit() 
{
  uint res = KMessageBox::Continue;

  if (mChanged) {
    res = KMessageBox::warningContinueCancel(kapp->mainWidget(),
				    i18n("There are unsaved changes. Are you sure you want to quit?"),
				    i18n("Quit"),
				    KStdGuiItem::quit());

  }

  return res == KMessageBox::Continue;
}

void KSVTopLevel::slotClearChanges()
{
  if (mChanged &&
      KMessageBox::Yes ==
      KMessageBox::questionYesNo(kapp->mainWidget(),
				 i18n("Do you really want to revert all unsaved changes?"),
				 i18n("Revert Configuration"),
				 i18n("&Revert"),
				 KStdGuiItem::cancel()))
    {
      mUndoList->undoAll();
	  mRedoList->clear();
    }
}

void KSVTopLevel::slotAcceptChanges() {
  if (KMessageBox::Continue ==
      KMessageBox::warningContinueCancel(kapp->mainWidget(),
				i18n("You're about to save the changes made to your init "
				     "configuration. Wrong settings can "
				     "make your system hang on startup.\n"
				     "Do you wish to continue?"),
				i18n("Save Configuration"),
				KStdGuiItem::save()))
    {
      mView->slotWriteSysV();
      initView();
    }
}

void KSVTopLevel::initView()
{
  const bool authEnabled = mAuth->isCheckEnabled();
  mAuth->setCheckEnabled(false);

  mUndoList->clear();
  mRedoList->clear();

  mView->initScripts();
  mView->initRunlevels();

  setChanged(false);

  // disable ToolsMenu_ entries when they can't do anything
  ServiceDlg* tmp = new ServiceDlg ("","", this);

  tmp->resetChooser (mView->scripts, false);
  if (!tmp->count())
    {
	  mToolsStartService->setEnabled (false);
	  mToolsStopService->setEnabled (false);
	  mToolsRestartService->setEnabled (false);
    }
  else
    {
	  mToolsStartService->setEnabled (true);
	  mToolsStopService->setEnabled (true);
	  mToolsRestartService->setEnabled (true);
    }

  tmp->resetChooser (mView->scripts, true);
  if (!tmp->count())
    {
	  mToolsEditService->setEnabled (false);
    }
  else
    {

	  mToolsEditService->setEnabled (true);
    }
  delete tmp;

  // reset tools
  mStartDlg->resetChooser (mView->scripts, false);
  mStopDlg->resetChooser (mView->scripts, false);
  mRestartDlg->resetChooser (mView->scripts, false);
  mEditDlg->resetChooser (mView->scripts, true);

  mAuth->setCheckEnabled(authEnabled);
}

void KSVTopLevel::initStatusBar()
{
  KStatusBar* status = statusBar();

  QHBox* visBox = new QHBox (status, "visBox");
  QButtonGroup* group = new QButtonGroup (this, "visButtonGroup");
  group->hide();
  connect (group, SIGNAL (clicked (int)), this, SLOT (toggleRunlevel (int)));

  QWhatsThis::add (visBox, i18n ("<p>Click on the checkboxes to <strong>show</strong> or "\
                                 "<strong>hide</strong> runlevels.</p> " \
                                 "<p>The list of currently visible runlevels is saved "\
                                 "when you use the <strong>Save Options command</strong>.</p>"));
  QToolTip::add (visBox, i18n ("Show only the selected runlevels"));

  new QLabel (i18n("Show runlevels:"), visBox, "visLabel");
  for (int i = 0; i < ksv::runlevelNumber; ++i)
	{
	  QString label; label.setNum (i);

	  mVisible[i] = new QCheckBox (label, visBox, label.latin1());
	  mVisible[i]->setChecked (mConfig->showRunlevel (i));

	  group->insert (mVisible[i]);
	}

  QHBox* authIconBox = new QHBox (status, "AuthIconBox");
  QWidget* strut = new QWidget (authIconBox, "Strut");
  strut->setFixedWidth (KDialog::spacingHint());
  mAuth = new RunlevelAuthIcon (mConfig->scriptPath(), mConfig->runlevelPath(), authIconBox);
  connect (mAuth, SIGNAL (authChanged(bool)), mView, SLOT(multiplexEnabled(bool)));
  connect (mAuth, SIGNAL (authChanged(bool)), this, SLOT(writingEnabled(bool)));

  QWhatsThis::add (authIconBox, i18n ("<p>If the lock is closed <img src=\"user|ksysv_locked\"/>, "\
                                      "you don't have the right " \
                                      "<strong>permissions</strong> to edit the init configuration.</p>" \
                                      "<p>Either restart %1 as root (or another more privileged user), " \
                                      "or ask your sysadmin to install %1 <em>suid</em> or " \
                                      "<em>sgid</em>.</p><p>The latter way is <strong>not</strong> "\
                                      "recommended though, due to security issues.</p>")
                   .arg (kapp->aboutData()->programName()).arg(kapp->aboutData()->programName()));

  authIconBox->setMinimumSize (authIconBox->minimumSizeHint());
  visBox->setMinimumSize (visBox->minimumSizeHint());

  status->addWidget (authIconBox, 0, false);
  status->insertItem ("", Status::Changed, 100);
  status->addWidget (visBox, 0, true);

  status->setItemAlignment (Status::Changed, AlignLeft|AlignVCenter);
}

void KSVTopLevel::slotShowConfig()
{
  if (!mPreferences)
    {
      mPreferences = KSVPreferences::self();

      connect (mPreferences, SIGNAL (updateColors ()),
               this, SLOT (updateColors ()));

      connect (mPreferences, SIGNAL (updateServicesPath ()),
               this, SLOT (updateServicesPath ()));

      connect (mPreferences, SIGNAL (updateRunlevelsPath ()),
               this, SLOT (updateRunlevelsPath ()));

      connect (mPreferences, SIGNAL (updateFonts ()),
               mView, SLOT (repaintRunlevels ()));
    }

//   mPreferences->setInitialSize (QSize (400,300), true);
  mPreferences->exec();
}

void KSVTopLevel::updateColors ()
{
  ksv::serviceCompletion ()->clear ();

  mView->setColors (mConfig->newNormalColor(),
                    mConfig->newSelectedColor(),
                    mConfig->changedNormalColor(),
                    mConfig->changedSelectedColor());
}

void KSVTopLevel::updateServicesPath ()
{
  mView->updateServicesAfterChange (mConfig->scriptPath());
  mAuth->setServicesPath (mConfig->scriptPath());
}

void KSVTopLevel::updateRunlevelsPath ()
{
  mView->updateRunlevelsAfterChange ();
  mAuth->setRunlevelPath (mConfig->runlevelPath());
}

void KSVTopLevel::slotReadConfig() {
  setLog(mConfig->showLog());
}

void KSVTopLevel::toggleLog() {
  const bool value = !mConfig->showLog();
  setLog(value);
}

void KSVTopLevel::saveOptions()
{
  mConfig->writeSettings();
}

void KSVTopLevel::slotUpdateRunning (const QString& text)
{
  statusBar()->changeItem(text, Status::Changed);
}

void KSVTopLevel::editCut() {
  KSVDragList* list = mView->getOrigin();

  if (list && list->currentItem())
	{
	  KSVDrag* mime = new KSVDrag (*list->currentItem()->data(), 0L, 0L);
	  kapp->clipboard()->setData (mime);

	  KSVData data = *list->currentItem()->data();
	  delete list->currentItem();

	  mUndoList->push (new RemoveAction (list, &data));
	  setChanged (true);
	}
}

void KSVTopLevel::editCopy()
{
  KSVDragList* list = mView->getOrigin();

  if (list)
	{
	  KSVDrag* mime = new KSVDrag (*static_cast<KSVItem*> (list->currentItem()), 0L, 0L);
	  kapp->clipboard()->setData (mime);
	}
}

void KSVTopLevel::editPaste()
{
  KSVDragList* list = mView->getOrigin();

  if (list)
    {
	  KSVData data;

	  if (KSVDrag::decodeNative (kapp->clipboard()->data(), data))
		{
		  KSVAction* action = 0L;

		  if (list->insert (data, list->currentItem(), action))
			{
			  setChanged (true);
			  mUndoList->push (new AddAction (list, list->match (data)->data()));
			}
		}
	}
  else
    qFatal("Bug: could not get origin of \"Paste\" event.\n" \
		   "Please notify the maintainer of this program,\n" \
		   "Peter Putzer <putzer@kde.org>.");
}

void KSVTopLevel::setChanged (bool val)
{
  mChanged = val;
  setCaption(val);

  mFileRevert->setEnabled (val);
  mFileSave->setEnabled (val);

  // update statusbar
  statusBar()->changeItem(val ? i18n(" Changed") : QString::null, Status::Changed);

  // clear messages
  statusBar()->clear();
}

void KSVTopLevel::properties()
{
  KSVDragList* list = mView->getOrigin();

  if (list)
    mView->infoOnData(list->currentItem());
}

void KSVTopLevel::scriptProperties()
{
  KSVDragList* list = mView->getOrigin();

  if (list)
    mView->slotScriptProperties(list->currentItem());
}

void KSVTopLevel::editUndo ()
{
  KSVAction* action = mUndoList->top();

  mUndoList->undoLast();

  mRedoList->push (action);
}

void KSVTopLevel::editRedo ()
{
  KSVAction* action = mRedoList->top();

  mRedoList->redoLast();

  setChanged (true);
  mUndoList->push (action);
}

void KSVTopLevel::setPaste (bool val)
{
  mEditPaste->setEnabled (val);
  mPasteAppend->setEnabled (val);
}

void KSVTopLevel::setLog (bool val)
{
  mConfig->setShowLog(val);

  mOptionsToggleLog->setChecked (val);

  mView->setDisplayScriptOutput(val);
}

void KSVTopLevel::writingEnabled (bool on)
{
  mFileLoad->setEnabled(on);
}

void KSVTopLevel::print()
{
// #define checkPage if (metrics.height() - y < fm.lineSpacing()) prt.newPage();

//   static KPrinter prt;
//   prt.setDocName(kapp->aboutData()->programName() + " Configuration");
//   prt.setCreator(kapp->aboutData()->programName());

//   static QPrintDialog* dlg = new QPrintDialog (&prt, this, "KSysV Print Dialog");
//   dlg->setCaption(kapp->makeStdCaption (i18n("Print")));

//   if (dlg->exec() == QDialog::Accepted)
//     {
//       int y = 10;
//       QPainter p;
//       p.begin( &prt );

//       QPaintDeviceMetrics metrics (&prt);

//       p.setFont (QFont("courier", 20, QFont::Bold));
//       QFontMetrics fm = p.fontMetrics();

//       p.drawText (10, y, i18n("%1 Configuration of %2")
//                   .arg (kapp->aboutData()->programName())
//                   .arg (ksv::hostname()));
//       y += fm.lineSpacing();

//       p.drawText (10, y, QDateTime::currentDateTime().toString());
//       y += fm.lineSpacing() * 2; // an extra empty line

//       for (int i = 0; i < ksv::runlevelNumber; ++i)
//         {
//           p.setFont (QFont("courier", 16, QFont::Bold));
//           QFontMetrics fm = p.fontMetrics();

//           p.drawText (10, y, i18n ("Runlevel %1").arg(i));
//           y += fm.lineSpacing();

//           checkPage

//           p.drawText (10, y, i18n ("Started Services"));
//           y += fm.lineSpacing() * 2; // an extra empty line

//        checkPage

//           p.setFont (QFont("courier", 10));
//           fm = p.fontMetrics();

//           for (QListViewItemIterator it (mView->startRL[i]);
//                it.current();
//                ++it)
//             {
//               KSVItem* item = static_cast<KSVItem*> (it.current());

//               y += fm.ascent();
//               p.drawText (10, y, item->toString());
//               y += fm.descent();

//        checkPage

//             }

//           p.setFont (QFont("courier", 16, QFont::Bold));
//           fm = p.fontMetrics();
//           y += fm.lineSpacing(); // an extra empty line
//           p.drawText (10, y, i18n ("Stopped Services"));
//           y += fm.lineSpacing() * 2; // an extra empty line

//        checkPage

//           p.setFont (QFont("courier", 10));
//           fm = p.fontMetrics();

//           for (QListViewItemIterator it (mView->stopRL[i]);
//                it.current();
//                ++it)
//             {
//               KSVItem* item = static_cast<KSVItem*> (it.current());
//        checkPage

//               y += fm.ascent();
//               p.drawText (10, y, item->toString());
//               y += fm.descent();
//             }
//        checkPage

//           p.setFont (QFont("courier", 16, QFont::Bold));
//           fm = p.fontMetrics();
//           y += fm.lineSpacing() * 3; // two extra empty line
//         }
//       //      QStringList lines = QStringList::split ('\n', mView->log(), true);
//       //       for(QStringList::Iterator it = lines.begin();
//       //           it != lines.end();
//       //           ++it)
//       //         {
//       //           y += fm.ascent();

//       //           QString line = *it;
//       //           if (line.isNull())
//       //             line = " ";

//       //           line.replace( QRegExp("\t"), "        " );

//       //           strncpy(buf,line.data(),160);

//       //           for (int j = 0 ; j <150; j++)
//       //             {
//       //               if (!isprint(buf[j]))
//       //                 buf[j] = ' ';
//       //             }

//       //           buf[line.length()] = '\0';
//       //           p.drawText( 10, y, buf );
//       //           y += fm.descent();
//       //         }

//       p.end();
//     }
}

void KSVTopLevel::printLog()
{
  KPrinter *prt = new KPrinter();
  prt->setDocName(kapp->aboutData()->programName() + " Log File");
  prt->setCreator(kapp->aboutData()->programName());

  if (prt->setup(this, i18n("Print Log File")))
    {
      KBusyManager::self()->setBusy (true);

      QPainter p (prt);
      QPaintDeviceMetrics pm (prt);
      const int height = pm.height();
      const int width = pm.width();

      QStringList lines = QStringList::split ('\n', mView->xmlLog(), true);
      QStringList::Iterator line = lines.begin();

      int y = 0;

      {
        QSimpleRichText rheading (i18n("<h1>KDE Sys-V Init Editor Log</h1>"),
                                  QFont("times"), QString::null, ksv::styleSheet(),
                                  QMimeSourceFactory::defaultFactory());
        rheading.setWidth (&p, width);
        int tmp_h = rheading.height();
        QRegion region (0, y, width, tmp_h);
        rheading.draw (&p, 0, y, region, colorGroup(), 0L);

        y += tmp_h;

        QSimpleRichText rdate (i18n("<h3>Printed on %1</h3><br/><br/>").arg(KGlobal::locale()->formatDateTime(QDateTime::currentDateTime())),
                               QFont("times"), QString::null, ksv::styleSheet(),
                               QMimeSourceFactory::defaultFactory());
        rdate.setWidth (&p, width);
        tmp_h = rdate.height();
	QRegion r2 (0, y, width, tmp_h);
        rdate.draw (&p, 0, y, r2, colorGroup(), 0L);

        y += tmp_h;
      }

      while (line != lines.end())
        {
          while (y < height)
            {
              QSimpleRichText rich (*line, QFont("times"), QString::null, ksv::styleSheet(),
                                    QMimeSourceFactory::defaultFactory());
              rich.setWidth (&p, width);

              int tmp_h = rich.height();
              if (y + tmp_h > height)
                break;

              QRegion region (0, y, width, tmp_h);
              rich.draw (&p, 0, y, region, colorGroup(), 0L);

              y += tmp_h;
              ++line;

              if (line == lines.end())
                goto printing_finished;

              kapp->processEvents();
            }

          prt->newPage();
          y = 0;
        }

    printing_finished:

      p.end();

      KBusyManager::self()->restore();
      delete prt;
    }
}

void KSVTopLevel::catchCannotGenerateNumber()
{
  if (mConfig->showMessage (ksv::CouldNotGenerateSortingNumber)) {
    KMessageBox::information (kapp->mainWidget(),
                              i18n ("<p>Unable to generate a valid " \
                                    "sorting number for this position. This means " \
                                    "that there was no number available between "\
                                    "the two adjacent services, and the service " \
                                    "did not fit in lexically.</p>" \
                                    "<p>Please adjust the sorting numbers manually "\
                                    "via the <strong>Properties dialog box</strong>.</p>"),
                              i18n ("Unable to Generate Sorting Number"),
                              ksv::notifications[ksv::CouldNotGenerateSortingNumber]);
  }

  statusBar()->message (i18n ("Unable to generate sorting number. Please change manually."),
                        5000);
}

void KSVTopLevel::closeEvent (QCloseEvent* e)
{
  if (KBusyManager::self()->isBusy())
    {
      // do not allow a close during clearing => otherwise we get a segfault
	  e->ignore();
      return;
    }

  KMainWindow::closeEvent (e);
}

void KSVTopLevel::dispatchEdit ()
{
  KSVDragList* list = mView->getOrigin();

  if (!list)
    {
      mEditCopy->setEnabled (false);
      mEditCut->setEnabled (false);
      mEditProperties->setEnabled (false);
      setPaste (false);

      return;
    }

  KSVItem* current = list->currentItem();

  if (current) // there's a list, and it's got items...
	{
      mEditCopy->setEnabled (true);
      mEditCut->setEnabled (true);
      mEditProperties->setEnabled (true);
	}
  else // no current item
	{
      mEditCopy->setEnabled (false);
      mEditCut->setEnabled (false);
      mEditProperties->setEnabled (false);
	}

  if (mView->scripts == list)
	{
      mEditCut->setEnabled (false);
	  setPaste (false);
	}
  else
	{
	  QMimeSource* mime = kapp->clipboard()->data();

	  if (mime && mime->provides ("application/x-ksysv"))
		setPaste (true);
	  else
		setPaste (false);
	}
}

void KSVTopLevel::enableUndo()
{
  mEditUndo->setEnabled (true);
}

void KSVTopLevel::disableUndo()
{
  mEditUndo->setEnabled (false);
  setChanged(false);
}

void KSVTopLevel::enableRedo()
{
  mEditRedo->setEnabled (true);
}

void KSVTopLevel::disableRedo ()
{
  mEditRedo->setEnabled (false);
}

void KSVTopLevel::slotSaveLog()
{
  static const QString& filter = ksv::logFileFilter();
  static const QString& ext = ksv::logFileExtension();
  QString filename = KFileDialog::getSaveFileName(0L, filter, this);

  if (filename.isEmpty())
    return;
  else if (filename.right(ext.length()) != ext)
    filename += ext;

  QFile file(filename);

  file.open( IO_WriteOnly | IO_Raw );
  QTextStream s(&file);

  s << "KDE System V Init Editor"
    << endl
    << QDateTime::currentDateTime().toString()
    << endl << endl
    << mView->log()
    << endl;

  file.close();
}

void KSVTopLevel::enableLogActions ()
{
  mFileSaveLog->setEnabled (true);
  mFilePrintLog->setEnabled (true); // disabled due to complexity
}

void KSVTopLevel::setCaption (bool changed)
{
  setPlainCaption (kapp->makeStdCaption(KNetwork::KResolver::localHostName(), true, changed));
}

KActionCollection* KSVTopLevel::filteredActions ()
{
  // remove unwanted (internal) actions
  static KActionCollection coll = *actionCollection();
  static bool initialized = false;

  if (!initialized)
    {
      coll.take(mPasteAppend);
      coll.take(mOpenDefault);
      initialized = true;
    }

  return &coll;
}

void KSVTopLevel::configureKeys ()
{
  KKeyDialog::configure (filteredActions(), this, true);
}

void KSVTopLevel::configureToolbars ()
{
  KEditToolbar dlg(filteredActions(), xmlFile(), true, this);
  connect(&dlg, SIGNAL( newToolbarConfig() ), this, SLOT( slotNewToolbarConfig() ));
  dlg.exec();
}

void KSVTopLevel::slotNewToolbarConfig()
{
  createGUI (xmlFile());
}

void KSVTopLevel::dispatchEditService (bool val)
{
  mToolsEditService->setChecked (val);
}

void KSVTopLevel::dispatchStartService (bool val)
{
  mToolsStartService->setChecked (val);
}

void KSVTopLevel::dispatchStopService (bool val)
{
  mToolsStopService->setChecked (val);
}

void KSVTopLevel::dispatchRestartService (bool val)
{
  mToolsRestartService->setChecked (val);
}

void KSVTopLevel::pasteAppend ()
{
  mView->pasteAppend ();
}

void KSVTopLevel::editService ()
{
  mView->editService ();
}

void KSVTopLevel::toggleRunlevel (int index)
{
  bool state = mVisible[index]->isChecked();

  if (state)
	mView->showRunlevel (index);
  else
	mView->hideRunlevel (index);

  mConfig->setShowRunlevel (index, state);
}

void KSVTopLevel::saveAs ()
{
  static const QString& filter = ksv::nativeFileFilter();
  static const QString& ext = ksv::nativeFileExtension();
  QString filename = KFileDialog::getSaveFileName(0L, filter, this);

  if (filename.isEmpty())
    return;
  else if (filename.right(ext.length()) != ext)
    filename += ext;

  QFile file(filename);
  // we're busy
  KBusyManager::self()->setBusy (true);

  file.open (IO_WriteOnly | IO_Raw);
  QDataStream s (&file);

  if (ksv::IO::saveConfiguration (s, mView->startRL, mView->stopRL))
    {
      statusBar()->message(i18n("Configuration package saved successfully."), 5000);
    }
  else
    {
      kdDebug(3000) << "ERROR saving file" << endl;
    }

  file.close();

  KBusyManager::self()->restore();
}

void KSVTopLevel::load ()
{
  static const QString& filter = ksv::nativeFileFilter();
  QString filename = KFileDialog::getOpenFileName(0L, filter, this);

  if (filename.isEmpty())
    return;

  QFile file(filename);
  KBusyManager::self()->setBusy (true);

  file.open (IO_ReadOnly | IO_Raw);
  QDataStream s (&file);
  QValueList<KSVData>* startLists = new QValueList<KSVData>[ksv::runlevelNumber];
  QValueList<KSVData>* stopLists = new QValueList<KSVData>[ksv::runlevelNumber];

  if (ksv::IO::loadSavedConfiguration (s, startLists, stopLists))
    {
      statusBar()->message(i18n("Configuration package loaded successfully."), 5000);
      mView->mergeLoadedPackage (startLists, stopLists);
    }
  else
    {
      kdDebug (3000) << "ERROR loading file" << endl;
    }

  file.close();

  delete[] startLists;
  delete[] stopLists;

  KBusyManager::self()->restore();
}

void KSVTopLevel::pushUndoAction (KSVAction* action)
{
  mRedoList->clear();
  mUndoList->push (action);

  setChanged(true);
}

#include "TopWidget.moc"
