#ifndef NID3_H
#define NID3_H

#include <noatun/player.h>
#include <noatun/plugin.h>
#include <noatun/pref.h>
#include <noatuntags/tags.h>

#include <qobject.h>

class KFileMetaInfo;
class KAction;

class MetaTagLoader:public QObject, public Tags, public Plugin {
  Q_OBJECT
  public:
    MetaTagLoader();
    ~MetaTagLoader();

  public slots:
    bool update(PlaylistItem & item);
    void editTag();

  private:
    bool setProperty(KFileMetaInfo &info, PlaylistItem &item, const QString &key, const QString &property);
    int menuID;
	 KAction *mAction;
};

#endif
