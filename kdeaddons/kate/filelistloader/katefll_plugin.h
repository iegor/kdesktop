 /***************************************************************************
                          katefll_plugin.h  -  description
                             -------------------
    begin                : FRE July 12th 2002
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/
 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PLUGIN_KATE_FLL_H
#define _PLUGIN_KATE_FLL_H

#include <qstring.h>

#include <kate/plugin.h>
#include <kate/application.h>
#include <kate/view.h>
#include <kate/document.h>
#include <kate/documentmanager.h>
#include <kate/mainwindow.h>
#include <kate/viewmanager.h>

#include <kstandarddirs.h>

class PluginKateFileListLoader : public Kate::Plugin, Kate::PluginViewInterface
{
  Q_OBJECT

  public:
    PluginKateFileListLoader (QObject * =0, const char * =0, const QStringList =QStringList());
    virtual ~PluginKateFileListLoader();

    void addView (Kate::MainWindow *win);
    void removeView (Kate::MainWindow *win);

  public slots:
    void updateInit();

  protected:
    void addURLToList(const KURL& url);
    void removeURLFromList(const KURL& url);

  protected slots:
    void slotOpenList();
    void slotOpenList(const KURL& url);
    void slotSaveList();
    void slotSaveListAs();
    void save();

  private:
    QPtrList<class PluginView> m_views;
    KRecentFilesAction *m_recentFiles;
    KConfig* m_config;
    KURL m_oldInitURL;
    KURL m_saveURL;
    bool m_saveAs;
};

#endif // _PLUGIN_KATE_FLL_H
