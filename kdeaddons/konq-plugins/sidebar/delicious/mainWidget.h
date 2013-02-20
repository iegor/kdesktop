// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
//////////////////////////////////////////////////////////////////////////
// mainWidget.h                                                         //
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

#ifndef _MAINWIDGET_H_
#define _MAINWIDGET_H_

#include "widget.h"

#include <kio/jobclasses.h>
#include <kparts/browserextension.h>

class QStringList;
class KURL;
class KConfig;

/**
 * Main widget of the del.icio.us bookmarks panel
 */
class MainWidget: public MainWidget_base
{
    Q_OBJECT
public:
    MainWidget( KConfig * config, QWidget * parent );
    ~MainWidget();

    /**
     * @return all the tags user has
     * (used in the DCOP iface)
     */
    QStringList tags() const;

    /**
     * @return all the (currently visible) bookmark (URLs)
     * (used in the DCOP iface)
     */
    QStringList bookmarks() const;

    /**
     * Set the internal URL to @p url
     */
    void setCurrentURL( const KURL & url );

public slots:
    /**
     * Show a dialog for adding a new bookmark
     */
    void slotNewBookmark();

private slots:
    /**
     * Start the job to get the list of tags
     */
    void slotGetTags();

    /**
     * Fill the Tags listview with job->data()
     * (parses the XML returned by the server)
     */
    void slotFillTags( KIO::Job * job );

    /**
     * Start the job to get the list of bookmarks
     * for checked tags
     */
    void slotGetBookmarks();

    /**
     * Fill the Bookmarks listview with job->data()
     * (parses the XML returned by the server)
     */
    void slotFillBookmarks( KIO::Job * job );

    /**
     * Handle clicking on a bookmark (KDE mode)
     */
    void slotBookmarkExecuted( QListViewItem * item );

    /**
     * Handle middle clicking a bookmark
     */
    void slotBookmarkClicked( int button, QListViewItem * item, const QPoint & pnt, int col );

    /**
     * Popup a tag context menu over @p item and position @pos
     */
    void slotTagsContextMenu( QListViewItem * item, const QPoint & pos, int col );

    /**
     * Popup a bookmark context menu over @p item and position @pos
     */
    void slotBookmarksContextMenu( QListViewItem * item, const QPoint & pos, int col );

    /**
     * Put a checkmark before all tags
     */
    void slotCheckAllTags();

    /**
     * Cancel the checkmark before all tags
     */
    void slotUncheckAllTags();

    /**
     * Toggle the checkmark before all tags
     */
    void slotToggleTags();

    /**
     * Starts a singleshot timer once an item (tag) has been toggled. Timer will update bookmarks.
     */
    void itemToggled();

    /**
     * Display a dialog box that allows renaming of tags
     */
    void slotRenameTag();

    /**
     * Delete the selected bookmark
     */
    void slotDeleteBookmark();

signals:
    /**
     * Emit a signal to the plugin interface that a @p url has been left-clicked
     */
    void signalURLClicked( const KURL & url, const KParts::URLArgs & args = KParts::URLArgs() );

    /**
     * Emit a signal to the plugin interface that a @p url has been mid-clicked
     */
    void signalURLMidClicked( const KURL & url, const KParts::URLArgs & args = KParts::URLArgs() );

private:
    /**
     * @return list of checked tags
     */
    QStringList checkedTags() const;

    /**
     * Save the tag list to the config file
     */
    void saveTags();

    /**
     * Load the tag list from the config file
     */
    void loadTags();

    QTimer *m_updateTimer;
    KURL m_currentURL;
    QStringList m_tags;
    KConfig * m_config;
};

#endif
