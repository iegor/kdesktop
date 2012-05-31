// -*- C++ -*-
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

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <qwidget.h>

class QHBoxLayout;
class QLabel;
class KLineEdit;
class QPushButton;
class QCheckBox;
class QString;

class SearchWidget : public QWidget
{
  Q_OBJECT

public:
  SearchWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
  ~SearchWidget();

  QString getText() const;
  bool caseSensitive() const;

public slots:
  virtual void setFocus();
  virtual void show();
  virtual void hide();

signals:
  void findNextText();
  void findPrevText();
  void searchEnabled(bool);
  void stopSearch();

protected:
  virtual void keyPressEvent(QKeyEvent*);

private slots:
  void textChanged();

private:
  QPushButton* stopButton;
  QLabel* searchLabel;
  KLineEdit* searchText;
  QPushButton* findNextButton;
  QPushButton* findPrevButton;
  QCheckBox* caseSensitiveCheckBox;

  QHBoxLayout* layout;
};

#endif
