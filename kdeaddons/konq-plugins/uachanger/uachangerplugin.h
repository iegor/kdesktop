/*
    Copyright (c) 2001 Dawit Alemayehu <adawit@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License (LGPL) as published by the Free Software Foundation;
    either version 2 of the License, or (at your option) any later
    version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef __UACHANGER_PLUGIN_H
#define __UACHANGER_PLUGIN_H

#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>

#include <kurl.h>
#include <klibloader.h>
#include <kparts/plugin.h>

class KHTMLPart;
class KActionMenu;
class KConfig;

namespace KIO
{
  class Job;
}

class UAChangerPlugin : public KParts::Plugin
{
  Q_OBJECT

public:
  UAChangerPlugin( QObject* parent, const char* name,
                   const QStringList & );
  ~UAChangerPlugin();

protected slots:
  void slotDefault();
  void parseDescFiles();
  void updateIOSlaves();

  void slotConfigure();
  void slotAboutToShow();
  void slotApplyToDomain();
  void slotItemSelected(int);
  void slotStarted(KIO::Job*);
  void slotReloadDescriptions();

protected:
  QString findTLD (const QString &hostname);
  QString filterHost (const QString &hostname);

private:
  void loadSettings();
  void saveSettings();

  int m_selectedItem;
  bool m_bApplyToDomain;
  bool m_bSettingsLoaded;

  KHTMLPart* m_part;
  KActionMenu* m_pUAMenu;
  KConfig* m_config;

  KURL m_currentURL;
  QString m_currentUserAgent;

  QStringList m_lstAlias;
  QStringList m_lstIdentity;

  typedef QValueList<int> BrowserGroup;
  typedef QMap<QString,BrowserGroup> AliasMap;
  typedef QMap<QString,QString> BrowserMap;

  typedef AliasMap::Iterator AliasIterator;
  typedef AliasMap::ConstIterator AliasConstIterator;

  BrowserMap m_mapBrowser;
  AliasMap m_mapAlias;
};

#endif
