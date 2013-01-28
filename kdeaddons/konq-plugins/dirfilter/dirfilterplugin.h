/*
   Copyright (C) 2000, 2001, 2002 Dawit Alemayehu <adawit@kde.org>

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

#ifndef __DIR_FILTER_PLUGIN_H
#define __DIR_FILTER_PLUGIN_H

#include <qmap.h>
#include <qstringlist.h>


#include <kurl.h>
#include <kfileitem.h>
#include <klibloader.h>
#include <kparts/plugin.h>

class KActionMenu;
class KonqDirPart;
class KLineEdit;


namespace KParts
{
  struct URLArgs;
}

namespace KIO
{
  class Job;
}

class SessionManager
{
public:

  ~SessionManager ();
  static SessionManager* self ();
  QStringList restoreMimeFilters (const KURL& url) const;
  QString restoreTypedFilter(const KURL& url) const;
  void save (const KURL& url, const QStringList& filters);
  void save (const KURL& url, const QString& typedFilter);

  bool showCount;
  bool useMultipleFilters;

protected:

  QString generateKey (const KURL& url) const;
  void loadSettings ();
  void saveSettings ();

private:

  SessionManager ();

private:

  int m_pid;
  bool m_bSettingsLoaded;
  static SessionManager* m_self;
  QMap<QString,QStringList> m_filters;
  QMap<QString,QString> m_typedFilter;
};


class DirFilterPlugin : public KParts::Plugin
{
  Q_OBJECT

public:

  DirFilterPlugin (QObject* parent, const char* name, const QStringList &);
  ~DirFilterPlugin ();

protected:

  struct MimeInfo
  {
    MimeInfo()
    {
      id = 0;
      useAsFilter = false;
    }

    int id;
    bool useAsFilter;

    QString mimeType;
    QString iconName;
    QString mimeComment;

    QMap<QString, bool> filenames;
  };

  void loadSettings();
  void saveSettings();

private slots:
  void slotReset();
  void slotTimeout();
  void slotOpenURL();
  void slotShowPopup();
  void slotShowCount();
  void slotMultipleFilters();
  void slotItemSelected(int);
  void slotItemRemoved(const KFileItem *);
  void slotItemsAdded(const KFileItemList &);

private:
  KURL m_pURL;
  KonqDirPart* m_part;
  KActionMenu* m_pFilterMenu;

  KLineEdit *m_searchWidget;
  QMap<QString,MimeInfo> m_pMimeInfo;
  typedef QMap<QString,MimeInfo>::Iterator MimeInfoIterator;
};
#endif
