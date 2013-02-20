/*****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2004 by Stanislav Visnovsky <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */

#include "catmanresource.h"
#include "catalogmanager.h"
#include "catalog.h"
#include "catalogmanagerapp.h"
#include "findinfilesdialog.h"
#include "kbabeldictbox.h"
#include "resources.h"
#include "projectpref.h"
#include "kbprojectmanager.h"
#include "projectwizard.h"
#include "msgfmt.h"
#include "toolaction.h"

#include <qlabel.h>
#include <qpainter.h>

#include <dcopclient.h>
#include <kapplication.h>
#include <kaction.h>
#include <kcmenumngr.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdatatool.h>
#include <kdialogbase.h>
//#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kprogress.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kwin.h>

#include <qfileinfo.h>
#include <qdir.h>
#include <qtimer.h>
#include <qbitmap.h>
#include <qwhatsthis.h>
#include <qheader.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qhbox.h>

using namespace KBabel;

WId CatalogManagerApp::_preferredWindow = 0;

QStringList CatalogManager::_foundFilesList;
QStringList CatalogManager::_toBeSearched;

CatalogManager::CatalogManager(QString configFile )
                 :KMainWindow(0,0)
{
   if ( configFile.isEmpty() )
	configFile = KBabel::ProjectManager::defaultProjectName();
   _configFile = configFile;

   init();
   restoreSettings();
   updateSettings();
}

CatalogManager::~CatalogManager()
{
   saveView();
   saveSettings(_configFile);
   delete config;
}

void CatalogManager::init()
{
    _foundToBeSent = 0;
    _totalFound = 0;
    _foundFilesList.clear();
    _toBeSearched.clear();
    _timerFind = new QTimer( this );
    connect(_timerFind, SIGNAL( timeout() ), this, SLOT(findNextFile()) );
    _searchStopped = false;

   _prefDialog=0;
   _findDialog=0;
   _replaceDialog=0;

   _project = KBabel::ProjectManager::open(_configFile);
   
   if ( _project == NULL )
   {
	KMessageBox::error( this, i18n("Cannot open project file\n%1").arg(_configFile)
	    , i18n("Project File Error"));

	_project = KBabel::ProjectManager::open(KBabel::ProjectManager::defaultProjectName());
   }
   
   connect( _project, SIGNAL (signalCatManSettingsChanged())
	, this, SLOT (updateSettings()));

   QWidget *view = new QWidget(this);
   QVBoxLayout* layout= new QVBoxLayout(view);
   layout->setMargin(0);
   layout->setSpacing(KDialog::spacingHint());

   _catalogManager=new CatalogManagerView(_project, view,"catalog manager");
   layout->addWidget(_catalogManager);
   layout->setStretchFactor(_catalogManager,1);

   connect(this,SIGNAL(settingsChanged(KBabel::CatManSettings))
            ,_catalogManager,SLOT(setSettings(KBabel::CatManSettings)));
   connect(_catalogManager,SIGNAL(openFile(QString,QString))
           ,this,SLOT(openFile(QString,QString)));
   connect(_catalogManager,SIGNAL(openFileInNewWindow(QString,QString))
           ,this,SLOT(openFileInNewWindow(QString,QString)));
   connect(_catalogManager,SIGNAL(openTemplate(QString,QString,QString))
           ,this,SLOT(openTemplate(QString,QString,QString)));
   connect(_catalogManager,SIGNAL(openTemplateInNewWindow(QString,QString,QString))
           ,this,SLOT(openTemplateInNewWindow(QString,QString,QString)));
   connect(_catalogManager,SIGNAL(gotoFileEntry(QString,QString,int))
           ,this,SLOT(openFile(QString,QString,int)));
   connect(_catalogManager, SIGNAL(selectedChanged(uint)),
           this, SLOT(selectedChanged(uint)));

   KWin::setIcons(winId(),BarIcon("catalogmanager",32)
           ,SmallIcon("catalogmanager"));

   QHBoxLayout* hBoxL = new QHBoxLayout(layout);
   _progressLabel = new QLabel(view);
   hBoxL->addWidget(_progressLabel);
   _progressBar=new KProgress(view);
   hBoxL->addWidget(_progressBar);
   hBoxL->setStretchFactor(_progressBar,1);

   _progressLabel->hide();
   _progressBar->hide();

   connect(_catalogManager,SIGNAL(prepareProgressBar(QString,int))
           , this, SLOT(prepareProgressBar(QString,int)));
   connect(_catalogManager,SIGNAL(clearProgressBar())
           , this, SLOT(clearProgressBar()));
   connect(_catalogManager,SIGNAL(progress(int))
           , _progressBar, SLOT(setProgress(int)));
//   connect(_catalogManager, SIGNAL(signalBuildTree(bool))
//	   , this, SLOT(enableMenuForFiles(bool)));
   connect(_catalogManager, SIGNAL(signalBuildTree(bool))
	   , this, SLOT(enableActions(bool)));
   connect(this, SIGNAL(searchStopped())
	   , _catalogManager, SLOT(stopSearch()));
   connect(_catalogManager, SIGNAL(prepareFindProgressBar(int))
	   , this, SLOT(prepareStatusProgressBar(int)));

   setCentralWidget(view);
   resize( 600,300);

   setupStatusBar();
   setupActions();


   QPopupMenu* popup;
   popup = (QPopupMenu*)(factory()->container("rmb_file", this));
   if(popup)
   {
       _catalogManager->setRMBMenuFile(popup);
   }
   popup = (QPopupMenu*)(factory()->container("rmb_dir", this));
   if(popup)
   {
       _catalogManager->setRMBMenuDir(popup);
   }

   connect(_catalogManager, SIGNAL(signalSearchedFile(int))
           , _statusProgressBar, SLOT(advance(int)));

   restoreView();
}

void CatalogManager::setupActions()
{
    KGlobal::iconLoader()->addAppDir("kbabel");

    KAction *action;

    // the file menu
    action = new KAction( i18n("&Open"), CTRL+Key_O, _catalogManager,
                          SLOT(slotOpenFile()),actionCollection(), "open");
    action->setEnabled(false);
    action = new KAction(i18n("&Open Template"),Key_Space,_catalogManager,
                         SLOT(slotOpenTemplate()),actionCollection(), "open_template");
    action->setEnabled(false);
    action = new KAction(i18n("Open in &New Window"),CTRL+SHIFT+Key_O,_catalogManager,
                         SLOT(slotOpenFileInNewWindow()),actionCollection(), "open_new_window");
    action->setEnabled(false);

    action = KStdAction::quit(kapp, SLOT (closeAllWindows()), actionCollection());

    actionMap["open_template"] = NEEDS_POT;

    // the edit menu
    action = new KAction( i18n("Fi&nd in Files..."), CTRL+Key_F, this,
                          SLOT(find()), actionCollection(), "find_in_files");
    action->setEnabled(false);
    action = new KAction( i18n("Re&place in Files..."), CTRL+Key_R, this,
                          SLOT(replace()), actionCollection(), "replace_in_files");
    action->setEnabled(false);
    action = new KAction( i18n("&Stop Searching"), "stop", Key_Escape, this,
                          SLOT(stopSearching()), actionCollection(), "stop_search");
    action->setEnabled(false);
    action = new KAction( i18n("&Reload"), "reload", KStdAccel::reload(), _catalogManager,
                          SLOT(updateCurrent()), actionCollection(), "reload");
    action->setEnabled(false);

    // the marking menu
    action = new KAction( i18n("&Toggle Marking"), CTRL+Key_M, _catalogManager,
                          SLOT(toggleMark()), actionCollection(), "toggle_marking");
    action->setEnabled(false);
    action = new KAction( i18n("Remove Marking"), 0, _catalogManager,
                          SLOT(slotClearMarksInDir()), actionCollection(), "remove_marking");
    action->setEnabled(false);
    action = new KAction( i18n("Toggle All Markings"), 0, _catalogManager,
                          SLOT(toggleAllMarks()), actionCollection(), "toggle_all_marking");
    action->setEnabled(false);
    action = new KAction( i18n("Remove All Markings"), 0, _catalogManager,
                          SLOT(clearAllMarks()), actionCollection(), "remove_all_marking");
    action->setEnabled(false);
    action = new KAction( i18n("Mark Modified Files"), 0, _catalogManager,
                          SLOT(markModifiedFiles()), actionCollection(), "mark_modified_files");
    // fixme to enabling this when loading is done using updateFinished() signal
    action->setEnabled(true);
    action = new KAction( i18n("&Load Markings..."), 0, _catalogManager,
                          SLOT(loadMarks()), actionCollection(), "load_marking");
    action->setEnabled(false);
    action = new KAction( i18n("&Save Markings..."), 0, _catalogManager,
                          SLOT(saveMarks()), actionCollection(), "save_marking");
    action->setEnabled(false);
    (void)new KAction(i18n("&Mark Files..."), 0, _catalogManager,
                      SLOT(slotMarkPattern()), actionCollection(), "mark_pattern");
    (void)new KAction(i18n("&Unmark Files..."), 0, _catalogManager,
                      SLOT(slotUnmarkPattern()), actionCollection(), "unmark_pattern");

    actionMap["remove_marking"]     = NEEDS_MARK;
    actionMap["remove_all_marking"] = NEEDS_MARK;
    actionMap["mark_pattern"]       = NEEDS_DIR;
    actionMap["unmark_pattern"]     = NEEDS_DIR | NEEDS_MARK;

    // go menu
    action = new KAction(i18n("Nex&t Untranslated"), "nextuntranslated", ALT+Key_Next,
                         _catalogManager, SLOT(gotoNextUntranslated()),actionCollection(), "go_next_untrans");
    action->setEnabled(false);
    action = new KAction(i18n("Prev&ious Untranslated"), "prevuntranslated", ALT+Key_Prior,
                         _catalogManager, SLOT(gotoPreviousUntranslated()),actionCollection(), "go_prev_untrans");
    action->setEnabled(false);
    action = new KAction(i18n("Ne&xt Fuzzy"), "nextfuzzy", CTRL+Key_Next,
                         _catalogManager, SLOT(gotoNextFuzzy()),actionCollection(), "go_next_fuzzy");
    action->setEnabled(false);
    action = new KAction(i18n("Pre&vious Fuzzy"), "prevfuzzy", CTRL+Key_Prior,
                         _catalogManager, SLOT(gotoPreviousFuzzy()),actionCollection(), "go_prev_fuzzy");
    action->setEnabled(false);
    action = new KAction(i18n("N&ext Fuzzy or Untranslated"), "nextfuzzyuntrans", CTRL+SHIFT+Key_Next,
                         _catalogManager, SLOT(gotoNextFuzzyOrUntranslated()),actionCollection(), "go_next_fuzzyUntr");
    action->setEnabled(false);
    action = new KAction(i18n("P&revious Fuzzy or Untranslated"), "prevfuzzyuntrans", CTRL+SHIFT+Key_Prior,
                         _catalogManager, SLOT(gotoPreviousFuzzyOrUntranslated()),actionCollection(), "go_prev_fuzzyUntr");
    action->setEnabled(false);

    action = new KAction(i18n("Next Err&or"), "nexterror", ALT+SHIFT+Key_Next,
                         _catalogManager, SLOT(gotoNextError()),actionCollection(), "go_next_error");
    action->setEnabled(false);
    action = new KAction(i18n("Previo&us Error"), "preverror", ALT+SHIFT+Key_Prior,
                         _catalogManager, SLOT(gotoPreviousError()),actionCollection(), "go_prev_error");
    action->setEnabled(false);
    action = new KAction(i18n("Next Te&mplate Only"), "nexttemplate", CTRL+Key_Down,
                         _catalogManager, SLOT(gotoNextTemplate()),actionCollection(), "go_next_template");
    action->setEnabled(false);
    action = new KAction(i18n("Previous Temp&late Only"), "prevtemplate", CTRL+Key_Up,
                         _catalogManager, SLOT(gotoPreviousTemplate()),actionCollection(), "go_prev_template");
    action->setEnabled(false);
    action = new KAction(i18n("Next Tran&slation Exists"), "nextpo", ALT+Key_Down,
                         _catalogManager, SLOT(gotoNextPo()),actionCollection(), "go_next_po");
    action->setEnabled(false);
    action = new KAction(i18n("Previous Transl&ation Exists"), "prevpo", ALT+Key_Up,
                         _catalogManager, SLOT(gotoPreviousPo()),actionCollection(), "go_prev_po");
    action->setEnabled(false);

    action = new KAction(i18n("Previous Marke&d"), "prevmarked", SHIFT+Key_Up,
                         _catalogManager, SLOT(gotoPreviousMarked()),actionCollection(), "go_prev_marked");
    action->setEnabled(false);
    action = new KAction(i18n("Next &Marked"), "nextmarked", SHIFT+Key_Down,
                         _catalogManager, SLOT(gotoNextMarked()),actionCollection(), "go_next_marked");
    action->setEnabled(false);

    // project menu
    // the project menu
   action = new KAction(i18n("&New..."), "filenew"
           , this, SLOT(projectNew()),actionCollection()
           ,"project_new");

   action = new KAction(i18n("&Open..."), "fileopen"
           , this, SLOT(projectOpen()),actionCollection()
           ,"project_open");

   action = new KAction(i18n("C&lose"), "fileclose"
           , this, SLOT(projectClose()),actionCollection()
           ,"project_close");

   action->setEnabled (_project->filename() != KBabel::ProjectManager::defaultProjectName() );

   action = new KAction(i18n("&Configure..."), "configure"
           , this, SLOT(projectConfigure()),actionCollection()
           ,"project_settings");

    // tools menu
    action = new KAction( i18n("&Statistics"), "statistics",  CTRL+Key_S,
                          _catalogManager, SLOT(statistics()), actionCollection(), "statistics");
    action->setEnabled(false);
    action = new KAction( i18n("S&tatistics in Marked"), "statistics",  CTRL+ALT+Key_S,
                          _catalogManager, SLOT(markedStatistics()), actionCollection(), "statistics_marked");
    action->setEnabled(false);
    action = new KAction( i18n("Check S&yntax"), "syntax", CTRL+Key_Y,
                          _catalogManager, SLOT(checkSyntax()), actionCollection(), "syntax");
    action->setEnabled(false);
    action = new KAction( i18n("S&pell Check"), "spellcheck",  CTRL+Key_I,
                          this, SLOT(spellcheck()), actionCollection(), "spellcheck");
    action->setEnabled(false);
    action = new KAction( i18n("Spell Check in &Marked"), "spellcheck", CTRL+ALT+Key_I,
                          this, SLOT(markedSpellcheck()), actionCollection(), "spellcheck_marked");
    action->setEnabled(false);
    action = new KAction( i18n("&Rough Translation"),  CTRL+Key_T,
                          _catalogManager, SLOT(roughTranslation()), actionCollection(), "rough_translation");
    action->setEnabled(false);
    action = new KAction( i18n("Rough Translation in M&arked"), CTRL+ALT+Key_T,
                          _catalogManager, SLOT(markedRoughTranslation()), actionCollection(), "rough_translation_marked");
    action->setEnabled(false);
    action = new KAction( i18n("Mai&l"), "mail_send", CTRL+Key_A,
                          _catalogManager, SLOT(mailFiles()), actionCollection(), "mail_file");
    action->setEnabled(false);
    action = new KAction( i18n("Mail Mar&ked"), "mail_send", CTRL+ALT+Key_A,
                          _catalogManager, SLOT(mailMarkedFiles()), actionCollection(),	"mail_file_marked");
    action->setEnabled(false);

    action = new KAction( i18n("&Pack"), "tar", CTRL+Key_B,
                         _catalogManager, SLOT(packageFiles()), actionCollection(), "package_file");
    action = new KAction( i18n("Pack &Marked"), "tar", CTRL+ALT+Key_B, _catalogManager, SLOT(packageMarkedFiles()), actionCollection(), "package_file_marked");
    action->setEnabled(false);

    actionMap["statistics_marked"]        = NEEDS_DIR | NEEDS_MARK;
    actionMap["syntax"]                   = NEEDS_PO;
    actionMap["spellcheck"]               = NEEDS_PO;
    actionMap["spellcheck_marked"]        = NEEDS_PO | NEEDS_MARK;
    actionMap["rough_translation_marked"] = NEEDS_MARK;
    actionMap["mail_file"]                = NEEDS_PO;
    actionMap["mail_file_marked"]         = NEEDS_PO | NEEDS_MARK;
    actionMap["package_file_marked"]      = NEEDS_PO | NEEDS_MARK;

    // dynamic tools
    QValueList<KDataToolInfo> tools = ToolAction::validationTools();

    QPtrList<KAction> actions = ToolAction::dataToolActionList(
	tools, _catalogManager, SLOT(validateUsingTool( const KDataToolInfo &, const QString& ))
	,"validate", false, actionCollection() );

    KActionMenu* m_menu = new KActionMenu(i18n("&Validation"), actionCollection(),
          "dynamic_validation");

    KAction*ac;

    for(ac = actions.first(); ac ; ac = actions.next() )
    {
	m_menu->insert(ac);
    }

    actions = ToolAction::dataToolActionList(
	tools, _catalogManager, SLOT(validateMarkedUsingTool( const KDataToolInfo &, const QString& ))
	,"validate", false, actionCollection(), "marked_" );
    m_menu = new KActionMenu(i18n("V&alidation Marked"), actionCollection(),
          "dynamic_validation_marked");

    for( ac = actions.first(); ac ; ac = actions.next() )
    {
	m_menu->insert(ac);
    }

    actionMap["dynamic_validation"]        = NEEDS_PO;
    actionMap["dynamic_validation_marked"] = NEEDS_PO | NEEDS_MARK;

    // CVS submenu
    // Actions for PO files
    (void)new KAction( i18n( "Update" ), "down", 0, _catalogManager,
      SLOT( cvsUpdate( ) ), actionCollection( ), "cvs_update" );
    (void)new KAction( i18n( "Update Marked" ), 0, _catalogManager,
      SLOT( cvsUpdateMarked( ) ), actionCollection( ), "cvs_update_marked" );
    (void)new KAction( i18n( "Commit" ), "up", 0, _catalogManager,
      SLOT( cvsCommit( ) ), actionCollection( ), "cvs_commit" );
    (void)new KAction( i18n( "Commit Marked" ), 0, _catalogManager,
      SLOT( cvsCommitMarked( ) ), actionCollection( ), "cvs_commit_marked" );
    (void)new KAction( i18n( "Status" ), 0, _catalogManager,
      SLOT( cvsStatus( ) ), actionCollection( ), "cvs_status" );
    (void)new KAction( i18n( "Status for Marked" ), 0, _catalogManager,
      SLOT( cvsStatusMarked( ) ), actionCollection( ), "cvs_status_marked" );
    (void)new KAction( i18n( "Show Diff" ), 0, _catalogManager,
      SLOT( cvsDiff( ) ), actionCollection( ), "cvs_diff" );

    // CVS
    actionMap["cvs_update"]        = NEEDS_PO | NEEDS_PO_CVS;
    actionMap["cvs_update_marked"] = NEEDS_PO | NEEDS_PO_CVS | NEEDS_MARK;
    actionMap["cvs_commit"]        = NEEDS_PO | NEEDS_PO_CVS;
    actionMap["cvs_commit_marked"] = NEEDS_PO | NEEDS_PO_CVS | NEEDS_MARK;
    actionMap["cvs_status"]        = NEEDS_PO | NEEDS_PO_CVS;
    actionMap["cvs_status_marked"] = NEEDS_PO | NEEDS_PO_CVS | NEEDS_MARK;
    actionMap["cvs_diff"]          = NEEDS_PO | NEEDS_PO_CVS;

    // SVN submenu
    // Actions for PO files
    (void)new KAction( i18n( "Update" ), "down", 0, _catalogManager,
    SLOT( svnUpdate( ) ), actionCollection( ), "svn_update" );
    (void)new KAction( i18n( "Update Marked" ), 0, _catalogManager,
    SLOT( svnUpdateMarked( ) ), actionCollection( ), "svn_update_marked" );
    (void)new KAction( i18n( "Commit" ), "up", 0, _catalogManager,
    SLOT( svnCommit( ) ), actionCollection( ), "svn_commit" );
    (void)new KAction( i18n( "Commit Marked" ), 0, _catalogManager,
    SLOT( svnCommitMarked( ) ), actionCollection( ), "svn_commit_marked" );
    (void)new KAction( i18n( "Status (Local)" ), 0, _catalogManager,
    SLOT( svnStatusLocal() ), actionCollection( ), "svn_status_local" );
    (void)new KAction( i18n( "Status (Local) for Marked" ), 0, _catalogManager,
    SLOT( svnStatusLocalMarked() ), actionCollection( ), "svn_status_local_marked" );
    (void)new KAction( i18n( "Status (Remote)" ), 0, _catalogManager,
    SLOT( svnStatusRemote() ), actionCollection( ), "svn_status_remote" );
    (void)new KAction( i18n( "Status (Remote) for Marked" ), 0, _catalogManager,
    SLOT( svnStatusRemoteMarked() ), actionCollection( ), "svn_status_remote_marked" );
    (void)new KAction( i18n( "Show Diff" ), 0, _catalogManager,
    SLOT( svnDiff( ) ), actionCollection( ), "svn_diff" );
    (void)new KAction( i18n( "Show Information" ), 0, _catalogManager,
    SLOT( svnInfo() ), actionCollection( ), "svn_info" );
    (void)new KAction( i18n( "Show Information for Marked" ), 0, _catalogManager,
    SLOT( svnInfoMarked() ), actionCollection( ), "svn_info_marked" );

    // SVN
    actionMap["svn_update"]        = NEEDS_PO | NEEDS_PO_SVN;
    actionMap["svn_update_marked"] = NEEDS_PO | NEEDS_PO_SVN | NEEDS_MARK;
    actionMap["svn_commit"]        = NEEDS_PO | NEEDS_PO_SVN;
    actionMap["svn_commit_marked"] = NEEDS_PO | NEEDS_PO_SVN | NEEDS_MARK;
    actionMap["svn_status_local"]        = NEEDS_PO | NEEDS_PO_SVN;
    actionMap["svn_status_local_marked"] = NEEDS_PO | NEEDS_PO_SVN | NEEDS_MARK;
    actionMap["svn_status_remote"]        = NEEDS_PO | NEEDS_PO_SVN;
    actionMap["svn_status_remote_marked"] = NEEDS_PO | NEEDS_PO_SVN | NEEDS_MARK;
    actionMap["svn_diff"]          = NEEDS_PO | NEEDS_PO_SVN;
    actionMap["svn_info"]          = NEEDS_PO | NEEDS_PO_SVN;
    actionMap["svn_info_marked"]   = NEEDS_PO | NEEDS_PO_SVN | NEEDS_MARK;

    // CVS Actions for POT files
    (void)new KAction( i18n( "Update Templates" ), 0, _catalogManager,
      SLOT( cvsUpdateTemplate( ) ), actionCollection( ), "cvs_update_template" );
    (void)new KAction( i18n( "Update Marked Templates" ), 0, _catalogManager,
      SLOT( cvsUpdateMarkedTemplate( ) ), actionCollection( ), "cvs_update_marked_template" );
    (void)new KAction( i18n( "Commit Templates" ), 0, _catalogManager,
      SLOT( cvsCommitTemplate( ) ), actionCollection( ), "cvs_commit_template" );
    (void)new KAction( i18n( "Commit Marked Templates" ), 0, _catalogManager,
      SLOT( cvsCommitMarkedTemplate( ) ), actionCollection( ), "cvs_commit_marked_template" );

    actionMap["cvs_update_template"]        = NEEDS_POT | NEEDS_POT_CVS;
    actionMap["cvs_update_marked_template"] = NEEDS_POT | NEEDS_POT_CVS | NEEDS_MARK;
    actionMap["cvs_commit_template"]        = NEEDS_POT | NEEDS_POT_CVS;
    actionMap["cvs_commit_marked_template"] = NEEDS_POT | NEEDS_POT_CVS | NEEDS_MARK;

    // SVN Actions for POT files
    (void)new KAction( i18n( "Update Templates" ), 0, _catalogManager,
    SLOT( svnUpdateTemplate( ) ), actionCollection( ), "svn_update_template" );
    (void)new KAction( i18n( "Update Marked Templates" ), 0, _catalogManager,
    SLOT( svnUpdateMarkedTemplate( ) ), actionCollection( ), "svn_update_marked_template" );
    (void)new KAction( i18n( "Commit Templates" ), 0, _catalogManager,
    SLOT( svnCommitTemplate( ) ), actionCollection( ), "svn_commit_template" );
    (void)new KAction( i18n( "Commit Marked Templates" ), 0, _catalogManager,
    SLOT( svnCommitMarkedTemplate( ) ), actionCollection( ), "svn_commit_marked_template" );

    actionMap["svn_update_template"]        = NEEDS_POT | NEEDS_POT_SVN;
    actionMap["svn_update_marked_template"] = NEEDS_POT | NEEDS_POT_SVN | NEEDS_MARK;
    actionMap["svn_commit_template"]        = NEEDS_POT | NEEDS_POT_SVN;
    actionMap["svn_commit_marked_template"] = NEEDS_POT | NEEDS_POT_SVN | NEEDS_MARK;

    // settings menu
    // FIXME: KStdAction::preferences(this, SLOT( optionsPreferences()), actionCollection());

    createStandardStatusBarAction();

    setStandardToolBarMenuEnabled ( true );

    // commands menus
    KActionMenu* actionMenu=new KActionMenu(i18n("Commands"), 0,
                                            actionCollection(), "dir_commands");
    _catalogManager->setDirCommandsMenu( actionMenu->popupMenu());

    actionMenu=new KActionMenu(i18n("Commands"), 0,
                               actionCollection(), "file_commands");
    _catalogManager->setFileCommandsMenu( actionMenu->popupMenu());

    action = new KAction(i18n("&Delete"),Key_Delete,_catalogManager,SLOT(slotDeleteFile()),actionCollection(), "delete");
    action->setEnabled(false);

#if KDE_IS_VERSION( 3, 2, 90 )
    setupGUI();
#else
    createGUI();
#endif
}

void CatalogManager::setupStatusBar()
{
    _foundLabel = new QLabel( "          ", statusBar());
    statusBar()->addWidget(_foundLabel,0);

    QHBox* progressBox = new QHBox(statusBar(), "progressBox" );
    progressBox->setSpacing(2);
    _statusProgressLabel = new QLabel( "", progressBox );
    _statusProgressBar = new KProgress( progressBox, "progressBar");
    _statusProgressBar->hide();

    statusBar()->addWidget(progressBox,1);
    statusBar()->setMinimumHeight(_statusProgressBar->sizeHint().height());

    QWhatsThis::add(statusBar(),
	i18n("<qt><p><b>Statusbar</b></p>\n"
         "<p>The statusbar displays information about progress of"
         " the current find or replace operation. The first number in <b>Found:</b>"
         " displays the number of files with an occurrence of the searched text not"
         " yet shown in the KBabel window. The second shows the total number of files"
         " containing the searched text found so far.</p></qt>"));
}

void CatalogManager::enableMenuForFiles(bool enable)
{
    stateChanged( "treeBuilt", enable ? StateNoReverse: StateReverse );
}

void CatalogManager::selectedChanged(uint actionValue)
{
  QMap<QString,uint>::Iterator it;
  for (it = actionMap.begin( ); it != actionMap.end( ); ++it) {
    KAction * action = actionCollection()->action(it.key( ).latin1( ));
    if (action) action->setEnabled((actionValue & it.data( )) == it.data( ));
  }
}

CatManSettings CatalogManager::settings() const
{
    return _catalogManager->settings();
}

void CatalogManager::updateSettings()
{
    _settings = _project->catManSettings();
    _catalogManager->setSettings(_settings);
   _openNewWindow=_settings.openWindow;
}

void CatalogManager::saveSettings( QString configFile )
{
    _settings = _catalogManager->settings(); // restore settings from the view

    _project->setSettings( _settings );

    config = new KConfig(configFile);

    _catalogManager->saveView(config);

    config->sync();
}

void CatalogManager::restoreSettings()
{
    _settings = _project->catManSettings();
    _openNewWindow=_settings.openWindow;
    _catalogManager->restoreView(_project->config());
}

void CatalogManager::setPreferredWindow(WId window)
{
    _preferredWindow = window;
    kdDebug(KBABEL_CATMAN) << "setPrefereedWindow set to :" << _preferredWindow << endl;
}

void CatalogManager::updateFile(QString fileWithPath)
{
    _catalogManager->updateFile(fileWithPath,true); //force update
}

void CatalogManager::updateAfterSave(QString fileWithPath, PoInfo &info)
{
    _catalogManager->updateAfterSave(fileWithPath, info);
}

CatalogManagerView *CatalogManager::view()
{
    return _catalogManager;
}

void CatalogManager::openFile(QString filename, QString package)
{
    DCOPClient * client = kapp->dcopClient();

    if( startKBabel() )
    {

        QByteArray data;
        QCString url = filename.local8Bit();
        QDataStream arg(data, IO_WriteOnly);
        arg << url;
        arg << package.utf8();
        arg << CatalogManagerApp::_preferredWindow;
        arg << ( _openNewWindow ? 1 : 0 );

        kdDebug(KBABEL_CATMAN) << "Open file with project " << _configFile << endl;

        QCString callfunc="openURL(QCString, QCString, WId,int)";
        if(_configFile != "kbabelrc" )
        {
            arg << _configFile.utf8();
            callfunc="openURL(QCString, QCString, WId,int,QCString)";
        }

        kdDebug(KBABEL_CATMAN) << callfunc << endl;
	
	// update the user timestamp for KBabel to get it a focus
	kapp->updateRemoteUserTimestamp ("kbabel");

        if( !client->send("kbabel","KBabelIFace", callfunc, data) )
            KMessageBox::error(this, i18n("Cannot send a message to KBabel.\n"
                                          "Please check your installation of KDE."));
    }
}

void CatalogManager::openFile(QString filename, QString package, int msgid)
{
    DCOPClient * client = kapp->dcopClient();

    if( startKBabel() )
    {
        QByteArray data;
        QCString url = filename.local8Bit();
        QDataStream arg(data, IO_WriteOnly);
        arg << url;
        arg << package.utf8();
	arg << msgid;

        kdDebug(KBABEL_CATMAN) << "Open file with project " << _configFile << endl;

        QCString callfunc="gotoFileEntry(QCString, QCString, int)";
        if(_configFile != "kbabelrc" )
        {
            arg << _configFile.utf8();
            callfunc="gotoFileEntry(QCString, QCString,int,QCString)";
        }

        kdDebug(KBABEL_CATMAN) << callfunc << endl;

	// update the user timestamp for KBabel to get it a focus
	kapp->updateRemoteUserTimestamp ("kbabel");

        if( !client->send("kbabel","KBabelIFace", callfunc, data) )
            KMessageBox::error(this, i18n("Cannot send a message to KBabel.\n"
                                          "Please check your installation of KDE."));
    }
}

void CatalogManager::openFileInNewWindow(QString filename, QString package)
{
    DCOPClient * client = kapp->dcopClient();

    if( startKBabel() )
    {

        QByteArray data;
        QCString url = filename.local8Bit();
        QDataStream arg(data, IO_WriteOnly);
        arg << url;
        arg << package.utf8();
        arg << CatalogManagerApp::_preferredWindow;
        arg << ((int)1);

        QCString callfunc="openURL(QCString, QCString, WId,int)";
        if(_configFile != "kbabelrc" )
        {
            arg << _configFile.utf8();
            callfunc="openURL(QCString, QCString, WId,int,QCString)";
        }

	// update the user timestamp for KBabel to get it a focus
	kapp->updateRemoteUserTimestamp ("kbabel");

        if( !client->send("kbabel","KBabelIFace", callfunc, data) )
            KMessageBox::error(this, i18n("Cannot send a message to KBabel.\n"
                                          "Please check your installation of KDE."));
    }
}

void CatalogManager::openTemplate(QString openFilename,QString saveFilename,QString package)
{
    DCOPClient * client = kapp->dcopClient();

    if( startKBabel() ) {
        QByteArray data;
        QCString url = openFilename.local8Bit();
        QDataStream arg(data, IO_WriteOnly);
        arg << url;
        url = saveFilename.utf8();
        arg << url;
        arg << package.utf8();
        arg << (_openNewWindow ? 1 : 0 );

        QCString callfunc="openTemplate(QCString,QCString,QCString,int)";
        if(_configFile != "kbabelrc" )
        {
            arg << _configFile.utf8();
            callfunc="openTemplate(QCString,QCString,QCString,int,QCString)";
        }

	// update the user timestamp for KBabel to get it a focus
	kapp->updateRemoteUserTimestamp ("kbabel");

        if( !client->send("kbabel","KBabelIFace", callfunc, data) )
            KMessageBox::error(this, i18n("Cannot send a message to KBabel.\n"
                                          "Please check your installation of KDE."));
    }
}

void CatalogManager::openTemplateInNewWindow(QString openFilename,QString saveFilename,QString package)
{
    DCOPClient * client = kapp->dcopClient();

    if( startKBabel() ) {
        QByteArray data;
        QCString url = openFilename.local8Bit();
        QDataStream arg(data, IO_WriteOnly);
        arg << url;
        url = saveFilename.utf8();
        arg << url;
        arg << package.utf8();
        arg << ((int)1);

        QCString callfunc="openTemplate(QCString,QCString,QCString,int)";
        if(_configFile != "kbabelrc" )
        {
            arg << _configFile.utf8();
            callfunc="openTemplate(QCString,QCString,QCString,int,QCString)";
        }

	// update the user timestamp for KBabel to get it a focus
	kapp->updateRemoteUserTimestamp ("kbabel");

        if( !client->send("kbabel","KBabelIFace", callfunc, data) )
            KMessageBox::error(this, i18n("Cannot send a message to KBabel.\n"
                                          "Please check your installation of KDE."));
    }
}

void CatalogManager::spellcheck()
{
    DCOPClient * client = kapp->dcopClient();

    QStringList fileList = _catalogManager->current();

    if( startKBabel() ) {
        QByteArray data;
        QDataStream arg(data, IO_WriteOnly);
        arg << fileList;

	// update the user timestamp for KBabel to get it a focus
	kapp->updateRemoteUserTimestamp ("kbabel");

        if( !client->send("kbabel","KBabelIFace", "spellcheck(QStringList)", data) )
            KMessageBox::error(this, i18n("Cannot send a message to KBabel.\n"
                                          "Please check your installation of KDE."));
    }
}

void CatalogManager::markedSpellcheck()
{
    DCOPClient * client = kapp->dcopClient();

    QStringList fileList = _catalogManager->marked();

    if( startKBabel() ) {
        QByteArray data;
        QDataStream arg(data, IO_WriteOnly);
        arg << fileList;

	// update the user timestamp for KBabel to get it a focus
	kapp->updateRemoteUserTimestamp ("kbabel");

        if( !client->send("kbabel","KBabelIFace", "spellcheck(QStringList)", data) )
            KMessageBox::error(this, i18n("Cannot send a message to KBabel.\n"
                                          "Please check your installation of KDE."));
    }
}

bool CatalogManager::startKBabel()
{
    QCString service;
    QString result;

    DCOPClient * client = kapp->dcopClient();

    // find out, if there is a running kbabel
    QCStringList apps = client->registeredApplications();
    for( QCStringList::Iterator it = apps.begin() ; it != apps.end() ; ++it )
    {
        QString clientID = *it;
        if( clientID=="kbabel" )
        {
            service = *it;
            break;
        }
    }

    // if there is no running kbabel, start one
    if( service.isEmpty() )
    {
        QString app = "kbabel";
        QString url = "";
        if( kapp->startServiceByDesktopName(app,url, &result, &service))
        {
            KMessageBox::error( this, i18n("Unable to use KLauncher to start KBabel.\n"
                                           "You should check the installation of KDE.\n"
                                           "Please start KBabel manually."));
            return false;
        } else sleep(1);
    }

    return true;
}


void CatalogManager::prepareProgressBar(QString msg, int max)
{
   _progressBar->setTotalSteps(max);
   _progressBar->setProgress(0);
   _progressLabel->setText(msg);

   _progressBar->show();
   _progressLabel->show();
}

void CatalogManager::clearProgressBar()
{
   _progressBar->setProgress(0);

   _progressBar->hide();
   _progressLabel->hide();
}

void CatalogManager::prepareStatusProgressBar(QString msg, int max)
{
   _totalFound = 0;
   _foundToBeSent = 0;
   _statusProgressBar->setTotalSteps(max);
   _statusProgressLabel->setText(msg);
   _foundLabel->setText( i18n("Found: 0/0") );

   _statusProgressBar->show();
   _statusProgressLabel->show();
}

void CatalogManager::prepareStatusProgressBar(int max)
{
   _statusProgressBar->setTotalSteps(max);
}

void CatalogManager::clearStatusProgressBar()
{
   _statusProgressBar->setValue(0);

   _statusProgressBar->hide();
   _statusProgressLabel->hide();
   _foundLabel->setText("          ");
}

void CatalogManager::setNumberOfFound(int toBeSent, int total)
{
    _foundLabel->setText(i18n("Found: %1/%2").arg(toBeSent).arg(total));
}

void CatalogManager::decreaseNumberOfFound()
{
    if( _foundToBeSent > 0 ) {
        _foundToBeSent--;
        setNumberOfFound( _foundToBeSent, _totalFound );
    }
}

void CatalogManager::slotHelp()
{
   kapp->invokeHelp("CATALOGMANAGER","kbabel");
}

void CatalogManager::find()
{
    if( !_findDialog ) _findDialog = new FindInFilesDialog(false,this);

    if( _findDialog->exec("") == QDialog::Accepted )
    {
        _timerFind->stop();
        _searchStopped = false;
        _catalogManager->stop(false); // surely we are not in process of quitting, since there is no window and user cannot invoke Find
        prepareStatusProgressBar(i18n("Searching"),1); // just show the progress bar

        // enable stop action to stop searching
        KAction *action = (KAction*)actionCollection()->action("stop_search");
        action->setEnabled(true);

        _findOptions = _findDialog->findOpts();

        // get from options the information for ignoring text parts
        _findOptions.contextInfo = QRegExp( _project->miscSettings().contextInfo );
        _findOptions.accelMarker = _project->miscSettings().accelMarker;

        _foundFilesList.clear();
        kdDebug(KBABEL_CATMAN) << "Calling catalogmanagerview::find" << endl;
        QString url = _catalogManager->find(_findOptions, _toBeSearched );

        if( _catalogManager->isStopped() ) return;
        if( !url.isEmpty() )
        {
            if( startKBabel() )
            {
                QCString funcCall("findInFile(QCString,QCString,QString,int,int,int,int,int,int,int,int,int,int)");
                DCOPClient *client = kapp->dcopClient();
                QByteArray data;
                QDataStream arg(data, IO_WriteOnly);
                arg << client->appId();
                arg << url.utf8();
                arg << _findOptions.findStr;
                arg << (_findOptions.caseSensitive ? 1 : 0);
                arg << (_findOptions.wholeWords ? 1 : 0);
                arg << (_findOptions.isRegExp ? 1 : 0);
                arg << (_findOptions.inMsgid ? 1 : 0);
                arg << (_findOptions.inMsgstr ? 1 : 0);
                arg << (_findOptions.inComment ? 1 : 0);
                arg << (_findOptions.ignoreAccelMarker ? 1 : 0);
                arg << (_findOptions.ignoreContextInfo ? 1 : 0);
                arg << (_findOptions.askForNextFile ? 1 : 0);
                arg << (_findOptions.askForSave ? 1 : 0);
		if(_configFile != "kbabelrc" ) {
        	   arg << _configFile.utf8();
                   funcCall="findInFile(QCString,QCString,QString,int,int,int,int,int,int,int,int,int,int,QCString)";
                }
		kdDebug(KBABEL) << "DCOP: " << QString(data.data()) << endl;
                if( !client->send("kbabel","KBabelIFace",
                                  funcCall, data)
                    ) {
                    KMessageBox::error( this, i18n("DCOP communication with KBabel failed."), i18n("DCOP Communication Error"));
                    stopSearching();
                    return;
                }

                if( !_toBeSearched.isEmpty() )
                {
                    _totalFound = 1;
                    _foundToBeSent = 0;
                    setNumberOfFound( 0, 1 );	// one found, but already sent
                    _timerFind->start(100,true);
                } else stopSearching();
            }
            else
            {
                KMessageBox::error( this, i18n("KBabel cannot be started."), i18n("Cannot Start KBabel"));
                stopSearching();
            }

        }
        else
        {
            if( !_searchStopped) KMessageBox::information(this, i18n("Search string not found!"));
            stopSearching();
        }
    }
}

void CatalogManager::replace()
{
    if( !_replaceDialog ) _replaceDialog = new FindInFilesDialog(true,this);


    if( _replaceDialog->exec("") == QDialog::Accepted )
    {
        _timerFind->stop();
        _searchStopped = false;
        _catalogManager->stop(false); // surely we are not in process of quitting, since there is no window and user cannot invoke Find
        prepareStatusProgressBar(i18n("Searching"),1); // just show the progress bar

        // enable stop action to stop searching
        KAction *action = (KAction*)actionCollection()->action("stop_search");
        action->setEnabled(true);

        ReplaceOptions options = _replaceDialog->replaceOpts();

        _findOptions = options;

        // get from options the information for ignoring text parts
        options.contextInfo = QRegExp( _project->miscSettings().contextInfo );
        options.accelMarker = _project->miscSettings().accelMarker;

        _foundFilesList.clear();
        QString url = _catalogManager->find(options, _toBeSearched );

        if( _catalogManager->isStopped() ) return;
        if( !url.isEmpty() )
        {
            if( startKBabel() )
            {
		QCString funcCall("replaceInFile(QCString,QCString,QString,QString,int,int,int,int,int,int,int,int,int,int,int)");
                DCOPClient *client = kapp->dcopClient();
                QByteArray data;
                QDataStream arg(data, IO_WriteOnly);
                
                arg << client->appId();
                arg << url.utf8();
                arg << options.findStr;
                arg << options.replaceStr;
                arg << (options.caseSensitive ? 1 : 0);
                arg << (options.wholeWords ? 1 : 0);
                arg << (options.isRegExp ? 1 : 0);
                arg << (options.inMsgid ? 1 : 0);
                arg << (options.inMsgstr ? 1 : 0);
                arg << (options.inComment ? 1 : 0);
                arg << (options.ignoreAccelMarker ? 1 : 0);
                arg << (options.ignoreContextInfo ? 1 : 0);
                arg << (options.ask ? 1 : 0);
                arg << (options.askForNextFile ? 1 : 0);
                arg << (options.askForSave ? 1 : 0);
		if(_configFile != "kbabelrc" ) {
        	   arg << _configFile.utf8();
                   funcCall="replaceInFile(QCString,QCString,QString,QString,int,int,int,int,int,int,int,int,int,int,int,QCString)";
                }
                if( !client->send("kbabel","KBabelIFace",
                                  funcCall, data)
                    ) {
                    KMessageBox::error( this, i18n("DCOP communication with KBabel failed."), i18n("DCOP Communication Error"));
                    stopSearching();
                    return;
                }

                if( !_toBeSearched.isEmpty() )
                {
                    _totalFound = 1;
                    setNumberOfFound( 0, 1 );
                    _timerFind->start(100,true);
                } else stopSearching();
            }
            else
            {
                KMessageBox::error( this, i18n("KBabel cannot be started."), i18n("Cannot Start KBabel"));
                stopSearching(); // update window
            }

        }
        else
        {
            if( !_searchStopped ) KMessageBox::information(this, i18n("Search string not found!"));
            stopSearching(); // update window
        }
    }
}

void CatalogManager::findNextFile()
{
    _timerFind->stop(); // stop the timer for lookup time
    if(_toBeSearched.empty() )
    {
        stopSearching();
        return;
    }
    QString file = _toBeSearched.first();
    _toBeSearched.pop_front();
    if( PoInfo::findInFile( file, _findOptions ) )
    {
        _foundFilesList.append(file);
        _totalFound++;
        _foundToBeSent++;
        setNumberOfFound(_foundToBeSent,_totalFound);
    }
    _statusProgressBar->advance(1);
    if( !_toBeSearched.empty() )
        _timerFind->start(100,true); // if there is more files to be searched, start the timer again
    else
        stopSearching();
}

void CatalogManager::stopSearching()
{
    _searchStopped = true;
    emit searchStopped();
    // clear the list of files to be searched
    _toBeSearched.clear();

    // fake that we are over (fake, because findNextFile can still be running for the last file
    clearStatusProgressBar(); // clear the status bar, we are finished
    // disable stop action as well
    KAction *action = (KAction*)actionCollection()->action("stop_search");
    action->setEnabled(false);
}

void CatalogManager::optionsPreferences()
{
   if(!_prefDialog)
   {
      _prefDialog = new KBabel::ProjectDialog(_project);
   }

   _prefDialog->exec();
}

void CatalogManager::newToolbarConfig()
{
    createGUI();
    restoreView();
}

void CatalogManager::optionsShowStatusbar(bool on)
{
    if( on )
        statusBar()->show();
    else
        statusBar()->hide();
}

bool CatalogManager::queryClose()
{
    _catalogManager->stop();
    saveView();
    saveSettings(_configFile);
    return true;
}

void CatalogManager::saveView()
{
    saveMainWindowSettings( KGlobal::config(), "View");
}


void CatalogManager::restoreView()
{
    applyMainWindowSettings( KGlobal::config(), "View");

    KToggleAction * toggle = (KToggleAction*)actionCollection()->
	action(KStdAction::stdName(KStdAction::ShowStatusbar));
    toggle->setChecked(!statusBar()->isHidden() );
}


void CatalogManager::projectNew()
{
    KBabel::Project::Ptr p = KBabel::ProjectWizard::newProject();
    if( p )
    {
	disconnect( _project, SIGNAL (signalCatManSettingsChanged())
	    , this, SLOT (updateSettings()));
        _project = p;
	connect( _project, SIGNAL (signalCatManSettingsChanged())
	    , this, SLOT (updateSettings()));

	_configFile = _project->filename();
        restoreSettings();
	updateSettings();
        changeProjectActions(p->filename());
	emit settingsChanged(_settings);
    }
}

void CatalogManager::projectOpen()
{
    QString oldproject = _project->filename();
    if( oldproject == KBabel::ProjectManager::defaultProjectName() )
    {
        oldproject = QString();
    }
    const QString file = KFileDialog::getOpenFileName(oldproject, QString::null, this);
    if (file.isEmpty())
    {
        return;
    }
    KBabel::Project::Ptr p = KBabel::ProjectManager::open(file);
    if( p )
    {
	disconnect( _project, SIGNAL (signalCatManSettingsChanged())
	    , this, SLOT (updateSettings()));
        _project = p;
	connect( _project, SIGNAL (signalCatManSettingsChanged())
	    , this, SLOT (updateSettings()));

	_configFile = p->filename();
        restoreSettings();
	updateSettings();
        changeProjectActions(file);
	emit settingsChanged(_settings);

    }
    else
    {
	KMessageBox::error (this, i18n("Cannot open project file %1").arg(file));
    }
}

void CatalogManager::projectClose()
{
    disconnect( _project, SIGNAL (signalCatManSettingsChanged())
	    , this, SLOT (updateSettings()));
    _project = KBabel::ProjectManager::open(KBabel::ProjectManager::defaultProjectName());
    connect( _project, SIGNAL (signalCatManSettingsChanged())
	    , this, SLOT (updateSettings()));
    _configFile = _project->filename();
    restoreSettings();
    updateSettings();
    changeProjectActions(KBabel::ProjectManager::defaultProjectName());
    emit settingsChanged(_settings);
}

void CatalogManager::changeProjectActions(const QString& project)
{
    bool def = ( project == KBabel::ProjectManager::defaultProjectName() ) ;

    KAction* saveAction=(KAction*)actionCollection()->action( "project_close" );
    saveAction->setEnabled( ! def );
}

void CatalogManager::projectConfigure()
{
    KBabel::ProjectDialog* _projectDialog = new ProjectDialog(_project);

    connect (_projectDialog, SIGNAL (settingsChanged())
	, this, SLOT (updateSettings()));

    // settings are updated via signals
    _projectDialog->exec();

    delete _projectDialog;
}

void CatalogManager::enableActions()
{
    enableActions(true);
}

void CatalogManager::disableActions()
{
    enableActions(false);
}

void CatalogManager::enableActions(bool enable)
{
    KAction* action;
    // the file menu
    
    action = (KAction*)actionCollection()->action( "open" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "open_new_window" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "find_in_files" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "replace_in_files" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "reload" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "toggle_marking" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "toggle_all_marking" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "mark_modified_files" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "load_marking" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "save_marking" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_next_untrans" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_prev_untrans" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_next_fuzzy" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_prev_fuzzy" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_next_fuzzyUntr" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_prev_fuzzyUntr" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_next_error" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_prev_error" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_next_template" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_prev_template" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_next_po" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_prev_po" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_next_marked" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "go_prev_marked" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "statistics" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "package_file" );
    action->setEnabled(enable);

    action = (KAction*)actionCollection()->action( "rough_translation" );
    action->setEnabled(enable);
}

#include "catalogmanager.moc"
