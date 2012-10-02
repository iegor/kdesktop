/*
    cwbuglistcontainer.cpp  -  Container for the bug list

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

#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwidgetstack.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kdialog.h>
#if KDE_IS_VERSION( 3, 2, 90 )
#include <klistviewsearchline.h>
#endif
#include <kdebug.h>

#include "bugsystem.h"

#include "cwloadingwidget.h"
#include "buglvi.h"
#include "kbbprefs.h"
#include "kfind.h"

#include "cwbuglistcontainer.h"
#include <kfinddialog.h>

using namespace KBugBusterMainWindow;

CWBugListContainer::CWBugListContainer( QWidget *parent , const char * name )
  : QWidget( parent, name ), m_find(0), m_findItem(0)
{
    QBoxLayout *topLayout = new QVBoxLayout( this );
    topLayout->setSpacing( KDialog::spacingHint() );
    topLayout->setMargin( KDialog::marginHint() );

    m_listLabel = new QLabel( this );
    topLayout->addWidget( m_listLabel );
    topLayout->setStretchFactor( m_listLabel, 0 );

    QFont f = m_listLabel->font();
    f.setBold( true );
    m_listLabel->setFont( f );

    m_listStack = new QWidgetStack( this );

    // Create Outstanding Bugs listview
    m_listBugs = new KListView( m_listStack );

    topLayout->addWidget( new KListViewSearchLineWidget( m_listBugs, this ) );
    topLayout->addWidget( m_listStack );
    topLayout->setStretchFactor( m_listStack, 1 );

    m_listBugs->addColumn( i18n( "Number" ) );
    m_listBugs->addColumn( i18n( "Age" ) );
    m_listBugs->addColumn( i18n( "Title" ), 500 ); // so that the widthmode isn't "Maximum"
    m_listBugs->addColumn( i18n( "Status" ) );
    m_listBugs->addColumn( i18n( "Severity" ) );
    m_listBugs->addColumn( i18n( "Sender" ), 150 ); // idem. hardcoded widths suck a bit, but...
    m_listBugs->setAllColumnsShowFocus( true );
    m_listBugs->setColumnAlignment( 0, AlignRight );
    m_listBugs->setSorting( 0, false );
    m_listBugs->setShowSortIndicator( true );
    m_listBugs->setSelectionMode( QListView::Extended ); // needed for merging bugs

    m_listBugs->restoreLayout( KBBPrefs::instance()->config(), "BugListLayout" );

    connect( m_listBugs, SIGNAL( executed( QListViewItem * ) ),
             SLOT( execute( QListViewItem * ) ) );
    connect( m_listBugs, SIGNAL( returnPressed( QListViewItem * ) ),
             SLOT( execute( QListViewItem * ) ) );
    connect( m_listBugs, SIGNAL( currentChanged( QListViewItem * ) ),
             SLOT( changeCurrent( QListViewItem * ) ) );

    // Fill WidgetStack in Outstanding Bugs pane
    m_listLoading = new CWLoadingWidget( CWLoadingWidget::TopFrame,
                                         m_listStack );
    connect( m_listLoading, SIGNAL( clicked() ), SIGNAL( searchPackage() ) );

    m_listStack->addWidget( m_listBugs, 0 );
    m_listStack->addWidget( m_listLoading,  1 );

    setNoList();

    connect( BugSystem::self(), SIGNAL( bugListLoading( const Package &, const QString & ) ),
             SLOT( setLoading( const Package &, const QString & ) ) );
    connect( BugSystem::self(), SIGNAL( bugListLoading( const QString & ) ),
             SLOT( setLoading( const QString & ) ) );
    connect( BugSystem::self(), SIGNAL( bugListCacheMiss( const Package & ) ),
             SLOT( setCacheMiss( const Package & ) ) );
    connect( BugSystem::self(), SIGNAL( bugListCacheMiss( const QString & ) ),
             SLOT( setCacheMiss( const QString & ) ) );
    connect( BugSystem::self(), SIGNAL( commandQueued( BugCommand * ) ),
             SLOT( markBugCommand( BugCommand * ) ) );
    connect( BugSystem::self(), SIGNAL( commandCanceled( const QString & ) ),
             SLOT( clearCommand( const QString & ) ) );
}

CWBugListContainer::~CWBugListContainer()
{
    m_listBugs->saveLayout( KBBPrefs::instance()->config(), "BugListLayout" );
    KBBPrefs::instance()->writeConfig();
    delete m_find;
}

void CWBugListContainer::setBugList( const QString &label, const Bug::List &bugs )
{
    // List pane is invisible by default, make visible
    show();

    m_listBugs->clear();
    emit resetProgressBar();
    bool showClosed = KBBPrefs::instance()->mShowClosedBugs;
    bool showWishes = KBBPrefs::instance()->mShowWishes;
    uint noBugs = 0;
    uint noWishes = 0;

    for ( Bug::List::ConstIterator it = bugs.begin(); it != bugs.end(); ++it )
    {
        if ( ( *it ).status() != Bug::Closed || showClosed )
        {
            if ( ( *it ).severity() != Bug::Wishlist || showWishes )
                new BugLVI( m_listBugs, *it );

            if ( ( *it ).severity() != Bug::Wishlist )
                noBugs++;
            else
                noWishes++;
        }
    }

    m_listLabel->setText( i18n( "%1 (%2 bugs, %3 wishes)" ).arg( label ).arg( noBugs ).arg( noWishes ) );
    m_listStack->raiseWidget( 0 );
}

void CWBugListContainer::setBugList( const Package &package, const QString &component, const Bug::List &bugs )
{
    QString listLabel;
    if ( component.isEmpty() )
    {
        if ( package.components().count() > 1 )
            listLabel = i18n( "Product '%1', all components" ).arg( package.name() );
        else
            listLabel = i18n( "Product '%1'" ).arg( package.name() );
    }
    else
    {
        listLabel = i18n( "Product '%1', component '%2'" ).arg( package.name(), component );
    }

    setBugList( listLabel, bugs );
}

void CWBugListContainer::execute( QListViewItem *lvi )
{
    BugLVI *item = dynamic_cast<BugLVI *>( lvi );
    if( !item )
    {
        kdWarning() << "CWBugListContainer::execute() Selected bug "
                    << lvi->text( 0 )
                    << " is not a BugLVI! Ignoring event." << endl;
        return;
    }

    emit executed( item->bug() );
}

void CWBugListContainer::changeCurrent( QListViewItem *lvi )
{
    if( !lvi ) {
        emit currentChanged( Bug() );
        return;
    }

    BugLVI *item = dynamic_cast<BugLVI *>( lvi );
    if( !item )
    {
        kdWarning() << "CWBugListContainer::changeCurrent() Selected bug "
                    << lvi->text( 0 )
                    << " is not a BugLVI! Ignoring event." << endl;
        return;
    }

    emit currentChanged( item->bug() );
}

void CWBugListContainer::setNoList()
{
    m_listLabel->setText( i18n("Outstanding Bugs") );
    m_listLoading->setText( i18n( "Click here to select a product" ) );
    m_listStack->raiseWidget( 1 );
}

void CWBugListContainer::setLoading( const Package &package, const QString &component )
{
    if ( component.isEmpty() )
        setLoading( i18n( "Retrieving List of Outstanding Bugs for Product '%1'..." ).arg( package.name() ) );
    else
        setLoading( i18n( "Retrieving List of Outstanding Bugs for Product '%1' (Component %2)..." ).arg( package.name(), component ) );
}

void CWBugListContainer::setLoading( const QString &label )
{
    m_listLoading->setText( label );
    m_listStack->raiseWidget( 1 );
}

void CWBugListContainer::setCacheMiss( const Package &package )
{
    setCacheMiss( i18n( "Package '%1'" ).arg( package.name() ) );
}

void CWBugListContainer::setCacheMiss( const QString &label )
{
    m_listLoading->setText( i18n( "%1 is not available offline." ).arg( label ) );
    m_listStack->raiseWidget( 1 );
}

void CWBugListContainer::markBugCommand( BugCommand *cmd )
{
    BugLVI *item = (BugLVI *)m_listBugs->firstChild();
    while( item ) {
        if ( item->bug().number() == cmd->bug().number() ) {
            item->setCommandState( BugCommand::Queued );
            break;
        }
        item = (BugLVI *)item->nextSibling();
    }
    m_listBugs->triggerUpdate();
}

void CWBugListContainer::clearCommand( const QString &bug )
{
    BugLVI *item = (BugLVI *)m_listBugs->firstChild();
    while( item ) {
        if ( item->bug().number() == bug ) {
            item->setCommandState( BugCommand::None );
            break;
        }
        item = (BugLVI *)item->nextSibling();
    }
    m_listBugs->triggerUpdate();
}

void CWBugListContainer::searchBugByTitle( int options, const QString& pattern )
{
    m_find = new KFind( pattern, options, this );
    // Connect signals to code which handles highlighting
    // of found text.
    connect(m_find, SIGNAL( highlight( const QString &, int, int ) ),
            this, SLOT( searchHighlight( const QString &, int, int ) ) );
    connect(m_find, SIGNAL( findNext() ), this, SLOT( slotFindNext() ) );

    m_findItem = (BugLVI *)m_listBugs->firstChild();
    if ( options & KFindDialog::FromCursor && m_listBugs->currentItem() )
        m_findItem = (BugLVI *)m_listBugs->currentItem();

    slotFindNext();
}

// Note: if a 'find next' action is added, then one should also ensure
// that m_findItem never becomes dangling (i.e. clear it when clearing the listview).
void CWBugListContainer::slotFindNext()
{
    KFind::Result res = KFind::NoMatch;
    while( res == KFind::NoMatch && m_findItem ) {

        if ( m_find->needData() )
            m_find->setData( m_findItem->text(2) );

        // Let KFind inspect the text fragment, and display a dialog if a match is found
        res = m_find->find();

        if ( res == KFind::NoMatch ) {
            if ( m_find->options() & KFindDialog::FindBackwards )
                m_findItem = (BugLVI *)m_findItem->itemAbove();
            else
                m_findItem = (BugLVI *)m_findItem->itemBelow();
        }
    }
    if ( res == KFind::NoMatch ) // i.e. at end
       if ( m_find->shouldRestart() ) {
           m_findItem = (BugLVI *)m_listBugs->firstChild();
           slotFindNext();
       } else {
           delete m_find;
           m_find = 0L;
       }
}

void CWBugListContainer::searchHighlight( const QString &, int, int )
{
    if ( m_findItem ) {
        m_listBugs->clearSelection();
        m_listBugs->setSelected( m_findItem, true );
        m_listBugs->ensureItemVisible( m_findItem );
    }
}

QStringList CWBugListContainer::selectedBugs() const
{
    QStringList lst;
    BugLVI *item = (BugLVI *)m_listBugs->firstChild();
    while( item ) {
        if ( item->isSelected() )
            lst.append( item->bug().number() );
        item = (BugLVI *)item->nextSibling();
    }
    return lst;
}

#include "cwbuglistcontainer.moc"

/* vim: set et ts=4 sw=4 softtabstop=4: */
