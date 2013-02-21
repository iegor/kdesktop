/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
// Author: Damyan Pepper
//
// This widget is used to provide a list of all the properties that are
// found in the package's property dictionary
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


#ifndef PACKAGEPROPERTIES_H
#define PACKAGEPROPERTIES_H
#include "../config.h"

// Standard Headers

// Qt Headers
#include <qpainter.h>
#include <qstringlist.h>
#include <qregexp.h>

// KDE Headers
#include <klocale.h>
#include <ktextbrowser.h>

// kpackage Headers
#include "packageInfo.h"

class packageInfo;

class packagePropertiesWidget : public KTextBrowser
{
  Q_OBJECT
  ///////////// METHODS ------------------------------------------------------
public:

  packagePropertiesWidget(QWidget *parent=0);
  // constructor

  ~packagePropertiesWidget();
  // destructor

  void changePackage(packageInfo *p);

  void setSource( const QString & name );
  // url selected

protected:



  ///////////// DATA ---------------------------------------------------------
private:
  packageInfo *package;

  void initTranslate();
  void iList(const QString &txt, const QString &itxt);

  QDict<QString> trl ;
  // allow for translation of labels

  QStringList pList;
  // list specifying order of property distplay

  QStringList cList;
  // list giving order of currently displayed properties

  QString stmp;
  // text accumulation buffer

  void depends(const QString &f);
  // translate depends string

  void dor(const QString &f);
  // translate depends string

  void delement(const QString &f);
  // translate depends element
};


#endif
