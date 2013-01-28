/*
 * Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.	IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>

#include <qcheckbox.h>
#include <qlayout.h>

#include "configmodule.h"
#include "configmodule.moc"
CharlatanConfigModule::CharlatanConfigModule(QObject * parent)
  :
  CModule
  (
   i18n("Charlatan"),
   i18n("Charlatan Interface Settings"),
   "appearance",
   parent
  )
{
  scroll_ = new QCheckBox(i18n("Scroll song title"), this);

  QVBoxLayout * layout = new QVBoxLayout(this);

  layout->addWidget(scroll_);

	layout->addStretch(100);

  reopen();
}

  void
CharlatanConfigModule::save()
{
  KConfig * c(KGlobal::config());
  c->setGroup("CharlatanInterface");
  c->writeEntry("ScrollTitle", scroll_->isChecked());
  c->sync();
  emit(saved());
}

  void
CharlatanConfigModule::reopen()
{
  KConfig * c(KGlobal::config());
  c->setGroup("CharlatanInterface");
  scroll_->setChecked(c->readBoolEntry("ScrollTitle", true));
}

// vim:ts=2:sw=2:tw=78:noet
