/*
** Copyright (C) 2000 Peter Putzer <putzer@kde.org>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include "ksvconfigwizard.h"

#include <qlayout.h>

#include <kdialog.h>
#include <kfiledialog.h>
#include <klineedit.h>
#include <qpushbutton.h>

/* 
 *  Constructs a KSVConfigWizard which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The wizard will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal wizard.
 */
KSVConfigWizard::KSVConfigWizard (QWidget* parent, const char* name, bool modal, WFlags fl)
  : ConfigWizard (parent, name, modal, fl),
    mChosenDistribution (Debian)
{
  mDistributionBoxLayout->setSpacing (KDialog::spacingHint());
  mDistributionBoxLayout->setMargin (KDialog::marginHint());

  mSpacer->setFixedHeight (KDialog::spacingHint());

  // set the default paths
  chooseDistribution (mChosenDistribution);

  // enable finish-button
  setFinishEnabled (mFinishedPage, true);

  // some connections
  connect (this, SIGNAL (selected (const QString&)),
           this, SLOT (selectedPage (const QString&)));

  // hide cancel-button
  cancelButton()->hide();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KSVConfigWizard::~KSVConfigWizard()
{
    // no need to delete child widgets, Qt does it all for us
}

void KSVConfigWizard::browseServices()
{
  QString path = KFileDialog::getExistingDirectory(mServicesPath->text(), this);

  if (!path.isEmpty())
    mServicesPath->setText(path);
  
  mServicesPath->setFocus();
}

void KSVConfigWizard::browseRunlevels()
{
  QString path = KFileDialog::getExistingDirectory(mRunlevelPath->text(), this);

  if (!path.isEmpty())
    mRunlevelPath->setText(path);

  mRunlevelPath->setFocus();
}

void KSVConfigWizard::selectedPage (const QString& title)
{
  if (title == "Configuration Complete")
    finishButton()->setDefault (true);
  else
    nextButton()->setDefault (true);
}

QString KSVConfigWizard::runlevelPath ()
{
  return mRunlevelPath->text();
}

QString KSVConfigWizard::servicesPath ()
{
  return mServicesPath->text();
}

void KSVConfigWizard::chooseDistribution(int which)
{
  mChosenDistribution = static_cast<Distribution> (which);

  QString rlpath; QString spath;
  switch (mChosenDistribution)
    {
    case Debian:
    case Corel:
      rlpath = "/etc";
      spath = "/etc/init.d";
      break;

    case RedHat:
    case Mandrake:
	 case Conectiva:
      rlpath = "/etc/rc.d";
      spath = "/etc/rc.d/init.d";
      break;

    case SuSE:
      rlpath = "/etc/rc.d";
      spath = "/etc/init.d";
      break;

    default:
      rlpath = "/etc/rc.d";
      spath = "/etc/rc.d/init.d";  
    }

  mRunlevelPath->setText (rlpath);
  mServicesPath->setText (spath);

  if (which != Other)
    setAppropriate (mPathsPage, false);
  else
    setAppropriate (mPathsPage, true);
}

#include "ksvconfigwizard.moc"
