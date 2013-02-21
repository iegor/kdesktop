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



#ifndef OPTIONS_H
#define OPTIONS_H

#include "../config.h"

// Standard Headers
#include <stdio.h>

// Qt Headers
#include <qdir.h>
#include <qwidget.h>
#include <qfiledialog.h>
#include <qgroupbox.h>

// KDE headers
#include <kapplication.h>
#include <kfiledialog.h>

#include <kpackage.h>

class KURLRequester;


class QVBoxLayout;
class QGroupBox;
class QCheckBox;
class QPushButton;
class KComboBox;
class QButtonGroup;
class QRadioButton;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class Options : public KDialogBase
{
  Q_OBJECT

public:

  Options ( QWidget *parent = 0);
  ~Options();

  void restore();
  // show window, setting the buttons

  void setValues();
  // set the dialog to match options values

private:

  bool verifyFL;
  bool PkgRead;
  bool fRemote;
  int  DCache, dc, PCache, pc, privCmd, prc;
  QString CacheDir;

  QVBoxLayout* vl;

  QVBoxLayout* vt;
  QGroupBox *framet;
  QGroupBox *packageBox[kpinterfaceN];
  QCheckBox *packageHandle[kpinterfaceN];
  QPushButton *locate[kpinterfaceN];

  QGroupBox *hh;
  QCheckBox *huse;
  KComboBox *hosts;

  QVBoxLayout* vb;
  QButtonGroup *bg;
  QRadioButton *disp[4];

  QVBoxLayout*  vc;
  QButtonGroup *bc;
  QRadioButton *dcache[3];

  QVBoxLayout*  vp;
  QButtonGroup *bp;
  QRadioButton *pcache[3];

  QVBoxLayout*  vs;
  QButtonGroup *bs;
  QRadioButton *privs[3];
  
  KURLRequester *cachedir;

  QVBoxLayout* vr;
  QGroupBox *framer;
  QCheckBox *pkgRead;

  QVBoxLayout* vf;
  QGroupBox *framem;
  QCheckBox *valid;


public slots:
    void scanLocates();
    void apply_slot();
    void cancel_slot();
    void PDCache(int);
    void PPCache(int);
    void PPrivs(int);
  
private slots:
    void insHosts();
    void useRemote();
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class Opts
{
public:
  void readSettings(const QString &initHost = "");
  void writeSettings();

  void readLaterSettings();
  // options to be setup after package interfaces setup

  QStringList hostList;
  // list of hosts to choose from

  bool VerifyFL;
  // config: verify the file list

  bool PkgRead;
  // read information about uninstalled packages from each RPM file itself

  bool handlePackage[kpinterfaceN];

  enum {INSTALLED, UPDATED, NEW, ALL};
  enum {ALWAYS, SESSION, NEVER};

  int DCache;
  // how much to cache uninstall package directories

  int PCache;
  // how much to cache uninstall packages
  
  int privCmd;
  // which command to use to execute priveliged commands
  enum {SUcmd=0, SUDOcmd=1, SSHcmd=2};

  QString CacheDir;
  // cache directory

  Opts(const QString &initHost);
  ~Opts();
};
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#endif
