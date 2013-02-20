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



#ifndef DEBLOCATE_H
#define DEBLOCATE_H

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
#include <qlineedit.h>

// KDE headers
#include <kbuttonbox.h>
#include <kdialogbase.h>

class pkgInterface;
class updateLoc;
class cacheObj;
class LcacheObj;
class KComboBox;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class dpanel : public QWidget
{
  Q_OBJECT

public:
  dpanel(QWidget *parent, const char * name  = 0);
  dpanel(dpanel *basep,  const QString &Pfilter,  bool bsubdirs,
	 QWidget *parent, const char * name = 0);
  ~dpanel();


  QString getText() const;
  void setText(const QString &s);
  bool getUse() const;
  void setUse(int n);
  bool getSubdirs() const;
  void setSubdirs(int n);

private:

  QCheckBox *psubdirs;
  QCheckBox *puse;
  QHBoxLayout* pack;
  QLineEdit *pent;
  QPushButton *pbut;

  dpanel *base;
  QString filter;

 public slots:
  void fileOpen();
  void dirOpen();

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class apanel : public QWidget
{
  Q_OBJECT

public:
  apanel( QWidget *parent, const char * name  = 0 );
  ~apanel();


  QString getText() const;
  void setText(const QString &s);
  bool getUse();
  void setUse(int n);
  void clear();

private:

  QCheckBox *puse;
  QHBoxLayout* pack;
  QLineEdit *pent;



 public slots:

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class updateLoc : public QWidget
{
  Q_OBJECT

public:

  updateLoc (QWidget *p, int panelNumber, pkgInterface *inter, const QString &iname);
  ~updateLoc();

  virtual void readSettings() = 0;
  virtual void writeSettings() = 0;

  virtual void applyS(LcacheObj *slist) = 0;

  void doBase(const QString &bmsg);

  QString interName;
  int panNumber;

  bool haveBase;

  pkgInterface *interface;

  QVBoxLayout *vf;
  dpanel *base;

  QGroupBox *fbase;
  QVBoxLayout* vbase;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class aUpdateLoc : public updateLoc
{
  Q_OBJECT

public:

  aUpdateLoc (QWidget *p, int panelNumber, pkgInterface *inter, const QString &iname,
			 int numberLines, const QString &label);
  ~aUpdateLoc();


  void readSettings();
  void writeSettings();

  void applyS(LcacheObj *slist);

private:

   QString packL, packU, packS,  availB;

  int wdth;

  QPushButton  *butloc;

  QHBoxLayout* hloc;

  int numLines;
  enum { PNUM = 100 };
  apanel *ap[PNUM];

  QVBoxLayout* vl;
  QVBoxLayout* vtop;
  QGroupBox *frame1;
  KButtonBox* hb;

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class pdUpdateLoc : public updateLoc
{
  Q_OBJECT

public:

  pdUpdateLoc (QWidget *p, int panelNumber, pkgInterface *inter,
	       const QString &iname, int numberLines, const QString &filter,
	       const QString &lmsg, QString bmsg = 0,
	       bool subdirs = FALSE);
  ~pdUpdateLoc();


  void readSettings();
  void writeSettings();

  void applyS(LcacheObj *slist);

private:

   QString packL, packU, packS,  availB;

  int wdth;

  QPushButton  *butloc;

  QHBoxLayout* hloc;

  int numLines;
  enum { PNUM = 40 };
  dpanel *dp[PNUM];

  QVBoxLayout* vl;
  QVBoxLayout* vtop;
  QGroupBox *frame1;
  KButtonBox* hb;

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class cUpdateLoc : public updateLoc
{
  Q_OBJECT

public:

  cUpdateLoc (QWidget *p, int panelNumber, pkgInterface *inter,
	      const QString &iname, int numberLines, const QString &baseMsg,
		       const QString &boxLabels, const QString &boxValues);
  ~cUpdateLoc();


  void readSettings();
  void writeSettings();

  void applyS(LcacheObj *slist);

private:

  QString  packC,  availB;

  QPushButton  *butloc;

  QHBoxLayout* hloc;

  int numLines;
  enum { PNUM = 40 };
  KComboBox *combo[PNUM];

  QVBoxLayout* vl;
  QVBoxLayout* vtop;
  QGroupBox *frame1;
  KButtonBox* hb;



};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class Locations : public KDialogBase
{
  Q_OBJECT

public:
  Locations( const QString &msg);
  ~Locations();

  void dLocations(int numberDirs,  int numberLines,
		  pkgInterface *inter, const QString &iname,
		  const QString &label, const QString &filter, const QString &dirMsg,
		  bool subdirs=TRUE );

  void pLocations(int numberDirs,  int numberLines,
		  pkgInterface *inter, const QString &iname,
		  const QString &label, const QString &filter,
		  const QString &packMsg, QString baseMsg = 0,
		  bool subdirs=FALSE);

  void cLocations(int numberDirs, int numberLines,
		  pkgInterface *inter, const QString &iname, const QString &label,
		  const QString &boxLabels, const QString &baseMsg, const QString &boxValues);

  void aLocations(int numberDirs,   int numberLines,
		 pkgInterface *inter, const QString &iname, const QString &label);



  // bmsg indicates the panel has a base entry
  void restore();

  int numPanels;
  enum { PANNUM = 10 };
  updateLoc *pn[PANNUM];

public slots:
  void apply_slot();
  void write_slot();

signals:
  void returnVal(LcacheObj *);

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif
