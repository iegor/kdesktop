/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
// Author: Damyan Pepper
//
// This file contains the definition of the class packageInfo
//
// packageInfo is used to store information regarding an  package.
// This information is normally gained by querying the  database or
// by querying an  package.
//
// The package information consists of a set of properties.  These 
// properties are stored in a dictionary that is passed to the 
// constructor.  The properties can be accessed using the function
// `getProperty'.
//
// In addition, packageInfo objects can place themselves inside
// a tree list with the function `place'.  Doing this creates
// a tree list item object that can be accessed with the function
// `item'.
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

#ifndef PACKAGEINFO_H
#define PACKAGEINFO_H
#include "../config.h"

#include <qmap.h>
#include <qdict.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qlistview.h>

class pkgInterface;
class KpTreeListItem;
class KpTreeList;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class packageInfo
{
public:
  packageInfo(QMap<QString, QString> _info, pkgInterface *type);
  // Constructor: create a packageInfo object using the property
  // dictionary from _info

  packageInfo(QMap<QString, QString> _info, const QString &_filename);
  // Constructor: same as above, but also sets filename to _filename.
  // This is used in the case that the package info was obtained from
  // an uninstalled package.

  ~packageInfo();
  // Distructor

  /**
   *  Look ups the property `property' from the dictionary
   */
  QString getProperty(const QString &property);

  /**
   *  Checks whether the property `property' is defined in the dictionary
   */
  bool hasProperty(const QString &property);

  void fixup();
  // Initialize fields if missing

  KpTreeListItem *place(KpTreeList *tree, bool InsertI=FALSE);
  // places the object in the treelist `tree' and initialises
  // `item'.  If necessary, new groups will be added to `tree'.

  KpTreeListItem  *getItem();
  // returns the treelist item object for this package or
  // NULL if the object hasn't been placed

  void setFilename(const QString &f);

  QString getUrl();
  // return URL of package file

  bool isFileLocal();
  // True if package file is local or cached file

  bool isInstallable();
  // True if package can be installed

  bool isFetchable();
  // True if package needs to be fetched

  QString getFilename();

  QString fetchFilename();
  // gets the filename, fetching package if necessary

  int newer(packageInfo *p);
  // if package p is newer

  void pkgFileIns(const QString &fileName);
  // Insert a package from a file into package tree

  bool pkgInsert(QPtrList<packageInfo> *pki, const QString &exp, bool installed,
	      bool infoPackage = FALSE);
  // insert packgeInfo either installed or not installed

  QMap<QString, QString> info;
  // This stores the property dictionary of the package

  KpTreeListItem *item;
  // This stores the tree list item for this package (or NULL if
  // the package hasn't been placed in a tree list)

  pkgInterface *interface;
  // interface points to the class of the package (deb, rpm etc)

  bool smerge( const QString &exp);
  // merge with already existing NOLIST package info

  bool display(int treeType);
  // if this package is to be should in the tree list

  enum {UNSET, AVAILABLE,  INSTALLED, BAD_INSTALL, UPDATED,
	NEW, NOLIST, HIDDEN};
  int packageState;

  bool updated;

private:
  int getDigElement(const QString &s, int *pos);
  QString getNdigElement(const QString &s, int *pos);
  // break up version string

  int pnewer(const QString &s, const QString &sp);
  // compare parts of a version string
  
  QString url;
  // This stores the filename of the package the info was obtained from.
  // If it is empty then the info was obtained from an installed package.

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#endif

