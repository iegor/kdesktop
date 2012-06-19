/*
    centralwidget.cpp  -  Central widget for the KBB main window

    copyright   : (c) 2001 by Martijn Klingens
    email       : klingens@kde.org

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include <qsplitter.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qlayout.h>

#include <kdialog.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <klistview.h>
#include <kinputdialog.h>

#include "kbbprefs.h"
#include "bugsystem.h"
#include "packagelvi.h"
#include "buglvi.h"
#include "msginputdialog.h"
#include "packageselectdialog.h"
#include "cwbugdetails.h"
#include "bugcommand.h"
#include "severityselectdialog.h"
#include "cwsearchwidget.h"
#include "cwbuglistcontainer.h"
#include "cwbugdetailscontainer.h"
#include "bugserver.h"

#include "centralwidget.h"
#include <kfiledialog.h>
#include <kmessagebox.h>
#include "loadallbugsdlg.h"

using namespace KBugBusterMainWindow;

CentralWidget::CentralWidget( const QCString &initialPackage,
                              const QCString &initialComponent,
                              const QCString &initialBug, QWidget *parent,
                              const char * name )
  : QWidget( parent, name )
{
    // Master layout
    ( new QVBoxLayout( this, 0,
                       KDialog::spacingHint() ) )->setAutoAdd( true );

    // Create QSplitter children
    m_vertSplitter = new QSplitter( QSplitter::Vertical, this );
    m_listPane     = new CWBugListContainer( m_vertSplitter );
    m_horSplitter  = new QSplitter( QSplitter::Horizontal,m_vertSplitter );
// The search pane isn't used. Should we remove the code?
    m_searchPane   = new CWSearchWidget( m_horSplitter );
    m_bugPane      = new CWBugDetailsContainer( m_horSplitter );

    m_searchPane->hide();
//    m_listPane->hide();

    m_searchPane->setSizePolicy( QSizePolicy( QSizePolicy::Minimum,
                                                    QSizePolicy::Minimum ) );
    m_horSplitter->setResizeMode( m_searchPane, QSplitter::FollowSizeHint );

    connect( m_listPane, SIGNAL( resetProgressBar() ),
             SIGNAL( resetProgressBar() ) );
    connect( m_bugPane, SIGNAL( resetProgressBar() ),
             SIGNAL( resetProgressBar() ) );

    // Start the proper jobs for loading the package lists
    connect( BugSystem::self(),
             SIGNAL( packageListAvailable( const Package::List & ) ),
             SLOT( updatePackageList( const Package::List & ) ) );
    connect( BugSystem::self(),
             SIGNAL( bugListAvailable( const Package &, const QString &, const Bug::List & ) ),
             SLOT( updateBugList( const Package &, const QString &, const Bug::List & ) ) );
    connect( BugSystem::self(),
             SIGNAL( bugListAvailable( const QString &, const Bug::List & ) ),
             SLOT( updateBugList( const QString &, const Bug::List & ) ) );
    connect( BugSystem::self(),
             SIGNAL( bugDetailsAvailable( const Bug &, const BugDetails & ) ),
             SLOT( updateBugDetails( const Bug &, const BugDetails & ) ) );

    connect( BugSystem::self(), SIGNAL( loadingError( const QString & ) ),
             SLOT( showLoadingError( const QString & ) ) );

    connect( m_bugPane, SIGNAL( signalCloseBug() ), SLOT( closeBug() ) );
    connect( m_bugPane, SIGNAL( signalCloseBugSilently() ), SLOT( closeBugSilently() ) );
    connect( m_bugPane, SIGNAL( signalReopenBug() ), SLOT( reopenBug() ) );
    connect( m_bugPane, SIGNAL( signalReassignBug() ), SLOT( reassignBug() ) );
    connect( m_bugPane, SIGNAL( signalTitleBug() ), SLOT( titleBug() ) );
    connect( m_bugPane, SIGNAL( signalSeverityBug() ), SLOT( severityBug() ) );
    connect( m_bugPane, SIGNAL( signalReplyBug() ), SLOT( replyBug() ) );
    connect( m_bugPane, SIGNAL( signalReplyPrivateBug() ), SLOT( replyPrivateBug() ) );

    connect( m_bugPane, SIGNAL( signalClearCommand() ), SLOT( clearCommand() ) );

    // Add the selection slots for the listviews
    connect( m_searchPane->m_searchPackages,
             SIGNAL( activated( const QString & ) ),
             SLOT( slotRetrieveBugList( const QString & ) ) );

    connect( m_listPane, SIGNAL( executed( const Bug & ) ),
             SLOT( slotRetrieveBugDetails( const Bug & ) ) );
    connect( m_listPane, SIGNAL( currentChanged( const Bug & ) ),
             SLOT( slotSetActiveBug( const Bug & ) ) );

    connect( m_listPane, SIGNAL( searchPackage() ), SIGNAL( searchPackage() ) );
    connect( m_bugPane, SIGNAL( searchBugNumber() ), SIGNAL( searchBugNumber() ) );

    m_bLoadingAllBugs = false;

    initialize( initialPackage, initialComponent, initialBug );
}

CentralWidget::~CentralWidget()
{
//    kdDebug() << "CentralWidget::~CentralWidget()" << endl;
}

void CentralWidget::initialize( const QString& p, const QString &c, const QString& b )
{
//    kdDebug() << "CentralWidget::initialize(): package: '" << p
//              << "' bug: '" << b << "'" << endl;

    BugServerConfig cfg = BugSystem::self()->server()->serverConfig();
    QString package = p.isEmpty() ? cfg.currentPackage() : p;
    QString bug = b.isEmpty() ? cfg.currentBug() : b;
    QString component = c.isEmpty() ? cfg.currentComponent() : c;

    m_listPane->setNoList();
    m_bugPane->setNoBug();

    BugSystem::self()->retrievePackageList();
    if ( !package.isEmpty() ) {
        m_currentPackage = BugSystem::self()->package( package );
        m_currentComponent = component;
        BugSystem::self()->retrieveBugList( m_currentPackage, m_currentComponent );

        if ( !bug.isEmpty() ) {
            m_currentBug = BugSystem::self()->bug( m_currentPackage,
                                                   m_currentComponent, bug );
            BugSystem::self()->retrieveBugDetails( m_currentBug );
        }
    } else {
        if ( !bug.isEmpty() ) {
            // ### bad way to instanciating a bug object! doesn't restore details!
            m_currentBug = Bug::fromNumber( bug ); // bug number specified on cmdline. Is it a problem that we don't have details ?
            BugSystem::self()->retrieveBugDetails( m_currentBug );
        }
    }
}

void CentralWidget::readConfig()
{
    m_horSplitter->setSizes( KBBPrefs::instance()->mSplitter2 );
    m_vertSplitter->setSizes( KBBPrefs::instance()->mSplitter1 );
}

void CentralWidget::writeConfig()
{
#if 0
    kdDebug() << "m_vertSplitter" << endl;
    QValueList<int> sizes = m_vertSplitter->sizes();
    QValueList<int>::ConstIterator it;
    for( it = sizes.begin(); it != sizes.end(); ++it ) {
        kdDebug() << "  " << (*it) << endl;
    }
#endif

    KBBPrefs::instance()->mSplitter1 = m_vertSplitter->sizes();
    KBBPrefs::instance()->mSplitter2 = m_horSplitter->sizes();

    BugServer *server = BugSystem::self()->server();
    server->serverConfig().setCurrentPackage( m_currentPackage.name() );
    server->serverConfig().setCurrentComponent( m_currentComponent );
    server->serverConfig().setCurrentBug( m_currentBug.number() );
}

void CentralWidget::slotRetrieveBugList( const QString &package )
{
    slotRetrieveBugList( package, QString::null );
}

void CentralWidget::slotRetrieveBugList( const QString &p, const QString &component )
{
    if ( p.isEmpty() ) return;

    Package package = m_packageList[ p ];

    if ( package.isNull() ) {
       // Invalid package, return
       return;
    }

    if ( ( package == m_currentPackage ) && ( m_currentComponent == component ) ) {
        return; // Nothing to do
    }

    m_currentComponent = component;
    m_currentPackage = package;

    BugSystem::self()->retrieveBugList( m_currentPackage, m_currentComponent );
}

QString CentralWidget::currentNumber() const
{
   if( m_currentBug.isNull() )
     return "";
   else
     return m_currentBug.number();
}

QString CentralWidget::currentTitle() const
{
   if( m_currentBug.isNull() )
     return "";
   else
     return m_currentBug.title();
}

void CentralWidget::slotRetrieveBugDetails( const Bug &bug )
{
    if( m_currentBug == bug )
        return; // Nothing to do

    m_currentBug = bug;
    BugSystem::self()->retrieveBugDetails( m_currentBug );
}

void CentralWidget::slotSetActiveBug( const Bug &bug )
{
    if( bug.isNull() )
    {
        return;
    }

    if( m_activeBug == bug )
        return; // Nothing to do

    m_activeBug = bug;
}

void CentralWidget::updatePackageList( const Package::List &pkgs )
{
    // ### needs proper implementation ;-)

    m_searchPane->m_searchPackages->clear();
    m_searchPane->m_searchPackages->completionObject()->clear();
//    m_bugPane->m_bugDetails->m_bugPackage->clear();
    emit resetProgressBar();

    Package::List::ConstIterator it = pkgs.begin();
    for ( ; it != pkgs.end(); ++it )
    {
        m_packageList[ ( *it ).name() ] = *it;
        m_searchPane->m_searchPackages->insertItem( ( *it ).name() );
        m_searchPane->m_searchPackages->
                  completionObject()->addItem( ( *it ).name() );
//        m_bugPane->m_bugDetails->m_bugPackage->insertItem( ( *it ).name() );
    }

/*
    if( m_bugPane->m_bugStack->id(
                m_bugPane->m_bugStack->visibleWidget() ) != 0 )
    {
        m_bugPane->m_bugDetails->m_bugPackage->setCurrentItem( -1 );
    }
*/
}

void CentralWidget::updateBugList( const Package &pkg, const QString &component, const Bug::List &bugs )
{
    m_listPane->setBugList( pkg, component, bugs );
}

void CentralWidget::updateBugList( const QString &label, const Bug::List &bugs )
{
    m_listPane->setBugList( label, bugs );
}

void CentralWidget::updateBugDetails( const Bug &bug, const BugDetails &bd )
{
    if ( !m_bLoadingAllBugs )
        m_bugPane->setBug( bug, bd );
}

void CentralWidget::slotReloadPackageList()
{
    BugSystem::self()->cache()->invalidatePackageList();
    BugSystem::self()->retrievePackageList();
}

void CentralWidget::slotReloadPackage()
{
    if (!m_currentPackage.isNull()) {
        BugSystem::self()->cache()->invalidateBugList( m_currentPackage, m_currentComponent );
        BugSystem::self()->retrieveBugList( m_currentPackage, m_currentComponent );
    }
}

void CentralWidget::slotLoadMyBugs()
{
    BugSystem::self()->retrieveMyBugsList();
}

void CentralWidget::slotReloadBug()
{
    if (!m_currentBug.isNull()) {
        BugSystem::self()->cache()->invalidateBugDetails( m_currentBug );
        BugSystem::self()->retrieveBugDetails( m_currentBug );
    }
}

void CentralWidget::updatePackage()
{
    if (!m_currentPackage.isNull()) {
        BugSystem::self()->retrieveBugList( m_currentPackage, m_currentComponent );
    }
}

void CentralWidget::slotExtractAttachments()
{
    if (!m_currentBug.isNull()) {
        // Grab bug details (i.e. full-text) from cache, then extract attachments from it
        BugDetails details = BugSystem::self()->cache()->loadBugDetails( m_currentBug );
        QValueList<BugDetails::Attachment> attachments = details.extractAttachments();
        if ( !attachments.isEmpty() )
        {
            QStringList fileList;
            for ( QValueList<BugDetails::Attachment>::Iterator it = attachments.begin() ; it != attachments.end() ; ++it )
            {
                // Handle duplicates
                if ( fileList.contains( (*it).filename ) )
                {
                    int n = 2; // looks stupid to have "blah" and "1-blah", start at 2
                    QString fn = QString::number(n) + '-' + (*it).filename;
                    while ( fileList.contains( fn ) )
                    {
                        ++n;
                        fn = QString::number(n) + '-' + (*it).filename;
                    }
                    (*it).filename = fn;
                }
                fileList += (*it).filename;
            }

            int res = KMessageBox::questionYesNoList( this,
                                                      i18n("Found the following attachments. Save?"),
                                                      fileList, QString::null, KStdGuiItem::save(), KStdGuiItem::dontSave() );
            if ( res == KMessageBox::No )
                return;
            QString dir = KFileDialog::getExistingDirectory( QString::null, this, i18n("Select Folder Where to Save Attachments") );
            if ( !dir.isEmpty() )
            {
                if ( !dir.endsWith( "/" ) )
                    dir += '/';
                for ( QValueList<BugDetails::Attachment>::Iterator it = attachments.begin() ; it != attachments.end() ; ++it )
                {
                    QString filename = m_currentBug.number() + '-' + (*it).filename;
                    QFile file( dir + filename );
                    if ( file.open( IO_WriteOnly ) )
                        file.writeBlock( (*it).contents );
                    else
                        kdError() << "Couldn't save attachment to " << filename << endl;
                    file.close();
                }
            }
        }
    }
}

void CentralWidget::mergeBugs()
{
    QStringList bugNumbers = m_listPane->selectedBugs();
    if ( bugNumbers.count() >= 2 ) {
      BugSystem::self()->queueCommand(
          new BugCommandMerge( bugNumbers, m_currentPackage ) );
    }
}

void CentralWidget::unmergeBugs()
{
    BugSystem::self()->queueCommand(
        new BugCommandUnmerge( m_currentBug, m_currentPackage ) );
}

void CentralWidget::closeBug()
{
    MsgInputDialog *dlg = new MsgInputDialog( MsgInputDialog::Close,
                                              m_currentBug, m_currentPackage,
					      m_bugPane->bugDetailsWidget()->selectedText(), this );
    dlg->show();
}

void CentralWidget::closeBugSilently()
{
    BugSystem::self()->queueCommand(
            new BugCommandCloseSilently( m_currentBug, m_currentPackage ) );
}

void CentralWidget::reopenBug()
{
    BugSystem::self()->queueCommand(
        new BugCommandReopen( m_currentBug, m_currentPackage ) );
}

void CentralWidget::reassignBug()
{
    PackageSelectDialog *dlg = new PackageSelectDialog( this );
    dlg->exec();

    dlg->setPackages( BugSystem::self()->packageList() );
    BugServerConfig cfg = BugSystem::self()->server()->serverConfig();
    dlg->setRecentPackages( cfg.recentPackages() );

    Package package = dlg->selectedPackage();

    if ( package.isNull() ) {
        return;
    }

    BugSystem::self()->queueCommand(
        new BugCommandReassign( m_currentBug, package.name(), m_currentPackage ) );
}

void CentralWidget::titleBug()
{
    bool ok = false;
    QString title = KInputDialog::getText( i18n("Change Bug Title"),
                                           i18n( "Please enter a new title:" ),
                                           m_currentBug.title(), &ok, this );
    if ( ok && !title.isEmpty() ) {
        BugSystem::self()->queueCommand(
            new BugCommandRetitle( m_currentBug, title, m_currentPackage ) );
    }
}

void CentralWidget::severityBug()
{
    SeveritySelectDialog *dlg = new SeveritySelectDialog( this );
    dlg->setSeverity( m_currentBug.severity() );
    int result = dlg->exec();
    if ( result == QDialog::Accepted ) {
        BugSystem::self()->queueCommand(
            new BugCommandSeverity( m_currentBug,
                                    dlg->selectedSeverityAsString(), m_currentPackage ) );
    }
}

void CentralWidget::replyBug()
{
    MsgInputDialog *dlg = new MsgInputDialog( MsgInputDialog::Reply,
                                              m_currentBug, m_currentPackage,
					      m_bugPane->bugDetailsWidget()->selectedText(), this );
    dlg->show();
}

void CentralWidget::replyPrivateBug()
{
    MsgInputDialog *dlg = new MsgInputDialog( MsgInputDialog::ReplyPrivate,
                                              m_currentBug, m_currentPackage,
					      m_bugPane->bugDetailsWidget()->selectedText(), this );
    dlg->show();
}

void CentralWidget::clearCommand()
{
  BugSystem::self()->clearCommands( m_currentBug.number() );
}

void CentralWidget::searchBugByTitle( int options, const QString& pattern )
{
  m_listPane->searchBugByTitle( options, pattern );
}

void CentralWidget::slotRetrieveAllBugDetails()
{
    m_bLoadingAllBugs = true;
    // Make a modal dialog to show the progress, and block usage of main window.
    LoadAllBugsDlg dlg( m_currentPackage, m_currentComponent );
    dlg.exec();
    m_bLoadingAllBugs = false;
}

void CentralWidget::showLoadingError( const QString &text )
{
    KMessageBox::error( this, text );
}

CWBugDetails *CentralWidget::bugDetailsWidget()
{
    return m_bugPane->bugDetailsWidget();
}

#include "centralwidget.moc"

/* vim: set et ts=4 sw=4 softtabstop=4: */
