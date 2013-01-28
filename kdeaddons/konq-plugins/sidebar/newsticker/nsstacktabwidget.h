// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
                          nsstacktabwidget.h  -  description
                             -------------------
    begin                : Sat 07.09.2002
    copyright            : (C) 2002-2004 Marcus Camen
    email                : mcamen@mcamen.de
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

#ifndef _NSSTACKTABWIDGET_H_
#define _NSSTACKTABWIDGET_H_

#include <qptrdict.h>
#include "nspanel.h"


class QVBoxLayout;
class QPushButton;
class QStringList;
class KPopupMenu;
class KAboutData;
class KAboutApplication;
class KBugReport;
class KConfigDialog;

namespace KSB_News {

  class NewRssSourceDlg;

  class NSStackTabWidget : public QWidget {
    Q_OBJECT

  public:
    NSStackTabWidget(QWidget *parent = 0, const char *name = 0,
                     QPixmap appIcon = QPixmap());
    void addStackTab(NSPanel *nsp, QWidget *page);
    void delStackTab(NSPanel *nsp);
    void updateTitle(NSPanel *nsp);
    void updatePixmap(NSPanel *nsp);
    bool isEmpty() const;
    bool isRegistered(const QString &key);

  protected:
    bool eventFilter(QObject *obj, QEvent *ev);

  private slots:
    void buttonClicked();
    void slotShowAbout();
    void slotShowBugreport();
    void slotRefresh();
    void slotClose();
    void slotConfigure();
    void slotConfigure_okClicked();

  private:
    QPtrDict<QWidget> pages;
    QPtrDict<QWidget> pagesheader;
    QVBoxLayout *layout;
    QWidget *currentPage;
    KPopupMenu *popup, *helpmenu;
    KAboutData *m_aboutdata;
    KAboutApplication *m_about;
    KBugReport *m_bugreport;
    QPushButton *m_last_button_rightclicked;
    KConfigDialog *m_confdlg;
    QStringList m_our_rsssources;
  };


} // namespace KSB_News


#endif
