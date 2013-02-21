// (c) 2000 Peter Putzer

#include <qframe.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qgrid.h>
#include <qdir.h>
#include <qcheckbox.h>

#include <kdebug.h>
#include <klocale.h>
#include <klineedit.h>
#include <kiconloader.h>
#include <kcolorbutton.h>
#include <kapplication.h>
#include <kmessagebox.h>

#include "ksv_conf.h"
// #include "ksvcolorconfig.h"
#include "ksvmiscconfig.h"
#include "ksvpathconfig.h"
#include "ksvlookandfeel.h"
// #include "ksvfontconfig.h"
#include "ksv_core.h"

#include "PreferencesDialog.h"

KSVPreferences::KSVPreferences (QWidget* parent)
  :  KDialogBase (IconList, i18n("Configure"), Help|Ok|Apply|Cancel, Ok,
				  parent, "KSysV Preferences", true, true),
  mConfig (KSVConfig::self())
{
  setMinimumSize (sizeHint ());

  /**
   * Look & Feel Page
   */
  QFrame* lafPage = addPage (i18n ("Look & Feel"), QString::null, DesktopIcon ("colorize", 32));
  mLookAndFeel = new KSVLookAndFeel (lafPage);
  connect( mLookAndFeel, SIGNAL( configChanged() ), this, SLOT( slotChanged() ) );

  QVBoxLayout* lafLayout = new QVBoxLayout (lafPage);
  lafLayout->addWidget (mLookAndFeel);

//   /**
//    * Colors Page
//    */
//   QFrame* colorPage = addPage (i18n("Colors"), QString::null, DesktopIcon ("colorize", 32));
//   mColorConfig = new KSVColorConfig(colorPage);

//   QVBoxLayout* colorLayout = new QVBoxLayout(colorPage);
//   colorLayout->addWidget(mColorConfig);

//   /**
//    * Fonts Page
//    */
//   QFrame* fontPage = addPage (i18n("Fonts"), QString::null, DesktopIcon ("fonts", 32));
//   mFontConfig = new KSVFontConfig(fontPage);

//   QVBoxLayout* fontLayout = new QVBoxLayout (fontPage);
//   fontLayout->addWidget(mFontConfig);

  /**
   * Paths Page
   */
  QFrame* pathPage = addPage (i18n ("Paths"), QString::null, DesktopIcon ("kfm", 32));
  mPathConfig = new KSVPathConfig(pathPage);
  connect( mPathConfig, SIGNAL( configChanged() ), this, SLOT( slotChanged() ) );

  QVBoxLayout *pathLayout = new QVBoxLayout (pathPage);
  pathLayout->addWidget (mPathConfig);

  /**
   * Miscellaneous Page
   */
  QFrame* miscPage = addPage (i18n ("Miscellaneous"), i18n("Settings Not Fitting Anywhere Else"),
                              DesktopIcon ("misc", 32));
  mMiscConfig = new KSVMiscConfig(miscPage);
  connect( mMiscConfig, SIGNAL( configChanged() ), this, SLOT( slotChanged() ) );

  QVBoxLayout *miscLayout = new QVBoxLayout (miscPage);
  miscLayout->addWidget (mMiscConfig);

  connect (mMiscConfig->mShowAll, SIGNAL (clicked()), this, SLOT (showAllMessages()));

  reReadConfig();
  enableButton( Apply, false );
  configChanged = false;
}

KSVPreferences::~KSVPreferences ()
{
}

void KSVPreferences::slotChanged()
{
  enableButton( Apply, true );
  configChanged = true;
}

void KSVPreferences::reReadConfig()
{
  mPathConfig->mRunlevelPath->setText(mConfig->runlevelPath());
  mPathConfig->mServicesPath->setText(mConfig->scriptPath());

  mLookAndFeel->mNewNormal->setColor (mConfig->newNormalColor());
  mLookAndFeel->mNewSelected->setColor (mConfig->newSelectedColor());

  mLookAndFeel->mChangedNormal->setColor (mConfig->changedNormalColor ());
  mLookAndFeel->mChangedSelected->setColor (mConfig->changedSelectedColor ());

  mLookAndFeel->setServiceFont (mConfig->serviceFont());
  mLookAndFeel->setNumberFont (mConfig->numberFont());

  reReadMessages();
}

void KSVPreferences::slotCancel ()
{
  reject();
  reReadConfig();
}

void KSVPreferences::slotApply ()
{
  if (checkPaths())
  {
    setConfig();
    enableButton( Apply, false );
    configChanged = false;
  }
}

bool KSVPreferences::checkPaths ()
{
  bool result = true;

  if (!QDir(mPathConfig->mServicesPath->text()).exists())
    result = (KMessageBox::warningContinueCancel(this,
                                                 i18n ("The service folder you specified does "
                                                       "not exist.\n"
                                                       "You can continue if you want to, "
                                                       "or you can click Cancel "
                                                       "to select a new folder."),
                                                 i18n ("Warning"))
              != KMessageBox::Cancel);

  if (!QDir(mPathConfig->mRunlevelPath->text()).exists())
    result = result
      && (KMessageBox::warningContinueCancel(this,
                                             i18n ("The runlevel folder you specified does "
                                                   "not exist.\n"
                                                   "You can continue if you want to, "
                                                   "or you can click Cancel "
                                                   "to select a new folder."),
                                             i18n ("Warning"))
          != KMessageBox::Cancel);

  return result;
}

void KSVPreferences::slotOk ()
{
  if (checkPaths())
    {
      accept();
      setConfig ();
    }
}

QColor KSVPreferences::newNormal() const
{
  return mLookAndFeel->mNewNormal->color();
}

QColor KSVPreferences::newSelected() const
{
  return mLookAndFeel->mNewSelected->color();
}

QColor KSVPreferences::changedNormal() const
{
  return mLookAndFeel->mChangedNormal->color();
}

QColor KSVPreferences::changedSelected() const
{
  return mLookAndFeel->mChangedSelected->color();
}


KSVPreferences* KSVPreferences::self ()
{
  static KSVPreferences* prefs = new KSVPreferences (kapp->mainWidget());

  return prefs;
}

void KSVPreferences::setConfig ()
{
  // if necessary, update service path
  {
    QString sp = mPathConfig->mServicesPath->text();
    if (sp != mConfig->scriptPath())
      {
        mConfig->setScriptPath (sp);

        emit updateServicesPath ();
      }
  }

  // if necesssary, update root path for runlevels
  {
    QString rp = mPathConfig->mRunlevelPath->text();
    if (rp != mConfig->runlevelPath ())
      {
        mConfig->setRunlevelPath (rp);
        
        emit updateRunlevelsPath ();
      }
  }
  
  // if necessary, update miscellaneous settings
  {
    mConfig->setShowMessage (ksv::RunlevelsReadOnly,
                             mMiscConfig->mWarnReadOnly->isChecked());
    mConfig->setShowMessage (ksv::CouldNotGenerateSortingNumber,
                             mMiscConfig->mWarnSortingNumber->isChecked());
  }

  // if necessary, update colors
  {
    QColor nn = newNormal();
    QColor ns = newSelected();
    QColor cn = changedNormal();
    QColor cs = changedSelected();
    
    if (nn != mConfig->newNormalColor() ||
        ns != mConfig->newSelectedColor() ||
        cn != mConfig->changedNormalColor() ||
        cs != mConfig->changedSelectedColor())
      {
        mConfig->setNewNormalColor (nn);
        mConfig->setNewSelectedColor (ns);
        mConfig->setChangedNormalColor (cn);
        mConfig->setChangedSelectedColor (cs);
        
        emit updateColors ();
      }
  }
  
  // if necessary, update fonts
  {
    const QFont& sf = mLookAndFeel->serviceFont();
    const QFont& ns = mLookAndFeel->numberFont();
    
    if (sf != mConfig->serviceFont() || ns != mConfig->numberFont())
      {
        mConfig->setServiceFont (sf);
        mConfig->setNumberFont (ns);

        emit updateFonts();
      }
  }
}

void KSVPreferences::showAllMessages ()
{
  KMessageBox::enableAllMessages ();
  
  // update checkboxes
  reReadMessages();
}

void KSVPreferences::reReadMessages()
{
  mMiscConfig->mWarnReadOnly->setChecked (mConfig->showMessage (ksv::RunlevelsReadOnly));
  mMiscConfig->mWarnSortingNumber->setChecked (mConfig->showMessage (ksv::CouldNotGenerateSortingNumber));
}

void KSVPreferences::showEvent (QShowEvent* e)
{
  reReadMessages();

  KDialogBase::showEvent (e);
}

#include "PreferencesDialog.moc"
