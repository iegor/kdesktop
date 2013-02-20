/* This file is part of the KDE project
 * Copyright (C) 2002 Rolf Magnus <ramagnus@kde.org>
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

#include "kfile_desktop.h"

#include <kurl.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <kdesktopfile.h>
#include <kmimetype.h>

typedef KGenericFactory<KDotDesktopPlugin> DotDesktopFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_desktop, DotDesktopFactory("kfile_desktop"))

KDotDesktopPlugin::KDotDesktopPlugin(QObject *parent, const char *name,
                                     const QStringList &preferredItems)
    : KFilePlugin(parent, name, preferredItems)
{
    kdDebug(7034) << ".desktop plugin\n";

    KFileMimeTypeInfo* info;
    KFileMimeTypeInfo::GroupInfo* group;
    KFileMimeTypeInfo::ItemInfo* item;
    
    info  = addMimeTypeInfo("application/x-desktop");
    group = addGroupInfo(info, "General", i18n("General"));

    item = addItemInfo(group, "Name", i18n("Name"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Name);
    item = addItemInfo(group, "Comment", i18n("Comment"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Description);

    addItemInfo(group, "Type", i18n("Type"), QVariant::String);

    addItemInfo(group, "Device", i18n("Device"), QVariant::String);
    addItemInfo(group, "Mount Point", i18n("Mount Point"), QVariant::String);
    addItemInfo(group, "File System", i18n("File System"), QVariant::String);
    addItemInfo(group, "Writable", i18n("Writable"), QVariant::Bool);
    
    addItemInfo(group, "File Type", i18n("File Type"), QVariant::String);
    addItemInfo(group, "Service Type", i18n("Service Type"), QVariant::String);
    addItemInfo(group, "Preferred Items", i18n("Preferred Items"), QVariant::String);    
    addItemInfo(group, "Link To", i18n("Link To"), QVariant::String);
}

bool KDotDesktopPlugin::readInfo( KFileMetaInfo& info, uint )
{
    if (  info.path().isEmpty() ) // remote file
        return false;

    KDesktopFile file(info.path(), true);
    
    QString s;
    
    KFileMetaInfoGroup group = appendGroup(info, "General");

    s = file.readName();
    if (!s.isEmpty()) appendItem(group, "Name", s);

    s = file.readComment();
    if (!s.isEmpty()) appendItem(group, "Comment", s);

    QString type = file.readType();
    if (type == "FSDevice")
    {
        appendItem(group, "Type", i18n("Device"));

        s = file.readDevice();
        if (!s.isEmpty()) appendItem(group, "Device", s);

        s = file.readEntry("MountPoint");
        if (!s.isEmpty()) appendItem(group, "Mount Point", s);

        s = i18n(file.readEntry("FSType").local8Bit());
        if (!s.isEmpty()) appendItem(group, "File System", s);

        appendItem(group, "Writable",
                    QVariant(!file.readBoolEntry("ReadOnly", true), 42));
        
    }
    else if (type == "Service")
    {
        appendItem(group, "Type", i18n("Service"));

        s = file.readEntry("MimeType");
        if (!s.isEmpty())
        {
            KMimeType::Ptr mt = KMimeType::mimeType(s);
            appendItem(group, "File Type", mt->comment());
        }
        
        QString sType = file.readEntry("ServiceTypes");
        appendItem(group, "Service Type", sType);

        if (sType == "KFilePlugin")
        {
            QStringList preferred = file.readListEntry("PreferredItems");
            appendItem(group, "Preferred Items", preferred);
        }
    }
    else if (type == "Link")
    {
        QString url = file.readPathEntry("URL");
        appendItem(group, "Link To", url);
    }

    return true;
}

#include "kfile_desktop.moc"
