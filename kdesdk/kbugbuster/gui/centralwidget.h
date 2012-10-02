/*
    centralwidget.h  -  Central widget for the KBB main window

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

#ifndef KBBMAINWINDOW_CENTRALWIDGET_H
#define KBBMAINWINDOW_CENTRALWIDGET_H

#include <qwidget.h>

#include "package.h"
#include "bug.h"
#include "bugdetails.h"

class QSplitter;
class QListViewItem;

namespace KBugBusterMainWindow
{

class CWSearchWidget;
class CWBugListContainer;
class CWBugDetailsContainer;
class CWBugDetails;

/**
 * @author Martijn Klingens
 */
class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    CentralWidget( const QCString &initialPackage,
                   const QCString &initalComponent,const QCString& initialBug,
                   QWidget* parent = 0, const char* name = 0 );
    ~CentralWidget();

    void initialize( const QString &initialPackage = QString::null,
                     const QString &initalComponent = QString::null,
                     const QString &initialBug = QString::null );
 
    void readConfig();
    void writeConfig();

    void searchBugByTitle( int options, const QString& pattern );

    virtual QString currentNumber() const;
    virtual QString currentTitle() const;

    void updatePackage();

    CWBugDetails *bugDetailsWidget();

public slots:
    void slotRetrieveBugList( const QString &package, const QString &component );
    void slotRetrieveBugList( const QString &package );
    void slotRetrieveBugDetails( const Bug & );
    void slotSetActiveBug( const Bug & );
    void slotRetrieveAllBugDetails();

    void updatePackageList( const Package::List &pkgs );
    void updateBugList( const Package &pkg, const QString &component, const Bug::List &bugs );
    void updateBugList( const QString &label, const Bug::List &bugs );
    void updateBugDetails( const Bug &, const BugDetails & );

    void slotReloadPackageList();
    void slotReloadPackage();
    void slotReloadBug();
    void slotExtractAttachments();

    /**
     * Load the bugs the user reported himself, or for which he is the assigned to person
     */
    void slotLoadMyBugs();

    void mergeBugs();
    void unmergeBugs();

    void closeBug();
    void closeBugSilently();
    void reopenBug();
    void titleBug();
    void severityBug();
    void replyBug();
    void replyPrivateBug();
    void reassignBug();

    void clearCommand();

signals:
    void resetProgressBar();
    void searchPackage(); // when clicking on the initial package widget
    void searchBugNumber(); // when clicking on the initial bug-details widget

protected slots:
    void showLoadingError( const QString & );

private:
    CWSearchWidget        *m_searchPane;
    CWBugListContainer    *m_listPane;
    CWBugDetailsContainer *m_bugPane;

    QSplitter *m_vertSplitter;
    QSplitter *m_horSplitter;

    /**
     * Other status info
     */
    Package m_currentPackage;
    QString m_currentComponent;
    Bug     m_currentBug;

    QMap<QString, Package> m_packageList;

    /**
     * We do multi-select, but the close/reopen buttons are per-item and
     * on highlight instead of on execute! Hence this different member
     */
    Bug m_activeBug;

    // For "load all bugs"
    bool m_bLoadingAllBugs;
};

}   // namespace

#endif // KBBMAINWINDOW_CENTRALWIGET_H

/* vim: set et ts=4 softtabstop=4 sw=4: */

