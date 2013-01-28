/* This file is part of the KDE project
   Copyright (C) 2001 Holger Freyther <freyther@yahoo.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; version 2
   of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _kmetamenu_h
#define _kmetamenu_h

#include <qpopupmenu.h>
#include <qptrlist.h>

#include <kaction.h>

class QIconSet;
class QStringList;
class KConfig;
class KDirMenu;
class KIMContactMenu;
class KIMProxy;
class KURL;

class KMetaMenu : public QPopupMenu {
    Q_OBJECT
public:

     KMetaMenu( QWidget *parent, const KURL &url, const QString &text, 
             const QString &key, KIMProxy * imProxy = 0L );
     KMetaMenu();
     ~KMetaMenu();
     void writeConfig( const QString &path );
public slots:
     void slotFileChosen( const QString &path);
     void slotFastPath( );
     void slotBrowse( );
signals:
     void fileChosen( const QString &path );
     void contactChosen( const QString &uid );
private:
     KDirMenu *m_root;
     KDirMenu *m_home;
     KDirMenu *m_etc;
     KDirMenu *m_current;
     KIMContactMenu *m_contacts;
     KIMProxy *m_proxy;
     KAction *m_browse;
     QStringList list;
     KConfig *conf;
     QString group;
     QPtrList<KAction> actions;
};

#endif

