/***************************************************************************
                          navview.h  -  description
                             -------------------
    begin                : Tue May 1 2001
    copyright            : (C) 2001 by Richard Moore
    email                : rich@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NAVVIEW_H
#define NAVVIEW_H

#include <qwidget.h>
#include <klistview.h>

class NavViewItem;

/**
  * @author Richard Moore
  */
class NavView : public KListView
{
  Q_OBJECT

  public: 
    NavView( QWidget *parent = 0, const char *name = 0 );
    ~NavView();

    /**
      Builds the complete tree.
     */
    void buildTree();

    /**
      Expands all currently visible items.
     */
    void expandVisibleTree();

  signals:
    void selected( QObject *object );

  protected slots:
     void selectItem( QListViewItem *item );

  private:
    void createSubTree( NavViewItem* );
};

#endif
