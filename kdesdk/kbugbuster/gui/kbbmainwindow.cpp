/***************************************************************************
                       kbbmainwindow.cpp  -  description
                             -------------------
    copyright            : (C) 2001 by Martijn Klingens
    email                : klingens@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kbbmainwindow.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qwidgetstack.h>

#include <kaction.h>
#include <kbookmarkmenu.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kinputdialog.h>
#include <klineedit.h>
#include <klistview.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kstdguiitem.h>
#include <kedittoolbar.h>


#include "bugcommand.h"
#include "bugserver.h"
#include "bugserverconfig.h"
#include "bugsystem.h"
#include "centralwidget.h"
#include "cwbugdetails.h"
#include "kbbbookmarkmanager.h"
#include "kbbprefs.h"
#include "kfinddialog.h"
#include "packageselectdialog.h"
#include "preferencesdialog.h"

#define ID_STATUS_MSG 1

using namespace KBugBusterMainWindow;

class TextViewer : public KDialogBase
{
  public:
    TextViewer( const QString &title, QWidget *parent = 0 )
      : KDialogBase( Plain, title, Ok, Ok, parent, 0,
                     false )
    {
      QFrame *topFrame = plainPage();

      QBoxLayout *topLayout = new QVBoxLayout( topFrame );

      mTextView = new QTextEdit( topFrame );
      mTextView->setReadOnly( true );
      mTextView->setTextFormat( PlainText );
      topLayout->addWidget( mTextView );

      resize( 600, 400 );
    }

    void setText( const QString &text )
    {
      mTextView->setText( text );
    }

  private:
    QTextEdit *mTextView;
};

KBookmarkManager* KBBBookmarkManager::s_bookmarkManager;

KBBMainWindow::KBBMainWindow( const QCString &initialPackage,
                              const QCString &initialComponent,
                              const QCString &initialBug,
                              QWidget * , const char * name )
  : KMainWindow( 0, name ), mPreferencesDialog( 0 ), mResponseViewer( 0 ),
    mBugSourceViewer( 0 ), mPackageSelectDialog( 0 )
{
    BugSystem::self()->setCurrentServer( KBBPrefs::instance()->mCurrentServer );

    m_statusLabel = new QLabel( i18n( "Welcome to <b>KBugBuster</b>." ), statusBar() );
    m_statusLabel->setMaximumHeight( statusBar()->fontMetrics().height() + 6 );
    m_statusLabel->setIndent( KDialog::marginHint() / 2 );
    statusBar()->addWidget( m_statusLabel, 1 );

    m_mainWidget = new CentralWidget( initialPackage, initialComponent,
                                      initialBug, this );
    setCentralWidget( m_mainWidget );

    initActions();

    m_progressBar = new QProgressBar( 100, statusBar() );
    m_progressBar->setCenterIndicator( true );
    m_progressBar->setMinimumWidth( 150 );
    m_progressBar->setMaximumHeight( statusBar()->fontMetrics().height() + 6 );
    statusBar()->addWidget( m_progressBar );
    connect( m_mainWidget, SIGNAL( resetProgressBar() ),
             m_progressBar, SLOT( reset() ) );
    connect( m_mainWidget, SIGNAL( searchPackage() ),
             this, SLOT( searchPackage() ) );
    connect( m_mainWidget, SIGNAL( searchBugNumber() ),
             this, SLOT( searchBugNumber() ) );

    connect( BugSystem::self(), SIGNAL( infoMessage( const QString & ) ),
             SLOT( slotStatusMsg( const QString & ) ) );

    connect( BugSystem::self(), SIGNAL( infoMessage( const QString & ) ),
             SLOT( slotStatusMsg( const QString & ) ) );
    connect( BugSystem::self(), SIGNAL( infoPercent( unsigned long ) ),
             SLOT( slotSetPercent( unsigned long ) ) );

    m_mainWidget->readConfig();
}

KBBMainWindow::~KBBMainWindow()
{
//    kdDebug() << "KBBMainWindow::~KBBMainWindow()" << endl;
    delete m_pBookmarkMenu;

    m_mainWidget->writeConfig();

    KBBPrefs::instance()->writeConfig();
}

void KBBMainWindow::initActions()
{
    // Prepare and create XML GUI
    fileQuit          = KStdAction::quit( this,
                        SLOT( close() ), actionCollection() );
    fileQuit->setToolTip( i18n( "Quit KBugBuster" ) );

    new KAction( i18n("See &Pending Changes"), "contents", 0, this, SLOT( slotListChanges() ),
                 actionCollection(), "file_seechanges" );
    new KAction( i18n("&Submit Changes"), "mail_send", 0, this, SLOT( slotSubmit() ),
                 actionCollection(), "file_submit" );

    reloadpacklist = new KAction( i18n("Reload &Product List"), "reload", CTRL+Qt::Key_F5, m_mainWidget, SLOT( slotReloadPackageList() ),
                 actionCollection(), "reload_packagelist" );
    reloadpack= new KAction( i18n("Reload Bug &List (for current product)"), "reload", Qt::Key_F5, m_mainWidget, SLOT( slotReloadPackage() ),
                 actionCollection(), "reload_package" );
    reloadbug = new KAction( i18n("Reload Bug &Details (for current bug)"), "reload", SHIFT+Qt::Key_F5, m_mainWidget, SLOT( slotReloadBug() ),
                 actionCollection(), "reload_bug" );
    loadMyBugs = new KAction( i18n( "Load &My Bugs List" ), 0, m_mainWidget, SLOT( slotLoadMyBugs() ),
                 actionCollection(), "load_my_bugs" );
    reloadall = new KAction( i18n("Load All Bug Details (for current product)"), Qt::Key_F6, m_mainWidget, SLOT( slotRetrieveAllBugDetails() ), actionCollection(), "load_allbugs" );
    new KAction( i18n("Extract &Attachments"), "filesave", Qt::Key_F4, m_mainWidget, SLOT( slotExtractAttachments() ),
                 actionCollection(), "extract_attachments" );

    new KAction( i18n("Clear Cache"), 0, this, SLOT( clearCache() ),
                 actionCollection(), "clear_cache" );

    new KAction( i18n("&Search by Product..."), "goto", CTRL+Qt::Key_P, this,
                 SLOT( searchPackage() ), actionCollection(), "search_package" );
    new KAction( i18n("Search by Bug &Number..."), "filefind", CTRL+Qt::Key_N, this,
                 SLOT( searchBugNumber() ), actionCollection(), "search_bugnumber" );
    // For now "Description" searches by title. Maybe later we can have a
    // full-text search interfacing bugs.kde.org and rename the current one to "By Title".
    new KAction( i18n("Search by &Description...") ,"find", CTRL+Qt::Key_D, this,
                 SLOT( searchDescription() ), actionCollection(), "search_description" );

//    new KAction( i18n("&Merge"), "view_remove", CTRL+Qt::Key_M, m_mainWidget,
//                 SLOT( mergeBugs() ), actionCollection(), "cmd_merge" );
//    new KAction( i18n("&Unmerge"), "view_top_bottom", CTRL+SHIFT+Qt::Key_M, m_mainWidget,
//                 SLOT( unmergeBugs() ), actionCollection(), "cmd_unmerge" );
    new KAction( i18n("C&lose..."), "edittrash", CTRL+Qt::Key_L, m_mainWidget,
                 SLOT( closeBug() ), actionCollection(), "cmd_close" );
//    new KAction( i18n("Clos&e Silently"), "edittrash", CTRL+Qt::Key_E, m_mainWidget,
//                 SLOT( closeBugSilently() ), actionCollection(), "cmd_close_silently" );
    new KAction( i18n("Re&open"), "idea", CTRL+Qt::Key_O, m_mainWidget,
                 SLOT( reopenBug() ), actionCollection(), "cmd_reopen" );
//    new KAction( i18n("Re&assign..."), "folder_new", CTRL+Qt::Key_A, m_mainWidget,
//                 SLOT( reassignBug() ), actionCollection(), "cmd_reassign" );
//    new KAction( i18n("Change &Title..."), "text_under", CTRL+Qt::Key_T, m_mainWidget,
//                 SLOT( titleBug() ), actionCollection(), "cmd_title" );
//    new KAction( i18n("Change &Severity..."), "edit", CTRL+Qt::Key_S, m_mainWidget,
//                 SLOT( severityBug() ), actionCollection(), "cmd_severity" );
    new KAction( i18n("&Reply..."), "mail_replyall",CTRL+Qt::Key_R , m_mainWidget,
                 SLOT( replyBug() ), actionCollection(), "cmd_reply" );
    new KAction( i18n("Reply &Privately..."), "mail_reply", CTRL+Qt::Key_I, m_mainWidget,
                 SLOT( replyPrivateBug() ), actionCollection(), "cmd_replyprivate" );

    KStdAction::showMenubar(this, SLOT( slotToggleMenubar() ), actionCollection() );
#if KDE_IS_VERSION( 3, 1, 90 )
    createStandardStatusBarAction();
    setStandardToolBarMenuEnabled(true);
#endif

    m_disconnectedAction = new KToggleAction( i18n("&Disconnected Mode"), 0,
                                              this,
                                              SLOT( slotDisconnectedAction() ),
                                              actionCollection(),
                                              "settings_disconnected" );
    m_disconnectedAction->setChecked( BugSystem::self()->disconnected() );

    // Bookmarks menu
    m_pamBookmarks = new KActionMenu( i18n( "&Bookmarks" ), "bookmark", actionCollection(), "bookmarks" );
    m_pBookmarkMenu = new KBookmarkMenu( KBBBookmarkManager::self(), this, m_pamBookmarks->popupMenu(), actionCollection(), true );

    KStdAction::preferences( this, SLOT(preferences()), actionCollection() );

    KToggleAction *toggleTmp =  new KToggleAction( i18n("Show Closed Bugs"), "recycled", 0, this, SLOT( slotToggleDone() ),
                 actionCollection(), "cmd_toggle_done" );
#if KDE_IS_VERSION( 3, 2, 90 )
    toggleTmp->setCheckedState(i18n("Hide Closed Bugs"));
#endif
    toggleTmp->setChecked( KBBPrefs::instance()->mShowClosedBugs );

    toggleTmp =new KToggleAction( i18n("Show Wishes"), "bookmark", 0, this, SLOT( slotToggleWishes() ),
                 actionCollection(), "cmd_toggle_wishes" );
#if KDE_IS_VERSION( 3, 2, 90 )
    toggleTmp->setCheckedState(i18n("Hide Wishes"));
#endif
    toggleTmp->setChecked(KBBPrefs::instance()->mShowWishes);

    mSelectServerAction = new KSelectAction( i18n( "Select Server" ), 0, 0,
                                             this,
                                             SLOT( slotSelectServer() ),
                                             actionCollection(),
                                             "select_server" );

    setupSelectServerAction();

    if ( KBBPrefs::instance()->mDebugMode ) {
      new KAction( i18n("Show Last Server Response..."), 0 , this,
                   SLOT( showLastResponse() ), actionCollection(),
                   "debug_lastresponse" );
      new KAction( i18n("Show Bug HTML Source..."), 0 , this,
                   SLOT( showBugSource() ), actionCollection(),
                   "debug_showbugsource" );
    }

#if KDE_IS_VERSION( 3, 2, 90 )
    setupGUI( (ToolBar | Keys | StatusBar | Save | Create ), "kbugbusterui.rc" );
#else
    createGUI();
#endif
}

void KBBMainWindow::slotToggleMenubar()
{
    if ( !hasMenuBar() )
        return;

    if ( menuBar()->isVisible() )
        menuBar()->hide();
    else
        menuBar()->show();
}

void KBBMainWindow::setupSelectServerAction()
{
    QStringList servers;
    int current = -1;
    QValueList<BugServer *> serverList = BugSystem::self()->serverList();
    QValueList<BugServer *>::ConstIterator it;
    for ( it = serverList.begin(); it != serverList.end(); ++it ) {
      QString name = (*it)->serverConfig().name();
      servers.append( name );
      if ( name == KBBPrefs::instance()->mCurrentServer ) {
        current = servers.count() - 1;
      }
    }
    mSelectServerAction->setItems( servers );
    if ( current >= 0 ) {
      mSelectServerAction->setCurrentItem( current );
    }
}

QString KBBMainWindow::currentURL() const
{
  QString number=m_mainWidget->currentNumber();

  if (number.isEmpty())
    return "";
  else
    return "bug:"+number;
}

QString KBBMainWindow::currentTitle() const
{
  return "#"+m_mainWidget->currentNumber()+": "+m_mainWidget->currentTitle();
}

void KBBMainWindow::openBookmarkURL( const QString & url )
{
    if ( url.left(4)=="bug:" ) {
      QString bugnumber = url.mid(4);
      m_mainWidget->slotRetrieveBugDetails( Bug::fromNumber( bugnumber ) );
    }
}

// --- SLOT IMPLEMENTATIONS -------------------------------------------------

void KBBMainWindow::slotDisconnectedAction()
{
    BugSystem::self()->setDisconnected( m_disconnectedAction->isChecked() );

    bool enable = !m_disconnectedAction->isChecked();

    reloadpacklist->setEnabled( enable );
    reloadpacklist->setEnabled( enable );
    reloadpack->setEnabled( enable );
    reloadbug->setEnabled( enable );
    reloadall->setEnabled( enable );
    loadMyBugs->setEnabled( enable );
}

void KBBMainWindow::slotStatusMsg( const QString &text )
{
    // Change status message permanently
    m_statusLabel->setText( text );
}

void KBBMainWindow::slotSubmit()
{
    BugSystem::self()->sendCommands();
}

void KBBMainWindow::slotListChanges()
{
    QStringList list = BugSystem::self()->server()->listCommands();

    if (list.count() > 0)
    {
       int ret = KMessageBox::questionYesNoList( this, i18n("List of pending commands:"),
                                              list, QString::null, KStdGuiItem::clear(), KStdGuiItem::close() );
       if ( ret == KMessageBox::Yes )
       {
           // Ask for confirmation, it's too easy to click the wrong button in the above dlg box
           if ( KMessageBox::warningContinueCancel( this, i18n("Do you really want to delete all commands?"),
                               i18n("Confirmation Required"), KGuiItem( i18n("&Delete"), "editdelete"), "clearcommands", true)
                   == KMessageBox::Continue )
               BugSystem::self()->clearCommands();
       }
   }
   else
   {
       KMessageBox::information( this, i18n("There are no pending commands.") );
   }
}

void KBBMainWindow::slotSetPercent( unsigned long percent )
{
    // KIO::Job::percent() shouldn't return an unsigned long. - Frerich
    m_progressBar->setProgress( percent );
}

void KBBMainWindow::searchPackage()
{
    if ( !mPackageSelectDialog ) {
        mPackageSelectDialog = new PackageSelectDialog( this );
    }
    mPackageSelectDialog->setPackages( BugSystem::self()->packageList() );
    BugServerConfig cfg = BugSystem::self()->server()->serverConfig();
    QStringList recent = cfg.recentPackages();
    kdDebug() << "MainWindow RECENT: " << recent.join(",") << endl;
    mPackageSelectDialog->setRecentPackages( recent );

    mPackageSelectDialog->exec();
    Package package = mPackageSelectDialog->selectedPackage();

    if ( package.isNull() ) {
        return;
    }

    QString component = mPackageSelectDialog->selectedComponent();
    m_mainWidget->slotRetrieveBugList( package.name(), component );
}

void KBBMainWindow::searchBugNumber()
{
    bool ok = false;
    QString result = KInputDialog::getText( i18n("Search for Bug Number"),
                                            i18n("Please enter a bug number:"),
                                            QString::null, &ok, this );
    if ( ok ) {
      //Strip whitespace and # if needed
      result = result.stripWhiteSpace();
      if (result[0]=='#')
        result = result.mid(1);
    }

    if ( ok && !result.isEmpty()) {
        // ### bad way to instantiate a bug! doesn't get us the details!
        // Right - but do we need the details in this case ? There's no listview entry here... (DF)
        m_mainWidget->slotRetrieveBugDetails( Bug::fromNumber( result ) );
    }
}

void KBBMainWindow::searchDescription()
{
    kdDebug() << "KBBMainWindow::searchDescription()." << endl;
    //KMessageBox::sorry(this,i18n("searchDescription(): to be implemented."),i18n("Not implemented"));
    KFindDialog dlg( this );
    if ( dlg.exec() == KDialogBase::Accepted )
        m_mainWidget->searchBugByTitle( dlg.options(), dlg.pattern() );
}

bool KBBMainWindow::queryClose()
{
    if ( ! BugSystem::self()->server()->commandsPending() ) return true;

    int result = KMessageBox::warningYesNoCancel(this,i18n("There are unsent bug commands."
                                                           " Do you want to send them now?"), QString::null, i18n("Send"), i18n("Do Not Send"));
    if ( result == KMessageBox::Cancel ) return false;
    if ( result == KMessageBox::Yes ) {
        BugSystem::self()->sendCommands();
    }
    return true;
}

void KBBMainWindow::preferences()
{
    if (!mPreferencesDialog) {
      mPreferencesDialog  = new PreferencesDialog(this);
      connect( mPreferencesDialog, SIGNAL( configChanged() ),
               SLOT( setupSelectServerAction() ) );
      connect( mPreferencesDialog, SIGNAL( configChanged() ),
               m_mainWidget, SLOT( slotReloadPackage() ) );
    }
    mPreferencesDialog->show();
    mPreferencesDialog->raise();
}

void KBBMainWindow::updatePackage()
{
    m_mainWidget->updatePackage();
}

void KBBMainWindow::slotToggleDone()
{
  KBBPrefs::instance()->mShowClosedBugs=!(KBBPrefs::instance()->mShowClosedBugs);
  updatePackage();
}

void KBBMainWindow::slotToggleWishes()
{
  KBBPrefs::instance()->mShowWishes=!(KBBPrefs::instance()->mShowWishes);
  updatePackage();
}

void KBBMainWindow::slotSelectServer()
{
  m_mainWidget->writeConfig();

  QString currentServer = mSelectServerAction->currentText();

  BugSystem::self()->setCurrentServer( currentServer );

  m_mainWidget->initialize();
}

void KBBMainWindow::showLastResponse()
{
  if ( !mResponseViewer ) {
    mResponseViewer = new TextViewer( i18n("Last Server Response"), this );
  }

  mResponseViewer->setText( BugSystem::lastResponse() );

  mResponseViewer->show();
  mResponseViewer->raise();
}

void KBBMainWindow::showBugSource()
{
  if ( !mBugSourceViewer ) {
    mBugSourceViewer = new TextViewer( i18n("Bug HTML Source"), this );
  }

  mBugSourceViewer->setText( m_mainWidget->bugDetailsWidget()->source() );

  mBugSourceViewer->show();
  mBugSourceViewer->raise();
}

void KBBMainWindow::clearCache()
{
  BugSystem::self()->server()->cache()->clear();
}

#include "kbbmainwindow.moc"

/* vim: set et ts=4 sw=4 softtabstop=4: */

