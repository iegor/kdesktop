// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
                          sidebar_news.cpp - The real sidebar plugin
                             -------------------
    begin                 : Sat June 23 13:35:30 CEST 2001
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

#include <dcopclient.h>
#include <qtimer.h>
#include <qbuffer.h>
#include <qwidgetstack.h>
#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdesktopfile.h>
#include <kiconloader.h>
#include <kdemacros.h>
#include "sidebar_news.h"
#include "nsstacktabwidget.h"
#include "norsswidget.h"
#include "sidebarsettings.h"


namespace KSB_News {

  KonqSidebar_News::KonqSidebar_News(KInstance *inst, QObject *parent,
                                     QWidget *widgetParent,
                                     QString &desktopName, const char* name)
    : KonqSidebarPlugin(inst, parent, widgetParent, desktopName, name),
      DCOPObject("sidebar-newsticker")
  {
    // get the application icon
    // FIXME: as konqueror knows the icon there might be a possibility to
    //        access the already present QPixmap
    KDesktopFile desktopFile(desktopName, true);
    QString iconName = desktopFile.readIcon();
    KIconLoader iconLoader;
    m_appIcon = iconLoader.loadIcon(iconName, KIcon::Small);

    // create all sidebar widgets
    widgets = new QWidgetStack(widgetParent, "main_widgetstack");
    newswidget = new NSStackTabWidget(widgets, "feedbrowser_stackchld",
                                      m_appIcon);
    noRSSwidget = new NoRSSWidget(widgets, "nofeed_stackchld");
    widgets->addWidget(newswidget);
    widgets->addWidget(noRSSwidget);
    widgets->raiseWidget(noRSSwidget);
    noRSSwidget->show();

    // try to connect to the DCOP service
    if (checkDcopService() > 0) {
      KMessageBox::sorry(widgets,
                         i18n("<qt>Cannot connect to RSS service. Please make "
                              "sure the <strong>rssservice</strong> program "
                              "is available (usually distributed as part "
                              "of kdenetwork).</qt>"),
                         i18n("Sidebar Newsticker"));
      noRSSwidget->setEnabled(false);
    } else {
      m_rssservice = DCOPRef("rssservice", "RSSService");

      QStringList reslist = SidebarSettings::sources();
      QStringList::iterator it;
      for (it = reslist.begin(); it != reslist.end(); ++it) {
        addedRSSSource(*it);
      }

      // fetch added and removed RSS sources
      connectDCOPSignal("rssservice", m_rssservice.obj(), "added(QString)",
                        "addedRSSSource(QString)", false);
      connectDCOPSignal("rssservice", m_rssservice.obj(), "removed(QString)",
                        "removedRSSSource(QString)", false);

      // show special widget if there are no RSS sources available
      if (newswidget->isEmpty()) {
        widgets->raiseWidget(noRSSwidget);
        noRSSwidget->show();
      } else {
        widgets->raiseWidget(newswidget);
      }
    }
  }


  KonqSidebar_News::~KonqSidebar_News() {
  }



  void *KonqSidebar_News::provides(const QString &) {return 0;}

  void KonqSidebar_News::emitStatusBarText (const QString &) {;}

  QWidget *KonqSidebar_News::getWidget(){return widgets;}

  void KonqSidebar_News::handleURL(const KURL &/*url*/) {;}


///////// startup of the DCOP servce ///////////////////////////////////////

  int KonqSidebar_News::checkDcopService() {
    QString rdfservice_error;
    int err = 0;

    if (! kapp->dcopClient()->isApplicationRegistered("rssservice"))
      if (KApplication::startServiceByDesktopName("rssservice", QString(),
                                                  &rdfservice_error) > 0)
        err = 1;

    return err;
  }



///////// helper methods ///////////////////////////////////////////////////

  NSPanel *KonqSidebar_News::getNSPanelByKey(QString key) {
    NSPanel *nsp = NULL, *current_nsp;

    for (current_nsp = nspanelptrlist.first(); current_nsp;
         current_nsp = nspanelptrlist.next()) {
      if (current_nsp->key() == key)
        nsp = current_nsp;
    }

    return nsp;
  }


  void KonqSidebar_News::addedRSSSource(QString key) {
    kdDebug(90140) << "KonqSidebar_News::addedRSSSource: " << key << endl;

    // Only add RSS source if we have registered the URI before in
    // NSStackTabWidget.
    if (newswidget->isRegistered(key)) {
      NSPanel *nspanel = new NSPanel(this,
                      QString(QString("sidebar-newsticker-")+key).latin1(),
                      key, &m_rssservice);
      nspanel->setTitle(key);
      nspanelptrlist.append(nspanel);

      // add preliminary widgets for this newssource
      if (! nspanel->listbox()) {
        TTListBox *listbox = new TTListBox(newswidget, "article_lb");
        newswidget->addStackTab(nspanel, listbox);
        connect(listbox, SIGNAL(executed(QListBoxItem *)),
                this, SLOT(slotArticleItemExecuted(QListBoxItem *)));
        listbox->insertItem(i18n("Connecting..." ));
        nspanel->setListbox(listbox);
      }

      // listen to updates
      connect(nspanel, SIGNAL(documentUpdated(NSPanel *)),
              this, SLOT(updateArticles(NSPanel *)));
      connect(nspanel, SIGNAL(documentUpdated(NSPanel *)),
              this, SLOT(updateTitle(NSPanel *)));
      connect(nspanel, SIGNAL(pixmapUpdated(NSPanel *)),
              this, SLOT(updatePixmap(NSPanel *)));

      if (widgets->visibleWidget() != newswidget)
        widgets->raiseWidget(newswidget);
    }
  }


  void KonqSidebar_News::removedRSSSource(QString key) {
    kdDebug(90140) << "inside KonqSidebar_News::removedSource " << key << endl;

    if (NSPanel *nsp = getNSPanelByKey(key)) {
      newswidget->delStackTab(nsp);
      delete nspanelptrlist.take(nspanelptrlist.findRef(nsp));
    } else
      kdWarning() << "removedSource called for non-existing id" << endl;

    if (newswidget->isEmpty())
      widgets->raiseWidget(noRSSwidget);
  }


/////////////////////////////////////////////////////////////////////

  void KonqSidebar_News::slotArticleItemExecuted(QListBoxItem *item) {
    if (!item) return;

    NSPanel *current_nspanel, *nspanel = NULL;
    for (current_nspanel = nspanelptrlist.first(); current_nspanel;
         current_nspanel = nspanelptrlist.next()) {
      if (current_nspanel->listbox() == item->listBox())
        nspanel = current_nspanel;
    }

    int subid = nspanel->listbox()->index(item);
    QString link = nspanel->articleLinks()[subid];

    emit openURLRequest(KURL(link));

  }



//////////// update article headlines ////////////

  void KonqSidebar_News::updateArticles(NSPanel *nsp) {
    nsp->listbox()->clear();

    QStringList articleList = nsp->articles();
    QStringList::iterator it;
    for (it = articleList.begin(); it != articleList.end(); ++it)
      nsp->listbox()->insertItem((*it));
  }


/////////// Title stuff /////////////////////////////////////

  void KonqSidebar_News::updateTitle(NSPanel *nsp) {
    newswidget->updateTitle(nsp);
  }


/////////// Pixmap stuff /////////////////////////////////////

  void KonqSidebar_News::updatePixmap(NSPanel *nsp) {
    newswidget->updatePixmap(nsp);
  }



/////////////////////////////////////////////////////////


  extern "C" {
    KDE_EXPORT void* create_konq_sidebarnews(KInstance *instance, QObject *par,
                                             QWidget *widp,
                                             QString &desktopname,
                                             const char *name) {
      KGlobal::locale()->insertCatalogue("konqsidebar_news");
      return new KonqSidebar_News(instance, par, widp, desktopname, name);
    }
  }

  extern "C" {
    KDE_EXPORT bool add_konq_sidebarnews(QString* fn, QString*,
                                         QMap<QString,QString> *map) {
      map->insert("Type", "Link");
      map->insert("Icon", "konqsidebar_news");
      map->insert("Name", i18n("Newsticker"));
      map->insert("Open", "false");
      map->insert("X-KDE-KonqSidebarModule", "konq_sidebarnews");
      fn->setLatin1("news%1.desktop");
      return true;
    }
  }

} // namespace KSB_News

#include "sidebar_news.moc"
