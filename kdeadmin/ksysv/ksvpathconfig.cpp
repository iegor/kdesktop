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
#include "ksvpathconfig.h"

#include <klineedit.h>
#include <kfiledialog.h>

/* 
 *  Constructs a KSVPathConfig which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KSVPathConfig::KSVPathConfig( QWidget* parent,  const char* name, WFlags fl )
    : PathConfiguration( parent, name, fl )
{
  mSpacer->setFixedHeight (KDialog::spacingHint());
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KSVPathConfig::~KSVPathConfig()
{
    // no need to delete child widgets, Qt does it all for us
}

void KSVPathConfig::browseServices()
{
  QString path = KFileDialog::getExistingDirectory(mServicesPath->text(), this);

  if (!path.isEmpty())
    mServicesPath->setText(path);
  
  mServicesPath->setFocus();
}

void KSVPathConfig::browseRunlevels()
{
  QString path = KFileDialog::getExistingDirectory(mRunlevelPath->text(), this);

  if (!path.isEmpty())
    mRunlevelPath->setText(path);

  mRunlevelPath->setFocus();
}

void KSVPathConfig::slotChanged()
{
  emit configChanged();
}

#include "ksvpathconfig.moc"

