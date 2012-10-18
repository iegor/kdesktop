#ifndef __WINSKINCONFIG_H
#define __WINSKINCONFIG_H

#include <noatun/pref.h>
#include <qwidget.h>

class QVBoxLayout;
class WaSkinManager;
class QSlider;

class WinSkinConfig:public CModule {
    Q_OBJECT
  public:
    WinSkinConfig(QWidget * parent, WaSkinManager *waManager);

    void save();

  public slots:
    void reopen();

  private slots:
    void selected();
    void install();
    void remove();

  private:
    WaSkinManager *mWaSkinManager;
    QListBox *skin_list;
    QString orig_skin;
    QVBoxLayout *vbox;
    QPushButton *buttonRemove;
    QSlider *scrollSpeed;
};

#endif
