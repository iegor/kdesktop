// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
                          nsstacktabwidget.cpp  -  description
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

#include <qpushbutton.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qptrdict.h>
#include <qsizepolicy.h>
#include <qtooltip.h>
#include <qcursor.h>
#include <qimage.h>
#include <kdebug.h>
#include <kaboutdata.h>
#include <kaboutapplication.h>
#include <kpopupmenu.h>
#include <kiconloader.h>
#include <kstringhandler.h>
#include <kapplication.h>
#include <kbugreport.h>
#include <kstdguiitem.h>
#include <kconfigdialog.h>
#include "sidebarsettings.h"
#include "configfeeds.h"
#include "nsstacktabwidget.h"


namespace KSB_News {

  NSStackTabWidget::NSStackTabWidget(QWidget *parent, const char *name,
                                     QPixmap appIcon) : QWidget(parent, name) {
    currentPage = 0;
    layout = new QVBoxLayout(this);

    pagesheader.setAutoDelete(TRUE);
    pages.setAutoDelete(TRUE);

    // about dialog
    m_aboutdata = new KAboutData("konq_sidebarnews",
                                 I18N_NOOP("Newsticker"),
                                 KDE::versionString(),
                                 I18N_NOOP("RSS Feed Viewer"),
                                 KAboutData::License_LGPL,
                                 I18N_NOOP("(c) 2002-2004, the Sidebar Newsticker developers"));
    m_aboutdata->addAuthor("Marcus Camen", I18N_NOOP("Maintainer"),
                           "mcamen@mcamen.de");
    m_aboutdata->addAuthor("Frerich Raabe", "librss",
                           "raabe@kde.org");
    m_aboutdata->addAuthor("Ian Reinhart Geiser", "dcoprss",
                           "geiseri@kde.org");
    m_aboutdata->addAuthor("Joseph Wenninger",
                           I18N_NOOP("Idea and former maintainer"),
                           "jowenn@kde.org");
    m_aboutdata->setProductName("konqueror/sidebar newsticker");
    m_about = new KAboutApplication(m_aboutdata, this);


    // bugreport dialog
    m_bugreport = new KBugReport(0, true, m_aboutdata);


    // popup menu
    popup = new KPopupMenu(this);
    popup->insertItem(KStdGuiItem::configure().iconSet(),
                      i18n("&Configure Newsticker..."), this,
                      SLOT(slotConfigure()));
    popup->insertItem(SmallIconSet("reload"), i18n("&Reload"), this,
                      SLOT(slotRefresh()));
    popup->insertItem(KStdGuiItem::close().iconSet(),
                      KStdGuiItem::close().text(), this, SLOT(slotClose()));
    popup->insertSeparator();

    // help menu
    helpmenu = new KPopupMenu(this);
    helpmenu->insertItem(appIcon, i18n("&About Newsticker"), this,
                      SLOT(slotShowAbout()));
    helpmenu->insertItem(i18n("&Report Bug..."), this,
                      SLOT(slotShowBugreport()));

    popup->insertItem(KStdGuiItem::help().iconSet(),
                      KStdGuiItem::help().text(), helpmenu);


    // read configuration from disk and initialize widget
    m_our_rsssources = SidebarSettings::sources();
  }


  void NSStackTabWidget::addStackTab(NSPanel *nsp, QWidget *page) {
    QPushButton *button = new QPushButton(this);

    button->setText(KStringHandler::rPixelSqueeze(nsp->title(),
                                                  button->fontMetrics(),
                                                  button->width() - 4 ));
    button->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                      QSizePolicy::Preferred));
    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    QToolTip::add(button, nsp->title());

    // eventFiler for the title button
    button->installEventFilter(this);

    QScrollView *sv = new QScrollView(this);
    sv->setResizePolicy(QScrollView::AutoOneFit);
    sv->addChild(page);
    sv->setFrameStyle(QFrame::NoFrame);
    page->show();

    pagesheader.insert(nsp, button);
    pages.insert(nsp, sv);

    layout->addWidget(button);
    layout->addWidget(sv);
    button->show();
    if (pages.count() == 1) {
      currentPage = sv;
      sv->show();
    } else {
      sv->hide();
    }
  }



  void NSStackTabWidget::delStackTab(NSPanel *nsp) {
    pages.remove(nsp);
    pagesheader.remove(nsp);

    if (pages.count() >= 1) {
      QPtrDictIterator<QWidget> it(pages);
      QWidget *previousPage = currentPage;
      currentPage = it.current();
      if (currentPage != previousPage)
        currentPage->show();
    }
  }



  void NSStackTabWidget::updateTitle(NSPanel *nsp) {
    QPushButton *pb = (QPushButton *)pagesheader.find(nsp);
    if (! pb->pixmap())
      pb->setText(nsp->title());
  }



  void NSStackTabWidget::updatePixmap(NSPanel *nsp) {
    QPushButton *pb = (QPushButton *)pagesheader.find(nsp);
    QPixmap pixmap = nsp->pixmap();
    if ((pixmap.width() > 88) || (pixmap.height() > 31)) {
      QImage image = pixmap.convertToImage();
      pixmap.convertFromImage(image.smoothScale(88, 31, QImage::ScaleMin));
    }
    pb->setPixmap(pixmap);
  }




  void NSStackTabWidget::buttonClicked() {
    QPushButton *pb = (QPushButton*)sender();
    NSPanel *nsp = NULL;

    // Which NSPanel belongs to pb
    QPtrDictIterator<QWidget> it(pagesheader);
    for (; it.current(); ++it) {
      QPushButton *currentWidget = (QPushButton *)it.current();
      if (currentWidget == pb)
        nsp = (NSPanel *)it.currentKey();
    }

    if (! nsp)
      return;

    // Find current ScrollView
    QWidget *sv = pages.find(nsp);

    // Change visible page
    if (currentPage != sv) {
      nsp->refresh();
      if (currentPage)
        currentPage->hide();
      currentPage = sv;
      currentPage->show();
    }
  }



  bool NSStackTabWidget::eventFilter(QObject *obj, QEvent *ev) {
    if (ev->type() == QEvent::MouseButtonPress
        && ((QMouseEvent *)ev)->button() == QMouseEvent::RightButton) {
      m_last_button_rightclicked = (QPushButton *)obj;
      popup->exec(QCursor::pos());
      return true;
    } else if (ev->type() == QEvent::Resize) {
      QPushButton *pb = (QPushButton *)obj;

      const QPixmap *pm = pb->pixmap();
      if ( ! pm ) {
        // Which NSPanel belongs to pb
        NSPanel *nsp = NULL;
        QPtrDictIterator<QWidget> it(pagesheader);
        for (; it.current(); ++it) {
          QPushButton *currentWidget = (QPushButton *)it.current();
          if (currentWidget == pb)
            nsp = (NSPanel *)it.currentKey();
        }
        pb->setText(KStringHandler::rPixelSqueeze(nsp->title(),
                                                  pb->fontMetrics(),
                                                  pb->width() - 4 ));
      }
      return true;
    }
    return false;   // pass through event
  }


  void NSStackTabWidget::slotConfigure() {
    // An instance of your dialog could be already created and could be
    // cached, in which case you want to display the cached dialog
    // instead of creating another one
    if (KConfigDialog::showDialog("settings"))
      return;

    // KConfigDialog didn't find an instance of this dialog, so lets create it
    m_confdlg = new KConfigDialog(this, "settings", SidebarSettings::self(),
                                  KDialogBase::Plain,
                                  KDialogBase::Ok|KDialogBase::Cancel| \
                                                  KDialogBase::Default| \
                                                  KDialogBase::Apply,
                                  KDialogBase::Ok,
                                  true);
    ConfigFeeds *conf_widget = new ConfigFeeds(0, "feedcfgdlg");
    m_confdlg->addPage(conf_widget, i18n("RSS Settings"), QString());

    // User edited the configuration - update your local copies of the 
    // configuration data
    connect(m_confdlg, SIGNAL(settingsChanged()), this,
            SLOT(slotConfigure_okClicked()));

    m_confdlg->show();
  }


  void NSStackTabWidget::slotConfigure_okClicked() {
    DCOPRef rss_document("rssservice", "RSSService");

    // remove old sources and old stack tabs
    QStringList::iterator it;
    for (it = m_our_rsssources.begin(); it != m_our_rsssources.end(); ++it) {
      rss_document.call("remove", (*it));
    }

    // read list of sources
    m_our_rsssources = SidebarSettings::sources();

    // add new sources and new stack tabs
    for (it = m_our_rsssources.begin(); it != m_our_rsssources.end(); ++it) {
      rss_document.call("add", (*it));
    }

    // save configuration to disk
    SidebarSettings::setSources(m_our_rsssources);
    SidebarSettings::writeConfig();
  }


  void NSStackTabWidget::slotShowAbout() {
    m_about->show();
  }


  void NSStackTabWidget::slotShowBugreport() {
    m_bugreport->show();
  }


  void NSStackTabWidget::slotRefresh() {
    NSPanel *nsp = NULL;

    // find appendant NSPanel
    QPtrDictIterator<QWidget> it(pagesheader);
    for (; it.current(); ++it) {
      QPushButton *currentWidget = (QPushButton *)it.current();
      if (currentWidget == m_last_button_rightclicked)
        nsp = (NSPanel *)it.currentKey();
    }

    if (! nsp) {
      return;
    } else {
      nsp->refresh();
    }
  }


  void NSStackTabWidget::slotClose() {
    NSPanel *nsp = NULL;
    // find appendant NSPanel
    QPtrDictIterator<QWidget> it(pagesheader);
    for (; it.current(); ++it) {
      QPushButton *currentWidget = (QPushButton *)it.current();
      if (currentWidget == m_last_button_rightclicked)
        nsp = (NSPanel *)it.currentKey();
    }

    if (! nsp) {
      return;
    } else {
      // TODO: check, if rssservice is available

      // deregister RSS source and save configuration to disk
      m_our_rsssources.remove(nsp->key());
      SidebarSettings::setSources(m_our_rsssources);
      SidebarSettings::writeConfig();

      DCOPRef rss_document("rssservice", "RSSService");
      rss_document.call("remove", nsp->key());
    }
  }


  bool NSStackTabWidget::isEmpty() const {
    return pagesheader.isEmpty();
  }


  bool NSStackTabWidget::isRegistered(const QString &key) {
    m_our_rsssources = SidebarSettings::sources();
    if (m_our_rsssources.findIndex(key) == -1)
      return false;
    else
      return true;
  }


} // namespace KSB_News;


#include "nsstacktabwidget.moc"
