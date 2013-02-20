/*
    Copyright (c) 2002-2003 Alexander Kellett <lypanov@kde.org>

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

#ifndef __CRASHES_PLUGIN_H
#define __CRASHES_PLUGIN_H

#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>

#include <kurl.h>
#include <klibloader.h>
#include <kparts/plugin.h>

class KHTMLPart;
class KActionMenu;

class CrashesPlugin : public KParts::Plugin
{
  Q_OBJECT

public:
  CrashesPlugin( QObject* parent, const char* name,
                   const QStringList & );
  ~CrashesPlugin();

protected slots:
  void slotAboutToShow();
  void slotClearCrashes();
  void slotItemSelected(int);
  void slotGroupSelected(int);
  void newBookmarkCallback( const QString &, const QCString &, const QString & );
  void endFolderCallback( );

private:
  int m_selectedItem;

  KHTMLPart* m_part;
  KActionMenu* m_pCrashesMenu;

  typedef QPair<QString,QCString> Crash;
  typedef QValueList<Crash> CrashesList;

  CrashesList m_crashesList;

  typedef QPair<int, int> CrashRange;
  typedef QValueList<CrashRange> CrashRangesList;

  CrashRangesList m_crashRangesList;

};

#endif
