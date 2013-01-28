// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
    Copyright: norsswidget.h
    Marcus Camen  <mcamen@mcamen.de>
***************************************************************************/

/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef _NORSSWIDGET_H_
#define _NORSSWIDGET_H_


class KConfigDialog;


namespace KSB_News {

  class NoRSSWidget : public QWidget {
    Q_OBJECT

  public:
    NoRSSWidget(QWidget *parent = 0, const char *name = 0);

  private slots:
    void slotBtnClicked();
    void slotConfigure_okClicked();

  private:
    KConfigDialog *m_confdlg;
  };


} // namespace KSB_News

#endif
