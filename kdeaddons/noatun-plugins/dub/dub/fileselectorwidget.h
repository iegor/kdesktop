/*-*-c++-*-*****************************************************************
 *   Copyright (C) 2001 by Bernd Gehrmann bernd@kdevelop.org               *
 *   Copyright (C) 2002 by Eray Ozkural erayo@cs.bilkent.edu.tr            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _FILESELECTORWIDGET_H_
#define _FILESELECTORWIDGET_H_

#include <qlayout.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qstrlist.h>
#include <qtooltip.h>

#include <klistview.h>
#include <qvbox.h>
#include <qwidget.h>
#include <kfile.h>
#include <kurlcombobox.h>
#include <kurlcompletion.h>
#include <kcombobox.h>
#include <kprotocolinfo.h>
#include <kdiroperator.h>

class DubPlayListItem;

class FileSelectorWidget : public QWidget
{
  Q_OBJECT

  friend class DubPlayListItem;

public:
  FileSelectorWidget(QWidget *parent);
  ~FileSelectorWidget();

  KDirOperator* dirOperator() {return dir;}
  KDirLister* dirLister() { return dir->dirLister(); }
  KURL currentDirectory();

public slots:
  void slotFilterChange(const QString&);
  void setDir(KURL);

private slots:
  void cmbPathActivated( const KURL& u );
  void cmbPathReturnPressed( const QString& u );
  void dirUrlEntered( const KURL& u );
  void dirFinishedLoading();
  void fileHighlighted(const KFileItem *);
  void fileSelected(const KFileItem *);
//	void setCurrentDocDir();

protected:
  void focusInEvent(QFocusEvent*);

private:

  KURLComboBox *cmbPath;
  KHistoryCombo * filter;
  QLabel* filterIcon;
  KDirOperator * dir;
  QPushButton *home, *up, *back, *forward, *cfdir;

};

#endif
