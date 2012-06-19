/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

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
#ifndef KBABELDICTBOX_H
#define KBABELDICTBOX_H

#include "kbabeldictiface.h"
#include "searchengine.h"
#include "catalog.h"

#include <qdict.h>
#include <qguardedptr.h>
#include <qptrlist.h>
#include <qstringlist.h>

class KListView;
class QLabel;
class QListViewItem;
class QPopupMenu;
class QPushButton;
class QSplitter;
class QTextView;
class QToolButton;

struct ModuleInfo
{
   QString id;
   QString name;
   
   bool editable;
};


class KDE_EXPORT KBabelDictBox : public QWidget, virtual public KBabelDictIFace
{ 
    Q_OBJECT

public:
   KBabelDictBox( QWidget* parent = 0, const char* name = 0
                  , WFlags fl = 0 );
   ~KBabelDictBox();

   /** @returns ids of available modules */
   virtual QStringList modules();

   /** @returns (translated) names of available modules */
   QStringList moduleNames();

   /**
    * @returns a list with information about the available modules
    * Take care about, that you have to delete the items by yourself.
    */
   QPtrList<ModuleInfo> moduleInfos();
   
   /** @returns preferencesWidgets of modules */
   QPtrList<PrefWidget> modPrefWidgets(QWidget* parent);
   
   int activeModule();
   bool activeModuleEditable();

   /**
    * lets the modules always update their options before
    * starting a new search
    */
   void setAutoUpdateOptions(bool on);

   void nextResult();
   void prevResult();
   void startSearch(const QString);
   void startTranslationSearch(const QString);
   void startDelayedSearch(const QString);
   void startDelayedTranslationSearch(const QString);
   QString translate(const QString);
   QString searchTranslation(const QString, int& score);
   QString fuzzyTranslation(const QString, int& score);
   void stopSearch();
   void setActiveModule(QString name);

   /**
    * @returns true, if a search is active at the moment
    */
   bool isSearching();

   /**
    * Saves the settings including the settings of the different modules
    * in the given config object.
    */
   void saveSettings(KConfigBase *config);
   
   /**
    * Saves the settings of the given module
    * in the given config object.
    */
   void saveSettings(const QString& moduleId, KConfigBase *config);
   
   /**
    * Reads the settings including the settings of the different modules
    * from the given config object.
    */
   void readSettings(KConfigBase *config);

   /**
    * Reads the settings of the given module
    * from the given config object.
    */
   void readSettings(const QString& moduleId, KConfigBase *config);
   


   /** @returns the translation of the current search result */
   QString translation();

   bool hasSelectedText() const;
   QString selectedText() const;

   void setRMBMenu(QPopupMenu *popup);
   

   virtual bool eventFilter(QObject *, QEvent*);
   

   /**
    * A hack for kbabel to get a list of messages for a specific package
    * from dbsearchengine. When dbsearchengine allows multiple access to
    * the database this will get removed.
    */
   bool messagesForPackage(const QString& package
           , QValueList<KBabel::DiffEntry>& resultList, QString& error); 
   
public slots:
   virtual void setActiveModule(int);
   void slotStartSearch(const QString&);
   void slotStopSearch();
   void slotNextResult();
   void slotPrevResult();

   void about();
   void aboutModule(const QString& moduleID);
   void aboutActiveModule();

   /**
    * sets the name of the package currently edited
    */
   void setEditedPackage(const QString& packageName);
   
   /**
    * sets the filepath of the package currently edited
    */
   void setEditedFile(const QString& path);

   /** sets the language code to use */
   void setLanguage(const QString& languageCode
           , const QString& languageName);


   void setTextChanged(const QStringList& orig, const QString& translation,
           uint pluralForm, const QString& description);
   
   /**
    * if text is marked, copy this into the clipboard, otherwise
    * copy the current translation into the clipboard
    */
   void copy();
   
   void configure(const QString& moduleID, bool modal=false);
   void edit(const QString& moduleID);
   void edit();

   void clear();
   

signals:
   void searchStarted();
   void searchStopped();
   void progressed(int);
   void progressStarts(const QString&);
   void progressEnds();
   void activeModuleChanged(int);
   void activeModuleChanged(bool editable);
   void errorInModule(const QString& error);

   /** 
    * emitted when either the order 
    * or the number of modules was changed
    */
   void modulesChanged();

protected slots:
   void showResult(QListViewItem*);
   void addResult(const SearchResult*);
   void nextInfo();
   void showDetailsOnly();
   void showListOnly();
   void clearModuleResults();
   void editFile();
   void showContextMenu(KListView *, QListViewItem *, const QPoint&);

   /**
    * This slots gets connected to a SearchEngine's searchStopped() signal,
    * when a new search is requested although a search is currently active.
    */
   void startDelayedSearch();
   void startDelayedTranslationSearch();
   

   /**
    * This slot is connected to the finished signal of @ref KDialogBase
    * to destroy this dialog after the user has closed it.
    */
   void destroyConfigDialog();
   
protected:
   virtual void wheelEvent(QWheelEvent*);
   
private:
   /**
   * Register the module and connect the slots
   */
   void registerModule( SearchEngine* module);
   
   QPtrList<SearchEngine> moduleList;
   int active;
   int currentResult;
   int currentInfo;
   int total;

   /** stores a string to be used with delayed search */
   QString searchText;

   QDict<QWidget> prefDialogs;

   QLabel *translatorLabel;
   QLabel *locationLabel;
   QLabel *totalResultsLabel;
   QLabel *currentLabel;
   QLabel *dateLabel;
   QPushButton *moreButton;
   QPushButton *nextButton;
   QPushButton *prevButton;
   QTextView *origView;
   QTextView *translationView;
   QSplitter *viewContainer;
   KListView *resultListView;
   QSplitter *resultSplitter;

   QToolButton *listButton;
   QToolButton *detailButton;

   int editFileIndex;
   QGuardedPtr<QPopupMenu> rmbPopup;
};

#endif // KBABELDICTBOX_H
