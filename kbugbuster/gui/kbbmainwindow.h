/*
    kbbmainwindow.h - KBugBuster's main window

    Copyright (c) 2001-2004 by Martijn Klingens      <klingens@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#ifndef KBBMAINWINDOW_H
#define KBBMAINWINDOW_H

#include <kapplication.h>
#include <kbookmarkmanager.h>
#include <kmainwindow.h>
#include <qmap.h>

#include "package.h"
#include "bug.h"
#include "bugdetails.h"

class KAction;
class KActionMenu;
class KBookmarkMenu;
class KToggleAction;
class KSelectAction;
class QLabel;
class QListViewItem;
class QProgressBar;
class PreferencesDialog;
class TextViewer;
class PackageSelectDialog;

namespace KBugBusterMainWindow
{
    class CentralWidget;
}

/**
 * @author Martijn Klingens
 */
class KBBMainWindow : public KMainWindow, virtual public KBookmarkOwner
{
    Q_OBJECT
  public:
    /**
     * construtor of KBugBusterApp, calls all init functions to create the application.
     */
    KBBMainWindow( const QCString &initialPackage = "",
                   const QCString &initialCpomponent = "",
                   const QCString &initialBug = "",
                   QWidget* parent = 0, const char* name = 0 );
    ~KBBMainWindow();

    /// Overloaded functions of KBookmarkOwner
    virtual void openBookmarkURL( const QString & _url );
    virtual QString currentTitle() const;
    virtual QString currentURL() const;

  public slots:
    /**
     * Event handlers for our KActions
     */
    void slotStatusMsg( const QString &text );
    void slotDisconnectedAction();
    void slotSubmit();
    void slotListChanges();
    void slotSetPercent( unsigned long percent );
    void slotSelectServer();

    void showLastResponse();
    void showBugSource();

    void clearCache();

    /**
     * Other event handlers
     */

    void searchPackage();
    void searchBugNumber();
    void searchDescription();

    void preferences();
    void updatePackage();
    void slotToggleDone();
    void slotToggleWishes();

  protected:
    virtual bool queryClose();

  protected slots:
    void setupSelectServerAction();
    void slotToggleMenubar();

  private:
    void initActions();

    /**
     * Our main widget
     */
    KBugBusterMainWindow::CentralWidget *m_mainWidget;

    /**
     * Used KActions
     */
    KAction *fileQuit;
    KAction *reloadpacklist;
    KAction *reloadpack;
    KAction *reloadbug;
    KAction *reloadall;
    KAction *loadMyBugs;
    KToggleAction *m_disconnectedAction;

    /**
     * Status bar label. We need this, because the default Qt version doesn't
     * support rich text in the messages
     */
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;

    PreferencesDialog *mPreferencesDialog;

    KActionMenu *m_pamBookmarks;
    KBookmarkMenu* m_pBookmarkMenu;

    KSelectAction *mSelectServerAction;

    TextViewer *mResponseViewer;
    TextViewer *mBugSourceViewer;

    PackageSelectDialog *mPackageSelectDialog;
};

#endif

/* vim: set et ts=4 softtabstop=4 sw=4: */

