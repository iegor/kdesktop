// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
    Copyright: nspanel.h
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

#ifndef _konq_sidebar_news_nspanelh_
#define _konq_sidebar_news_nspanelh_

#include <qstring.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <dcopref.h>
#include <dcopobject.h>
#include <kio/job.h>
#include <klistbox.h>


class QBuffer;
class QTimer;

namespace KSB_News {

////////////////////////////////////////////////////////////////
// ListBox including ToolTip for item
////////////////////////////////////////////////////////////////
  class TTListBox : public KListBox, QToolTip {
  public:
    TTListBox (QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    void clear();

  protected:
    virtual void maybeTip(const QPoint &);
  };



  class NSPanel : public QObject, public DCOPObject {
    Q_OBJECT
    K_DCOP

  public:
    NSPanel(QObject *parent, const char *name, const QString &key,
            DCOPRef *rssservice);
    ~NSPanel();

    void setTitle(const QString &tit);
    void setListbox(TTListBox *lb);
    void setPixmap(const QPixmap &pm);
    void setPixmapBuffer(QBuffer *buf);
    void setJob(KIO::Job *kio_job);

    TTListBox *listbox() const;
    QPixmap pixmap();
    QString key() const;
    QString title() const;
    QStringList articles();
    QStringList articleLinks();
    bool isValid() const;

  k_dcop:
    virtual void emitDocumentUpdated(DCOPRef);
    virtual void emitPixmapUpdated(DCOPRef);

  private:
    DCOPRef *m_rssservice;
    DCOPRef m_rssdocument;
    QString m_key;
    QString m_title;
    TTListBox *m_listbox;
    QPixmap m_pixmap;
    int m_count;
    QStringList m_articles;        // TODO: use proper container
    QStringList m_articlelinks;    // TODO: use proper container
    int m_timeoutinterval;
    QTimer *m_timer;
    bool m_isValid;

  signals:
    void documentUpdated(NSPanel *);
    void pixmapUpdated(NSPanel *);

  public slots:
    void refresh();

  };


} // namespace KSB_News

#endif
