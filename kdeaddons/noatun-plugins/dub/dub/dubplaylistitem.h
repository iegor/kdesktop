//
//
// C++ Interface dubplaylistitem.h
//
// Description: 
//
//
// Author: Eray Ozkural (exa), (C) 2001
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef DUBPLAYLISTITEM_H
#define DUBPLAYLISTITEM_H

#include <kfileitem.h>
#include <dubplaylistitem.h>

#include <noatun/playlist.h>

/**noatun playlistitem implementation
  *@author Eray Ozkural (exa)
  */

class KFileItem;

class DubPlaylistItem : public PlaylistItemData  {
public: 
  DubPlaylistItem(const KFileItem&);
  virtual ~DubPlaylistItem();

  virtual QString property(const QString &, const QString & = 0) const;
  virtual void setProperty(const QString &, const QString &);
  virtual void clearProperty(const QString &);
  virtual QStringList properties() const;
  virtual bool isProperty(const QString &) const;
  virtual KURL url() const;
  KFileItem& getFileItem() {
    return fileItem;
  }
  /** No descriptions */
  virtual void remove();

private:
  KFileItem fileItem;
  typedef QMap<QString,QString> Properties;
  Properties property_map;

//	virtual QString file() const;

};

#endif
