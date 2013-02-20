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

#ifndef _PLUGIN_KANT_HEADER_H
#define _PLUGIN_KANT_HEADER_H

#include <qstring.h>

#include <kate/plugin.h>
#include <kate/application.h>
#include <kate/view.h>
#include <kate/document.h>
#include <kate/documentmanager.h>
#include <kate/mainwindow.h>
#include <kate/viewmanager.h>

class PluginKateOpenHeader : public Kate::Plugin, Kate::PluginViewInterface
{
  Q_OBJECT

  public:
    PluginKateOpenHeader( QObject* parent = 0, const char* name = 0, const QStringList& = QStringList() );
    virtual ~PluginKateOpenHeader();

    void addView (Kate::MainWindow *win);
    void removeView (Kate::MainWindow *win);
  
  public slots:
    void slotOpenHeader ();
    void tryOpen( const KURL& url, const QStringList& extensions );   
    
  private:
  QPtrList<class PluginView> m_views;
};

#endif // _PLUGIN_KANT_OPENHEADER_H
