/*
    cwbugdetailscontainer.h  -  Container for bug details

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

#ifndef KBBMAINWINDOW_CWBUGDETAILSCONTAINER_H
#define KBBMAINWINDOW_CWBUGDETAILSCONTAINER_H

#include "bug.h"
#include "bugdetails.h"

#include "cwbugdetailscontainer_base.h"

class BugCommand;

namespace KBugBusterMainWindow
{

class CWBugDetails;
class CWLoadingWidget;

/**
 * @author Martijn Klingens
 */
class CWBugDetailsContainer : public CWBugDetailsContainer_Base
{
    Q_OBJECT

public:
    CWBugDetailsContainer( QWidget* parent = 0, const char* name = 0 );
    ~CWBugDetailsContainer();

    void setBug( const Bug &, const BugDetails & );

    CWBugDetails *bugDetailsWidget() { return m_bugDetails; }

public slots:
    void setNoBug();
    void setLoading( const Bug &bug );
    void setCacheMiss( const Bug &bug );

    void enableButtons( const Bug & );

signals:
    void resetProgressBar();
    void searchBugNumber();

    void signalCloseBug();
    void signalCloseBugSilently();
    void signalReopenBug();
    void signalReassignBug();
    void signalTitleBug();
    void signalSeverityBug();
    void signalReplyBug();
    void signalReplyPrivateBug();

    void signalClearCommand();

private slots:
    void showCommands( const Bug & );
    void clearCommand( const QString & );
    void commandQueued( BugCommand * );

private:
    void hideCommands();

    CWLoadingWidget *m_bugLoading;
    CWBugDetails    *m_bugDetails;
    Bug              m_bug;
};

}   // namespace

#endif

/* vim: set et ts=4 softtabstop=4 sw=4: */

