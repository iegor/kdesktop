/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2004 by Stanislav Visnovsky <visnovsky@kde.org>

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
#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include <qdict.h>
#include <qlistview.h>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <qguardedptr.h>
#include <qmap.h>

#include <kdeversion.h>
#include <kmainwindow.h>
#include <kdirwatch.h>
#include <kprocess.h>
#include <qptrlist.h>

#include "projectsettings.h"
#include "kbproject.h"
#include "catalog.h"
#include "catalogmanagerview.h"

class CatManListItem;
class QPixmap;
class QPopupMenu;
class QTimer;
class KProgress;
class KAction;
class KConfig;
class FindInFilesDialog;

namespace KBabel
{
    class PoInfo;
    class ProjectDialog;
}

class CatalogManager : public KMainWindow
{
   Q_OBJECT
public:
   CatalogManager(QString configfile = QString() );
   ~CatalogManager();

   KBabel::CatManSettings settings() const;
   /**
   * Sets the window, in which the files should be opened.
   * This is set by KBabel::openCatalogManager
   */
   void setPreferredWindow(WId id);

   /** updates the file fileWithPath in the @ref CatalogManagerView */
   void updateFile(QString fileWithPath);
   void updateAfterSave(QString fileWithPath, KBabel::PoInfo &info);
   
   CatalogManagerView *view();
   
   void pause(bool flag) { if( _catalogManager ) _catalogManager->pause (flag); }

   static QStringList _foundFilesList;
   static QStringList _toBeSearched;

public slots:
   /** updates the settings from the project */
   void updateSettings();
   void enableMenuForFiles(bool enable);
   void selectedChanged(uint actionValue);
   virtual void slotHelp();

   virtual void find();
   virtual void replace();
   virtual void stopSearching();
   virtual void optionsPreferences();
   virtual void optionsShowStatusbar(bool on);
   virtual void dummySlot() {}
   
    void projectNew();
    void projectOpen();
    void projectClose();
    void projectConfigure();
    void changeProjectActions(const QString& project);

   virtual void clearProgressBar();
   virtual void prepareProgressBar(QString msg, int max);

   virtual void clearStatusProgressBar();
   virtual void prepareStatusProgressBar(QString msg, int max);
   virtual void prepareStatusProgressBar(int max);
   
   virtual void setNumberOfFound( int toBeSent, int total );
   virtual void decreaseNumberOfFound();
   
protected slots:
   virtual void findNextFile();
   virtual bool queryClose();

signals:
   void settingsChanged(KBabel::CatManSettings);
   void signalQuit();
   void searchStopped();

private:
   void init();
   
   void restoreView();
   void saveView();
   
   void saveSettings( QString configFile = QString::null );

   void setupActions();
   void setupStatusBar();
   
   bool startKBabel();
   
private slots:
   /**
   * calls @ref KBabel::open where as preferred windos _preferredWindow
   * is used. If this is deleted meanwhile, the first window in
   * @ref KMainWindow::memberList is used.
   */
   void openFile(QString filename,QString package);
   void openFile(QString filename,QString package, int msgid);
   void openFileInNewWindow(QString filename,QString package);
   /**
   * calls @ref KBabel::openTemplate where as preferred windos _preferredWindow
   * is used. If this is deleted meanwhile, the first window in
   * @ref KMainWindow::memberList is used.
   */
   void openTemplate(QString openFilename,QString saveFileName,QString package);
   void openTemplateInNewWindow(QString openFilename,QString saveFileName,QString package);
   
   void markedSpellcheck();
   void spellcheck();
   
   void newToolbarConfig();

   /** updates views and _settings variable */
   void restoreSettings();
   
   void enableActions();
   void disableActions();
   
   void enableActions(bool enable);
   
private:
   CatalogManagerView* _catalogManager;
   
   WId _preferredWindow;

   bool _openNewWindow;

   FindInFilesDialog* _findDialog;
   FindInFilesDialog* _replaceDialog;
   KBabel::ProjectDialog* _prefDialog;
   
   /// update progress bar
   KProgress* _progressBar;
   QLabel* _progressLabel;
   
   /// statusbar progress bar
   KProgress* _statusProgressBar;
   QLabel* _statusProgressLabel;
   QLabel* _foundLabel;
   int _foundToBeSent;
   int _totalFound;
   
   QTimer* _timerFind;
   bool _searchStopped;
   
   KBabel::CatManSettings _settings;
   KBabel::MiscSettings _miscSettings;
   
   /// options used in findNextFile
   KBabel::FindOptions _findOptions;

   
   /// project configuration file
   QString _configFile;
   KBabel::Project::Ptr _project;

   KConfig* config;

   QMap<QString,uint> actionMap;
};

#endif // CATALOGMANAGER_H
