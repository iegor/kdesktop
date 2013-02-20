// (c) 2000 Peter Putzer

#include <qframe.h>
#include <qlabel.h>
#include <qtextview.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>

#include <kdebug.h>
#include <klocale.h>
#include <klineedit.h>
#include <kiconloader.h>

#include "SpinBox.h"
#include "OldView.h"
#include "ksv_conf.h"
#include "ksv_core.h"
#include "Data.h"
#include "Properties.h"

KSVServicePropertiesDialog::KSVServicePropertiesDialog (KSVData& data, QWidget* parent)
  : KPropertiesDialog (KURL(data.filenameAndPath()),
                       parent, "KSVServicePropertiesDialog", true, false),
  mData (data)
{
  KSVServicesPage* page = new KSVServicesPage (data, this);
  insertPlugin (page);
  
  showPage (page->pageIndex ());
}

KSVServicePropertiesDialog::~KSVServicePropertiesDialog ()
{
}

KSVServicesPage::KSVServicesPage (KSVData& data, KPropertiesDialog* props)
  : KPropsDlgPlugin (props),
    mData (data),
    mPage (props->addVBoxPage (i18n("&Service"))),
    mIndex (props->pageIndex (mPage))
{
  mPage->setSpacing (KDialog::spacingHint());

  QVBox* desc = new QVBox (mPage);
  desc->setSpacing (1);

  QLabel* label = new QLabel(i18n("Description:"), desc);
  label->setFixedHeight (label->sizeHint().height());

  QString text;
  ksv::getServiceDescription (data.filename(), text);
  mDesc = new QTextView (QString("<p>%1</p>").arg (text), QString::null, desc);

  QButtonGroup* buttons = new QButtonGroup (1, Vertical, i18n ("Actions"), mPage);
  QPushButton* b = new QPushButton (i18n ("&Edit"), buttons);
  connect (b, SIGNAL (clicked()), props, SLOT (doEdit()));

  QFrame* spacer = new QFrame (buttons);
  spacer->setMinimumWidth (KDialog::spacingHint());

  b = new QPushButton (i18n ("&Start"), buttons);
  connect (b, SIGNAL (clicked()), props, SLOT (doStart()));

  b = new QPushButton (i18n ("S&top"), buttons);
  connect (b, SIGNAL (clicked()), props, SLOT (doStop()));

  b = new QPushButton (i18n ("&Restart"), buttons);
  connect (b, SIGNAL (clicked()), props, SLOT (doRestart()));
}

KSVServicesPage::~KSVServicesPage ()
{
}

void KSVServicesPage::applyChanges ()
{
}

void KSVServicePropertiesDialog::doEdit ()
{
  emit editService (mData.filenameAndPath ());
}

void KSVServicePropertiesDialog::doStart ()
{
  emit startService (mData.filenameAndPath ());
}

void KSVServicePropertiesDialog::doStop ()
{
  emit stopService (mData.filenameAndPath ());
}

void KSVServicePropertiesDialog::doRestart ()
{
  emit restartService (mData.filenameAndPath ());
}


KSVEntryPropertiesDialog::KSVEntryPropertiesDialog (KSVData& data, QWidget* parent)
  : KPropertiesDialog (data.label(), parent, "KSVEntryPropertiesDialog", true),
    mData (data)
{
  KSVEntryPage* page1 = new KSVEntryPage (data, this);
  insertPlugin (page1);

  KSVServicesPage* page2 = new KSVServicesPage (data, this);
  insertPlugin (page2);
}

KSVEntryPropertiesDialog::~KSVEntryPropertiesDialog ()
{
}

KSVEntryPage::KSVEntryPage (KSVData& data, KPropertiesDialog* props)
  : KPropsDlgPlugin (props),
    mData (data),
    mPage (props->addPage (i18n("&Entry"))),
    mIndex (props->pageIndex (mPage))
{
  QGridLayout* top = new QGridLayout (mPage, 4, 2, 0, KDialog::spacingHint());

  QLabel* labelLabel = new QLabel (i18n ("&Name:"), mPage);
  mLabelEdit = new KLineEdit (mPage);
  mLabelEdit->setText (mData.label());
  labelLabel->setBuddy (mLabelEdit);

  QLabel* serviceLabel = new QLabel (i18n ("&Points to service:"), mPage);
  mServiceEdit = new KLineEdit (mPage);
  mServiceEdit->setCompletionObject (ksv::serviceCompletion(), true);
  mServiceEdit->setText (mData.filename());
  serviceLabel->setBuddy (mServiceEdit);

  QLabel* numberLabel = new QLabel (i18n ("&Sorting number:"), mPage);
  mNumberEdit = new KSVSpinBox (mPage);
  mNumberEdit->setValue (mData.number());
  numberLabel->setBuddy (mNumberEdit);

  QLabel* iconLabel = new QLabel (mPage);
  iconLabel->setPixmap (DesktopIcon ("ksysv", 48));

  top->addWidget (labelLabel, 0, 0);
  top->addWidget (mLabelEdit, 0, 1);
  top->addWidget (serviceLabel, 1, 0);
  top->addWidget (mServiceEdit, 1, 1);
  top->addWidget (numberLabel, 2, 0);
  top->addWidget (mNumberEdit, 2, 1);
  top->addWidget (iconLabel, 3, 0);

  connect (mServiceEdit, SIGNAL (textChanged (const QString&)),
           this, SLOT (emitChanged()));
  connect (mLabelEdit, SIGNAL (textChanged (const QString&)),
           this, SLOT (emitChanged()));
  connect (mNumberEdit, SIGNAL (valueChanged (int)),
           this, SLOT (emitChanged()));
}

KSVEntryPage::~KSVEntryPage ()
{
}

void KSVEntryPage::applyChanges ()
{
  if (mNumberEdit->value() != mData.number())
    {
      mData.setNumber (mNumberEdit->value());
    }

  if (mLabelEdit->text() != mData.label())
    {
      mData.setLabel (mLabelEdit->text());
    }

  if (mServiceEdit->text() != mData.filename())
    {
      mData.setFilename (mServiceEdit->text());
      ksv::serviceCompletion ()->addItem (mData.filename());
    }
}

void KSVEntryPage::emitChanged ()
{
  emit changed();
}

void KSVEntryPropertiesDialog::doEdit ()
{
  emit editService (mData.filenameAndPath ());
}

void KSVEntryPropertiesDialog::doStart ()
{
  emit startService (mData.filenameAndPath ());
}

void KSVEntryPropertiesDialog::doStop ()
{
  emit stopService (mData.filenameAndPath ());
}

void KSVEntryPropertiesDialog::doRestart ()
{
  emit restartService (mData.filenameAndPath ());
}

#include "Properties.moc"
