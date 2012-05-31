// -*- C++ -*-
/* This file is part of the KDE project
   Copyright (C) 2004 Wilfried Huss <Wilfried.Huss@gmx.at>

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

#ifndef MARKLIST_H
#define MARKLIST_H

#include "pageNumber.h"

#include <qpixmap.h>
#include <qptrvector.h>
#include <qscrollview.h>

class QCheckBox;
class QLabel;
class KPopupMenu;

class DocumentPageCache;

class MarkList;
class MarkListWidget;


/****** ThumbnailWidget ******/


class ThumbnailWidget : public QWidget
{
  Q_OBJECT

public:
  ThumbnailWidget(MarkListWidget* parent_, const PageNumber& _pageNumber, DocumentPageCache*);

private:
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent*);

private slots:
  void setThumbnail();

private:
  PageNumber pageNumber;

  bool needsUpdating;

  DocumentPageCache* pageCache;

  MarkListWidget* parent;

  QPixmap thumbnail;
};


/****** MarkListWidget ******/


class MarkListWidget : public QWidget
{
  Q_OBJECT

public:
  MarkListWidget(QWidget* _parent, MarkList*, const PageNumber& _pageNumber, DocumentPageCache*, bool _showThumbnail = true);

  bool isChecked() const;

  bool isVisible();

public slots:
  void toggle();
  void setChecked( bool checked );

  void setSelected( bool selected );

  int setNewWidth(int width);

signals:
  /** Emitted when the Page is selected in the ThumbnailView. */
  void selected(const PageNumber&);

  /** Emitted on right click. */
  void showPopupMenu(const PageNumber& pageNumber, const QPoint& position);

protected:
  virtual void mousePressEvent(QMouseEvent*);

private:

  bool showThumbnail;

  ThumbnailWidget* thumbnailWidget;
  QCheckBox* checkBox;
  QLabel* pageLabel;
  QColor _backgroundColor;

  const PageNumber pageNumber;

  DocumentPageCache* pageCache;

  static const int margin = 5;

  MarkList* markList;
};


/****** MarkList ******/


class MarkList: public QScrollView
{
    Q_OBJECT

public:
  MarkList(QWidget* parent = 0, const char* name = 0);
  virtual ~MarkList();

  void setPageCache(DocumentPageCache*);

  QValueList<int> selectedPages() const;

  PageNumber currentPageNumber() { return currentPage; }

  PageNumber numberOfPages() { return widgetList.count(); }

  virtual QSize sizeHint() const { return QSize(); }

public slots:
  void setNumberOfPages(int numberOfPages, bool showThumbnails = true);

  void thumbnailSelected(const PageNumber& pageNumber);
  void setCurrentPageNumber(const PageNumber& pageNumber);

  void clear();

  void slotShowThumbnails(bool);
  void repaintThumbnails();

  void updateWidgetSize(const PageNumber&);

protected:
  virtual void viewportResizeEvent(QResizeEvent*);

  virtual void mousePressEvent(QMouseEvent*);

signals:
  void selected(const PageNumber&);

private slots:
  void showPopupMenu(const PageNumber& pageNumber, const QPoint& position);

  void selectAll();
  void selectEven();
  void selectOdd();
  void toggleSelection();
  void removeSelection();

private:
  QPtrVector<MarkListWidget> widgetList;

  PageNumber currentPage;

  PageNumber clickedThumbnail;

  DocumentPageCache* pageCache;

  bool showThumbnails;

  KPopupMenu* contextMenu;
};

#endif
