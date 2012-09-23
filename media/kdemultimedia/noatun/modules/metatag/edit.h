#ifndef EDITOR_H
#define EDITOR_H

#include <kdialogbase.h>
#include <noatun/playlist.h>
#include <qptrlist.h>
#include <qvariant.h>

class KFileMetaInfo;
class KFileMetaInfoItem;
class QGridLayout;

struct MetaWidget {
    QWidget *widget;
    QString key;
};

class Editor:public KDialogBase {
    Q_OBJECT
  public:
    Editor();

  signals:
    void saved(PlaylistItem &);

  public slots:
    void open(const PlaylistItem & i);

  protected slots:
    void save();
    void modified();

  protected:
    bool keyAddable(const KFileMetaInfo &, QString);
    QString keyGroup(const KFileMetaInfo &, QString);

    void saveControl(KFileMetaInfo& meta_info, const MetaWidget&);
    MetaWidget *createControl(KFileMetaInfo& meta_info, const QString &label, const QString &key, QVariant::Type default_type, bool optional, QWidget *parent);

    QPtrList<MetaWidget> mControls;

    QWidget *mMainWidget;
    QGridLayout *mGrid;
    int mNextRow;

    bool mFileWritable;
    bool mDirty;

    QLabel *mFile;
    QLabel *mFileIcon;
    const char *filename;
    PlaylistItem item;
};

#endif
