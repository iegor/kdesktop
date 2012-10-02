/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
                2001-2004 by Stanislav Visnovsky <visnovsky@kde.org>
  Copyright (C) 2005, 2006 by Nicolas GOUTTE <goutte@kde.org>

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
#ifndef CATALOGMANAGERVIEW_H
#define CATALOGMANAGERVIEW_H

#include <qdict.h>
#include <qlistview.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qfileinfo.h>
#include <qguardedptr.h>

#include <kdialogbase.h>
#include <kdirwatch.h>
#include <kprocess.h>
#include <qptrlist.h>

#include "kbproject.h"
#include "projectsettings.h"
#include "findoptions.h"
#include "cvsresources.h"
#include "svnresources.h"

class CatManListItem;
class QPixmap;
class QPopupMenu;
class KBabelDictBox;
class CatManPreferences;
class QTextEdit;
class KProgress;
class KConfig;
class KDataToolInfo;
class ValidateProgressDialog;
class CVSHandler;
class SVNHandler;
class MarkPatternDialog;
class ValidationOptions;

namespace KBabel
{
    class KBabelMailer;
    class PoInfo;
}

class CatalogManagerView : public QListView
{
   Q_OBJECT
public:
   CatalogManagerView(KBabel::Project::Ptr project, QWidget* parent=0, const char* name=0);
   virtual ~CatalogManagerView();

   KBabel::CatManSettings settings() const;
   /** clears the tree */
   virtual void clear();

   /** pauses the update timer */
   void pause(bool);

   /** if file fileWithPath exists in the treeview, this is updated */
   void updateFile(QString fileWithPath, bool force = false);

   void updateAfterSave(QString fileWithPath, KBabel::PoInfo &info);

   bool isActive() const {return _active;}
   bool isStopped() const {return _stop;}

   void restoreView(KConfig *config);
   void saveView(KConfig *config) const;

   void setRMBMenuFile( QPopupMenu *m);
   void setRMBMenuDir( QPopupMenu *m);
   void setDirCommandsMenu( QPopupMenu *m);
   void setFileCommandsMenu( QPopupMenu *m);

   CVSHandler * cvsHandler();
   SVNHandler * svnHandler();

public slots:
   void setSettings(KBabel::CatManSettings newSettings);
   void toggleMark();
   /**
   * removes all marks in directory of current item (must be a directory)
   */
   void slotClearMarksInDir();
   void clearAllMarks();
   void toggleAllMarks();
   void markModifiedFiles();
   void loadMarks();
   void saveMarks();
   void slotMarkPattern( );
   void slotUnmarkPattern( );
   /**
   * traverses all childs in the directory of the current item
   * (including all subdirectories) and displays some statistics
   * about the translations. If the item is a file, its
   * parent directory is used instead.
   */
   void statistics();
   /**
   * traverses all marked childs in the directory of the current item
   * (including all subdirectories) and displays some statistics
   * about the translations. If the item is a file, its
   * parent directory is used instead.
   */
   void markedStatistics();
   /**
	* calls @ref Msgfmt::checkSyntax, to check the po-file of
	* the selected item
	*/
   void checkSyntax();

   void roughTranslation();
   void markedRoughTranslation();

   /** Send the selected item as a compressed mail attachment. If the
    *  selected item is a directory send the items contained in the
    *  directory.
    */
   void mailFiles();
   /** Send the marked items as a compressed mail attachment.
    */
   void mailMarkedFiles();

   void packageFiles();
   void packageMarkedFiles();

   void cvsUpdate( );
   void cvsUpdateMarked( );
   void cvsCommit( );
   void cvsCommitMarked( );
   void cvsStatus( );
   void cvsStatusMarked( );
   void cvsUpdateTemplate( );
   void cvsUpdateMarkedTemplate( );
   void cvsCommitTemplate( );
   void cvsCommitMarkedTemplate( );
   void cvsDiff( );

   void svnUpdate( );
   void svnUpdateMarked( );
   void svnCommit( );
   void svnCommitMarked( );
   void svnStatusRemote();
   void svnStatusRemoteMarked();
   void svnStatusLocal();
   void svnStatusLocalMarked();
   void svnUpdateTemplate( );
   void svnUpdateMarkedTemplate( );
   void svnCommitTemplate( );
   void svnCommitMarkedTemplate( );
   void svnDiff( );
   void svnInfo();
   void svnInfoMarked();

   QString find(KBabel::FindOptions &options, QStringList &rest);

   void showLog();

   void stop(bool s = true);

   /**
   * Stop searching, do not try to proceed to the next file
   * @ref @find will return clear list of rest to be searched
   * and @ref QString::null, if search string was not is the last searched file
   */
   void stopSearch();

   /**
   * Information for this file has been read. If the file is in
   * @ref @_readInfoFileList, it will update progress bar by emitting @ref @progress
   */
   void fileInfoRead( QString file );

   void gotoNextUntranslated();
   void gotoPreviousUntranslated();
   void gotoNextFuzzy();
   void gotoPreviousFuzzy();
   void gotoNextFuzzyOrUntranslated();
   void gotoPreviousFuzzyOrUntranslated();
   void gotoNextError();
   void gotoPreviousError();
   void gotoNextTemplate();
   void gotoPreviousTemplate();
   void gotoNextPo();
   void gotoPreviousPo();
   void gotoNextMarked();
   void gotoPreviousMarked();

   void validateUsingTool( const KDataToolInfo &, const QString& );
   void validateMarkedUsingTool( const KDataToolInfo &, const QString& );

   void showError( const QString package, const int num);

   void updateCurrent();

   /**
    * An update for more than one file has become necessary. For instance
    * after 'cvs commit' or 'svn commit' the file contents have not changed
    * but the CVS/SVN file status could have changed.
    */
   void updateFiles( const QStringList& files );

   /**
   * Returns the list of all currently selected files. If current selection is dir,
   * it returns list of all its children.
   */
   QStringList current();
   /**
   * Returns the list of all currently marked files.
   */
   QStringList marked();

signals:
   void openFile(QString filename,QString package);
   void openFileInNewWindow(QString filename,QString package);
   void openTemplate(QString openFilename,QString saveFileName,QString package);
   void openTemplateInNewWindow(QString openFilename,QString saveFileName,QString package);
   void gotoFileEntry(QString filename,QString package,int msgid);
   void prepareProgressBar(QString msg,int max);
   void progress(int);
   void clearProgressBar();
   void prepareFindProgressBar(int max);
   void signalBuildTree(bool done);
   void signalSearchedFile(int count);

   void newValidationFile(QString);
   void newValidationTool(QString);
   void setValidationProgressBar(int);
   void advanceValidationFileProgressBar(int);
   void setMaxValidationProgressBar(int);
   void setMaxValidationFileProgressBar(int);

   /**
    * The selected item in the tree view has changed.
    * This signal emits the corresponding action value for this item.
    * @param actionValue Action value for the selected item.
    */
   void selectedChanged(uint actionValue);

signals:
   void updateFinished();

protected:
   /**
   * builds the tree under dir relDir, but does not update any files
   * this functions always traverses all subdirs
   *
   * @param relDir the relative dir under the po- and pot- base directories
   * @param fast if true, no files will be updated
   *
   * @return true, if the directory contains any po or pot-files
   * @see CatalogManagerView::buildDir
   * @see CatalogManagerView::updateDir
   */
   bool buildDir(QString relDir,bool fast=true);

   /**
   * This function is traversing the real directory on the
   * disc using baseDir as the
   * base directory and starts at baseDir+relDir
   * @param extension the extension of the files in this directory
   * @param fast if true, no file will be updated
   *
   * @return true, if the directory contains any po or pot-files
   * @see CatalogManagerView::buildDir
   * @see CatalogManagerView::updateDir
   */
   bool buildDir(const QString& baseDir,const QString& relDir, const QString extension,bool fast=true);

   /**
   * updates dir relDir and if any new subdir is added
   * builds this with @ref buildDir
   *
   * This function doesn't enters subdirs except when a new subdir is added.
   * @see CatalogManagerView::buildDir
   */
   void updateDir(QString relDir);

   /**
   * stops the update timer and the dirwatch
   * @see KDirWatch::stop
   * @see QTimer::stop
   */
   virtual void hideEvent(QHideEvent*);
   /**
   * restarts the update timer and the dirwatch
   * @see KDirWatch::start
   * @see QTimer::start
   */
   virtual void showEvent(QShowEvent*);

   /** used for dragging */
   virtual void contentsMousePressEvent(QMouseEvent* e);
   /** used for dragging */
   virtual void contentsMouseMoveEvent(QMouseEvent* e);

   void showStatistics( CatManListItem *i, QStringList &packages);

protected slots:
   /** rebuilds the tree*/
   void buildTree();
   /**
   * recurse all visible files and updates them if necessary
   * @see CatManListItem::checkUpdate
   */
   void checkUpdate();

   /** this is called from KDirWatch when a directory has changed */
   void directoryChanged(const QString& dir);
   /** this is called from KDirWatch when a directory has been deleted */
   void directoryDeleted(const QString& dir);

   void showContentsMenu(QListViewItem *, const QPoint &, int col);
   /** does the default action on the currently selected item*/
   void activateItem(QListViewItem *);
   /** emits the state of the selected item using selectedChanged*/
   void checkSelected();
   /** calls @ref activateItem with the selected item as argument*/
   void slotOpenFile();
   void slotOpenFileInNewWindow();
   /** emits signal @ref openTemplate */
   void slotOpenTemplate();
   /** deletes the po-file on the disc, that belongs to the selected item */
   void slotDeleteFile();
   /** toggles the mark of the selected item */
   void slotToggleMark();
   /**
   * toggles all marks in directory of current item (must be a directory)
   */
   void slotToggleMarksInDir();
   void slotDirCommand(int);
   void slotFileCommand(int);

private slots:
   void showOutput(KProcess *proc, char *buffer, int buflen);
   void processEnded(KProcess *proc);
   void columnClicked(QListViewItem * item, const QPoint & pnt, int c);

   void slotToggleCVSOrSVNColumn( bool );

   void slotValidPOCVSRepository( bool );
   void slotValidPOSVNRepository( bool );
   void slotValidPOTCVSRepository( bool );
   void slotValidPOTSVNRepository( bool );

private:
   void toggleColumn( uint id, bool show);

   void readMarker(KConfig *config);
   void saveMarker(KConfig *config) const;
   /**
   * remove marked entries, which are not in the current file list
   */
   void updateMarkerList();

   /**
    * Mark or unmark entries.
    *
    * @param mark If true the items are marked, if false the marks are removed.
    */
   void setPatternMarks(bool mark);

   /**
	* deletes item with package name (relative directory) relDir
	* and makes sure, that all subitems are removed from the lists
	*/
   void deleteDirItem(QString relDir);

   bool hasMatchingWords( QStringList &itemWords, QStringList &searchWords);

   CatManListItem *itemBelow( CatManListItem *item );
   CatManListItem *itemAbove( CatManListItem *item );

   void validate_internal( const QStringList&, const KDataToolInfo &, const QString& );

   void doCVSCommand( CVS::Command cmd, bool marked = false, bool templates = false );
   void doSVNCommand( SVN::Command cmd, bool marked = false, bool templates = false );

private:
   QDict<CatManListItem> _dirList;
   QDict<CatManListItem> _fileList;

   KDirWatch *_dirWatch;
   QTimer *_updateTimer;

   // list of files for which was calculated the progress bar for reading file info
   QStringList _readInfoFileList;
   // current count of already read files in @ref @_readInfoFileList.
   int _readInfoCount;

   KBabel::CatManSettings _settings;

   QStringList _markerList;

   bool _active;
   // stopping, application quit
   bool _stop;
   // stop searching, do not proceed to the next file
   bool _stopSearch;
   int _updateNesting;

   QPtrList<KProcess> _pendingProcesses;

   QTextEdit* _logView;
   KDialogBase* _logWindow;
   QPopupMenu* _fileContentsMenu;
   QPopupMenu* _dirContentsMenu;
   QPopupMenu* _dirCommandsMenu;
   QPopupMenu* _fileCommandsMenu;

   /** used for starting a drag */
   QPoint _pressPos;

   KBabelDictBox* _dictBox;

   KBabel::KBabelMailer* mailer;

   CVSHandler* cvshandler;
   SVNHandler* svnhandler;

   /// Is the PO path a valid CVS repository?
   bool m_validPOCVSRepository;
   /// Is the POT path a valid CVS repository?
   bool m_validPOTCVSRepository;
   /// Is the PO path a valid SVN repository?
   bool m_validPOSVNRepository;
   /// Is the POT path a valid SVN repository?
   bool m_validPOTSVNRepository;

   MarkPatternDialog * markPatternDialog;

   //validation
   ValidateProgressDialog* _validateDialog;
   ValidationOptions* _validateOptions;
   KDialogBase* _validateOptionsDlg;
   bool _markAsFuzzy;
   bool _ignoreFuzzy;

   KBabel::Project::Ptr _project;
};

#endif // CATALOGMANAGERVIEW_H
