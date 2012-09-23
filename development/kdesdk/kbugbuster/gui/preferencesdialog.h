#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <kdialogbase.h>

class QCheckBox;
class QRadioButton;
class QLineEdit;
class QListView;
class KIntNumInput;
class ServerListView;

class PreferencesDialog : public KDialogBase
{
    Q_OBJECT
  public:
    PreferencesDialog( QWidget* parent = 0, const char* name = 0 );
    ~PreferencesDialog();

    void createServerItem( ServerListView *listView, const QString &name,
                           const QString &url, const QString &version );

  public:
    void readConfig();
    void writeConfig();

  signals:
    void configChanged();

  protected slots:
    void setDefaults();
    void slotApply();
    void slotOk();
    void slotCancel();

    void addServer();
    void editServer();
    void removeServer();

    void selectServer();

  protected:
    void setupServerPage();
    void setupAdvancedPage();


  private:
    QCheckBox *mShowClosedCheckBox;
    QCheckBox *mShowWishesCheckBox;
    QCheckBox *mShowVotedCheckBox;
    QCheckBox *mSendBccCheckBox;
    KIntNumInput *mMinVotesInput;
    QRadioButton *mKMailButton;
    QRadioButton *mDirectButton;
    QRadioButton *mSendmailButton;
    QListView *mServerList;
};

class ServerListView;
class ServerItem;

class SelectServerDlg : public KDialogBase
{
    Q_OBJECT
public:
    SelectServerDlg(PreferencesDialog *parent, const char */*name*/ );
    ServerItem *serverSelected();
protected slots:
    void slotDoubleClicked( QListViewItem *);

protected:
    ServerListView *list;
};


#endif
