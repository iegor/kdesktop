/* This file is part of the KDE project

   Copyright (C) 2003 Georg Robbers <Georg.Robbers@urz.uni-hd.de>

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

#ifndef _ARKPLUGIN_H_
#define _ARKPLUGIN_H_

#include <konq_popupmenu.h>
#include <kfileitem.h>
#include <kconfig.h>

class KAction;
class QSignalMapper;

class ArkMenu : public KonqPopupMenuPlugin {
  Q_OBJECT
public:
    ArkMenu( KonqPopupMenu *, const char *name, const QStringList & list );
    virtual ~ArkMenu();

public slots:
    void slotCompressAsDefault();
    void slotCompressAs( int pos );
    void slotAddTo( int pos );
    void slotAdd();
    void slotExtractHere();
    void slotExtractToSubfolders();
    void slotExtractTo();
    void slotPrepareCompAsMenu();
    void slotPrepareAddToMenu();

protected:
    void extMimeTypes();
    void compMimeTypes();
    void compressAs( const QStringList &name, const KURL & compressed );

    void stripExtension( QString & name );

private:
    QString m_name, m_ext;
    QValueList<KURL> m_urlList;
    QStringList m_urlStringList;
    KURL::List m_archiveList;
    QStringList m_archiveMimeTypes;
    QStringList m_extractMimeTypes;
    QStringList m_extensionList;
    KActionMenu * m_compAsMenu;
    KActionMenu * m_addToMenu;
    QSignalMapper * m_compAsMapper;
    QSignalMapper * m_addToMapper;
    KConfig * m_conf;
    QString m_dir; //contains the currect directory
};

#endif

