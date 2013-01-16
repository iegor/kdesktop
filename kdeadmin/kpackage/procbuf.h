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



#include "../config.h"
#include <kprocess.h>
#include <kdialog.h> 
#include <qobject.h> 
#include <qtimer.h>

#ifndef PROCBUF
#define PROCBUF

class Modal : public KDialog {
  Q_OBJECT
public:
  Modal(QString msg, QWidget *parent, const char * name );
  void terminate();
};

class procbuf: public QObject
{
  Q_OBJECT

public:
  procbuf();
  ~procbuf();
  void setup(QString);
  int start(QString  msg, bool errorDlg = TRUE,
	    int timeout=0, QString timeMsg = "");

  QString buf;
  KProcess *proc; 
  Modal *m;
  QString command;
  bool timed;
  QTimer *tm;

public slots:
  void slotReadInfo(KProcess *, char *, int);
  void slotExited(KProcess *);
  void slotTimeout();
};
#endif
