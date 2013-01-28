 /***************************************************************************
                          katefll_initplugin.h  -  description
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

class InitPluginKateFileListLoader : public Kate::InitPlugin
{
  Q_OBJECT

  public:
  	InitPluginKateFileListLoader (QObject * =0, const char * =0, const QStringList =QStringList());
	virtual ~InitPluginKateFileListLoader();
        virtual int actionsKateShouldNotPerformOnRealStartup();
	virtual int initKate();

  signals:
	void updateInit();

};

#endif // _PLUGIN_KATE_FLL_H
