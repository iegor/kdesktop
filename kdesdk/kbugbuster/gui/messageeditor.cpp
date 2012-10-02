#include <qcombobox.h>
#include <ktextedit.h>
#include <kinputdialog.h>
#include <qlayout.h>
#include <qlabel.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "kbbprefs.h"

#include "messageeditor.h"
#include <qpushbutton.h>
#include "messageeditor.moc"

MessageEditor::MessageEditor( QWidget *parent )
  : KDialogBase(Plain,i18n("Edit Message Buttons"),Ok|Cancel,Ok,parent,0,
                true,true)
{
  QFrame *topFrame = plainPage();
  QBoxLayout *topLayout = new QVBoxLayout(topFrame,0,spacingHint());

  QBoxLayout *selectionLayout = new QHBoxLayout;
  topLayout->addLayout(selectionLayout);

  QLabel *selectionLabel = new QLabel(i18n("Button:"),topFrame);
  selectionLayout->addWidget(selectionLabel);

  mSelectionCombo = new QComboBox(topFrame);
  selectionLayout->addWidget(mSelectionCombo);
  connect(mSelectionCombo,SIGNAL(activated(int)),SLOT(changeMessage()));

  QPushButton *addButton = new QPushButton(i18n("Add Button..."),topFrame);
  selectionLayout->addWidget(addButton);
  connect(addButton,SIGNAL(clicked()),SLOT(addButton()));

  QPushButton *removeButton = new QPushButton(i18n("Remove Button"),topFrame);
  selectionLayout->addWidget(removeButton);
  connect(removeButton,SIGNAL(clicked()),SLOT(removeButton()));

  mMessageEdit = new KTextEdit(topFrame);
  topLayout->addWidget(mMessageEdit,1);

  updateConfig();
}

void MessageEditor::updateConfig()
{
  mMessageButtons = KBBPrefs::instance()->mMessageButtons;

  mSelectionCombo->clear();

  QMap<QString,QString>::ConstIterator it;
  for(it = mMessageButtons.begin();it != mMessageButtons.end();++it) {
    mSelectionCombo->insertItem(it.key());
  }

  updateMessage();
}

void MessageEditor::addButton()
{
  QString txt;
  txt = KInputDialog::getText(i18n("Add Message Button"),
	i18n("Enter button name:"), QString::null,
	NULL, this );

  if ( !txt.isNull() ) {
    saveMessage();
    mSelectionCombo->insertItem(txt);
    mMessageButtons.insert(txt,"");
    mSelectionCombo->setCurrentItem(mSelectionCombo->count()-1);
    updateMessage();
  }

}

void MessageEditor::removeButton()
{
  int result = KMessageBox::warningContinueCancel(this,
      i18n("Remove the button %1?").arg(mSelectionCombo->currentText()),
      i18n("Remove"), KGuiItem( i18n("Delete"), "editdelete") );

  if (result == KMessageBox::Continue) {
    mMessageButtons.remove(mSelectionCombo->currentText());
    mSelectionCombo->removeItem(mSelectionCombo->currentItem());
    mSelectionCombo->setCurrentItem(0);
    updateMessage();
  }
}

void MessageEditor::changeMessage()
{
  saveMessage();
  updateMessage();
}

void MessageEditor::updateMessage()
{
  mCurrentButton = mSelectionCombo->currentText();

  mMessageEdit->setText(mMessageButtons[mCurrentButton]);
}

void MessageEditor::saveMessage()
{
  mMessageButtons.replace(mCurrentButton,mMessageEdit->text());
}

void MessageEditor::slotOk()
{
  saveMessage();

  KBBPrefs::instance()->mMessageButtons = mMessageButtons;
  accept();
}
