#ifndef _LYRICSCMODULE_H_
#define _LYRICSCMODULE_H_

#include <noatun/pref.h>
#include <qvaluevector.h>
#include <klocale.h>

class KLineEdit;
class KListBox;
class KButtonBox;

struct SearchProvider {
  QString name;
  QString url;
};

class LyricsCModule : public CModule {
  Q_OBJECT
  public:
    LyricsCModule(QObject *_parent);
  public slots:
    virtual void save(void);
    virtual void reopen(void);
    void newSearch(QString name = i18n( "New Search Provider" ), QString query = "");
    void delSearch();
    void moveUpSearch();
    void moveDownSearch();
    void selected( QListBoxItem *i );
    void nameChanged( const QString &name );
    void queryChanged( const QString &query );
  protected:
    KListBox *providersBox;
    KButtonBox *boxButtons;
    KLineEdit *nameEdit;
    KLineEdit *queryEdit;
    QValueVector<SearchProvider> mProviders;
};





#endif
