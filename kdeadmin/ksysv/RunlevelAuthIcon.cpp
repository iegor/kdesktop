// (c) 2000 Peter Putzer <putzer@kde.org>

#include <qtimer.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qtooltip.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kdialog.h>
#include <klocale.h>

#include "ksv_core.h"
#include "RunlevelAuthIcon.h"
#include <qlabel.h>

RunlevelAuthIcon::RunlevelAuthIcon (const QString& servicesPath, const QString& runlevelPath,
									QWidget* parent, const char* name)
  : KAuthIcon (parent, name),
	mTimer (new QTimer (this)),
	mServicesInfo (new QFileInfo (servicesPath)),
    mRLInfo (new QFileInfo* [ksv::runlevelNumber]),
	mOld (false),
	mInterval (1000),
    mCheckEnabled(false)
{
  lockText = i18n("Editing disabled - please check your permissions");
  openLockText = i18n("Editing enabled");

  lockLabel->setText (lockText);
  lockLabel->hide();

  lockPM = UserIcon ("ksysv_locked");
  openLockPM = UserIcon ("ksysv_unlocked");

  lockBox->setPixmap (lockPM);

  lockBox->setMargin (1);
  lockBox->setFrameStyle (QFrame::NoFrame);
  lockBox->setFixedSize (lockBox->sizeHint());

  connect (mTimer, SIGNAL (timeout()), this, SLOT (timerEvent()));
  mTimer->start (mInterval);

  for (int i = 0; i < ksv::runlevelNumber; ++i)
	{
	  mRLInfo[i] = new QFileInfo ((runlevelPath + "/rc%1.d").arg(i));
	}

  updateStatus();
  layout->activate();
}

RunlevelAuthIcon::~RunlevelAuthIcon ()
{
  delete mServicesInfo;

  for (int i = 0; i < ksv::runlevelNumber; ++i)
	{
	  delete mRLInfo[i];
	}

  delete[] mRLInfo;
}

void RunlevelAuthIcon::updateStatus ()
{
  if (!mCheckEnabled)
    return;

  const bool res = status();

  if (mOld != res)
	{
	  lockBox->setPixmap (res ? openLockPM : lockPM);
	  lockLabel->setText (res ? openLockText : lockText);

	  QToolTip::remove (this);
	  QToolTip::add (this, lockLabel->text());

      mOld = res;
	  emit authChanged (res);
	}
  else
    mOld = res;
}

bool RunlevelAuthIcon::status () const
{
  bool result = mServicesInfo->isWritable();

  for (int i = 0; i < ksv::runlevelNumber; ++i)
    result = result && mRLInfo[i]->isWritable();

  return result;
}

void RunlevelAuthIcon::timerEvent ()
{
  for (int i = 0; i < ksv::runlevelNumber; ++i)
	{
	  mRLInfo[i]->refresh();
	}
  
  mServicesInfo->refresh();

  updateStatus();
}

void RunlevelAuthIcon::setServicesPath (const QString& path)
{
  mTimer->stop();

  mServicesInfo->setFile (path);

  mTimer->start(mInterval);
}

void RunlevelAuthIcon::setRunlevelPath (const QString& path)
{
  mTimer->stop();

  for (int i = 0; i < ksv::runlevelNumber; ++i)
	{
	  mRLInfo[i]->setFile ((path + "/rc%1.d").arg(i));
	}

  mTimer->start(mInterval);
}

void RunlevelAuthIcon::setRefreshInterval (int interval)
{
  mInterval = interval;

  mTimer->stop();
  mTimer->start (mInterval);
}

void RunlevelAuthIcon::setCheckEnabled (bool on)
{
  kdDebug(3000) << "enabling authicon " << on << endl;
  mCheckEnabled = on;

  // refresh everything
  mOld = !status();
  timerEvent();
}

#include "RunlevelAuthIcon.moc"
