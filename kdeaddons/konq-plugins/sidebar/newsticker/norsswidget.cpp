// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
    norsswidget.cpp
    Copyright: Marcus Camen  <mcamen@mcamen.de>
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

#include <qlayout.h>
#include <qsizepolicy.h>
#include <dcopref.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <kconfigdialog.h>
#include "sidebarsettings.h"
#include "configfeeds.h"
#include "norsswidget.h"


namespace KSB_News {

  NoRSSWidget::NoRSSWidget(QWidget *parent, const char *name)
    : QWidget(parent, name) {

    QVBoxLayout *topLayout = new QVBoxLayout(this);

    topLayout->addStretch();

    KPushButton *btn = new KPushButton(i18n("&Configure"), this);
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    connect(btn, SIGNAL(clicked()), this, SLOT(slotBtnClicked()));
    topLayout->addWidget(btn, 0, Qt::AlignHCenter);

    topLayout->addStretch();
  }


  void NoRSSWidget::slotBtnClicked() {
    // An instance of your dialog could be already created and could be
    // cached, in which case you want to display the cached dialog
    // instead of creating another one
    if (KConfigDialog::showDialog("settings"))
      return;

    // KConfigDialog didn't find an instance of this dialog, so lets create it
    m_confdlg = new KConfigDialog(this, "settings", SidebarSettings::self(),
                                  KDialogBase::Plain,
                                  KDialogBase::Ok|KDialogBase::Cancel|KDialogBase::Default,
                                  KDialogBase::Ok,
                                  true);
    ConfigFeeds *conf_widget = new ConfigFeeds(0, "feedcfgdlg");
    m_confdlg->addPage(conf_widget, i18n("RSS Settings"), "pixmap_name");

    // User edited the configuration - update your local copies of the 
    // configuration data
    connect(m_confdlg, SIGNAL(settingsChanged()), this,
            SLOT(slotConfigure_okClicked()));

    m_confdlg->show();
  }


  void NoRSSWidget::slotConfigure_okClicked() {
    DCOPRef rss_document("rssservice", "RSSService");

    // read list of sources
    QStringList m_our_rsssources = SidebarSettings::sources();

    // add new sources
    QStringList::iterator it;
    for (it = m_our_rsssources.begin(); it != m_our_rsssources.end(); ++it) {
      rss_document.call("add", ( *it ));
    }

    // save configuration to disk
    SidebarSettings::setSources(m_our_rsssources);
    SidebarSettings::writeConfig();
  }

} // namespace KSB_News

#include "norsswidget.moc"
