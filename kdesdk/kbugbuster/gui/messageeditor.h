#ifndef MESSAGEEDITOR_H
#define MESSAGEEDITOR_H

#include <kdialogbase.h>

class QComboBox;
class KTextEdit;

class MessageEditor : public KDialogBase {
    Q_OBJECT
  public:
    MessageEditor( QWidget *parent );

  protected slots:
    void slotOk();

  private slots:
    void addButton();
    void removeButton();
    void changeMessage();
    void saveMessage();
    void updateMessage();
    void updateConfig();
    
  private:
    QComboBox *mSelectionCombo;
    KTextEdit *mMessageEdit;
    
    QString mCurrentButton;
    QMap <QString,QString> mMessageButtons;
};

#endif
