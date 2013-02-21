/***************************************************************************
    begin                : Sun Oct 3 1999
    copyright            : (C) 1997-99 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#ifndef KSV_TRASH_H
#define KSV_TRASH_H

#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>

// forward declarations
class QLabel;
class KIconLoader;
class KSVItem;
class KSVAction;

class KSVTrash : public QFrame
{
  Q_OBJECT
  
public:
  KSVTrash (QWidget* parent = 0, const char* name = 0);
  virtual ~KSVTrash();
  
  virtual QSize sizeHint() const;

protected:
  /**
   * Overridden from @ref QDropSite
   */
  virtual void dragMoveEvent ( QDragMoveEvent* );
  
  /**
   * Overridden from @ref QDropSite
   */
  virtual void dragLeaveEvent ( QDragLeaveEvent* );

  /**
   * Overridden from @ref QDropSite
   */
  virtual void dropEvent ( QDropEvent* );

  virtual bool eventFilter ( QObject*, QEvent* );

private:
  KIconLoader* mKIL;
  QLabel* mLabel;
  bool mOpen;
  int mPixmapWidth;
  
signals:
  void undoAction (KSVAction*);
};

#endif
