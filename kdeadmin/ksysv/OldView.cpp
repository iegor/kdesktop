/***************************************************************************
    begin                : Sun Oct 3 1999
    copyright            : (C) 1997-2000 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

/*****************************************
 **                                     **
 **            Main Widget              **
 **                                     **
 *****************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qprogressdialog.h>
#include <qkeycode.h>
#include <qmessagebox.h>
#include <qgroupbox.h>
#include <qaccel.h>
#include <qscrollbar.h>
#include <qtextedit.h>
#include <qcstring.h>
#include <qclipboard.h>
#include <qheader.h>
#include <qlabel.h>
#include <qstylesheet.h>
#include <qwhatsthis.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qvaluelist.h>
#include <qsplitter.h>
#include <qmap.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kcursor.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobalsettings.h>
#include <kaboutdata.h>
#include <kdialog.h>
#include <kdirwatch.h>
#include <kcompletion.h>
#include <krun.h>
#include <kopenwith.h>
#include <kmimemagic.h>

#include "kbusymanager.h"
#include "Properties.h"
#include "kscroller.h"
#include "IOCore.h"
#include "ksvdraglist.h"
#include "ksvdrag.h"
#include "trash.h"
#include "ksv_core.h"
#include "ksv_conf.h"
#include "OldView.h"
#include "ActionList.h"
#include "TopWidget.h"

KSVContent::KSVContent (KPopupMenu* openWithMenu, KSVTopLevel* parent, const char* name)
  : QSplitter (QSplitter::Vertical, parent, name),
    startRL (new KSVDragList*[ksv::runlevelNumber]),
    stopRL (new KSVDragList*[ksv::runlevelNumber]),
    conf(KSVConfig::self()),
	mScriptBox (0L),
    mRunlevels (new QVBox*[ksv::runlevelNumber]),
	mOrigin (0L),
    mOpenWithMenu (openWithMenu), m_buffer( QCString() )
{
  setOpaqueResize( KGlobalSettings::opaqueResize() );

  KXMLGUIFactory* factory = parent->factory();
  mItemMenu = static_cast<KPopupMenu*> (factory->container ("item_menu", parent));
  mItemMenu->insertTitle (i18n ("Runlevel Menu"), -1, 0);
  mContextMenu = static_cast<KPopupMenu*> (factory->container ("list_menu", parent));
  mContextMenu->insertTitle (i18n ("Runlevel Menu"), -1, 0);
  mScriptMenu = static_cast<KPopupMenu*> (factory->container ("script_menu", parent));
  mScriptMenu->insertTitle (i18n ("Services Menu"), -1, 0);

  mScroller = new KScroller (this);
  mContent = new QFrame(mScroller, "KSysV Real Content");

  mScroller->setContent (mContent);

  initLList();

  // watch services dir
  KDirWatch* dirwatch = KDirWatch::self();
  dirwatch->addDir (conf->scriptPath ());
  connect (dirwatch, SIGNAL (dirty (const QString&)),
           this, SLOT (updateServicesAfterChange (const QString&)));
  connect (dirwatch, SIGNAL (created (const QString&)),
           this, SLOT (updateServicesAfterChange (const QString&)));
  connect (dirwatch, SIGNAL (deleted (const QString&)),
           this, SLOT (updateServicesAfterChange (const QString&)));

  setSizes(KSVContent::panningFactorToSplitter (conf->panningFactor()));

  // someone must have focus
  scripts->setFocus();

  // show/hide everything
  for (int i = 0; i < ksv::runlevelNumber; ++i)
	{
	  if (conf->showRunlevel (i))
		mRunlevels[i]->show();
	  else
		mRunlevels[i]->hide();
	}

  textDisplay->setStyleSheet (ksv::styleSheet());

  // Open With... menu
  connect (mOpenWithMenu, SIGNAL (activated (int)), this, SLOT (openWith (int)));

  calcMinSize();
}

KSVContent::~KSVContent()
{
  delete[] mRunlevels;

  delete[] startRL;
  delete[] stopRL;
  delete scripts;
  delete trash;
  delete textDisplay ;

}

void KSVContent::updateServicesAfterChange (const QString& dir)
{
  if (!dir.startsWith(conf->scriptPath()))
    return;

  //   const bool enabled = scripts->isEnabled ();
  initScripts();
}

void KSVContent::updateRunlevelsAfterChange ()
{
  // MUTEX
  KBusyManager::self()->setBusy (true);

  //  int i = 0;
  for (int i = 0; i < ksv::runlevelNumber; ++i) {
    startRL[i]->setEnabled(false);
    startRL[i]->clear();

    stopRL[i]->setEnabled(false);
    stopRL[i]->clear();
  }

  initRunlevels();

  for (int i = 0; i < ksv::runlevelNumber; ++i)
    {
      startRL[i]->setEnabled(true);
      stopRL[i]->setEnabled(true);
    }

  // refresh GUI
  qApp->processEvents();

  scripts->setFocus();

  KBusyManager::self()->restore();
}

void KSVContent::initLList()
{
  QHBoxLayout *lay = new QHBoxLayout( mContent, KDialog::marginHint(), KDialog::spacingHint() );
  mScriptBox = new QVBox (mContent);
  lay->addWidget(mScriptBox);
  mScriptBox->setSpacing (KDialog::spacingHint());

  QVBox* scriptLabelBox = new QVBox (mScriptBox);
  QLabel* servL = new QLabel (i18n("&Available\n" \
                                   "Services"), scriptLabelBox);

  // provide quickhelp
  QWhatsThis::add (scriptLabelBox,
				   i18n("<p>These are the <img src=\"small|exec\"/> <strong>services</strong> available on your computer. " \
						"To start a service, drag it onto the <em>Start</em> " \
						"section of a runlevel.</p>" \
						"<p>To stop one, do the same for the <em>Stop</em> section.</p>"));


  QFont bold_font = QFont(KGlobalSettings::generalFont());
  bold_font.setBold(TRUE);
  servL->setFont(bold_font);

  scripts = new KServiceDragList (scriptLabelBox, "Scripts");
  scripts->setAcceptDrops (false);
  scripts->setColumnWidthMode (KSVItem::SortNumber, QListView::Manual);
  scripts->setColumnWidth(KSVItem::SortNumber,0);
  scripts->setSorting (KSVItem::ServiceName);
  scripts->header()->setResizeEnabled (false, 0);

  scripts->setDefaultIcon (SmallIcon("exec"));
  mOrigin = scripts;

  // setBuddy
  servL->setBuddy(scripts);

  // doubleclick && return
  connect (scripts, SIGNAL(executed(QListViewItem*)),
		   this, SLOT(slotScriptProperties(QListViewItem*)));
  connect (scripts, SIGNAL (returnPressed (QListViewItem*)),
		   this, SLOT (slotScriptProperties (QListViewItem*)));

  // context menus
  connect (scripts, SIGNAL (contextMenu (KListView*, QListViewItem*, const QPoint&)),
 		   this, SLOT (popupServicesMenu (KListView*, QListViewItem*, const QPoint&)));

  // for cut & copy
  connect (scripts, SIGNAL (newOrigin ()),
		   this, SLOT (fwdOrigin ()));

  // for origin updates
  connect (scripts, SIGNAL (newOrigin (KSVDragList*)),
		   this, SLOT (fwdOrigin (KSVDragList*)));

  trash = new KSVTrash(mScriptBox, "Trash");
  connect (trash, SIGNAL (undoAction (KSVAction*)), this, SLOT (fwdUndoAction (KSVAction*)));
  QWhatsThis::add (trash,
				   i18n ("<p>You can drag services from a runlevel onto " \
                         "the <img src=\"small|trash\"/> <strong>trashcan</strong> to " \
						 "delete them from that runlevel.</p><p>The <strong>Undo command</strong> "\
						 "can be used to restore deleted entries.</p>"));

  for(int i = 0; i < ksv::runlevelNumber; ++i)
    {
      mRunlevels[i] = new QVBox (mContent);
      lay->addWidget(mRunlevels[i]);
      mRunlevels[i]->setSpacing (KDialog::spacingHint());

      // create QString for label
      QString _label (i18n("Runlevel &%1").arg(i));
      // and for the name
      QString _name (i18n("Runlevel %1").arg(i));

      QVBox* startBox = new QVBox (mRunlevels[i]);
      QWhatsThis::add (startBox,
					   i18n("<p>These are the services <strong>started</strong> in runlevel %1.</p>" \
							"<p>The number shown on the left of the <img src=\"user|ksysv_start\"/> icon " \
							"determines the order in which the services are started. " \
							"You can arrange them via drag and drop, as long as a suitable " \
							"<em>sorting number</em> can be generated.</p><p>If that's not possible, you have " \
							"to change the number manually via the <strong>Properties dialog box</strong>.</p>").arg(i));

      QLabel* rlL = new QLabel(_label, startBox);
      new QLabel(i18n("Start"), startBox);
      rlL->setFont(bold_font);

      // create the "START" list:
      startRL[i] = new KSVDragList(startBox, (_name + " START").latin1());
      startRL[i]->setDefaultIcon(SmallIcon("ksysv_start"));

      QVBox* stopBox = new QVBox (mRunlevels[i]);
      new QLabel(i18n("Stop"), stopBox);
      QWhatsThis::add (stopBox,
					   i18n("<p>These are the services <strong>stopped</strong> in runlevel %1.</p>" \
							"<p>The number shown on the left of the <img src=\"user|ksysv_stop\"/> icon " \
							"determines the order in which the services are stopped. " \
							"You can arrange them via drag and drop, as long as a suitable " \
							"<em>sorting number</em> can be generated.</p><p>If that's not possible, you have " \
							"to change the number manually via the <strong>Properties dialog box</strong>.</p>").arg(i));

      // create the "STOP" list:
      stopRL[i] = new KSVDragList(stopBox, (_name + " STOP").latin1());
      stopRL[i]->setDefaultIcon(SmallIcon("ksysv_stop"));

      // set the buddy widget for the "Runlevel %i" label... => the corresponding runlevel
      rlL->setBuddy(startRL[i]);

      // for cut'n'paste
      connect (startRL[i], SIGNAL (newOrigin ()),
			   this, SLOT (fwdOrigin ()));
      connect (startRL[i], SIGNAL (newOrigin (KSVDragList*)),
			   this, SLOT (fwdOrigin (KSVDragList*)));

      connect (stopRL[i], SIGNAL (newOrigin ()),
			   this, SLOT (fwdOrigin ()));
      connect (stopRL[i], SIGNAL (newOrigin (KSVDragList*)),
			   this, SLOT (fwdOrigin (KSVDragList*)));
    }

  lay->addStretch(1);

  connect (scripts, SIGNAL(undoAction(KSVAction*)),
		   this, SLOT(fwdUndoAction(KSVAction*)));

  // add text-diplay widget
  textDisplay = new QTextEdit( QString::null, QString::null, this, "TextDisplayWidget" );
  textDisplay->setTextFormat( Qt::RichText );
  textDisplay->setReadOnly( true );

  for (int i = 0; i < ksv::runlevelNumber; ++i)
	{
	  connect (startRL[i], SIGNAL(newOrigin()), stopRL[i], SLOT(slotNewOrigin()));
	  connect (stopRL[i], SIGNAL(newOrigin()), startRL[i], SLOT(slotNewOrigin()));

	  connect (startRL[i], SIGNAL(undoAction(KSVAction*)),
			   this, SLOT(fwdUndoAction(KSVAction*)));
	  connect (stopRL[i], SIGNAL(undoAction(KSVAction*)),
			   this, SLOT(fwdUndoAction(KSVAction*)));

	  // doubleclick && return
	  connect (startRL[i], SIGNAL(executed(QListViewItem*)),
			   this, SLOT(slotDoubleClick(QListViewItem*)));
	  connect (stopRL[i], SIGNAL(executed(QListViewItem*)),
			   this, SLOT(slotDoubleClick(QListViewItem*)));
	  connect (startRL[i], SIGNAL(returnPressed(QListViewItem*)),
			   this, SLOT(slotDoubleClick(QListViewItem*)));
	  connect (stopRL[i], SIGNAL(returnPressed(QListViewItem*)),
			   this, SLOT(slotDoubleClick(QListViewItem*)));

	  // context menus
	  connect (startRL[i], SIGNAL (contextMenu (KListView*, QListViewItem*, const QPoint&)),
			   this, SLOT (popupRunlevelMenu (KListView*, QListViewItem*, const QPoint&)));
	  connect (stopRL[i], SIGNAL (contextMenu (KListView*, QListViewItem*, const QPoint&)),
			   this, SLOT (popupRunlevelMenu (KListView*, QListViewItem*, const QPoint&)));

	  // cannot generate sorting number
	  connect (startRL[i], SIGNAL(cannotGenerateNumber()),
			   this, SLOT(fwdCannotGenerateNumber()));
	  connect (stopRL[i], SIGNAL(cannotGenerateNumber()),
			   this, SLOT(fwdCannotGenerateNumber()));

	  // connecting origin things for "Scripts", too
	  connect (scripts, SIGNAL(newOrigin()), startRL[i], SLOT(slotNewOrigin()));
	  connect (scripts, SIGNAL(newOrigin()), stopRL[i], SLOT(slotNewOrigin()));
	  connect (startRL[i], SIGNAL(newOrigin()), scripts, SLOT(slotNewOrigin()));
	  connect (stopRL[i], SIGNAL(newOrigin()), scripts, SLOT(slotNewOrigin()));

	  // use this loop for setting tooltips
 	  startRL[i]->setToolTip (i18n("Drag here to start services\n" \
                                   "when entering runlevel %1").arg(i));
 	  stopRL[i]->setToolTip (i18n("Drag here to stop services\n" \
                                  "when entering runlevel %1").arg(i));

	  for (int j = 0; j < ksv::runlevelNumber; ++j)
		{
		  if (i != j)
			{
			  connect (startRL[i], SIGNAL (newOrigin()), startRL[j], SLOT (slotNewOrigin()));
			  connect (stopRL[i], SIGNAL (newOrigin()), stopRL[j], SLOT (slotNewOrigin()));

			  connect (startRL[i], SIGNAL(newOrigin()), stopRL[j], SLOT(slotNewOrigin()));
			  connect (stopRL[i], SIGNAL(newOrigin()), startRL[j], SLOT(slotNewOrigin()));
			}
		}
	}
}

void KSVContent::fwdUndoAction (KSVAction* a)
{
  emit undoAction(a);
}

void KSVContent::initScripts() {
  QDir scriptDir = QDir(conf->scriptPath());
  if (!scriptDir.exists())
	return;

  scriptDir.setFilter (QDir::Files | QDir::Hidden |
                       QDir::NoSymLinks | QDir::Executable);

  scriptDir.setSorting (QDir::Name);

  //  const QFileInfoList *scriptList = scriptDir.entryInfoList();
  QFileInfoListIterator it (*scriptDir.entryInfoList());

  KCompletion* comp = ksv::serviceCompletion();
  comp->clear ();

  // clear the listview
  scripts->setEnabled(false);
  scripts->clear();

  QFileInfo* fi; QString name;
  while ((fi = it.current()))
    {
      name = fi->fileName();
      scripts->initItem(name,
                        ksv::IO::relToAbs(conf->scriptPath(), fi->dirPath(FALSE)),
                        name, 0);

      comp->addItem (name);

      ++it;

      // keep GUI alive
      qApp->processEvents();
    }

  scripts->setEnabled(true);
  scripts->setToolTip (i18n("The services available on your computer"));
}

void KSVContent::initRunlevels()
{
  for (int i = 0; i < ksv::runlevelNumber; ++i)
    {
      // clear the listviews
      startRL[i]->clear();
      stopRL[i]->clear();

      const QString _path = conf->runlevelPath() + QString("/rc%1.d").arg(i);

      if (!QDir(_path).exists())
		continue;

      QDir d = QDir(_path);
      d.setFilter( QDir::Files );
      d.setSorting( QDir::Name );

      const QFileInfoList *rlList = d.entryInfoList();
      QFileInfoListIterator it( *rlList ); // create list iterator
      QFileInfo* fi;                       // pointer for traversing

      while ( (fi=it.current()) )
		{                       // for each file...
		  info2Widget( fi, i);
		  ++it;        	      // goto next list element

          // keep GUI alive
          qApp->processEvents();
		}
    }
}

void KSVContent::info2Widget( QFileInfo* info, int index )
{
  if (!info->exists())
    return;

  QString f_name = info->fileName();

  QFileInfo link_info = QFileInfo(info->readLink());
  QString l_base = link_info.fileName();

  QString l_path = ksv::IO::relToAbs(conf->scriptPath(), link_info.dirPath(FALSE));

  QString name;
  int number;
  ksv::IO::dissectFilename( f_name, name, number );

  // finally insert the items...
  if ( f_name.left(1) == "S" )
    startRL[index]->initItem( l_base, l_path, name, number );
  else
    stopRL[index]->initItem( l_base, l_path, name, number );
}

void KSVContent::slotWriteSysV()
{
  appendLog(i18n("<vip>WRITING CONFIGURATION</vip>"),
			i18n("** WRITING CONFIGURATION **"));

  for (int i = 0; i < ksv::runlevelNumber; ++i)
    {
	  appendLog(i18n("<rl>RUNLEVEL %1</rl>").arg(i),
				i18n("** RUNLEVEL %1 **").arg(i));

      clearRL(i); // rm changed/deleted entries

      // process "Start"
	  KSVItem* item = 0L;
	  for (QListViewItemIterator it (startRL[i]);
		   (item = static_cast<KSVItem*> (it.current()));
		   ++it)
		{
		  if (item->isChanged() || item->isNew())
			writeToDisk (*item->data(), i, true);
		}

	  // process "Stop"
	  for (QListViewItemIterator it (stopRL[i]);
		   (item = static_cast<KSVItem*> (it.current()));
		   ++it)
		{
		  if (item->isChanged() || item->isNew())
			writeToDisk (*item->data(), i, false);
		}

	  appendLog("<br/><br/>", "\n");
	}

  appendLog("<br/>", "");
}

void KSVContent::writeToDisk(const KSVData& _w, int _rl, bool _start) {
  QString rich, plain;
  ksv::IO::makeSymlink (_w, _rl, _start, rich, plain);
  appendLog(rich, plain);
}

void KSVContent::repaintRunlevels ()
{
  for (int i = 0; i < ksv::runlevelNumber; ++i)
    {
      startRL[i]->triggerUpdate();
      stopRL[i]->triggerUpdate();
    }

  scripts->triggerUpdate();
}

void KSVContent::clearRL(int _rl)
{
  QString path = conf->runlevelPath() + QString("/rc%1.d").arg(_rl);

  QDir dir (path);

  KSVData* d = 0L;

  for (QPtrListIterator<KSVData> it (startRL[_rl]->getDeletedItems());
	   (d = it.current());
	   ++it)
	{
	  // ugly hack -> dont try to delete if entry is new (i.e. not save to disk)
	  if (d->newEntry() && d->originalRunlevel() != startRL[_rl]->name())
		break;

	  QFileInfo file (path + QString("/S%1%2").arg(d->numberString()).arg(d->label()));

	  QString rich, plain;
	  ksv::IO::removeFile (file, dir, rich, plain);
	  appendLog(rich, plain);
	}

  // keep GUI alive
  qApp->processEvents();

  for (QPtrListIterator<KSVData> it (stopRL[_rl]->getDeletedItems());
	   (d = it.current());
	   ++it)
	{
	  // ugly, too
	  if (d->newEntry() && d->originalRunlevel() != stopRL[_rl]->name())
		break;

	  QFileInfo file (path + QString("/K%1%2").arg(d->numberString()).arg(d->label()));

	  QString rich, plain;
	  ksv::IO::removeFile (file, dir, rich, plain);
	  appendLog(rich, plain);
	}

  // keep GUI alive
  qApp->processEvents();
}

void KSVContent::infoOnData (KSVItem* item)
{
  KSVData oldState = *item->data();
  KSVData newState = oldState;
  KSVEntryPropertiesDialog* props = new KSVEntryPropertiesDialog (newState, kapp->mainWidget());

  connect (props, SIGNAL (editService (const QString&)),
           this, SLOT (editService (const QString&)));
  connect (props, SIGNAL (startService (const QString&)),
           this, SLOT (startService (const QString&)));
  connect (props, SIGNAL (stopService (const QString&)),
           this, SLOT (stopService (const QString&)));
  connect (props, SIGNAL (restartService (const QString&)),
           this, SLOT (restartService (const QString&)));

  int res = props->exec();

  if (res == QDialog::Accepted
      && !(oldState == newState && oldState.number() == newState.number()))
    {
      item->copy (newState);

      reSortRL();

      emit undoAction(new ChangeAction(getOrigin(), &oldState, &newState));
    }
}

void KSVContent::stopService ()
{
  KSVContent::stopService (getOrigin()->currentItem()->filenameAndPath());
}

void KSVContent::stopService (const QString& path)
{
  KProcess *_proc = new KProcess();
  _proc->clearArguments();

  *_proc << path << "stop";

  connect(_proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotExitedProcess(KProcess*)));
  connect(_proc, SIGNAL(receivedStdout(KProcess*, char*, int)), this, SLOT(slotOutputOrError(KProcess*, char*, int)));
  connect(_proc, SIGNAL(receivedStderr(KProcess*, char*, int)), this, SLOT(slotOutputOrError(KProcess*, char*, int)));

  // refresh textDisplay
  appendLog(i18n("** <stop>Stopping</stop> <cmd>%1</cmd> **<br/>").arg(path),
			i18n("** Stopping %1 **").arg(path));

  _proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

  // notify parent
  emit sigRun(path + i18n(" stop"));
}

void KSVContent::startService ()
{
  KSVContent::startService (getOrigin()->currentItem()->filenameAndPath());
}

void KSVContent::startService (const QString& path)
{
  KProcess* _proc = new KProcess();
  _proc->clearArguments();

  *_proc << path << "start";

  connect(_proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotExitedProcess(KProcess*)));
  connect(_proc, SIGNAL(receivedStdout(KProcess*, char*, int)), this, SLOT(slotOutputOrError(KProcess*, char*, int)));
  connect(_proc, SIGNAL(receivedStderr(KProcess*, char*, int)), this, SLOT(slotOutputOrError(KProcess*, char*, int)));

  // refresh textDisplay
  appendLog(i18n("** <start>Starting</start> <cmd>%1</cmd> **<br/>").arg(path),
			i18n("** Starting %1 **").arg(path));

  _proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

  // notify parent
  emit sigRun(path + i18n(" start"));
}

void KSVContent::editService()
{
  editService (getOrigin()->currentItem()->filenameAndPath());
}

void KSVContent::editService (const QString& path)
{
  // unfortunately KRun::run() only takes an URL-list instead of a single
  // URL as an argument.
  KURL url; url.setPath (path); KURL::List urls; urls << url;
  KRun::run (*ksv::IO::preferredServiceForFile (path), urls);
}

void KSVContent::restartService ()
{
  KSVContent::restartService (getOrigin()->currentItem()->filenameAndPath());
}

void KSVContent::restartService (const QString& path)
{
  // restarting
  KProcess *_proc = new KProcess();
  _proc->clearArguments();

  *_proc << path << "restart";

  connect(_proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotExitDuringRestart(KProcess*)));
  connect(_proc, SIGNAL(receivedStdout(KProcess*, char*, int)), this, SLOT(slotOutputOrError(KProcess*, char*, int)));
  connect(_proc, SIGNAL(receivedStderr(KProcess*, char*, int)), this, SLOT(slotOutputOrError(KProcess*, char*, int)));

  // refresh textDisplay
  appendLog(i18n("** Re-starting <cmd>%1</cmd> **</br>").arg(path),
			i18n("** Re-starting %1 **").arg(path));

  _proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

  // notify parent
  emit sigRun(path + i18n(" restart"));
}

void KSVContent::slotOutputOrError( KProcess*, char* _buffer, int _buflen) {
  if (_buflen > 0) {
    m_buffer += QCString( _buffer, _buflen + 1 );
    appendLog( m_buffer );
  }
}

void KSVContent::slotExitedProcess( KProcess* proc ) {
  appendLog("<hr/>", "--------------");

  emit sigStop();
  delete proc;
}

void KSVContent::slotScriptsNotRemovable()
{
  emit sigNotRemovable();
}

void KSVContent::slotDoubleClick (QListViewItem* item) {
  infoOnData(static_cast<KSVItem*>(item));
}

void KSVContent::slotScriptProperties(QListViewItem* item)
{
  KSVServicePropertiesDialog* prop =
    new KSVServicePropertiesDialog (*static_cast<KSVItem*> (item)->data(), kapp->mainWidget());

  connect (prop, SIGNAL (editService (const QString&)),
           this, SLOT (editService (const QString&)));
  connect (prop, SIGNAL (startService (const QString&)),
           this, SLOT (startService (const QString&)));
  connect (prop, SIGNAL (stopService (const QString&)),
           this, SLOT (stopService (const QString&)));
  connect (prop, SIGNAL (restartService (const QString&)),
           this, SLOT (restartService (const QString&)));

  prop->exec();

  //  delete prop;
}

void KSVContent::slotExitDuringRestart( KProcess* proc )
{
  delete proc;
  proc = new KProcess(); // necessary because otherwise we still have some
                         // signals connected that screw up our output
  proc->clearArguments();

  connect(proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotExitedProcess(KProcess*)));
  connect(proc, SIGNAL(receivedStdout(KProcess*, char*, int)), this, SLOT(slotOutputOrError(KProcess*, char*, int)));
  connect(proc, SIGNAL(receivedStderr(KProcess*, char*, int)), this, SLOT(slotOutputOrError(KProcess*, char*, int)));

  proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);
}

KSVDragList* KSVContent::getOrigin()
{
  return mOrigin;
}

void KSVContent::setDisplayScriptOutput(bool val)
{
  if (val)
    {
      setSizes(KSVContent::panningFactorToSplitter (conf->panningFactor()));
      textDisplay->show();
    }
  else
    {
      conf->setPanningFactor (KSVContent::splitterToPanningFactor (sizes()));
      textDisplay->hide();
    }

  calcMinSize();
}

int KSVContent::splitterToPanningFactor (const QValueList<int>& list)
{
  const int cont_size = *list.at(0);
  const int log_size = *list.at(1);

  return cont_size * 100 / (cont_size + log_size);
}

const QValueList<int>& KSVContent::panningFactorToSplitter (int panningFactor)
{
  static QValueList<int> res;
  res.clear();

  res << panningFactor << 100 - panningFactor;

  return res;
}

void KSVContent::appendLog (const QString& rich, const QString& plain)
{
  static bool changed = false;

  if (!changed)
    {
      changed = true;
      emit logChanged();
    }

  mLogText += plain + "\n";
  mXMLLogText += rich + "\n";
  textDisplay->append (rich);
}

void KSVContent::appendLog(const QCString& _buffer)
{
    QStringList _lines = QStringList::split( "\n", QString::fromLocal8Bit( _buffer ) );
    for ( QStringList::Iterator it = _lines.begin(); it != _lines.end(); ++it )
        appendLog( *it, *it );

    m_buffer = QCString();
}

const QString& KSVContent::log () const
{
  return mLogText;
}

const QString& KSVContent::xmlLog () const
{
  return mXMLLogText;
}

void KSVContent::fwdCannotGenerateNumber() {
  emit cannotGenerateNumber();
}

void KSVContent::updatePanningFactor()
{
  conf->setPanningFactor(KSVContent::splitterToPanningFactor(sizes()));
}

void KSVContent::fwdOrigin ()
{
  emit newOrigin();
}

void KSVContent::fwdOrigin (KSVDragList* list)
{
  mOrigin = list;
}

void KSVContent::showEvent (QShowEvent* e)
{
  calcMinSize();

  QSplitter::showEvent (e);
}

void KSVContent::reSortRL()
{
  getOrigin()->sort();
}

void KSVContent::pasteAppend()
{
  KSVDragList* list = getOrigin();

  if (list)
    {
	  KSVData data;

	  if (KSVDrag::decodeNative (kapp->clipboard()->data(), data))
		{
		  KSVAction* action = 0L;

		  if (list->insert (data, list->lastItem(), action))
			{
			  emit undoAction (action);
			}
		}
	}
}

void KSVContent::resizeEvent (QResizeEvent* e)
{
  updatePanningFactor();

  QSplitter::resizeEvent (e);
}

void KSVContent::moveEvent (QMoveEvent* e)
{
  QSplitter::moveEvent (e);
}

void KSVContent::setColors (const QColor& newNormal,
                            const QColor& newSelected,
                            const QColor& changedNormal,
                            const QColor& changedSelected)
{
  for (int i = 0; i < ksv::runlevelNumber; ++i)
    {
      startRL[i]->setNewNormalColor (newNormal);
      startRL[i]->setNewSelectedColor (newSelected);
      startRL[i]->setChangedNormalColor (changedNormal);
      startRL[i]->setChangedSelectedColor (changedSelected);
      startRL[i]->viewport()->update();

      stopRL[i]->setNewNormalColor (newNormal);
      stopRL[i]->setNewSelectedColor (newSelected);
      stopRL[i]->setChangedNormalColor (changedNormal);
      stopRL[i]->setChangedSelectedColor (changedSelected);
      stopRL[i]->viewport()->update();
    }
}

void KSVContent::multiplexEnabled (bool val)
{
  QListView* list = getOrigin();


  if (list)
	{
	  list->clearSelection();
	  list->setCurrentItem (0L);
	}


  for (int i = 0; i < ksv::runlevelNumber; ++i)
    {
      startRL[i]->setEnabled (val);
      startRL[i]->setAcceptDrops (val);

      stopRL[i]->setEnabled (val);
      stopRL[i]->setAcceptDrops (val);
    }

  if (!val)
    {
      mOrigin = 0L;
      emit newOrigin ();

      KSVConfig *config = KSVConfig::self();
      QFileInfo *file = new QFileInfo( config->scriptPath() );

      if ( !file->exists() )
      {
	int choice = KMessageBox::warningYesNo
	  (kapp->mainWidget(),
	   i18n ("<p>You have specified that your system's init "  \
		 "scripts are located in the folder "           \
		 "<tt><b>%1</b></tt>, but this folder does not "\
		 "exist. You probably selected the wrong "         \
		 "distribution during configuration.</p> "         \
		 "<p>If you reconfigure %2, it may be possible to "\
		 "fix the problem. If you choose to reconfigure, " \
		 "you should shut down the application "           \
		 "and the configuration wizard will appear the "   \
		 "next time %3 is run. If you choose not to "      \
		 "reconfigure, you will not be able to view or "   \
		 "edit your system's init configuration.</p>"      \
		 "<p>Would you like to reconfigure %4?</p>")
	   .arg (config->scriptPath())
	   .arg (kapp->aboutData()->programName())
	   .arg (kapp->aboutData()->programName())
	   .arg (kapp->aboutData()->programName()),
	   i18n("Folder Does Not Exist"),i18n("Reconfigure"),i18n("Do Not Reconfigure"));

	if ( choice == KMessageBox::Yes )
	{
	  config->setConfigured(false);
	  config->writeSettings();
	}
      }
      else
      {
	KMessageBox::information (kapp->mainWidget(),
				  i18n ("<p>You do not have the right permissions "   \
					"to edit your system's init configuration. "     \
					"However, you are free to browse the runlevels.</p>" \
					"<p>If you really want to edit the "                 \
					"configuration, either <strong>restart</strong> "    \
					"%1 <strong>as root</strong> (or another privileged "\
					"user), or ask your sysadmin to install %2 "         \
					"<em>suid</em> or <em>sgid</em>.</p>"                \
					"<p>The latter way is not recommended though, "      \
					"due to security issues.</p>")
				  .arg (kapp->aboutData()->programName())
				  .arg (kapp->aboutData()->programName()),
				  i18n("Insufficient Permissions"),
				  ksv::notifications[ksv::RunlevelsReadOnly]);
      }

      delete file;
    }
}

void KSVContent::hideRunlevel (int index)
{
  mRunlevels[index]->hide();
  calcMinSize();
}

void KSVContent::showRunlevel (int index)
{
  mRunlevels[index]->show();
  calcMinSize();
}

void KSVContent::popupRunlevelMenu (KListView* list, QListViewItem* i, const QPoint& p)
{
  if (i)
	mItemMenu->exec (p, 1);
  else
	{
	  if (!list->header()->rect().contains (list->mapFromGlobal(p)))
		mContextMenu->exec (p, 1);
	}
}

void KSVContent::popupServicesMenu (KListView*, QListViewItem* i, const QPoint& p)
{
  if (i)
    {
      mOpenWithMenu->clear();

      mOpenWithOffers
        = ksv::IO::servicesForFile (static_cast<KSVItem*>(i)->filenameAndPath());

      int i = 0;
      for (KTrader::OfferList::Iterator it = mOpenWithOffers.begin();
           it != mOpenWithOffers.end();
           ++it)
        {
          mOpenWithMenu->insertItem (SmallIconSet((*it)->icon()), (*it)->name(), i);
          ++i;
        }

      if (i >= 1)
        mOpenWithMenu->insertSeparator();

      mOpenWithMenu->insertItem (i18n ("&Other..."), this, SLOT (openWith()));

      mScriptMenu->exec (p, 1);
    }
}

void KSVContent::openWith ()
{
  KURL url; url.setPath(static_cast<KSVItem*>(getOrigin()->currentItem())->filenameAndPath());
  KURL::List urls; urls.append (url);

	KRun::displayOpenWithDialog (urls);

  kdDebug(3000) << "Opening with..." << endl;
}

void KSVContent::openWith (int index)
{
  if (index < 0)
    return;

  KService::Ptr service = *mOpenWithOffers.at (index);
  KURL url; url.setPath(static_cast<KSVItem*>(getOrigin()->currentItem())->filenameAndPath());
  KURL::List urls; urls.append (url);

  KRun::run (*service, urls);

  kdDebug(3000) << "Opening with " << service->exec() << endl;
}

void KSVContent::calcMinSize ()
{
  // Cryptic code alert: Changing w or h will change mMinSize
  QCOORD& w = mMinSize.rwidth();
  QCOORD& h = mMinSize.rheight();

  w = 2 * KDialog::marginHint() + mScriptBox->sizeHint().width();
  h = 2 * KDialog::marginHint() + mScriptBox->sizeHint().height();

  for (int i = 0; i < ksv::runlevelNumber; ++i)
    {
      if (mRunlevels[i]->isHidden())
        continue;

      w += KDialog::spacingHint() + mRunlevels[i]->sizeHint().width();
      h = kMax (h, mRunlevels[i]->sizeHint().height());
    }

  mContent->layout()->setEnabled(false);
  mContent->setMinimumSize(mMinSize);
  mScroller->updateScrollBars();
  mContent->layout()->setEnabled(true);
}

void KSVContent::mergeLoadedPackage (QValueList<KSVData>* start,
                                     QValueList<KSVData>* stop)
{
  for (int i = 0; i < ksv::runlevelNumber; ++i)
    {
      merge (start[i], startRL[i]);
      merge (stop[i], stopRL[i]);
    }
}

void KSVContent::merge (QValueList<KSVData>& list, KSVDragList* widget)
{
  typedef QMap<KSVData, bool> LoadMap;
  LoadMap loaded;

  for (QValueListIterator<KSVData> it = list.begin();
       it != list.end();
       ++it)
    {
      KSVItem* exists = widget->match (*it);
      if (exists)
        {
          KSVData oldState = *exists->data();
		  exists->setNumber ((*it).number());
          exists->setLabel ((*it).label());
          exists->setFilename ((*it).filename());
          exists->setPath ((*it).path());
          loaded[*exists->data()] = true;

          if (exists->isChanged())
            {
              emit undoAction (new ChangeAction (widget, &oldState, exists->data()));
            }
        }
      else
        {
          KSVItem* item = new KSVItem (widget, *it);
          item->setNew (true);
          loaded[*item->data()] = true;

          emit undoAction (new AddAction (widget, item->data()));
        }

      kapp->processEvents ();
    }

  QPtrList<KSVItem> deleteList;
  deleteList.setAutoDelete (true);

  for (QListViewItemIterator it (widget); it.current(); ++it)
    {
      KSVItem* item = static_cast<KSVItem*> (it.current());

      if (!loaded[*item->data()])
        {
          deleteList.append (item);

          emit undoAction (new RemoveAction (widget, item->data()));
        }

      kapp->processEvents ();
    }

  widget->sort();
  kapp->processEvents();
}

#include "OldView.moc"
