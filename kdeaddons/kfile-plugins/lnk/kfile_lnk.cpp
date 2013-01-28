/***************************************************************************
 *   Copyright (C) 2004 by Martin Koller                                   *
 *   m.koller@surfeu.at                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

// format of a .lnk file retrieved from:
// http://www.i2s-lab.com/Papers/The_Windows_Shortcut_File_Format.pdf

#include <config.h>
#include "kfile_lnk.h"
#include "read_lnk.h"

#include <kgenericfactory.h>

//--------------------------------------------------------------------------------

typedef KGenericFactory<lnkPlugin> lnkFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_lnk, lnkFactory( "kfile_lnk" ))

//--------------------------------------------------------------------------------

lnkPlugin::lnkPlugin(QObject *parent, const char *name,
                       const QStringList &args)
    : KFilePlugin(parent, name, args)
{
  KFileMimeTypeInfo* info = addMimeTypeInfo( "application/x-win-lnk" );

  // our new group
  KFileMimeTypeInfo::GroupInfo* group = 0;
  group = addGroupInfo(info, "lnkInfo", i18n("Windows Link File Information"));

  KFileMimeTypeInfo::ItemInfo* item;

  // our new items in the group
  item = addItemInfo(group, "TargetSize", i18n("Size of Target"), QVariant::Int);
  setUnit(item, KFileMimeTypeInfo::Bytes);

  addItemInfo(group, "Where", i18n("Location"), QVariant::String);
  addItemInfo(group, "PointsTo", i18n("Points To"), QVariant::String);
  addItemInfo(group, "Description", i18n("Description"), QVariant::String);
}

//--------------------------------------------------------------------------------

bool lnkPlugin::readInfo( KFileMetaInfo& info, uint /*what*/)
{
  KFileMetaInfoGroup group = appendGroup(info, "lnkInfo");

  LNKInfo lnkInfo;
  bool ret = readLNK(info.url(), lnkInfo);

  if ( ! ret ) return false;

  if ( ! lnkInfo.isDirectory )  // not a directory
    appendItem(group, "TargetSize", lnkInfo.fileSize);

  if ( ! lnkInfo.isFileOrDir )  // points to something != file or directory
    return false;

  if ( ! lnkInfo.isNetworkPath )
  {
    appendItem(group, "Where", i18n("on Windows disk: %1").arg(lnkInfo.volumeName));  // volume label
    appendItem(group, "PointsTo", QString("%1%2").arg(lnkInfo.driveName).arg(lnkInfo.path));
  }
  else
  {
    appendItem(group, "Where", i18n("on network share"));
    appendItem(group, "PointsTo", lnkInfo.path);
  }

  if ( ! lnkInfo.description.isNull() )   // has description string
    appendItem(group, "Description", lnkInfo.description);

  return true;
}

#include "kfile_lnk.moc"
