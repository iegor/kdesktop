/* This file is part of the KDE project
 * Copyright (C) 2002 Simon MacMullen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <config.h>
#include "kfile_folder.h"

#include <kgenericfactory.h>
#include <kio/global.h>

#include <qfileinfo.h>
#include <qdir.h>

typedef KGenericFactory<KFolderPlugin> KFolderFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_folder, KFolderFactory( "kfile_folder" ))

KFolderPlugin::KFolderPlugin(QObject *parent, const char *name,
                       const QStringList &args)
    
    : KFilePlugin(parent, name, args)
{
    KFileMimeTypeInfo* info = addMimeTypeInfo( "inode/directory" );
    
    KFileMimeTypeInfo::GroupInfo* group = 0L;

    group = addGroupInfo(info, "FolderInfo", i18n("Folder Information"));

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Items", i18n("Items"), QVariant::Int);
    item = addItemInfo(group, "Size", i18n("Size"), QVariant::Int);
    setUnit(item, KFileMimeTypeInfo::Bytes);
}

bool KFolderPlugin::readInfo( KFileMetaInfo& info, uint /*what*/)
{
    KFileMetaInfoGroup group = appendGroup(info, "FolderInfo");

    if (info.path().isEmpty())
       return false;
    
    QDir dir;
    if ( !dir.cd(info.path()) )
       return false;

    const QPtrList<QFileInfo> *fileList = dir.entryInfoList();
    if (!fileList)
       return false;

    QPtrListIterator<QFileInfo> list = QPtrListIterator<QFileInfo>(*fileList);
    
    QFileInfo* file;
    int items = 0;
    KIO::filesize_t bytes = 0;
    
    while ( (file = list.current()) != 0 ) {
        ++list;
        if (!file->fileName().startsWith(".")) {
            items++;
            bytes += file->size();
        }
    }

    appendItem(group, "Items", items);
    appendItem(group, "Size", bytes);
    
    return true;
}

#include "kfile_folder.moc"
