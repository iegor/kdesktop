/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
// This provides the installation options, plus the install and cancel
// buttons.  When the install button is clicked, the current package
// is installed.
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


#ifndef PKGINSTALL_OPTIONS
#define PKGINSTALL_OPTIONS

#include "../config.h"
// Qt headers
#include <qframe.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcheckbox.h>

#include <kdialog.h>

// kpackage headers
#include "packageInfo.h"
#include "pkgInterface.h"
#include "kpTerm.h"

class KListView;
class KPushButton;

class pkgOptions : public KDialog
{
  Q_OBJECT
public:
  pkgOptions( pkgInterface *pki, QWidget *parent=0, const QString &caption=QString::null);
  ~pkgOptions();

  bool setup(QPtrList<packageInfo> *pl, const QString &type);
  void setup(packageInfo *p, const QString &type);
  void reset();

  virtual QString doPackages(int installFlags, QPtrList<packageInfo> *p,
			     bool &test) = 0;

  QCheckBox **Boxs;
  // options buttons

  QCheckBox *Keep;
  // keep window 

  KListView *packages;

  int bnumber;
  // number of option buttons

  QLabel *title;
  // Widget title

  kpTerm *term;

  pkgInterface *pkgInt;

  bool modified;
  bool test;
  bool running;

protected:
  // This sets up the sub-widgets
  void setupWidgets(QPtrList<param> &pars);

private slots:  
  virtual void pkginstallButtonClicked();
  virtual void cancelButtonClicked();
  void slotSearch(QListViewItem *item);
  void keepToggle(bool);

public slots:
 void slotResult(QStringList &rlist, int ret);

signals:
  // This signal indicates that the widget has finished.
  void finished(int refresh);

protected:
  // The layout managers
  QBoxLayout *layout, *hlayout, *vlayout;

  // Sub widgets
  QPushButton *installButton;
  KPushButton *cancelButton;

  QString insType;
  bool installer;
  // install or uninstall

  bool keep;
  // keep the window

  QPtrList<packageInfo> *packList;

  void getKeep();

  void showEvent (QShowEvent *);
  void closeEvent (QCloseEvent * e );

  void terminate();
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class pkgOptionsI: public pkgOptions
{
  Q_OBJECT
public:
  pkgOptionsI(pkgInterface *pkg, QWidget *parent = 0);
  QString doPackages(int installFlags, QPtrList<packageInfo> *p, bool &test);
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class pkgOptionsU: public pkgOptions
{
  Q_OBJECT
public:
  pkgOptionsU(pkgInterface *pkg, QWidget *parent = 0);
  QString doPackages(int installFlags, QPtrList<packageInfo> *p, bool &test);
};

#endif
