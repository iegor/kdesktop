/***************************************************************************
                          pybrowse_part.h  -  description
                             -------------------
    begin                : Tue Aug 28 2001
    copyright            : (C) 2001 by Christian Bird
    email                : chrisb@lineo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PYBROWSE_PART_H_
#define _PYBROWSE_PART_H_

#include <kate/application.h>
#include <kate/document.h>
#include <kate/mainwindow.h>
#include <kate/plugin.h>
#include <kate/view.h>
#include <kate/viewmanager.h>
#include <kate/toolviewmanager.h>
#include <kate/documentmanager.h>

#include <kdockwidget.h>
#include <klibloader.h>
#include <klocale.h>
#include <qstring.h>
#include "kpybrowser.h"

class PluginViewPyBrowse : public QObject, KXMLGUIClient
{
  Q_OBJECT

  friend class KatePluginPyBrowse;

  public:
    PluginViewPyBrowse (Kate::MainWindow *w);
    ~PluginViewPyBrowse ();

  public slots:
    void slotShowPyBrowser();
    void slotSelected(QString name, int line);
    void slotUpdatePyBrowser();

  private:
    Kate::MainWindow *win;
    QWidget *my_dock;
    KPyBrowser *kpybrowser;
};

class KatePluginPyBrowse : public Kate::Plugin, public Kate::PluginViewInterface
{
  Q_OBJECT

  public:
    KatePluginPyBrowse( QObject* parent = 0, const char* name = 0, const QStringList& = QStringList() );
    ~KatePluginPyBrowse();

    void addView(Kate::MainWindow *win);
    void removeView(Kate::MainWindow *win);

  private:
    QPtrList<PluginViewPyBrowse> m_views;
};

#endif
