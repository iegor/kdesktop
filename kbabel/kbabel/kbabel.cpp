/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
                2002-2004 by Stanislav Visnovsky
                            <visnovsky@kde.org>

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

#include "kbabel.h"

#include "kbabelsettings.h"
#include "kbprojectsettings.h"
#include "kbabelpref.h"
#include "projectpref.h"
#include "kbabelsplash.h"
#include "regexpextractor.h"
#include "toolaction.h"
#include "commentview.h"
#include "contextview.h"
#include "charselectview.h"
#include "taglistview.h"
#include "sourceview.h"

#include <qdragobject.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qhbox.h>
#include <qwhatsthis.h>
#include <qsize.h>
#include <qtextcodec.h>
#include <qtooltip.h>
#include <qtimer.h>

#include <dcopclient.h>
#include <kdatatool.h>
#include <kpopupmenu.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kedittoolbar.h>
#include <kglobal.h>
#include <kled.h>
#include <klocale.h>
#include <kiconloader.h>
#include <ktoolbar.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kurl.h>
#include <kdialogbase.h>
#include <kprogress.h>
#include <kpushbutton.h>
#include <kmessagebox.h>
#include <kwin.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kspelldlg.h>
#include <ksqueezedtextlabel.h>
#include <kurldrag.h>

#include "resources.h"
#include "kbcatalog.h"
#include "dictionarymenu.h"
#include "kbabeldictbox.h"
#include "kbmailer.h"
#include "kbbookmarkhandler.h"
#include "kbprojectmanager.h"
#include "projectpref.h"
#include "projectwizard.h"

#include "version.h"

#define ID_STATUS_TOTAL 1
#define ID_STATUS_CURRENT 2
#define ID_STATUS_FUZZY 3
#define ID_STATUS_UNTRANS 4
#define ID_STATUS_EDITMODE 5
#define ID_STATUS_READONLY 6
#define ID_STATUS_CURSOR 7

// maximum number of recent files
#define MAX_RECENT 10

using namespace KBabel;

QPtrList<KBabelPreferences> KBabelMW::prefDialogs;

class MyKProgress: public KProgress
{
public:
    MyKProgress( QWidget *parent, const char *name ) : KProgress( parent, name )
    {
        setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
    }
    QSize sizeHint() const { return QSize( 1, 1);}
};

KBabelMW::KBabelMW(QString projectFile)
        : KDockMainWindow (), m_charselectorview(0)
{
    if ( projectFile.isEmpty() )
    	projectFile = KBabel::ProjectManager::defaultProjectName();
    _project = ProjectManager::open(projectFile);
    
    if ( _project == NULL ) // FIXME should not happen anymore
    {
	KMessageBox::error( this, i18n("Cannot open project file\n%1").arg(projectFile)
            , i18n("Project File Error"));
	_project = ProjectManager::open(KBabel::ProjectManager::defaultProjectName());
    }
    
    KBCatalog* catalog=new KBCatalog(projectFile);
    init(catalog);
}

KBabelMW::KBabelMW(KBCatalog* catalog, QString projectFile)
        : KDockMainWindow (), m_charselectorview(0)
{
    if ( projectFile.isEmpty() )
    	projectFile = KBabel::ProjectManager::defaultProjectName();
    _project = ProjectManager::open(projectFile);

    if ( _project == NULL )
    {
	KMessageBox::error( this, i18n("Cannot open project file\n%1").arg(projectFile)
            , i18n("Project File Error"));
	_project = ProjectManager::open(KBabel::ProjectManager::defaultProjectName());
    }
    
    init(catalog);
}

void KBabelMW::init(KBCatalog* catalog)
{
    _config = KSharedConfig::openConfig( "kbabelrc" );

    _toolsShortcuts.clear();

    _fuzzyLed=0;
    _untransLed=0;
    _errorLed=0;

    _projectDialog=0;

    _prefDialog=0;
    prefDialogs.setAutoDelete(true);

    _statusbarTimer = new QTimer(this, "statusbartimer");
    connect(_statusbarTimer,SIGNAL(timeout()),this
        ,SLOT(clearStatusbarMsg()));

    // FIXME:
    Q_ASSERT(_project);

    m_view=new KBabelView(catalog,this, _project);

    setXMLFile ("kbabelui.rc");

    createGUI (0);

    // accept dnd
    setAcceptDrops(true);


    // setup our menubars and toolbars
    setupStatusBar();
    setupActions();
    stateChanged( "fileopened" , StateReverse );
    stateChanged( "readonly", StateNoReverse );

        QPopupMenu* popup;
        popup = (QPopupMenu*)(factory()->container("rmb_edit", this));
        if(popup)
        {
                m_view->setRMBEditMenu(popup);
        }
        popup = (QPopupMenu*)(factory()->container("rmb_search", this));
        if(popup)
        {
                m_view->setRMBSearchMenu(popup);
        }


    connect(catalog,SIGNAL(signalUndoAvailable(bool)),this
            ,SLOT(enableUndo(bool)));
    connect(catalog,SIGNAL(signalRedoAvailable(bool)),this
            ,SLOT(enableRedo(bool)));
    connect(catalog,SIGNAL(signalNumberOfFuzziesChanged(uint)),this
            ,SLOT(setNumberOfFuzzies(uint)));
    connect(catalog,SIGNAL(signalNumberOfUntranslatedChanged(uint)),this
            ,SLOT(setNumberOfUntranslated(uint)));
    connect(catalog,SIGNAL(signalTotalNumberChanged(uint)),this
            ,SLOT(setNumberOfTotal(uint)));
    connect(catalog,SIGNAL(signalProgress(int)),_progressBar,SLOT(setProgress(int)));
    connect(catalog,SIGNAL(signalClearProgressBar()),this,SLOT(clearProgressBar()));
    connect(catalog,SIGNAL(signalResetProgressBar(QString,int))
           ,this,SLOT(prepareProgressBar(QString,int)));
    connect(catalog,SIGNAL(signalFileOpened(bool)),this,SLOT(enableDefaults(bool)));
    connect(catalog,SIGNAL(signalFileOpened(bool)),m_view,SLOT(newFileOpened(bool)));
    connect(catalog,SIGNAL(signalModified(bool)),this,SLOT(showModified(bool)));

    // allow the view to change the statusbar and caption
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));
    connect(m_view, SIGNAL(signalChangeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));
    connect(m_view,SIGNAL(signalFirstDisplayed(bool, bool)),this
           ,SLOT(firstEntryDisplayed(bool, bool)));
    connect(m_view,SIGNAL(signalLastDisplayed(bool, bool)),this
           ,SLOT(lastEntryDisplayed(bool, bool)));
    connect(m_view,SIGNAL(signalFuzzyDisplayed(bool)),this
           ,SLOT(fuzzyDisplayed(bool)));
    connect(m_view,SIGNAL(signalUntranslatedDisplayed(bool)),this
           ,SLOT(untranslatedDisplayed(bool)));
    connect(m_view,SIGNAL(signalFaultyDisplayed(bool)),this
           ,SLOT(faultyDisplayed(bool)));
    connect(m_view,SIGNAL(signalDisplayed(const KBabel::DocPosition&)),this
           ,SLOT(displayedEntryChanged(const KBabel::DocPosition&)));
    connect(m_view,SIGNAL(signalFuzzyAfterwards(bool)),this
            ,SLOT(hasFuzzyAfterwards(bool)));
    connect(m_view,SIGNAL(signalFuzzyInFront(bool)),this
            ,SLOT(hasFuzzyInFront(bool)));
    connect(m_view,SIGNAL(signalUntranslatedAfterwards(bool)),this
            ,SLOT(hasUntranslatedAfterwards(bool)));
    connect(m_view,SIGNAL(signalUntranslatedInFront(bool)),this
            ,SLOT(hasUntranslatedInFront(bool)));
    connect(m_view,SIGNAL(signalErrorAfterwards(bool)),this
            ,SLOT(hasErrorAfterwards(bool)));
    connect(m_view,SIGNAL(signalErrorInFront(bool)),this
            ,SLOT(hasErrorInFront(bool)));
    connect(m_view,SIGNAL(signalBackHistory(bool)),this
            ,SLOT(enableBackHistory(bool)));
    connect(m_view,SIGNAL(signalForwardHistory(bool)),this
            ,SLOT(enableForwardHistory(bool)));


    connect(m_view,SIGNAL(ledColorChanged(const QColor&)),this
                  ,SLOT(setLedColor(const QColor&)));


    connect(m_view,SIGNAL(signalSearchActive(bool)),this,SLOT(enableStop(bool)));

    connect(m_view,SIGNAL(signalProgress(int)),_progressBar,SLOT(setProgress(int)));
    connect(m_view,SIGNAL(signalClearProgressBar()),this,SLOT(clearProgressBar()));
    connect(m_view,SIGNAL(signalResetProgressBar(QString,int))
           ,this,SLOT(prepareProgressBar(QString,int)));

    connect(m_view,SIGNAL(signalDictionariesChanged())
           , this, SLOT(buildDictMenus()));
    connect(m_view,SIGNAL(signalCursorPosChanged(int,int)), this
            , SLOT(updateCursorPosition(int,int)));

    if(!catalog->currentURL().isEmpty())
    {
       enableDefaults(catalog->isReadOnly());
       setNumberOfFuzzies(catalog->numberOfFuzzies());
       setNumberOfUntranslated(catalog->numberOfUntranslated());
       setNumberOfTotal(catalog->numberOfEntries());

       enableUndo(catalog->isUndoAvailable());
       enableUndo(catalog->isRedoAvailable());

       m_view->emitEntryState();

       changeCaption(catalog->currentURL().prettyURL() );
    }

    mailer = new KBabelMailer( this, _project );

    bmHandler = new KBabelBookmarkHandler((QPopupMenu*)factory()->container("bookmark", this));
    // the earlier created KAction for "clear_bookmarks" is now reconnected
    KAction* action = actionCollection()->action("clear_bookmarks");
    if (action) {
      action->disconnect(SIGNAL(activated()));
      connect(action, SIGNAL(activated()),
              bmHandler, SLOT(slotClearBookmarks()));
    }
    connect(bmHandler, SIGNAL(signalBookmarkSelected(int)),
            this, SLOT(slotOpenBookmark(int)));
    connect(m_view, SIGNAL(signalNewFileOpened(KURL)),
            bmHandler, SLOT(slotClearBookmarks()));

    _config = KSharedConfig::openConfig( "kbabelrc" );
    restoreSettings();

    _config->setGroup("KBabel");

    if(!_config->hasKey("Version"))
    {
      QString encodingStr;
      switch(catalog->saveSettings().encoding)
      {
         case KBabel::ProjectSettingsBase::UTF8:
            encodingStr=QTextCodec::codecForName("UTF-8")->name();
            break;
         case KBabel::ProjectSettingsBase::UTF16:
            encodingStr=QTextCodec::codecForName("UTF-16")->name();
            break;
         default:
            encodingStr=QTextCodec::codecForLocale()->name();
      }

      if( KBabelSplash::instance ) KBabelSplash::instance->close(); //close splash screen window, if there is one

      KMessageBox::information(0,i18n("You have not run KBabel before. "
                   "To allow KBabel to work correctly you must enter some "
                   "information in the preferences dialog first.\n"
                   "The minimum requirement is to fill out the Identity page.\n"
                   "Also check the encoding on the Save page, which is currently "
                   "set to %1. You may want to change this setting "
                   "according to the settings of your language team.").arg(encodingStr));

      QTimer::singleShot(1,this,SLOT(projectConfigure()));
    }

    _config->writeEntry("Version",VERSION);
    _config->sync();

}

KBabelMW::~KBabelMW()
{
   if(_prefDialog)
   {
      prefDialogs.remove(_prefDialog);
   }
   if(_projectDialog)
   {
      delete _projectDialog;
   }
   delete mailer;
   delete bmHandler;
}


void KBabelMW::setSettings(SaveSettings saveOpts,IdentitySettings idOpts)
{
   m_view->updateSettings();
   m_view->catalog()->setSettings(saveOpts);
   m_view->catalog()->setSettings(idOpts);

   if(_fuzzyLed)
   {
      _fuzzyLed->setColor(KBabelSettings::ledColor());
   }
   if(_untransLed)
   {
      _untransLed->setColor(KBabelSettings::ledColor());
   }
   if(_errorLed)
   {
      _errorLed->setColor(KBabelSettings::ledColor());
   }

}

void KBabelMW::updateSettings()
{
   m_view->updateSettings();

   if(_fuzzyLed)
   {
      _fuzzyLed->setColor(KBabelSettings::ledColor());
   }
   if(_untransLed)
   {
      _untransLed->setColor(KBabelSettings::ledColor());
   }
   if(_errorLed)
   {
      _errorLed->setColor(KBabelSettings::ledColor());
   }

}


void KBabelMW::setupActions()
{
   KAction* action;

   // the file menu
   action = KStdAction::open(this, SLOT(fileOpen()), actionCollection());

   a_recent = KStdAction::openRecent(this, SLOT(openRecent(const KURL&)), actionCollection());
   a_recent->setMaxItems(MAX_RECENT);
   
   action = KStdAction::revert(m_view,SLOT(revertToSaved()),actionCollection());
   action=KStdAction::save(this, SLOT(fileSave()), actionCollection());
   action = KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
   action = new KAction(i18n("Save Sp&ecial..."), 0, this, SLOT(fileSaveSpecial()),
         actionCollection(), "save_special" );
   action = new KAction(i18n("Set &Package..."), 0, m_view, SLOT(setFilePackage()),
         actionCollection(), "set_package" );

   action = KStdAction::mail(this, SLOT(fileMail()), actionCollection());

   action = new KAction(i18n("&New View"), 0, this, SLOT(fileNewView()),
         actionCollection(), "file_new_view");

   action = new KAction(i18n("New &Window"), 0, this, SLOT(fileNewWindow()),
         actionCollection(), "file_new_window");
   action->setShortcut( KStdAccel::openNew() );

   action = KStdAction::quit(this, SLOT(quit()), actionCollection());



   // the edit menu
   action = KStdAction::undo(m_view, SLOT(undo()), actionCollection());
   action = KStdAction::redo(m_view, SLOT(redo()), actionCollection());
   action = KStdAction::cut(m_view, SIGNAL(signalCut()), actionCollection());
   action = KStdAction::copy(m_view, SIGNAL(signalCopy()), actionCollection());
   action = KStdAction::paste(m_view, SIGNAL(signalPaste()), actionCollection());
   action = KStdAction::selectAll(m_view, SIGNAL(signalSelectAll()), actionCollection());
   action = KStdAction::find(m_view, SLOT(find()), actionCollection());
   action = KStdAction::findNext(m_view, SLOT(findNext()), actionCollection());
   action = KStdAction::findPrev(m_view, SLOT(findPrev()), actionCollection());
   action = KStdAction::replace(m_view, SLOT(replace()), actionCollection());

   action = KStdAction::clear( m_view, SLOT(clear()), actionCollection(), "clear" );

   action = new KAction(i18n("Cop&y Msgid to Msgstr"), "msgid2msgstr", CTRL+Key_Space, m_view
                 ,SLOT(msgid2msgstr()), actionCollection(), "msgid2msgstr");
   action = new KAction(i18n("Copy Searc&h Result to Msgstr"), "search2msgstr",
                        CTRL+ALT+Key_Space, m_view, SLOT(search2msgstr()),
                        actionCollection(), "search2msgstr");
   action = new KAction(i18n("Copy Msgstr to Other &Plurals"), Key_F11, m_view
                   ,SLOT(plural2msgstr()), actionCollection(), "plural2msgstr");
   action = new KAction(i18n("Copy Selected Character to Msgstr"), Key_F10, m_charselectorview
                   ,SLOT(emitChar()), actionCollection(), "char2msgstr");

   a_unsetFuzzy = new KAction(i18n("To&ggle Fuzzy Status"), "togglefuzzy", CTRL+Key_U, m_view
                 , SLOT(removeFuzzyStatus()), actionCollection(), "edit_toggle_fuzzy");
   action = new KAction(i18n("&Edit Header..."), 0, m_view, SLOT(editHeader()),
         actionCollection(), "edit_edit_header");

   action = new KAction(i18n("&Insert Next Tag"), "insert_tag", CTRL+ALT+Key_N
           , m_view, SLOT(insertNextTag())
           , actionCollection(),"insert_next_tag");
   connect(m_view,SIGNAL(signalNextTagAvailable(bool)),action
           ,SLOT(setEnabled(bool)));
   action = new KAction(i18n("Insert Next Tag From Msgid P&osition"), "insert_tag", CTRL+Key_M
           , m_view, SLOT(insertNextTagMsgid())
           , actionCollection(),"insert_next_tag_msgid");
   connect(m_view,SIGNAL(signalNextTagAvailable(bool)),action
           ,SLOT(setEnabled(bool)));
   KActionMenu *actionMenu= new KActionMenu(i18n("Inser&t Tag"), "insert_tag"
         , actionCollection(),"insert_tag");
   m_view->setTagsMenu(actionMenu->popupMenu());
   connect(m_view,SIGNAL(signalTagsAvailable(bool)),actionMenu
           ,SLOT(setEnabled(bool)));
   connect(actionMenu,SIGNAL(activated()),m_view,SLOT(insertNextTag()));

   action = new KAction(i18n("Show Tags Menu"),CTRL+Key_Less
           , m_view, SLOT(showTagsMenu()), actionCollection(),"show_tags_menu");
   action->setEnabled(false);

   connect(m_view,SIGNAL(signalTagsAvailable(bool)),action
           ,SLOT(setEnabled(bool)));

   action = new KAction(i18n("Move to Next Tag"), 0, CTRL+ALT+Key_M
           , m_view, SLOT(skipToNextTag())
           , actionCollection(),"move_to_next_tag");

   action = new KAction(i18n("Move to Previous Tag"), 0, CTRL+ALT+Key_B
           , m_view, SLOT(skipToPreviousTag())
           , actionCollection(),"move_to_prev_tag");

   action = new KAction(i18n("Insert Next Argument"), "insert_arg", CTRL+ALT+Key_G
           , m_view, SLOT(insertNextArg())
           , actionCollection(),"insert_next_arg");
   connect(m_view,SIGNAL(signalNextArgAvailable(bool)),action
           ,SLOT(setEnabled(bool)));
   actionMenu= new KActionMenu(i18n("Inser&t Argument"), "insert_arg"
         , actionCollection(),"insert_arg");
   m_view->setArgsMenu(actionMenu->popupMenu());
   connect(m_view,SIGNAL(signalArgsAvailable(bool)),actionMenu
           ,SLOT(setEnabled(bool)));
   connect(actionMenu,SIGNAL(activated()),m_view,SLOT(insertNextArg()));

   action = new KAction(i18n("Show Arguments Menu"),CTRL+Key_Percent
           , m_view, SLOT(showArgsMenu()), actionCollection(),"show_args_menu");
   action->setEnabled(false);

   connect(m_view,SIGNAL(signalArgsAvailable(bool)),action
           ,SLOT(setEnabled(bool)));

    // next, the go-menu
    action = new KAction(i18n("&Previous"), "previous",
                 KStdAccel::shortcut(KStdAccel::Prior), m_view , SLOT(gotoPrev()),
                 actionCollection(),"go_prev_entry");
   action = new KAction(i18n("&Next"), "next",
                 KStdAccel::shortcut(KStdAccel::Next), m_view , SLOT(gotoNext()),
                 actionCollection(),"go_next_entry");
   action = KStdAction::goTo(m_view, SLOT(gotoEntry()), actionCollection());
   action->setShortcut( KStdAccel::gotoLine());
   action = KStdAction::firstPage(m_view, SLOT(gotoFirst()),actionCollection());
   action->setText(i18n("&First Entry"));
   action->setShortcut(CTRL+ALT+Key_Home);
   action = KStdAction::lastPage(m_view, SLOT(gotoLast()),actionCollection());
   action->setText(i18n("&Last Entry"));
   action->setShortcut(CTRL+ALT+Key_End);
   a_prevFoU = new KAction(i18n("P&revious Fuzzy or Untranslated"),"prevfuzzyuntrans",
         CTRL+SHIFT+Key_Prior, m_view,
         SLOT(gotoPrevFuzzyOrUntrans()),actionCollection(), "go_prev_fuzzyUntr");
   a_nextFoU = new KAction(i18n("N&ext Fuzzy or Untranslated"),"nextfuzzyuntrans",
         CTRL+SHIFT+Key_Next, m_view,
         SLOT(gotoNextFuzzyOrUntrans()),actionCollection(), "go_next_fuzzyUntr");
   a_prevFuzzy = new KAction(i18n("Pre&vious Fuzzy"),"prevfuzzy",
         CTRL+Key_Prior, m_view,
         SLOT(gotoPrevFuzzy()),actionCollection(), "go_prev_fuzzy");
   a_nextFuzzy = new KAction(i18n("Ne&xt Fuzzy"), "nextfuzzy",
         CTRL+Key_Next, m_view,
         SLOT(gotoNextFuzzy()),actionCollection(), "go_next_fuzzy");
   a_prevUntrans = new KAction(i18n("Prev&ious Untranslated"), "prevuntranslated",
         ALT+Key_Prior, m_view,
         SLOT(gotoPrevUntranslated()),actionCollection(), "go_prev_untrans");
   a_nextUntrans = new KAction(i18n("Nex&t Untranslated"), "nextuntranslated",
         ALT+Key_Next, m_view,
         SLOT(gotoNextUntranslated()),actionCollection(), "go_next_untrans");
   action = new KAction(i18n("Previo&us Error"), "preverror",
          SHIFT+Key_Prior, m_view,
         SLOT(gotoPrevError()),actionCollection(), "go_prev_error");
   action = new KAction(i18n("Next Err&or"), "nexterror",
         SHIFT+Key_Next, m_view,
         SLOT(gotoNextError()),actionCollection(), "go_next_error");
   action = new KAction(i18n("&Back in History"), "back", ALT+Key_Left, m_view,
         SLOT(backHistory()),actionCollection(), "go_back_history");
   action = new KAction(i18n("For&ward in History"), "forward", ALT+Key_Right, m_view,
         SLOT(forwardHistory()),actionCollection(), "go_forward_history");

    // the search menu
   actionMenu=new KActionMenu(i18n("&Find Text"),
         "transsearch",actionCollection(),"dict_search_all");
   connect(actionMenu,SIGNAL(activated()),m_view,SLOT(startSearch()));
   dictMenu = new DictionaryMenu(actionMenu->popupMenu(),actionCollection(),this);
   connect(dictMenu,SIGNAL(activated(const QString)), m_view
           , SLOT(startSearch(const QString)));

   actionMenu=new KActionMenu(i18n("F&ind Selected Text"),
         "transsearch",actionCollection(),"dict_search_selected");
   connect(actionMenu,SIGNAL(activated()),m_view,SLOT(startSelectionSearch()));
   selectionDictMenu = new DictionaryMenu(actionMenu->popupMenu(),actionCollection(),this);
   connect(selectionDictMenu,SIGNAL(activated(const QString)), m_view
           , SLOT(startSelectionSearch(const QString)));

   actionMenu=new KActionMenu(i18n("&Edit Dictionary"),
         "transsearch",actionCollection(),"dict_edit");
   editDictMenu = new DictionaryMenu(actionMenu->popupMenu(),actionCollection(),this);
   connect(editDictMenu,SIGNAL(activated(const QString)), m_view
           , SLOT(editDictionary(const QString)));


   actionMenu=new KActionMenu(i18n("Con&figure Dictionary"),
         "transsearch",actionCollection(),"dict_configure");
   configDictMenu = new DictionaryMenu(actionMenu->popupMenu(),actionCollection(),this);
   connect(configDictMenu,SIGNAL(activated(const QString)), m_view
           , SLOT(configureDictionary(const QString)));

   actionMenu=new KActionMenu(i18n("About Dictionary"), "transsearch",
         actionCollection(), "dict_about");
   aboutDictMenu = new DictionaryMenu(actionMenu->popupMenu(),actionCollection(),this);
   connect(aboutDictMenu,SIGNAL(activated(const QString)), m_view
           , SLOT(aboutDictionary(const QString)));

   buildDictMenus();

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

   a_recentprojects = new KRecentFilesAction(i18n("Open &Recent"), 0, this, SLOT(projectOpenRecent(const KURL&)), actionCollection(), "recent_projects");

    // the tools menu
   action = new KAction(i18n("&Spell Check..."), "spellcheck", CTRL+Key_I
           , m_view, SLOT(spellcheckCommon()),actionCollection()
           ,"spellcheck_common");
   action = new KAction(i18n("&Check All..."), "spellcheck_all", 0
           , m_view, SLOT(spellcheckAll()),actionCollection()
           ,"spellcheck_all");
   action = new KAction(i18n("C&heck From Cursor Position..."), "spellcheck_from_cursor", 0
           , m_view, SLOT(spellcheckFromCursor()),actionCollection()
           ,"spellcheck_from_cursor");
   action = new KAction(i18n("Ch&eck Current..."), "spellcheck_actual", 0
           , m_view, SLOT(spellcheckCurrent()),actionCollection()
           ,"spellcheck_current");
   action = new KAction(i18n("Check Fro&m Current to End of File..."), 0
           , m_view, SLOT(spellcheckFromCurrent()),actionCollection()
           ,"spellcheck_from_current");
   action = new KAction(i18n("Chec&k Selected Text..."), "spellcheck_selected", 0
           , m_view, SLOT(spellcheckMarked()),actionCollection()
           ,"spellcheck_marked");

   KToggleAction *toggleAction;

   toggleAction = new KToggleAction(i18n("&Diffmode"), "autodiff", 0
           ,actionCollection(), "diff_toggleDiff");
   connect(toggleAction,SIGNAL(toggled(bool)), m_view, SLOT(toggleAutoDiff(bool)));
   connect(m_view,SIGNAL(signalDiffEnabled(bool)), toggleAction
               , SLOT(setChecked(bool)));
   toggleAction->setChecked(m_view->autoDiffEnabled());

   action = new KAction(i18n("&Show Diff"), "diff", Key_F5
           , m_view, SLOT(diff()),actionCollection()
           ,"diff_diff");
   action = new KAction(i18n("S&how Original Text"), "contents", Key_F6
           , m_view, SLOT(diffShowOrig()),actionCollection()
           ,"diff_showOrig");

   action = new KAction(i18n("&Open File for Diff"), "fileopen" ,0
           , m_view, SLOT(openDiffFile()),actionCollection()
           ,"diff_openFile");

   action = new KAction(i18n("&Rough Translation..."), 0
           , m_view, SLOT(roughTranslation()),actionCollection()
           ,"rough_translation");

   action = new KAction(i18n("&Catalog Manager..."),"catalogmanager", 0 , this,
         SLOT(openCatalogManager()),actionCollection(), "open_catalog_manager");

   new KAction( i18n("Toggle Edit Mode"), 0, Key_Insert,this,SLOT(toggleEditMode()), actionCollection(), "toggle_insert_mode");
   
   new KAction( i18n("&Word Count"), 0, m_view, SLOT(wordCount()), actionCollection(), "word_count");   

    // next, the settings menu
    createStandardStatusBarAction();

    KStdAction::configureToolbars(this,SLOT(optionsEditToolbars()),actionCollection());

    KStdAction::keyBindings(guiFactory(),SLOT(configureShortcuts()),actionCollection());
    KStdAction::preferences(this,SLOT(optionsPreferences()),actionCollection());

    setStandardToolBarMenuEnabled ( true );

   action = new KAction(i18n("&Stop Searching"), "stop",Key_Escape, m_view,
         SLOT(stopSearch()),actionCollection(), "stop_search");
   action->setEnabled(false);

   new KAction(i18n("&Gettext Info"), 0, this,
           SLOT(gettextHelp()), actionCollection(), "help_gettext");


   // the bookmarks menu

   action = KStdAction::addBookmark(this, SLOT(slotAddBookmark()),
      actionCollection(), "add_bookmark");
   action->setEnabled(false);
   // this action is now connected to dummySlot(), and later reconnected
   // to bmHandler after that object actually is created
   new KAction(i18n("Clear Bookmarks"), 0, this, SLOT(dummySlot()),
      actionCollection(), "clear_bookmarks");

   setupDynamicActions();

   createGUI(0);

   QPopupMenu *popup = static_cast<QPopupMenu*>(factory()->container("settings",this));
   popup->insertItem( i18n("&Views"), dockHideShowMenu(), -1, 0 );
}


void KBabelMW::setupStatusBar()
{
    statusBar()->insertItem(i18n("Current: 0"),ID_STATUS_CURRENT);
    statusBar()->insertItem(i18n("Total: 0"),ID_STATUS_TOTAL);
    statusBar()->insertItem(i18n("Fuzzy: 0"),ID_STATUS_FUZZY);
    statusBar()->insertItem(i18n("Untranslated: 0"),ID_STATUS_UNTRANS);

    if(KBabelSettings::ledInStatusbar())
    {
       QColor ledColor=KBabelSettings::ledColor();
       QHBox* statusBox = new QHBox(statusBar(),"statusBox");
       statusBox->setSpacing(2);
       new QLabel(" "+i18n("Status: "),statusBox);
       _fuzzyLed = new KLed(ledColor,KLed::Off,KLed::Sunken,KLed::Rectangular
                   ,statusBox);
       _fuzzyLed->setFixedSize(15,12);
       new QLabel(i18n("fuzzy")+" ",statusBox);
       _untransLed = new KLed(ledColor,KLed::Off,KLed::Sunken,KLed::Rectangular
                   ,statusBox);
       _untransLed->setFixedSize(15,12);
       new QLabel(i18n("untranslated")+" ",statusBox);
       _errorLed = new KLed(ledColor,KLed::Off,KLed::Sunken,KLed::Rectangular
                   ,statusBox);
       _errorLed->setFixedSize(15,12);
       new QLabel(i18n("faulty")+" ",statusBox);

       statusBox->setFixedWidth(statusBox->sizeHint().width());
       statusBar()->addWidget(statusBox);
    }

    statusBar()->insertItem(i18n("INS"),ID_STATUS_EDITMODE);

    statusBar()->insertItem(i18n("RW"),ID_STATUS_READONLY);

    statusBar()->insertItem(i18n("Line: %1 Col: %2").arg(1).arg(1)
            ,ID_STATUS_CURSOR);

    QHBox* progressBox = new QHBox(statusBar(),"progressBox");
    progressBox->setSpacing(2);
    _progressLabel = new KSqueezedTextLabel( "", progressBox );
    _progressBar=new MyKProgress(progressBox,"progressbar");
    _progressBar->hide();
    progressBox->setStretchFactor(_progressBar,1);

    statusBar()->addWidget(progressBox,1);
    statusBar()->setMinimumHeight(progressBox->sizeHint().height());

    QWhatsThis::add(statusBar(),
       i18n("<qt><p><b>Statusbar</b></p>\n\
<p>The statusbar displays some information about the opened file,\n\
like the total number of entries and the number of fuzzy and untranslated\n\
messages. Also the index and the status of the currently displayed entry is shown.</p></qt>"));

}

void KBabelMW::setupDynamicActions()
{
   // dynamic validation tools
   QValueList<KDataToolInfo> tools = ToolAction::validationTools();

   QPtrList<KAction> actions = ToolAction::dataToolActionList(
      tools, m_view, SLOT(validateUsingTool( const KDataToolInfo &, const QString & )),
      "validate", false, actionCollection() );

   KActionMenu* m_menu = new KActionMenu(i18n("&Validation"), actionCollection(), "dynamic_validation_tools");

   KAction* ac = new KAction(i18n("Perform &All Checks"), CTRL+Key_E , m_view,
         SLOT(checkAll()),actionCollection(), "check_all");
   ac->setEnabled(false);
   m_menu->insert(ac);

   m_menu->insert( new KActionSeparator() );

   ac = new KAction(i18n("C&heck Syntax"), CTRL+Key_T , m_view,
         SLOT(checkSyntax()),actionCollection(), "check_syntax");
   ac->setEnabled(false);
   m_menu->insert(ac);

   for( ac = actions.first(); ac ; ac = actions.next() )
   {
        m_menu->insert(ac);
   }

   // dynamic modify tools

   // query available tools
   QValueList<KDataToolInfo> allTools = KDataToolInfo::query
        ("CatalogItem", "application/x-kbabel-catalogitem", KGlobal::instance());

   // skip read-only tools for single items
   QValueList<KDataToolInfo> modifyTools;

   QValueList<KDataToolInfo>::ConstIterator entry = allTools.begin();
   for( ; entry != allTools.end(); ++entry )
   {
        if( !(*entry).isReadOnly() )
        {
            modifyTools.append( (*entry) );
        }
   }

   // create corresponding actions
   actions = ToolAction::dataToolActionList(
      modifyTools, m_view, SLOT(modifyUsingTool( const KDataToolInfo &, const QString & )),
      "validate", true, actionCollection() );

   // skip validation actions
   for( ac = actions.first(); ac ; ac = actions.next() )
   {
        m_menu->insert(ac);
   }

   // insert tools
   m_menu = new KActionMenu(i18n("&Modify"), actionCollection(), "dynamic_modify_tools");
   for( ac = actions.first(); ac ; ac = actions.next() )
   {
        m_menu->insert(ac);
   }

   // query available tools for whole catalog
   allTools = KDataToolInfo::query
        ("Catalog", "application/x-kbabel-catalog", KGlobal::instance());

   // skip read-only tools
   entry = allTools.begin();
   for( ; entry != allTools.end(); ++entry )
   {
        if( !(*entry).isReadOnly() )
        {
            modifyTools.append( (*entry) );
        }
   }

   // create corresponding actions
   actions = ToolAction::dataToolActionList(
      modifyTools, m_view, SLOT(modifyUsingTool( const KDataToolInfo &, const QString & )),
      "validate", true, actionCollection() );

   // skip validation actions
   for( ac = actions.first(); ac ; ac = actions.next() )
   {
        m_menu->insert(ac);
   }

   // create corresponding actions
   actions = ToolAction::dataToolActionList(
      modifyTools, m_view, SLOT(modifyCatalogUsingTool( const KDataToolInfo &, const QString & )),
      "validate", true, actionCollection() );

   // insert tools
   m_menu = new KActionMenu(i18n("&Modify"), actionCollection(), "dynamic_modify_tools");
   for( ac = actions.first(); ac ; ac = actions.next() )
   {
        m_menu->insert(ac);
   }
}

void KBabelMW::saveSettings()
{
    {
        saveMainWindowSettings(_config, "View");
        writeDockConfig (_config, "View");
    }

    {
       a_recent->saveEntries(_config);
       a_recentprojects->saveEntries(_config,"Project");
    }

    _config->sync();
}

void KBabelMW::restoreSettings()
{
    {
       applyMainWindowSettings(_config, "View");
       KConfigGroupSaver saver(_config,"View");

       _config->setGroup("View");
       m_view->restoreView(_config);

        readDockConfig (_config, "View");
    }

    {
       a_recent->loadEntries(_config);

       a_recentprojects->loadEntries(_config, "Project");
    }
}



void KBabelMW::saveProperties(KConfig *config)
{
   m_view->saveSession(config);
}

void KBabelMW::readProperties(KConfig *config)
{
   m_view->restoreSession(config);

   // need to ensure that the windows is propertly setup also
   // for new views-only
   if(!m_view->currentURL().isEmpty())
   {
       KBCatalog* catalog=m_view->catalog();
       enableDefaults(catalog->isReadOnly());
       setNumberOfFuzzies(catalog->numberOfFuzzies());
       setNumberOfUntranslated(catalog->numberOfUntranslated());
       setNumberOfTotal(catalog->numberOfEntries());

       enableUndo(catalog->isUndoAvailable());
       enableUndo(catalog->isRedoAvailable());

       m_view->emitEntryState();

       changeCaption(catalog->currentURL().prettyURL() );
    }
}

bool KBabelMW::queryClose()
{
   if(m_view->isSearching())
   {
       connect(m_view,SIGNAL(signalSearchActive(bool)),this,SLOT(quit()));
       m_view->stopSearch();
       return false;
   }

   if(m_view->catalog()->isActive())
   {
       // stop the activity and try again
       m_view->catalog()->stop();
       QTimer::singleShot(0, this, SLOT( close() ));
       return false;
   }

   if(m_view->isModified())
   {
      switch(KMessageBox::warningYesNoCancel(this,
      i18n("The document contains unsaved changes.\n\
Do you want to save your changes or discard them?"),i18n("Warning"),
      KStdGuiItem::save(),KStdGuiItem::discard()))
      {
         case KMessageBox::Yes:
         {
            return m_view->saveFile();
         }
         case KMessageBox::No:
            return true;
         default:
            return false;
      }
   }

  return true;
}

bool KBabelMW::queryExit()
{
   saveSettings();
   _config->setGroup("View");
   m_view->saveView(_config);

   m_view->saveSettings();
   return true;
}

void KBabelMW::quit()
{
    close();
}


void KBabelMW::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(KURLDrag::canDecode(event));
}

void KBabelMW::dropEvent(QDropEvent *event)
{
    KURL::List uri;
    // see if we can decode a URI.. if not, just ignore it
    if (KURLDrag::decode(event, uri))
    {
       m_view->processUriDrop(uri,mapToGlobal(event->pos()));
    }
}

void KBabelMW::wheelEvent(QWheelEvent *e)
{
    m_view->wheelEvent(e);
}

void KBabelMW::openRecent(const KURL& url)
{
   KBabelView *view = KBabelView::viewForURL(url,QString::null);
   if(view)
   {
       KWin::activateWindow(view->topLevelWidget()->winId());
       return;
   }

   m_view->open(url);
}

void KBabelMW::open(const KURL& url)
{
   open(url,QString::null,false);
}

void KBabelMW::open(const KURL& url, const QString package, bool newWindow)
{
   kdDebug(KBABEL) << "opening file with project:" << _project->filename() << endl;
   kdDebug(KBABEL) << "URL:" << url.prettyURL() << endl;
   KBabelView *view = KBabelView::viewForURL(url, _project->filename());
   if(view)
   {
        kdDebug(KBABEL) << "there is a such view" << endl;
       KWin::activateWindow(view->topLevelWidget()->winId());
       return;
   }

   addToRecentFiles(url);

   if(newWindow)
   {
        kdDebug(KBABEL) << "creating new window"<< endl;
      fileNewWindow()->open(url, package,false);
   }
   else
   {
        m_view->open(url,package);
   }
}

void KBabelMW::openTemplate(const KURL& openURL,const KURL& saveURL,const QString& package, bool newWindow)
{
   if(newWindow)
   {
      fileNewWindow()->openTemplate(openURL,saveURL,package,false);
   }
   else
   {
      m_view->openTemplate(openURL,saveURL);
      m_view->catalog()->setPackage(package);
   }
}

void KBabelMW::fileOpen()
{
    m_view->open();

    KURL url=m_view->currentURL();
    addToRecentFiles(url);
}


void KBabelMW::addToRecentFiles(KURL url)
{
   if( url.isValid() && ! url.isEmpty() )
        a_recent->addURL(url);
}

void KBabelMW::fileSave()
{
    // do it asynchronously due to kdelibs bug
    QTimer::singleShot( 0, this, SLOT( fileSave_internal() ));
}

void KBabelMW::fileSave_internal()
{
    // this slot is called whenever the File->Save menu is selected,
    // the Save shortcut is pressed (usually CTRL+S) or the Save toolbar
    // button is clicked

    if(!m_view->isModified())
    {
       statusBar()->message(i18n("There are no changes to save."),2000);
    }
    else
    {
       // disable save
       KAction* saveAction=(KAction*)actionCollection()->action( KStdAction::name( KStdAction::Save) );
       saveAction->setEnabled(false);

       m_view->saveFile();

       KURL url=m_view->currentURL();

       DCOPClient *client = kapp->dcopClient();
       QByteArray data;
       QDataStream arg(data, IO_WriteOnly);
       arg << ((url.directory(false)+url.fileName()).utf8())  ;
       if( !client->send( "catalogmanager-*", "CatalogManagerIFace", "updatedFile(QCString)", data ))
        kdDebug(KBABEL) << "Unable to send file update info via DCOP" << endl;

       // reenable save action
       saveAction->setEnabled(true);
    }
}

void KBabelMW::fileSaveAs()
{
    m_view->saveFileAs();
    KURL url=m_view->currentURL();

    DCOPClient *client = kapp->dcopClient();
    QByteArray data;
    QDataStream arg(data, IO_WriteOnly);
    arg << ((url.directory(false)+url.fileName()).utf8())  ;
    if( !client->send( "catalogmanager-*", "CatalogManagerIFace", "updatedFile(QCString)", data ))
        kdDebug(KBABEL) << "Unable to send file update info via DCOP" << endl;
}

void KBabelMW::fileSaveSpecial()
{
    if( !m_view->saveFileSpecial() ) return;

    KURL url=m_view->currentURL();

    DCOPClient *client = kapp->dcopClient();
    QByteArray data;
    QDataStream arg(data, IO_WriteOnly);
    arg << ((url.directory(false)+url.fileName()).utf8())  ;
    if( !client->send( "catalogmanager-*", "CatalogManagerIFace", "updatedFile(QCString)", data ))
        kdDebug(KBABEL) << "Unable to send file update info via DCOP" << endl;
}

void KBabelMW::fileMail()
{
    if( m_view->isModified() ) fileSave();
    mailer->sendOneFile( m_view->currentURL() );
}

void KBabelMW::fileNewView()
{
   KBabelMW* b=new KBabelMW(m_view->catalog(),_project->filename());
   b->updateSettings();
   b->initBookmarks(bmHandler->bookmarks());
   b->show();
}

KBabelMW* KBabelMW::fileNewWindow()
{
   KBabelMW* b=new KBabelMW(_project->filename());
   b->setSettings(m_view->catalog()->saveSettings(),m_view->catalog()->identitySettings());
   b->show();

   return b;
}

void KBabelMW::toggleEditMode()
{
   bool ovr=!m_view->isOverwriteMode();

   m_view->setOverwriteMode(ovr);

   if(ovr)
      statusBar()->changeItem(i18n("OVR"),ID_STATUS_EDITMODE);
   else
      statusBar()->changeItem(i18n("INS"),ID_STATUS_EDITMODE);

}

void KBabelMW::optionsShowStatusbar(bool on)
{
   if(on)
   {
      statusBar()->show();
   }
   else
   {
      statusBar()->hide();
   }
}

void KBabelMW::optionsEditToolbars()
{
   saveMainWindowSettings( KGlobal::config(), "View" );
   KEditToolbar dlg(actionCollection());
   connect(&dlg, SIGNAL(newToolbarConfig()), this, SLOT(newToolbarConfig()));
   dlg.exec();
}

void KBabelMW::newToolbarConfig()
{
    createGUI(0);
    applyMainWindowSettings( KGlobal::config(), "View" );
}

void KBabelMW::optionsPreferences()
{
    if(!_prefDialog)
    {
        _prefDialog = new KBabelPreferences(m_view->dictionaries());
        prefDialogs.append(_prefDialog);

        connect(_prefDialog,SIGNAL(settingsChanged())
                ,m_view,SLOT(updateSettings()));
    }

    int prefHeight=_prefDialog->height();
    int prefWidth=_prefDialog->width();
    int width=this->width();
    int height=this->height();

    int x=width/2-prefWidth/2;
    int y=height/2-prefHeight/2;

    _prefDialog->move(mapToGlobal(QPoint(x,y)));

    if(!_prefDialog->isVisible())
    {
       _prefDialog->show();
    }

    _prefDialog->raise();
    KWin::activateWindow(_prefDialog->winId());
}

void KBabelMW::setLedColor(const QColor& color)
{
   if(_fuzzyLed)
   {
      _fuzzyLed->setColor(color);
   }
   if(_untransLed)
   {
      _untransLed->setColor(color);
   }
   if(_errorLed)
   {
      _errorLed->setColor(color);
   }
}

void KBabelMW::openCatalogManager()
{
    QCString service;
    QString result;

    DCOPClient * client = kapp->dcopClient();

    // find out, if there is a running catalog manager
    QCStringList apps = client->registeredApplications();
    for( QCStringList::Iterator it = apps.begin() ; it != apps.end() ; ++it )
    {
        QString clientID = *it;
        if( clientID.startsWith("catalogmanager") )
        {
            service = *it;
            break;
        }
    }

    // if there is no running catalog manager, start one
    if( service.isEmpty() )
    {
        QString prog = "catalogmanager";
        QString url = "";
        if( kapp->startServiceByDesktopName(prog,url, &result,&service))
        {
             KMessageBox::error(this, i18n("Unable to use KLauncher to start "
                "Catalog Manager. You should check the installation of KDE.\n"
                "Please start Catalog Manager manually."));
             return;
        }
    }

    // set preferred window
    QByteArray data;
    QDataStream arg(data, IO_WriteOnly);
    arg << (this->winId()) ;
    if( !client->send( service, "CatalogManagerIFace", "setPreferredWindow( WId )", data )) kdDebug(KBABEL) << "Unable to set preferred window via DCOP" << endl;
}



void KBabelMW::firstEntryDisplayed(bool firstEntry, bool firstForm)
{
   KAction* firstAction=(KAction*)actionCollection()->action(KStdAction::stdName(KStdAction::FirstPage));
   KAction* prevAction=(KAction*)actionCollection()->action("go_prev_entry");

   firstAction->setEnabled(!firstEntry);
   prevAction->setEnabled(!(firstEntry && firstForm));

}

void KBabelMW::lastEntryDisplayed(bool lastEntry, bool lastForm)
{
   KAction* lastAction=(KAction*)actionCollection()->action(KStdAction::stdName(KStdAction::LastPage));
   KAction* nextAction=(KAction*)actionCollection()->action("go_next_entry");

   lastAction->setEnabled(!lastEntry);
   nextAction->setEnabled(!(lastEntry && lastForm));
}

void KBabelMW::fuzzyDisplayed(bool flag)
{
    if(!_fuzzyLed)
       return;

    if(flag)
    {
       if(_fuzzyLed->state()==KLed::Off)
       {
          _fuzzyLed->on();
       }
    }
    else
    {
       if(_fuzzyLed->state()==KLed::On)
           _fuzzyLed->off();
    }
}

void KBabelMW::untranslatedDisplayed(bool flag)
{
    if(!_untransLed)
       return;

    // do not allow fuzzy toggle for untranslated
    KAction *action=actionCollection()->action("edit_toggle_fuzzy");
    if(action)
        action->setEnabled(!flag);


    if(flag)
    {
       if(_untransLed->state()==KLed::Off)
          _untransLed->on();
    }
    else
    {
       if(_untransLed->state()==KLed::On)
          _untransLed->off();
    }
}


void KBabelMW::faultyDisplayed(bool flag)
{
    if(!_errorLed)
       return;

    if(flag)
    {
       if(_errorLed->state()==KLed::Off)
          _errorLed->on();
    }
    else
    {
       if(_errorLed->state()==KLed::On)
          _errorLed->off();
    }
}


void KBabelMW::displayedEntryChanged(const KBabel::DocPosition& pos)
{
   statusBar()->changeItem(i18n("Current: %1").arg(pos.item+1),ID_STATUS_CURRENT);
  _currentIndex = pos.item;
}

void KBabelMW::setNumberOfTotal(uint number)
{
   statusBar()->changeItem(i18n("Total: %1").arg(number),ID_STATUS_TOTAL);
}

void KBabelMW::setNumberOfFuzzies(uint number)
{
   statusBar()->changeItem(i18n("Fuzzy: %1").arg(number),ID_STATUS_FUZZY);
}

void KBabelMW::setNumberOfUntranslated(uint number)
{
   statusBar()->changeItem(i18n("Untranslated: %1").arg(number),ID_STATUS_UNTRANS);
}

void KBabelMW::hasFuzzyAfterwards(bool flag)
{
   a_nextFuzzy->setEnabled(flag);

   // check if there is  a fuzzy or untranslated afterwards
   if( flag || a_nextUntrans->isEnabled() )
   {
       a_nextFoU->setEnabled(true);
   }
   else
   {
       a_nextFoU->setEnabled(false);
   }

}

void KBabelMW::hasFuzzyInFront(bool flag)
{
   a_prevFuzzy->setEnabled(flag);

   // check if there is  a fuzzy or untranslated in front
   if( flag || a_prevUntrans->isEnabled() )
   {
       a_prevFoU->setEnabled(true);
   }
   else
   {
       a_prevFoU->setEnabled(false);
   }
}

void KBabelMW::hasUntranslatedAfterwards(bool flag)
{
   a_nextUntrans->setEnabled(flag);

   // check if there is a fuzzy or untranslated afterwards
   if( flag || a_nextFuzzy->isEnabled() )
   {
       a_nextFoU->setEnabled(true);
   }
   else
   {
       a_nextFoU->setEnabled(false);
   }
}

void KBabelMW::hasUntranslatedInFront(bool flag)
{
   a_prevUntrans->setEnabled(flag);

   // check if there is  a fuzzy or translated in front
   if( flag || a_prevFuzzy->isEnabled() )
   {
       a_prevFoU->setEnabled(true);
   }
   else
   {
       a_prevFoU->setEnabled(false);
   }
}



void KBabelMW::hasErrorAfterwards(bool flag)
{
   KAction* action=actionCollection()->action("go_next_error");
   action->setEnabled(flag);
}

void KBabelMW::hasErrorInFront(bool flag)
{
   KAction* action=actionCollection()->action("go_prev_error");
   action->setEnabled(flag);
}


void KBabelMW::enableBackHistory(bool on)
{
   KAction* action=actionCollection()->action("go_back_history");
   action->setEnabled(on);
}

void KBabelMW::enableForwardHistory(bool on)
{
   KAction* action=actionCollection()->action("go_forward_history");
   action->setEnabled(on);
}


void KBabelMW::prepareProgressBar(QString msg,int max)
{
   if(_statusbarTimer->isActive())
        _statusbarTimer->stop();

   _progressBar->show();
   _progressLabel->setText(" "+msg);
   _progressBar->setTotalSteps(max);
   _progressBar->setProgress(0);

}

void KBabelMW::clearProgressBar()
{
   _progressBar->setProgress(0);
   _progressBar->hide();
   _progressLabel->setText("      ");
}


void KBabelMW::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    _progressLabel->setText(" "+text);

    if(_statusbarTimer->isActive())
        _statusbarTimer->stop();

    _statusbarTimer->start(5000,true);
}

void KBabelMW::clearStatusbarMsg()
{
    _progressLabel->setText("");
}

void KBabelMW::changeCaption(const QString& text)
{
    // display the text on the caption
    setCaption(text + ( _project->filename () != KBabel::ProjectManager::defaultProjectName() ?
            " (" + _project->name() + ")" : "" /* KDE 3.4: i18n("(No project)")*/ )
        ,m_view->isModified());
}


void KBabelMW::showModified(bool on)
{
    // display the text on the caption
    setCaption(m_view->catalog()->package(),on);

    KAction *action=actionCollection()->action(
            KStdAction::stdName(KStdAction::Save));
    action->setEnabled(on);

    action=actionCollection()->action(KStdAction::stdName(KStdAction::Revert));
    action->setEnabled(on);
}


void KBabelMW::enableDefaults(bool readOnly)
{
    stateChanged( "readonly", readOnly ? StateNoReverse : StateReverse );
    stateChanged( "fileopened", StateNoReverse );

    if(readOnly)
       statusBar()->changeItem(i18n("RO"),ID_STATUS_READONLY);
    else
       statusBar()->changeItem(i18n("RW"),ID_STATUS_READONLY);
}

void KBabelMW::enableUndo(bool on)
{
   KAction* action=actionCollection()->action(KStdAction::stdName(KStdAction::Undo));
   action->setEnabled(on);
}

void KBabelMW::enableRedo(bool on)
{
   KAction* action=actionCollection()->action(KStdAction::stdName(KStdAction::Redo));
   action->setEnabled(on);
}

void KBabelMW::enableStop(bool flag)
{
   KAction* action=(KAction*)actionCollection()->action("stop_search");
   action->setEnabled(flag);
}

void KBabelMW::gettextHelp()
{
    QString error;
    KApplication::startServiceByDesktopName("khelpcenter",
                QString("info:/gettext"), &error);

    if(!error.isEmpty())
    {
        KMessageBox::sorry(this,i18n("An error occurred while "
                "trying to open the gettext info page:\n%1").arg(error));
    }
}

void KBabelMW::buildDictMenus()
{
   QPtrList<ModuleInfo> dictList = m_view->dictionaries();
   dictList.setAutoDelete(true);

   dictMenu->clear();
   selectionDictMenu->clear();
   configDictMenu->clear();
   editDictMenu->clear();
   aboutDictMenu->clear();

   ModuleInfo *info;
   for(info = dictList.first(); info !=0; info = dictList.next())
   {
      QString accel="Ctrl+Alt+%1";
      dictMenu->add(info->name,info->id, accel);

      accel=QString("Ctrl+%1");
      selectionDictMenu->add(info->name,info->id, accel);

      configDictMenu->add(info->name,info->id);
      aboutDictMenu->add(info->name,info->id);

      if(info->editable)
      {
         dictMenu->add(info->name,info->id);
      }
   }
}

void KBabelMW::updateCursorPosition(int line, int col)
{
    statusBar()->changeItem(i18n("Line: %1 Col: %2").arg(line+1).arg(col+1)
            ,ID_STATUS_CURSOR);
}


KBabelMW *KBabelMW::winForURL(const KURL& url, QString project)
{
    KBabelMW *kb=0;

    KBabelView *v = KBabelView::viewForURL(url,project);
    if(v)
    {
        QObject *p = v->parent();
        while(p && !p->inherits("KBabelMW"))
        {
            p = p->parent();
        }

        if(p)
            kb = static_cast<KBabelMW*>(p);
    }

    return kb;
}

KBabelMW *KBabelMW::emptyWin(QString project)
{
    KBabelMW *kb=0;

    KBabelView *v = KBabelView::emptyView(project);
    if(v)
    {
        QObject *p = v->parent();
        while(p && !p->inherits("KBabelMW"))
        {
            p = p->parent();
        }

        if(p)
            kb = static_cast<KBabelMW*>(p);
    }

    return kb;
}

void KBabelMW::spellcheckMoreFiles(QStringList filelist)
{
    if( filelist.isEmpty() ) return;
    _toSpellcheck = filelist;
    connect( m_view, SIGNAL( signalSpellcheckDone(int) ), this, SLOT( spellcheckDone(int)));
    spellcheckDone( KS_IGNORE ); // use something else than KS_STOP
}

void KBabelMW::spellcheckDone( int result)
{
    if( _toSpellcheck.isEmpty() || result == KS_STOP)
    {
        disconnect( m_view, SIGNAL( signalSpellcheckDone(int)), this, SLOT(spellcheckDone( int)));
        KMessageBox::information( this, i18n("MessageBox text", "Spellchecking of multiple files is finished."),
            i18n("MessageBox caption", "Spellcheck Done"));
    }
    else
    {
        QString file = _toSpellcheck.first();
        _toSpellcheck.pop_front();
        if( m_view->isModified() ) fileSave();
        open(KURL( file ), QString::null, false);
        kdDebug(KBABEL) << "Starting another spellcheck" << endl;
        QTimer::singleShot( 1, m_view, SLOT(spellcheckAllMulti()));
    }
}

void KBabelMW::initBookmarks(QPtrList<KBabelBookmark> list)
{
  bmHandler->setBookmarks(list);
}

void KBabelMW::slotAddBookmark()
{
    bmHandler->addBookmark(_currentIndex+1, m_view->catalog()->msgid(_currentIndex).first());
}

void KBabelMW::slotOpenBookmark(int index)
{
  DocPosition pos;
  pos.item=index-1;
  pos.form=0;
  m_view->gotoEntry(pos);
}

void KBabelMW::projectNew()
{
    KBabel::Project::Ptr p = KBabel::ProjectWizard::newProject();
    if( p )
    {
        _project = p;
        m_view->useProject(p);
        changeProjectActions(p->filename());
    }
}

void KBabelMW::projectOpen()
{
    QString oldproject = m_view->project();
    if( oldproject == KBabel::ProjectManager::defaultProjectName() )
    {
        oldproject = QString();
    }
    const QString file = KFileDialog::getOpenFileName(oldproject, QString::null, this);
    if (file.isEmpty())
    {
        return;
    }
    
    projectOpen (file);
}

void KBabelMW::projectOpenRecent(const KURL& url)
{
    projectOpen (url.path());
    KBabel::Project::Ptr p = KBabel::ProjectManager::open(url.path());
    if( p )
    {
        _project = p;
        m_view->useProject(p);
        changeProjectActions(url.path());
    }
}

void KBabelMW::projectOpen(const QString& file)
{
    QString oldproject = m_view->project();
    if( oldproject == KBabel::ProjectManager::defaultProjectName() )
    {
        oldproject = "";
    }
    if (file.isEmpty())
    {
        return;
    }
    KBabel::Project::Ptr p = KBabel::ProjectManager::open(file);
    if( p )
    {
        _project = p;
        m_view->useProject(p);
        changeProjectActions(file);
    }
    else 
    {
	KMessageBox::error( this, i18n("Cannot open project file\n%1").arg(file)
            , i18n("Project File Error"));
	_project = ProjectManager::open(KBabel::ProjectManager::defaultProjectName());
	m_view->useProject(_project);
	changeProjectActions(KBabel::ProjectManager::defaultProjectName());
    }
}

void KBabelMW::projectClose()
{
    QString defaultProject = KBabel::ProjectManager::defaultProjectName();
    m_view->useProject( KBabel::ProjectManager::open(defaultProject) );
    _project = ProjectManager::open(defaultProject);
    changeProjectActions(defaultProject);
}

void KBabelMW::changeProjectActions(const QString& project)
{
    bool def = (project == KBabel::ProjectManager::defaultProjectName());

    KAction* saveAction=(KAction*)actionCollection()->action( "project_close" );
    saveAction->setEnabled( ! def );
    
    if (!def)
    {
	addToRecentProjects(project);
    }
    
    // if there is a project dialog, delete it (we have a different project now
    if (_projectDialog) 
    {
	delete _projectDialog;
	_projectDialog = NULL;
    }
}

void KBabelMW::projectConfigure()
{
    if(!_projectDialog)
    {
        _projectDialog = new ProjectDialog(_project);
	connect (_projectDialog, SIGNAL (settingsChanged())
	    , m_view, SLOT (updateProjectSettings()));
    }

    int prefHeight=_projectDialog->height();
    int prefWidth=_projectDialog->width();
    int width=this->width();
    int height=this->height();

    int x=width/2-prefWidth/2;
    int y=height/2-prefHeight/2;

    _projectDialog->move(mapToGlobal(QPoint(x,y)));

    if(!_projectDialog->isVisible())
    {
       _projectDialog->show();
    }

    _projectDialog->raise();
    KWin::activateWindow(_projectDialog->winId());
}

void KBabelMW::addToRecentProjects(KURL url)
{
   if( url.isValid() && ! url.isEmpty() )
	a_recentprojects->addURL(url);
}


#include "kbabel.moc"
