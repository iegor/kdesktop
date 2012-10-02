#ifndef MSGINPUTDIALOG_H
#define MSGINPUTDIALOG_H

#include <kdialogbase.h>

#include "bug.h"
#include "package.h"

class KTextEdit;
class QSplitter;
class KListBox;

class MsgInputDialog : public KDialogBase
{
    Q_OBJECT
  public:
    enum MessageType{ Close, Reply, ReplyPrivate };
  
    MsgInputDialog( MessageType, const Bug &, const Package &,
                    const QString &, QWidget *parent=0);
    virtual ~MsgInputDialog();

    QString message() const;

  protected slots:
    void slotOk();
    void slotCancel();

  private slots:
    void editPresets();
    void updatePresets();
    void slotPresetSelected( QListBoxItem * );
    void clearMessage();
    void queueCommand();
  
  private:
    void createButtons();
    void createLayout();
  
    void readConfig();
    void writeConfig();
  
    void insertQuotedMessage( const QString &quotedMsg );

    QComboBox *mRecipient;
    KTextEdit *mMessageEdit;
    QSplitter *mSplitter;
    KListBox *mPresets;

    Bug mBug;
    Package mPackage;
    MessageType mType;
};

#endif
