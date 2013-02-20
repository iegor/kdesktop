//
//
// C++ Implementation: dubplaylistitem.cpp
//
// Description: 
//
//
// Author: Eray Ozkural (exa) <erayo@cs.bilkent.edu.tr>, (C) 2001
//
// Copyright: See COPYING file that comes with this distribution
//
//

//#include dubplaylistitem.cpp


#include "dubplaylistitem.h"
#include <kfile.h>
#include <kfileitem.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

DubPlaylistItem::DubPlaylistItem(const KFileItem& fItem)
 : fileItem(fItem)
{
//   kdDebug(90010) << "construct playlist item " << fItem.url().prettyURL() << endl;

//  PlaylistItemData::addRef();
  PlaylistItemData::added();
}

DubPlaylistItem::~DubPlaylistItem(){
//   kdDebug(90010) << "destruct playlist item " << fileItem.url().prettyURL() << endl;

//  PlaylistItemData::removed();
  PlaylistItemData::removeRef();
}

QString DubPlaylistItem::property(const QString &key, const QString &def) const {
//   kdDebug(90010) << "property " << key << endl;
  if (isProperty(key)) {
    kdDebug(90010) << key << " -> " << property_map.find(key).data() << endl;
    return property_map.find(key).data();
  }
  else
    return def;
}

void DubPlaylistItem::setProperty(const QString &key, const QString &val) {
//   kdDebug(90010) << "set property" << endl;
  property_map[key] = val;
}

void DubPlaylistItem::clearProperty(const QString &key) {
//   kdDebug(90010) << "clear property" << endl;
  property_map.remove(key);
}

QStringList DubPlaylistItem::properties() const {
//   kdDebug(90010) << "properties" << endl;
  QStringList list;
  for (Properties::const_iterator i=property_map.begin();
       i!=property_map.end(); i++) {
    list.append(i.data());
  }
  return list;
}

bool DubPlaylistItem::isProperty(const QString &key) const {
//   kdDebug(90010) << "is property? " << key << endl;
  return (property_map.find(key) != property_map.end());
}

KURL DubPlaylistItem::url() const {
//   kdDebug(90010) << "item url" << endl;

  return fileItem.url();
}
/** No descriptions */
void DubPlaylistItem::remove() {
  KMessageBox::information(0, i18n("Do you really want to delete this file?"));
}
