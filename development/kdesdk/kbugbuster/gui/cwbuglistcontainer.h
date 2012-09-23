/*
    cwbuglistcontainer.h  -  Container for the bug list

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

#ifndef KBBMAINWINDOW_CWBUGLISTCONTAINER_H
#define KBBMAINWINDOW_CWBUGLISTCONTAINER_H

#include "package.h"
#include "bug.h"

#include <qwidget.h>

class KListView;
class KFind;
class BugCommand;
class BugLVI;

namespace KBugBusterMainWindow
{

class CWLoadingWidget;

/**
 * @author Martijn Klingens
 */
class CWBugListContainer : public QWidget
{
    Q_OBJECT

public:
    CWBugListContainer( QWidget* parent = 0, const char* name = 0 );
    ~CWBugListContainer();

    void setBugList( const Package &package, const QString &component, const Bug::List &bugs );

    /**
     * Overloaded method that takes a QString for the label. To be used when the
     * bug list doesn't belong to a package, liek search results
     */
    void setBugList( const QString &label, const Bug::List &bugs );

    void searchBugByTitle( int options, const QString& pattern );

    /** Return list of selected bugs in the listview. Used for merging. */
    QStringList selectedBugs() const;

public slots:
    void setNoList();
    void setLoading( const Package &package, const QString &component );
    void setLoading( const QString &label );
    void setCacheMiss( const Package &package );
    void setCacheMiss( const QString &label );
    void slotFindNext();

signals:
    void resetProgressBar();
    void searchPackage();

    void executed( const Bug & );
    void currentChanged( const Bug & );

private slots:
    void execute( QListViewItem * );
    void changeCurrent( QListViewItem * );

    void markBugCommand( BugCommand * );
    void clearCommand( const QString & );

    void searchHighlight( const QString &, int, int );

private:
    QLabel *m_listLabel;
    QWidgetStack *m_listStack;

    KListView *m_listBugs;
    KFind *m_find;
    BugLVI *m_findItem;

    CWLoadingWidget *m_listLoading;
};

}   // namespace

#endif

/* vim: set et ts=4 softtabstop=4 sw=4: */

