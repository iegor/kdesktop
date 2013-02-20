/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002-2004 by Stanislav Visnovsky
			    <visnovsky@kde.org>

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
#ifndef KBABEL_H
#define KBABEL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kdeversion.h>
#include <kdockwidget.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <kmdimainfrm.h>

#include "kbabelview.h"
#include "kbproject.h"

class KAction;
class KRecentFilesAction;
class KLed;
class KProgress;
class QHBox;
class QLabel;
class QTimer;

class KBCatalog;
class KBabelPreferences;
class DictionaryMenu;
class KBabelBookmark;
class KBabelBookmarkHandler;
class CommentView;
class CharacterSelectorView;

namespace KBabel
{
    class KBabelMailer;
    class ProjectDialog;
}

/**
 * This class serves as the main window for KBabel.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Matthias Kiefer <matthias.kiefer@gmx.de>
 * @version 0.1
 */
class KBabelMW : public KDockMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    KBabelMW(QString projectFile = QString());

    /** use this contructor, if you just want to create a new view of an existing catalog*/
    KBabelMW(KBCatalog* catalog, QString projectFile = QString());

    /**
     * Default Destructor
     */
    virtual ~KBabelMW();
    
    QString project() const { return _project->filename(); }

    void open(const KURL& url, const QString package, bool newWindow);
    void openTemplate(const KURL& openURL,const KURL& saveURL,const QString& package, bool newWindow=false);
    void projectOpen(const QString& filename);
    
    void spellcheckMoreFiles( QStringList filelist);

    void setSettings(KBabel::SaveSettings,KBabel::IdentitySettings);
    void updateSettings();

    /**
     * @return A pointer to a KBabel, that has opened file URL or 0 if no
     * KBabel was found
     */
    static KBabelMW *winForURL(const KURL& url, QString projectFile = QString::null);

    /**
     * @return A pointer to a KBabel, that has opened no file URL or 0 if no
     * KBabel was found
     */
    static KBabelMW *emptyWin(QString projectFile = QString::null);

public slots:
    void toggleEditMode();
    
protected:
    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    
    /**
     * Overrriden virtual for wheel event handling to forward to KBabelView
     */
    virtual void wheelEvent(QWheelEvent *e);

    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    virtual void saveProperties(KConfig *);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    virtual void readProperties(KConfig *);

    virtual bool queryExit();
    virtual bool queryClose();

private slots:
    void quit();

    void open(const KURL& url);
    void openRecent(const KURL& url);
    void fileOpen();
    void fileSave();
    void fileSave_internal();
    void fileSaveAs();
    void fileSaveSpecial();
    void fileMail();
    void fileNewView();
    KBabelMW* fileNewWindow();
    
    void projectNew();
    void projectOpen();
    void projectClose();
    void projectConfigure();
    void projectOpenRecent(const KURL& url);

    void addToRecentFiles(KURL url);
    void addToRecentProjects(KURL url);

    void optionsShowStatusbar(bool);
    void optionsEditToolbars();
    void newToolbarConfig();
    void optionsPreferences();

    /** opens the gettext info page */
    void gettextHelp();

    void firstEntryDisplayed(bool firstEntry, bool firstForm);
    void lastEntryDisplayed(bool lastEntry, bool lastForm);
    void fuzzyDisplayed(bool);
    void untranslatedDisplayed(bool);
    void faultyDisplayed(bool);
    void displayedEntryChanged(const KBabel::DocPosition& pos);
    void setNumberOfTotal(uint number);
    void setNumberOfFuzzies(uint number);
    void setNumberOfUntranslated(uint number);
    void hasFuzzyAfterwards(bool);
    void hasFuzzyInFront(bool);
    void hasUntranslatedAfterwards(bool);
    void hasUntranslatedInFront(bool);
    void hasErrorAfterwards(bool);
    void hasErrorInFront(bool);
    void updateCursorPosition(int line, int col);

    void enableBackHistory(bool);
    void enableForwardHistory(bool);

    void enableUndo(bool);
    void enableRedo(bool);
    void enableStop(bool);

    void openCatalogManager();

    /**
    * prepare the window and the progressbar for showing
    * activity. message is displayed left to the progressbar
    * and max is the maximum number for the progressbar
    */
    void prepareProgressBar(QString message,int max);
    /**
    * resets the progressBar and enables the window
    */
    void clearProgressBar();

    void changeStatusbar(const QString& text);
    void clearStatusbarMsg();
    void changeCaption(const QString& text);
    void showModified(bool);

    /**
    * enables menu- and toolbar items that are always enabled when a cat is opened
    */
    void enableDefaults(bool readOnly);

    void setLedColor(const QColor& color);


    void buildDictMenus();

    /**
    * used when creating standard toggle actions, because I prefer
    * using signal toggled(bool)
    */
    void dummySlot(){}

    void spellcheckDone( int result);

    /**
     * Create a new bookmark for the current msgid and add it to the list.
     */
    void slotAddBookmark();
    /**
     * Open the bookmark whose entry was just clicked in the menu.
     */
    void slotOpenBookmark(int index);

private:
    void init(KBCatalog* catalog);
    void setupActions();
    void setupDynamicActions();
    void changeProjectActions(const QString& project);
    void setupStatusBar();
    void saveSettings();
    void restoreSettings();
    /**
     * Init a new view of the current window with this window's bookmarks.
     *
     * @param list the list of bookmarks.
     */
    void initBookmarks(QPtrList<KBabelBookmark> list);

private:
    KBabelView *m_view;
    CharacterSelectorView * const m_charselectorview;
    CommentView * m_commentview;

    int _currentIndex;

    KProgress* _progressBar;
    QLabel* _progressLabel;
    KLed* _fuzzyLed;
    KLed* _untransLed;
    KLed* _errorLed;

    QTimer *_statusbarTimer;

    KBabelPreferences* _prefDialog;

    QStringList _toSpellcheck;

    /**
    * used for updating preferences, that are common in
    * the whole application
    */
    static QPtrList<KBabelPreferences> prefDialogs;


    // frequently used actions
    KAction* a_unsetFuzzy;
    KAction* a_prevFoU;
    KAction* a_nextFoU;
    KAction* a_prevFuzzy;
    KAction* a_nextFuzzy;
    KAction* a_prevUntrans;
    KAction* a_nextUntrans;
    
    KRecentFilesAction* a_recent;
    KRecentFilesAction* a_recentprojects;

    DictionaryMenu *dictMenu;
    DictionaryMenu *selectionDictMenu;
    DictionaryMenu *configDictMenu;
    DictionaryMenu *editDictMenu;
    DictionaryMenu *aboutDictMenu;

    friend class KBabelInterface;

    // project file
    KBabel::Project::Ptr _project;
    KSharedConfig::Ptr _config;
    KBabel::ProjectDialog* _projectDialog;
    
    QMap<QString,QString> _toolsShortcuts;

    KBabel::KBabelMailer* mailer;
    KBabelBookmarkHandler* bmHandler;
};

#endif // KBABEL_H
