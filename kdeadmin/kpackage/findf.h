/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/


#ifndef FINDF_H
#define FINDF_H

#include "../config.h"

// Standard Headers
#include <stdio.h>

// Qt Headers
#include <qdir.h>
#include <qwidget.h>
#include <qframe.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlayout.h>

// KDE headers
#include <kapplication.h>
#include <kmenubar.h>
#include <klistview.h>
#include <kdialogbase.h>

class FindF : public KDialogBase
{
  Q_OBJECT

public:

  FindF ( QWidget *parent = 0);
  ~FindF();
  void resizeEvent(QResizeEvent *);
  void dropEvent(QDropEvent *);
  void dragEnterEvent(QDragEnterEvent* e);
  void checkSearchAll();

  
private:
  void doFind(const QString &str);
  // Do the actual search

  QLineEdit *value;
  QListView *tab;
  QVBoxLayout* vl;
  QVBoxLayout* vtop, vf;

  QHBoxLayout* hb;
  QCheckBox *searchAll;
  QPixmap tick;

signals:
    void findf_signal();
    void findf_done_signal();

public slots:
  void done_slot();
  void ok_slot();
  void search(QListViewItem *);
  void textChanged ( const QString & text);

};

#endif
