// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
                          sidebar_news.h - The real sidebar plugin
                             -------------------
    begin                 : Sat July 23 20:35:30 CEST 2001
    copyright             : (C) 2001,2002 Marcus Camen, Joseph Wenninger
    copyright             : (C) 2003,2004 Marcus Camen
    email                 : Marcus Camen <mcamen@mcamen.de>
    idea and original code: jowenn@kde.org
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

#ifndef _konq_sidebar_news_h_
#define _konq_sidebar_news_h_

#include <konqsidebarplugin.h>
#include "nspanel.h"


class QPixmap;
class QWidgetStack;

namespace KSB_News {

  class NSStackTabWidget;
  class NoRSSWidget;

  class KonqSidebar_News : public KonqSidebarPlugin, DCOPObject {
    Q_OBJECT
    K_DCOP

  public:
    KonqSidebar_News(KInstance *instance, QObject *parent,
                     QWidget *widgetParent, QString &desktopName_,
                     const char* name=0);
    ~KonqSidebar_News();
    virtual void *provides(const QString &);
    void emitStatusBarText (const QString &);
    virtual QWidget *getWidget();

  k_dcop:
    virtual void addedRSSSource(QString);
    virtual void removedRSSSource(QString);

  protected:
    virtual void handleURL(const KURL &url);

  private:
    int checkDcopService();
    QWidgetStack *widgets;
    NSStackTabWidget *newswidget;
    NoRSSWidget *noRSSwidget;
    QPtrList<NSPanel> nspanelptrlist;
    NSPanel *getNSPanelByKey(QString key);
    DCOPRef m_rssservice;
    QPixmap m_appIcon;

  signals:
    // see <konqsidebarplugin.h>
    void openURLRequest(const KURL &url,
                        const KParts::URLArgs &args = KParts::URLArgs());

  private slots:
    void slotArticleItemExecuted(QListBoxItem *item);
    void updateArticles(NSPanel *nsp);
    void updateTitle(NSPanel *nsp);
    void updatePixmap(NSPanel *nsp);
  };

} // namespace KSB_News

#endif
