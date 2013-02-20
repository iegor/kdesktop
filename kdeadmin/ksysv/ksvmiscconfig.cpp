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
#include "ksvmiscconfig.h"

#include <qgroupbox.h>
#include <qlayout.h>

#include <kdialog.h>

/* 
 *  Constructs a KSVMiscConfig which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KSVMiscConfig::KSVMiscConfig( QWidget* parent,  const char* name, WFlags fl )
    : MiscConfiguration( parent, name, fl )
{
  mSpacer->setFixedHeight (KDialog::spacingHint());

  // Messages
  mMessagesBoxLayout->setMargin (KDialog::marginHint());
  mMessagesBoxLayout->setSpacing(KDialog::spacingHint());

//   // Editor
//   mEditorBoxLayout->setMargin (KDialog::marginHint());
//   mEditorBoxLayout->setSpacing (KDialog::spacingHint());
}

void KSVMiscConfig::slotChanged()
{
  emit configChanged();
}
/*
 *  Destroys the object and frees any allocated resources
 */
KSVMiscConfig::~KSVMiscConfig()
{
    // no need to delete child widgets, Qt does it all for us
}

#include "ksvmiscconfig.moc"

