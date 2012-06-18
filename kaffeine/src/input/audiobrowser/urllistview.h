/*
 * urllistview.h
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef URLLISTVIEW_H
#define URLLISTVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <klistview.h>

class KPopupMenu;
class QWidget;
class QDragObject;
class PlaylistItem;
class MRL;
class KIO::Job;

class UrlListView : public KListView
{
   Q_OBJECT
public:
 UrlListView(QWidget *parent=0, const char *name=0);
 ~UrlListView();

 void setCleared(bool);
 bool getCleared() const; /* was the playlist cleared or should we save it? */

public slots:
  virtual void clear();  /* reimplement slot */

signals:
  void signalPlayItem(QListViewItem* );  /* play selected in context menu */
  void signalCut();
  void signalCopy();
  void signalPaste();
  void signalSelectAll();
  void signalAddToQueue(MRL);
  void signalPlaylistFromSelected();

private slots:
  void slotShowContextMenu(QListViewItem*, const QPoint&, int);
  void slotCurrentChanged(QListViewItem *);
  void slotAddSubtitle();
  void slotShowInfo();
  void slotEditTitle();
  void slotPlayItem();
  void slotClicked(QListViewItem*, const QPoint&, int);
  void slotPlayNext();
  void openSubResult( KIO::Job* );
  void openSubData( KIO::Job*, const QByteArray& );

protected:
  virtual bool acceptDrag(QDropEvent* event) const;
  virtual void resizeEvent(QResizeEvent*);
  virtual QDragObject* dragObject();

private:
  void enableSubEntry();
  void disableSubEntry();

private:
  bool m_listCleared;
  int m_column5Width;  /* width of the fifth column */
  int m_column4Track;  /* width of the fourth column */

  PlaylistItem* m_itemOfContextMenu;
  KPopupMenu* m_contextMenu;
  KIO::Job *openSubJob;
  QByteArray openSubJobBuf;

};

#endif /* URLLISTVIEW_H */
