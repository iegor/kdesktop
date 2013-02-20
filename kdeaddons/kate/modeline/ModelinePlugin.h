 /***************************************************************************
                          ModelinePlugin.h  -  description
                             -------------------
    begin                : Mon Apr 1 2002
    copyright            : (C) 2002 by John Firebaugh
    email                : jfirebaugh@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ModelinePlugin_H_
#define _ModelinePlugin_H_

#include <kate/plugin.h>

class ModelinePlugin : public Kate::Plugin, Kate::PluginViewInterface
{
	Q_OBJECT
	
public:
	ModelinePlugin( QObject* parent = 0, const char* name = 0, const QStringList &args = QStringList() );
	virtual ~ModelinePlugin();
	
	void addView (Kate::MainWindow *win);
  void removeView (Kate::MainWindow *win);
private slots:
	void applyModeline();
  
private:
  QPtrList<class PluginView> m_views;
};

#endif // _ModelinePlugin_H_
