// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
    Copyright: nspanel.cpp
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

#include <qlistview.h>
#include <qfontmetrics.h>
#include <qtimer.h>
#include <kdebug.h>
#include <klistbox.h>
#include "nspanel.h"


namespace KSB_News {

////////////////////////////////////////////////////////////////
// ListBox including ToolTip for item
////////////////////////////////////////////////////////////////
  TTListBox::TTListBox(QWidget *parent, const char *name, WFlags f)
    : KListBox(parent, name, f),
      QToolTip(this) {
  }

  void TTListBox::clear() {
    KListBox::clear();
  }

  void TTListBox::maybeTip(const QPoint &point) {
    QListBoxItem *item = itemAt(point);
    if (item) {
      QString text = item->text();
      if (!text.isEmpty()) {
        // Show ToolTip only if necessary
        QFontMetrics fm(fontMetrics());
        int textWidth = fm.width(text);
        int widgetSpace = visibleWidth();
        if ((textWidth > widgetSpace) || (contentsX() > 0))
          tip(itemRect(item), text);
      }
    }
  }



  NSPanel::NSPanel(QObject *parent, const char *name, const QString &key,
                   DCOPRef *rssservice)
    :QObject(parent, name)
    ,DCOPObject(QString(QString("sidebar-newsticker-")+key).latin1())
    ,m_listbox()
    ,m_pixmap()
{
    kdDebug(90140) << "NSPanel: CTOR " << key << " " << rssservice << endl;

    m_rssservice = rssservice;
    m_key = key;
    m_rssdocument = m_rssservice->call("document(QString)", m_key);
    m_isValid = false;

    connectDCOPSignal("rssservice", m_rssdocument.obj(),
                      "documentUpdated(DCOPRef)",
                      "emitDocumentUpdated(DCOPRef)", false);
    connectDCOPSignal("rssservice", m_rssdocument.obj(),
                      "documentUpdated(DCOPRef)",
                      "emitTitleUpdated(DCOPRef)", false);
    connectDCOPSignal("rssservice", m_rssdocument.obj(),
                      "pixmapUpdated(DCOPRef)",
                      "emitPixmapUpdated(DCOPRef)", false);

    // updating of RSS documents
    m_timeoutinterval = 10 * 60 * 1000;   // 10 mins
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(refresh()));
    m_timer->start(m_timeoutinterval);
    refresh();
  }


  void NSPanel::refresh() {
    m_rssdocument.call("refresh()");
  }


  NSPanel::~NSPanel() {
  }


  void NSPanel::setTitle(const QString &tit) {
    m_title = tit;
  }


  void NSPanel::setListbox(TTListBox *lb) {
    m_listbox = lb;
  }


  void NSPanel::setPixmap(const QPixmap &pm) {
    m_pixmap = pm;
  }


  TTListBox *NSPanel::listbox() const {
    return m_listbox;
  }


  QPixmap NSPanel::pixmap() {
    return m_pixmap;
  }


  QString NSPanel::key() const {
    return m_key;
  }

  QString NSPanel::title() const {
    return m_title;
  }

  QStringList NSPanel::articles() {
    return m_articles;
  }

  QStringList NSPanel::articleLinks() {
    return m_articlelinks;
  }

  bool NSPanel::isValid() const {
    return m_isValid;
  }


  void NSPanel::emitDocumentUpdated(DCOPRef /*dcopref*/) {
    kdDebug(90140) << "NSPanel::emitDocumentUpdated" << endl;

    m_articles.clear();
    m_articlelinks.clear();
    m_count = m_rssdocument.call("count()");
    QString temp = m_rssdocument.call("title()");
    m_title = temp;
    m_isValid = true;
    for (int idx = 0; idx < m_count; ++idx) {
      DCOPRef rss_article = m_rssdocument.call("article(int)", idx);
      m_articles.append(rss_article.call("title()"));
      m_articlelinks.append(rss_article.call("link()"));
    }

    emit documentUpdated(this);
  }

  void NSPanel::emitPixmapUpdated(DCOPRef /*dcopref*/) {
    if (m_rssdocument.call("pixmapValid()")) {
      QPixmap tmp = m_rssdocument.call("pixmap()");
      m_pixmap = tmp;

      emit pixmapUpdated(this);
    }
  }

} // namespace KSB_News

#include "nspanel.moc"
