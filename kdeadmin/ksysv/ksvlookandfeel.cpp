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
#include "ksvlookandfeel.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kdialog.h>
#include <kfontdialog.h>

/* 
 *  Constructs a KSVLookAndFeel which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KSVLookAndFeel::KSVLookAndFeel( QWidget* parent,  const char* name, WFlags fl )
    : LookAndFeel( parent, name, fl )
{
  mTopLayout->setSpacing (KDialog::spacingHint());
  mColorGrid->setSpacing (KDialog::spacingHint());
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KSVLookAndFeel::~KSVLookAndFeel()
{
    // no need to delete child widgets, Qt does it all for us
}

/* 
 * protected slot
 */
void KSVLookAndFeel::chooseNumberFont()
{
  KFontDialog::getFont (mNumberFont, false, this);
  setNumberFont (mNumberFont);
  emit configChanged();
}

/*
 * protected slot
 */
void KSVLookAndFeel::chooseServiceFont()
{
  KFontDialog::getFont (mServiceFont, false, this);
  setServiceFont (mServiceFont);
  emit configChanged();
}

void KSVLookAndFeel::setServiceFont (const QFont& font)
{
  mServiceFontPreview->setFont(font);
  mServiceFontPreview->setText(font.family() + " " + QString::number(font.pointSize()));

  mServiceFont = font;
}

void KSVLookAndFeel::setNumberFont (const QFont& font)
{
  mNumberFontPreview->setFont(font);
  mNumberFontPreview->setText(font.family() + " " + QString::number(font.pointSize()));

  mNumberFont = font;
}

void KSVLookAndFeel::slotChanged()
{
  emit configChanged();
}
#include "ksvlookandfeel.moc"
