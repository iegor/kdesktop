/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
                2001-2003 by Stanislav Visnovsky <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */
#ifndef CATMANLISTITEM_H
#define CATMANLISTITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qstringlist.h>
#include <qlistview.h>
#include <qdatetime.h>
#include <qfileinfo.h>

#include "validateprogress.h"

class CatalogManagerView;
class QPixmap;
class KDataTool;

namespace KBabel
{
    class PoInfo;
}

class CatManListItem : public QListViewItem
{
public:
   /** the type of this item */
   enum Type{File,Dir};

   CatManListItem(CatalogManagerView *view, QListViewItem* parent,QString fullPath
				   ,QString fullPotPath,QString package);

   /** creates the toplevel root item with package name "/" */
   CatManListItem(CatalogManagerView *view, QListView* parent,QString fullPath, QString fullPotPath);

   /**
	* returns the package names (including relative path) of the
	* children of this item
	*/
   QStringList contentsList(bool onlyFiles=false) const;
   /**
   * returns the package names of all children of this item
   * (including all subdirectries)
   * @param onlyFiles flag, if only the names of files should be returned
   * @see CatManListItem::contentsList
   */
   QStringList allChildrenList(bool onlyFiles=false) const;

   /**
   * returns the relative file names of all children of this item
   * (including all subdirectries)
   * @param onlyFiles flag, if only the names of files should be returned
   * @param emptyDirs flag, if the empty dirs (dirs without PO files in them) should be returned
   * @param onlyModified, if only modified files should be returned
   * @see CatManListItem::contentsList
   */
   QStringList allChildrenFileList(bool onlyFiles=false, bool emptyDirs=false, bool onlyModified=false) const;

   void setMarked(bool on);
   bool marked() const;
   /**
   * checks if the file on the disc has changed,
   * reads information about the file and displays it
   * @param noParents flag, if the update has to include the parent
   * of the item, if the status has changed. Since at the first build of
   * the tree, the status of every item changes, this is not useful then.
   */
   void checkUpdate(bool noParents=false);
   void forceUpdate();

   /**
   * checks the corresponding PO file using validation tool. On
   * errors it fills the list of errors, which can be accessed
   * using @see errors().
   * @param validator  instance of KDataTool to be used for checking
   * @param progressSignalHangler widget, to which the checks should send progress signals
   * @param ignoreFuzzy flag, whether fuzzy messages in the file should be not checked
   * @param markAsFuzzy flag, whether the error messages should be marked as fuzzy (this alters the PO file)
   */
   void checkErrors(KDataTool* validator, QObject* progressSignalHandler, bool ignoreFuzzy, bool markAsFuzzy);

   /** return the absolute filename of the po-File */
   QString poFile() const;
   /** return the absolute filename of the pot-File */
   QString potFile() const;
   /** returns the package name (inlcuding relative path to base-directory) */
   QString package(bool rootSlash=true) const;

   /** returns the relative path of a dir or QString::null if not a dir. */
   QString packageDir( ) const;

   /** returns the package name (without path) */
   QString name() const;

   /**
   * returns the type of this item
   * @see CatManListItem::Type
   */
   Type type() const{return _type;}
   bool isDir() const;
   bool isFile() const;
   /** returns true, if the po-file exists*/
   bool hasPo() const;
   /** returns true, if the pot-file exists*/
   bool hasPot() const;
   bool isModified() const;
   /**
	* @return the number of fuzzy messages in the po-file,
	* 0 if no po-file exists
	*/
   int fuzzy() const;
   /**
	* @return the number of untranslated messages in the po-file,
	* @ref total if no po-file exists
	*/
   int untranslated() const;
   /** @return number of messages in the po- or pot-file */
   int total() const;
   /**
	* @return true, if there are untranslated or fuzzy items.
	* If this item is a directory, it returns true, if a subitem
	* contains untranslated or fuzzy items
	*/
   bool needsWork() const;
   /**
	* @return true, if there were errors while parsing the file
	*/
   bool hasErrors() const {return _hasErrors;}
   QValueList<IgnoreItem> errors() const {return _errors;}

   virtual QString key(int col,bool) const;
   virtual void setOpen(bool);

   /** paints the marking, if this package has no template */
   QPixmap paintExclamation(QPixmap*);

   void updateAfterSave( KBabel::PoInfo &po);

   QStringList &wordList() { return _wordList; }
   bool wordsUpdated() { return _wordListUpdated; }

   /** These are not in Qt, so we need to implement it ourselves*/
   QListViewItem *previousSibling();
   QListViewItem *lastChild();

private:
   void init(const QString& fullPath, const QString& fullPotPath,const QString& package);
   /**
   * updates the item
   * @param showPoInfo if true, reads information about the
   * file using @ref Catalog::info
   * ( slow for big files )
   * @param includeChildren flag, if possible children should be updated,too
   * @param noParents flag, if parents should be updated, when state
   * of the item has changed
   */
   void update(bool showPoInfo=true,bool includeChildren=false
				   , bool noParents=false );
   void updateParents();

private:
   /**
   * holds the date and the time this item was
   * last updated. This is used to check, if the file
   * on the disc has changed since last update.
   */
   QDateTime _lastUpdated;

   /** the po-file */
   QFileInfo _primary;
   /** the pot-file */
   QFileInfo _template;
   /**
   * The package name, includes the relative path beginning
   * at the base directory.
   * The package name begins with "/" and if this is a directory it end with "/"
   * The root item has the package name "/"
   * @see CatManListItem::CatManListItem
   */
   QString _package;
   Type _type;
   bool _marked;

   /** flag, to detect if file has been deleted or is new */
   bool _hasPo;
   /** flag, to detect if file has been deleted or is new */
   bool _hasPot;

   bool _isModified;
   /** flag, to detect if file has been modified or new */

   /** flag, if the PO-file has a syntax error */
   bool _hasErrors;
   /** a list of errors found by validation tool*/
   QValueList<IgnoreItem> _errors;

   /** parent view for this item, used for stopping the activity */
   CatalogManagerView *_view;

   /** index of words, but it does not contain any useful information as values */
   QStringList _wordList;
   bool _wordListUpdated;
};

#endif // CATMANLISTITEM_H
