// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
//////////////////////////////////////////////////////////////////////////
// mainWidget.cpp                                                       //
//                                                                      //
// Copyright (C)  2005  Lukas Tinkl <lukas@kde.org>                     //
//                                                                      //
// This program is free software; you can redistribute it and/or        //
// modify it under the terms of the GNU General Public License          //
// as published by the Free Software Foundation; either version 2       //
// of the License, or (at your option) any later version.               //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program; if not, write to the Free Software          //
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA            //
// 02111-1307, USA.                                                     //
//////////////////////////////////////////////////////////////////////////

#include "mainWidget.h"
#include "tagListItem.h"
#include "bookmarkListItem.h"

#include <qlistview.h>
#include <qdom.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qdatetime.h>

#include <kdebug.h>
#include <kio/job.h>
#include <krfcdate.h>
#include <klistview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kinputdialog.h>

MainWidget::MainWidget( KConfig * config, QWidget * parent )
    : MainWidget_base( parent ), m_config( config )
{
    loadTags();

    KIconLoader * il = KGlobal::iconLoader();

    btnRefreshTags->setIconSet( il->loadIconSet( "reload", KIcon::Small ) );
    btnRefreshBookmarks->setIconSet( il->loadIconSet( "reload", KIcon::Small ) );
    btnNew->setIconSet( il->loadIconSet( "bookmark_add", KIcon::Small ) );

    connect( ( QWidget * ) btnRefreshTags, SIGNAL( clicked() ),
             this, SLOT( slotGetTags() ) );

    connect( ( QWidget * ) btnRefreshBookmarks, SIGNAL( clicked() ),
             this, SLOT( slotGetBookmarks() ) );

    connect( ( QWidget * ) btnNew, SIGNAL( clicked() ),
             this, SLOT( slotNewBookmark() ) );

    connect( lvBookmarks, SIGNAL( executed( QListViewItem * ) ),
             this, SLOT( slotBookmarkExecuted( QListViewItem * ) ) );
    connect( lvBookmarks, SIGNAL( mouseButtonClicked ( int, QListViewItem *, const QPoint &, int ) ),
             this, SLOT( slotBookmarkClicked( int, QListViewItem *, const QPoint &, int ) ) );

    connect( lvTags, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint &, int ) ),
             this, SLOT( slotTagsContextMenu( QListViewItem *, const QPoint &, int ) ) );

    connect( lvBookmarks, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint &, int ) ),
             this, SLOT( slotBookmarksContextMenu( QListViewItem *, const QPoint &, int ) ) );

    m_updateTimer = new QTimer( this );
    connect( m_updateTimer, SIGNAL( timeout() ), SLOT( slotGetBookmarks() ) );

    slotGetTags();
}

MainWidget::~MainWidget()
{
    saveTags();
}

void MainWidget::setCurrentURL( const KURL & url )
{
    m_currentURL = url;
}

void MainWidget::slotGetTags()
{
    kdDebug() << k_funcinfo << endl;

    KIO::StoredTransferJob * job = KIO::storedGet( "http://del.icio.us/api/tags/get" );
    connect( job, SIGNAL( result( KIO::Job * ) ),
             this, SLOT( slotFillTags( KIO::Job * ) ) );
}

void MainWidget::slotFillTags( KIO::Job * job )
{
    kdDebug() << k_funcinfo << endl;

    if ( job->error() )
    {
        job->showErrorDialog( this );
        return;
    }

    lvTags->clear();
    m_tags.clear();

    // fill lvTags with job->data()
    QDomDocument doc;
    doc.setContent( static_cast<KIO::StoredTransferJob *>( job )->data() );
    QDomNodeList tags = doc.elementsByTagName( "tag" );
    for ( uint i = 0; i < tags.length(); ++i )
    {
        QDomElement tag = tags.item( i ).toElement();
        if ( !tag.isNull() )
        {
            TagListItem *item = new TagListItem( lvTags, tag.attribute( "tag" ), tag.attribute( "count" ).toInt() );
            m_tags.append( tag.attribute( "tag" ) );
            connect( item, SIGNAL( signalItemChecked( TagListItem * ) ), SLOT( itemToggled() ) );
        }
    }
}

void MainWidget::slotGetBookmarks()
{
    KURL url( "http://del.icio.us/api/posts/recent" );
    url.setQuery( "tag=" + checkedTags().join( " " ) );

    kdDebug() << k_funcinfo << url.url() << endl;

    KIO::StoredTransferJob * job = KIO::storedGet( url );
    connect( job, SIGNAL( result( KIO::Job * ) ),
             this, SLOT( slotFillBookmarks( KIO::Job * ) ) );
}

void MainWidget::slotFillBookmarks( KIO::Job * job )
{
    kdDebug() << k_funcinfo << endl;

    if ( job->error() )
    {
        job->showErrorDialog( this );
        return;
    }

    lvBookmarks->clear();

    // fill lvBookmarks with job->data()
    QDomDocument doc;
    doc.setContent( static_cast<KIO::StoredTransferJob *>( job )->data() );
    QDomNodeList posts = doc.elementsByTagName( "post" );

    for ( uint i = 0; i < posts.length(); ++i )
    {
        QDomElement post = posts.item( i ).toElement();
        if ( !post.isNull() )
        {
            new BookmarkListItem( lvBookmarks, post.attribute( "href" ), post.attribute( "description" ),
                                  KRFCDate::parseDateISO8601( post.attribute( "time" ) ) );
        }
    }
}

QStringList MainWidget::checkedTags() const
{
    QListViewItemIterator it( lvTags, QListViewItemIterator::Visible | QListViewItemIterator::Checked );

    QStringList tmp;

    while ( it.current() )
    {
        tmp.append( it.current()->text( 0 ) );
        ++it;
    }

    return tmp;
}

void MainWidget::slotBookmarkExecuted( QListViewItem * item )
{
    BookmarkListItem * bookmark = static_cast<BookmarkListItem *>( item );
    if ( bookmark )
    {
        kdDebug() << k_funcinfo << "Clicked bookmark URL: " << bookmark->url()  << endl;
        emit signalURLClicked( bookmark->url() );
    }
}

void MainWidget::slotBookmarkClicked( int button, QListViewItem * item, const QPoint &, int )
{
    BookmarkListItem * bookmark = static_cast<BookmarkListItem *>( item );
    if ( bookmark && button == Qt::MidButton ) // handle middle click
    {
        kdDebug() << k_funcinfo << "Middle clicked bookmark URL: " << bookmark->url()  << endl;
        emit signalURLMidClicked( bookmark->url() );
    }
}

QStringList MainWidget::tags() const
{
    return m_tags;
}

QStringList MainWidget::bookmarks() const
{
    QListViewItemIterator it( lvBookmarks );

    QStringList tmp;

    while ( it.current() )
    {
        tmp.append( static_cast<BookmarkListItem *>( it.current() )->url().url() );
        ++it;
    }

    return tmp;
}

void MainWidget::slotTagsContextMenu( QListViewItem *, const QPoint & pos, int )
{
    if ( lvTags->childCount() == 0 )
        return;

    QPopupMenu * tagMenu = new QPopupMenu( this );
    Q_CHECK_PTR( tagMenu );

    tagMenu->insertItem( i18n( "Check All" ), this, SLOT( slotCheckAllTags() ) );
    tagMenu->insertItem( i18n( "Uncheck All" ), this, SLOT( slotUncheckAllTags() ) );
    tagMenu->insertItem( i18n( "Toggle All" ), this, SLOT( slotToggleTags() ) );
    tagMenu->insertSeparator();
    tagMenu->insertItem( KGlobal::iconLoader()->loadIconSet( "edit", KIcon::Small ),
                         i18n( "Rename Tag..." ), this, SLOT( slotRenameTag() ) );

    tagMenu->exec( pos );
}

void MainWidget::slotCheckAllTags()
{
    QListViewItemIterator it( lvTags );
    while ( it.current() )
    {
        QCheckListItem * item = static_cast<QCheckListItem *>( *it );
        if ( item )
            item->setOn( true );
        ++it;
    }
}

void MainWidget::slotUncheckAllTags()
{
    QListViewItemIterator it( lvTags );
    while ( it.current() )
    {
        QCheckListItem * item = static_cast<QCheckListItem *>( *it );
        if ( item )
            item->setOn( false );
        ++it;
    }
}

void MainWidget::slotToggleTags()
{
    QListViewItemIterator it( lvTags );
    while ( it.current() )
    {
        QCheckListItem * item = static_cast<QCheckListItem *>( *it );
        if ( item )
            item->setOn( !item->isOn() );
        ++it;
    }
}

void MainWidget::itemToggled()
{
    m_updateTimer->start( 2000, true );
}

void MainWidget::slotNewBookmark()
{
    emit signalURLClicked( "http://del.icio.us/post/?url=" + m_currentURL.url() );
}

void MainWidget::saveTags()
{
    m_config->writeEntry( "Tags", m_tags );
}

void MainWidget::loadTags()
{
    m_tags = m_config->readListEntry( "Tags" );
}

void MainWidget::slotRenameTag()
{
    TagListItem * tag = static_cast<TagListItem *>( lvTags->currentItem() );
    if ( tag )
    {
        QString oldName = tag->name();
        QString newName = KInputDialog::getText( i18n( "Rename Tag" ), i18n( "Provide a new name for tag '%1':" ).arg( oldName ) );
        if ( !newName.isEmpty() )
        {
            KURL url( "http://del.icio.us/api/tags/rename" );
            url.addQueryItem( "old", oldName );
            url.addQueryItem( "new", newName );
            KIO::get( url );    // rename the tag

            tag->setName( newName );
        }
    }
}

void MainWidget::slotBookmarksContextMenu( QListViewItem *, const QPoint & pos, int )
{
    if ( lvBookmarks->childCount() == 0 )
        return;

    QPopupMenu * menu = new QPopupMenu( this );
    Q_CHECK_PTR( menu );

    menu->insertItem( KGlobal::iconLoader()->loadIconSet( "editdelete", KIcon::Small ),
                      i18n( "Delete Bookmark" ), this, SLOT( slotDeleteBookmark() ) );

    menu->exec( pos );
}

void MainWidget::slotDeleteBookmark()
{
    BookmarkListItem * bookmark = static_cast<BookmarkListItem *>( lvBookmarks->currentItem() );
    if ( bookmark )
    {
        int result = KMessageBox::warningContinueCancel( this, i18n( "Do you really want to remove the bookmark\n%1?" ).arg( bookmark->desc() ),
                                                         i18n( "Delete Bookmark" ), KStdGuiItem::del() );

        if ( result == KMessageBox::Continue )
        {
            KURL url( "http://del.icio.us/api/posts/delete" );
            url.addQueryItem( "url", bookmark->url().url() );
            kdDebug() << k_funcinfo << url << endl;
            KIO::get( url );

            delete bookmark;

            slotGetTags();      // re-read the tags
        }
    }
}

#include "mainWidget.moc"
