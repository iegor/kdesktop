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

#ifndef _PLUGIN_TEST_H_
#define _PLUGIN_TEST_H_

#include <konq_popupmenu.h>

#include "kmetamenu.h"

class KAction;
class KURL;
class KTestMenu : public KonqPopupMenuPlugin {
  Q_OBJECT
public:
    KTestMenu (KonqPopupMenu *, const char *name, const QStringList &list);
    virtual ~KTestMenu( );
    KMetaMenu *meta_copy_mmu;
    KMetaMenu *meta_move_mmu;
    KonqPopupMenu *popup;

private:
    KAction *my_action;
	KIMProxy *m_imProxy;
	
public slots:
    void slotPopupMaeh( );
    void slotStartCopyJob(const QString &path );
    //void slotStartCopyJob(const KURL &url );
    void slotStartMoveJob(const QString &path );
    //void slotStartMoveJob(const KURL &url );
    void slotFileTransfer( const QString &uid );
    void slotPrepareMenu( );
};

 
#endif

