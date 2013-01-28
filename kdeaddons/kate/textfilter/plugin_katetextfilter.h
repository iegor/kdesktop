 /***************************************************************************
                          plugin_katetextfilter.h  -  description
                             -------------------
    begin                : FRE Feb 23 2001
    copyright            : (C) 2001 by Joseph Wenninger
    email                : jowenn@bigfoot.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PLUGIN_KANT_TEXTFILTER_H
#define _PLUGIN_KANT_TEXTFILTER_H

#include <qstring.h>

#include <kate/plugin.h>
#include <kate/application.h>
#include <kate/view.h>
#include <kate/document.h>
#include <kate/documentmanager.h>
#include <kate/mainwindow.h>
#include <kate/viewmanager.h>

#include <kprocess.h>

class PluginKateTextFilter : public Kate::Plugin, public Kate::PluginViewInterface, public Kate::Command
{
  Q_OBJECT

  public:
    PluginKateTextFilter( QObject* parent = 0, const char* name = 0, const QStringList& = QStringList() );
    virtual ~PluginKateTextFilter();

    void addView (Kate::MainWindow *win);
    void removeView (Kate::MainWindow *win);

    // Kate::Command
    QStringList cmds ();
    bool exec (Kate::View *view, const QString &cmd, QString &msg);
    bool help (Kate::View *view, const QString &cmd, QString &msg);
  private:
    void runFilter( Kate::View *kv, const QString & filter );

  private:
    QString  m_strFilterOutput;
    KShellProcess * m_pFilterShellProcess;
    QPtrList<class PluginView> m_views;
    QStringList completionList;
  public slots:
    void slotEditFilter ();
    void slotFilterReceivedStdout (KProcess * pProcess, char * got, int len);
    void slotFilterReceivedStderr (KProcess * pProcess, char * got, int len);
    void slotFilterProcessExited (KProcess * pProcess);
    void slotFilterCloseStdin (KProcess *);
};

#endif // _PLUGIN_KANT_TEXTFILTER_H
// kate: space-indent on; indent-width 2; replace-tabs on; mixed-indent off;
