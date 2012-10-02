/*
 * Copyright (C) 2002 Marco Wegner <mail@marcowegner.de>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
 */


#include "kfile_po.h"

#include <qstringlist.h>

#include "poinfo.h"
#include <kgenericfactory.h>

using namespace KBabel;

typedef KGenericFactory<KPoPlugin> POFactory;
K_EXPORT_COMPONENT_FACTORY(kfile_po, POFactory("kfile_po"))

KPoPlugin::KPoPlugin(QObject *parent, const char *name,
                     const QStringList& args)
  : KFilePlugin(parent, name, args)
{
  KFileMimeTypeInfo* info = addMimeTypeInfo("application/x-gettext");

  KFileMimeTypeInfo::GroupInfo* group =
    addGroupInfo(info, "CatalogInfo", i18n("Catalog Information"));

  KFileMimeTypeInfo::ItemInfo* item;
  item = addItemInfo(group, "Total", i18n("Total Messages"), QVariant::Int);
  item = addItemInfo(group, "Fuzzy", i18n("Fuzzy Messages"), QVariant::Int);
  item = addItemInfo(group, "Untranslated", i18n("Untranslated Messages"), QVariant::Int);
  item = addItemInfo(group, "LastTranslator", i18n("Last Translator"), QVariant::String);
  item = addItemInfo(group, "LanguageTeam", i18n("Language Team"), QVariant::String);
  item = addItemInfo(group, "Revision", i18n("Revision"), QVariant::String);
}

bool KPoPlugin::readInfo(KFileMetaInfo& metaInfo, uint)
{
  PoInfo poInfo;
  QStringList wordList;
  ConversionStatus status = PoInfo::info(metaInfo.path(), poInfo, wordList, false, false, false);
  if (status == OK) {
    KFileMetaInfoGroup group = appendGroup(metaInfo, "CatalogInfo");

    appendItem(group, "Total", poInfo.total);
    appendItem(group, "Fuzzy", poInfo.fuzzy);
    appendItem(group, "Untranslated", poInfo.untranslated);
    appendItem(group, "LastTranslator", poInfo.lastTranslator);
    appendItem(group, "LanguageTeam", poInfo.languageTeam);
    appendItem(group, "Revision", poInfo.revision);

    return true;
  }
  return false;
}

#include "kfile_po.moc"
