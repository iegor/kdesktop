/***************************************************************************
    begin                : Tue Oct 5 1999
    copyright            : (C) 1999 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#ifndef KDLTOOLTIP_H
#define KDLTOOLTIP_H

#include <qtooltip.h>

class KSVDragList;

/**
  * @short custom tooltip for use in @ref KDragList
  * @author Peter Putzer
  */
class KDLToolTip : public QToolTip
{
public:
  KDLToolTip (KSVDragList *parent, QToolTipGroup* group = 0L);
  virtual ~KDLToolTip();

protected:
  /**
   * Reimplemented from QToolTip for internal reasons.
   */
  virtual void maybeTip (const QPoint&);

private:
  KSVDragList* mParent;
};

#endif

