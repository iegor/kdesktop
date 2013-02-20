/* This file is part of the KDE Project
   Copyright (C) 2001 Kurt Granroth <granroth@kde.org>
   Copyright (C) 2003 Rand2342 <rand2342@yahoo.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef __plugin_babelfish_h
#define __plugin_babelfish_h

#include <kparts/plugin.h>
#include <klibloader.h>

class KURL;
namespace KIO { class Job; }

class PluginBabelFish : public KParts::Plugin
{
  Q_OBJECT
public:
  PluginBabelFish( QObject* parent, const char* name,
	           const QStringList & );
  virtual ~PluginBabelFish();

public slots:
  void translateURL();
protected slots:
  void slotStarted( KIO::Job* );

private:
  KActionMenu* m_menu;
  KActionMenu* m_de;
  KActionMenu* m_en;
  KActionMenu* m_es;
  KActionMenu* m_fr;
  KActionMenu* m_it;
  KActionMenu* m_nl;
  KActionMenu* m_pt;
};

#endif
