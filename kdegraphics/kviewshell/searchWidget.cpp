/***************************************************************************
 *   Copyright (C) 2005 by Wilfried Huss                                   *
 *   Wilfried.Huss@gmx.at                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include <config.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <klineedit.h>


#include "searchWidget.h"

SearchWidget::SearchWidget(QWidget* parent, const char* name, WFlags fl)
  : QWidget(parent, name, fl)
{
  setName("SearchWidget");

  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

  layout = new QHBoxLayout(this, 4, 6, "SearchWidgetLayout");

  stopButton = new QPushButton(this, "stopButton");
  stopButton->setPixmap(KGlobal::iconLoader()->loadIcon("stop", KIcon::Small, KIcon::SizeSmall));
  layout->addWidget(stopButton);

  searchLabel = new QLabel(this, "searchLabel");
  searchLabel->setText(i18n("Search:"));
  layout->addWidget(searchLabel);

  searchText = new KLineEdit(this, "searchText");
  layout->addWidget(searchText);

  searchLabel->setBuddy(searchText);

  findPrevButton = new QPushButton(this, "findPrevButton");
  findPrevButton->setPixmap(KGlobal::iconLoader()->loadIcon("back", KIcon::NoGroup, KIcon::SizeSmall));
  QToolTip::add(findPrevButton, i18n("Find previous"));
  layout->addWidget(findPrevButton);

  findNextButton = new QPushButton(this, "findNextButton");
  findNextButton->setPixmap(KGlobal::iconLoader()->loadIcon("forward", KIcon::NoGroup, KIcon::SizeSmall));
  QToolTip::add(findNextButton, i18n("Find next"));
  layout->addWidget(findNextButton);

  caseSensitiveCheckBox = new QCheckBox(this, "caseSensitiveCheckBox");
  caseSensitiveCheckBox->setText(i18n("Case sensitive"));
  layout->addWidget(caseSensitiveCheckBox);

  connect(stopButton, SIGNAL(clicked()), this, SIGNAL(stopSearch()));

  connect(findNextButton, SIGNAL(clicked()), this, SIGNAL(findNextText()));
  connect(findPrevButton, SIGNAL(clicked()), this, SIGNAL(findPrevText()));

  connect(searchText, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged()));

  textChanged();
}

SearchWidget::~SearchWidget()
{
}

QString SearchWidget::getText() const
{
  return searchText->text();
}

bool SearchWidget::caseSensitive() const
{
  return caseSensitiveCheckBox->isChecked();
}

void SearchWidget::textChanged()
{
  bool empty = searchText->text().isEmpty();

  findNextButton->setDisabled(empty);
  findPrevButton->setDisabled(empty);
  emit searchEnabled(!empty);
}

void SearchWidget::show()
{
  searchText->setEnabled(true);
  searchText->selectAll();
  QWidget::show();
  emit searchEnabled(!searchText->text().isEmpty());
}

void SearchWidget::hide()
{
  searchText->setEnabled(false);
  QWidget::hide();
}

void SearchWidget::setFocus()
{
  searchText->setFocus();
}

void SearchWidget::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Escape)
    emit stopSearch();

  if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
  {
    if (e->state() == Qt::ShiftButton)
      emit findPrevText();
    else
      emit findNextText();
  }
}

#include "searchWidget.moc"
