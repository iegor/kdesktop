/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002-2005  by Stanislav Visnovsky
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
#ifndef KBABELVIEW_H
#define KBABELVIEW_H

class HidingMsgEdit;
class MsgMultiLineEdit;
class GotoDialog;
class QPopupMenu;
class KLed;
class FindDialog;
class ReplaceDialog;

namespace KBabel
{
    class EditCommand;
    class RegExpExtractor;
    class FindOptions;
    class ReplaceOptions;
}

class QListBoxItem;
class QTextView;
class QTabWidget;
class KListBox;
class KSpell;
class KSpellConfig;
class KBabelDictBox;
class KDataToolInfo;
struct ReplaceOptions;
struct ModuleInfo;

#include <kdockwidget.h>
#include <kurl.h>
#include <kconfig.h>
#include <qwidget.h>
#include <qstrlist.h>
#include <resources.h>

#include <catalogview.h>
#include "kbcatalog.h"
#include "kbproject.h"
#include "projectsettings.h"

class KBabelMW;
class CommentView;
class ContextView;
class KBCatalogListView;
class CharacterSelectorView;
class SourceView;
class TagListView;

/**
 * This is the main view class for KBabel.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 * @short Main view
 * @author Matthias Kiefer <matthias.kiefer@gmx.de>
 * @version 0.1
 */
class KBabelView : public QWidget, public KBabel::CatalogView
{
    Q_OBJECT
public:
    /**
    * Default constructor
    * @param buildLeds flag, if status leds should be created in editor
    */
    KBabelView(KBCatalog* catalog,KBabelMW *parent, KBabel::Project::Ptr project);

    /**
    * Destructor
    */
    virtual ~KBabelView();

    /**
     * @return the view, that has opened file url or 0 if this
     * file is not opened
     */
    static KBabelView *viewForURL(const KURL& url, const QString project);

    /**
     * @return the view, that has no opened file url or 0 if there
     * is no such view
     */
    static KBabelView *emptyView(const QString project);

    KURL currentURL() const;
    QString project() const { return _project->filename(); }
    void useProject (KBabel::Project::Ptr project);
    
    bool isLastView() const;
    bool isModified() const;
    /** the edit mode of the entry-editors*/
    bool isOverwriteMode() const;
    bool isReadOnly() const;
    /** the edit mode of the entry-editors*/
    void setOverwriteMode(bool ovr);
    bool isSearching() const;

    void saveView(KConfig *config);
    void restoreView(KConfig *config);
    void saveSession(KConfig*);
    void restoreSession(KConfig*);

    void readSettings(KConfig* config);
    void saveSettings();

    void readProject(KBabel::Project::Ptr project);
    void saveProject(KConfig* config);

    void openTemplate(const KURL& openURL, const KURL& saveURL);
    bool saveFile(bool checkSyntax=true);
    bool saveFileAs(KURL url = KURL(), bool checkSyntax=true);
    bool saveFileSpecial();

    /**
    * Checks, if the file has been modified. If true, it askes the user if he wants
    * to save, discard or cancel. If the users chose save, it saves the file.
    * @return true, if it is allowed to open a new file. false, if the user wants
    * to edit the file again.
    */
    bool checkModified();


    /**
    * Checks syntax of the current catalog. If the catalog is modified it
    * saves it under a temporary filename ( using @ref Catalog::saveTempFile ).
    *
    * @param msgOnlyAtError flag, if a message should be shown, only if
    * a error occured.
    * @param question flag, if only a information about the result should
    * be shown or a question, whether the user wants to continue or cancel
    *
    * @return true, if no error occured or if an error occured but the user
    * wants to continue anyway.
    */
    bool checkSyntax(bool msgOnlyAtError, bool question);

    /**
    * this is called from the catalog when updating his views.
    * reimplemented from @ref CatalogView
    * @param cmd the edit command that has been applied
    */
    virtual void update(KBabel::EditCommand* cmd, bool undo=false);

    KBCatalog* catalog() const{return _catalog;}

    void processUriDrop(KURL::List& uriList, const QPoint & pos);

   /**
   * checks the status of the displayed entry: last, first, fuzzy,...
   * and emits the appropriate signals
   */
   void emitEntryState();

   void setRMBEditMenu(QPopupMenu*);
   void setRMBSearchMenu(QPopupMenu*);
   void setTagsMenu(QPopupMenu*);
   void setArgsMenu(QPopupMenu*);

   QPtrList<ModuleInfo> dictionaries();
   KBabelDictBox* searchView() { return dictBox; }

   bool autoDiffEnabled() const {return _diffEnabled;}

public slots:
   
   void gotoEntry(const KBabel::DocPosition& pos, bool updateHistory=true);

    /** opens a filedialog and asks for an url */
    void open();
    void open(const KURL& url, const QString & package=QString::null, bool checkModified=true, bool newView=false);
    void setFilePackage();
    void revertToSaved();

    void updateSettings();
    void updateProjectSettings();

    void undo();
    void redo();
    void textCut();
    void textCopy();
    void textPaste();
    bool findNext();
    bool findPrev();
    void find();
    void findInFile(QCString fileSource, KBabel::FindOptions options);
    void replaceInFile(QCString fileSource, KBabel::ReplaceOptions options);
    void replace();
    void selectAll();
    void deselectAll();
    void clear();
    void msgid2msgstr();
    void search2msgstr();
    void plural2msgstr();
    void gotoFirst();
    void gotoLast();
    void gotoNext();
    void gotoPrev();
    void gotoEntry();
    void gotoNextFuzzyOrUntrans();
    void gotoPrevFuzzyOrUntrans();
    void gotoNextFuzzy();
    void gotoPrevFuzzy();
    void gotoNextUntranslated();
    void gotoPrevUntranslated();
    void gotoNextError();
    void gotoPrevError();

    void forwardHistory();
    void backHistory();

    void spellcheckAll();
    void spellcheckAllMulti();
    void spellcheckFromCursor();
    void spellcheckCurrent();
    void spellcheckFromCurrent();
    void spellcheckMarked();
    void spellcheckCommon();

    void roughTranslation();
    void diff();
    void toggleAutoDiff(bool on);
    void diffShowOrig();
    bool openDiffFile();
    void insertNextTag();
    void insertNextTagMsgid();
    void insertNextArg();
    void insertTagFromTool( const QString& tag );
    void showTagsMenu();
    void showArgsMenu();
    void skipToNextTag();
    void skipToPreviousTag();
    void skipToTagFromTool(int index);
    void wordCount();

    void removeFuzzyStatus();
    /** opens the header editor for the po-file */
    void editHeader();

    /** checks the syntax of the file by using msgftm */
    bool checkSyntax();

    /**
     * perform all checks listed above
     */
    bool checkAll();

    void stopSearch();
    void startSearch();
    void startSelectionSearch();
    void startSearch(const QString id);
    void startSelectionSearch(const QString id);

    void configureDictionary(const QString id);
    void editDictionary(const QString id);
    void aboutDictionary(const QString id);

    /** 
     * this was originally protected, but we need this to expose for
     * KBabelMW forwarding
     */
    virtual void wheelEvent(QWheelEvent*);
    
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual bool eventFilter(QObject*, QEvent* event);

signals:
    /** emited when a fuzzy catalogentry is shown */
    void signalFuzzyDisplayed(bool);
    /** emited when a untranslated catalogentry is shown */
    void signalUntranslatedDisplayed(bool);
    void signalFaultyDisplayed(bool);
    /** emited when the first catalogentry is shown */
    void signalFirstDisplayed(bool firstEntry, bool firstForm);
    /** emited when the last catalog entry is shown */
    void signalLastDisplayed(bool lastEntry, bool lastForm);
    
    void signalNextTag( int index );
    
    /**
    * emited when a new entry is shown
    * pos: position (index and plural form) of the currently shown entry
    */
    void signalDisplayed(const KBabel::DocPosition& pos);

    /**
    * emited when new entry is displayed and there is no
    * fuzzy entry afterwards in the catalog
    */
    void signalFuzzyAfterwards(bool);
    /**
    * emited when new entry is displayed and there is no
    * fuzzy entry in front of it in the catalog
    */
    void signalFuzzyInFront(bool);
    /**
    * emited when new entry is displayed and there is no
    * untranslated entry afterwards in the catalog
    */
    void signalUntranslatedAfterwards(bool);
    /**
    * emited when new entry is displayed and there is no
    * fuzzy entry in fornt of it in the catalog
    */
    void signalUntranslatedInFront(bool);

    void signalErrorAfterwards(bool);
    void signalErrorInFront(bool);

    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);
    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

    void signalNewFileOpened(KURL url);

    void signalResetProgressBar(QString,int);
    void signalProgress(int);
    void signalClearProgressBar();

    void signalSearchActive(bool);

    void signalDiffEnabled(bool);

    void signalForwardHistory(bool have);
    void signalBackHistory(bool have);

    void ledColorChanged(const QColor& color);

    void signalDictionariesChanged();

    void signalMsgstrChanged();

    void signalNextTagAvailable(bool);
    void signalTagsAvailable(bool);

    void signalNextArgAvailable(bool);
    void signalArgsAvailable(bool);

    void signalCursorPosChanged(int line, int col);

    void signalSpellcheckDone(int result);

    void signalCopy();
    void signalCut();
    void signalPaste();
    void signalSelectAll();

private:
   /**
   * inserts the content of the current catalog entry into
   * the fields in the view
   * @param delay flag, if the auto search should be started delayed
   * this is useful when a new file is opened
   * @param formID number of the plural form to be displayed. Use 0 for
   * no plural form
   */
   void updateEditor(int formID=0, bool delay=false);

   void initDockWidgets();
   
   void startSearch(bool delay);

   /**
    * makes some checks like checkings arguments and accels etc
    * @param onlyWhenChanged flag, if message should only be shown
    * when status changed
    */
   void autoCheck(bool onlyWhenChanged);

   /**
    * Create instances of tools currently setup for autochecks
    */
   void setupAutoCheckTools();

   /**
    * internal function to find next string given with @ref FindDialog
    * starting at position pos
    * @return true, if search was successful
    */
   bool findNext_internal(KBabel::DocPosition& pos, bool forReplace=false, bool mark=true);
   /**
    * internal function to find previous string given with @ref FindDialog
    * starting at position pos
    * @return true, if search was successful
    */
   bool findPrev_internal(KBabel::DocPosition& pos, bool forReplace=false, bool mark=true);

   /**
    * makes the real work
    * @param autoDiff flag, if called from @ref autoDiff()
    */
   void diffInternal(bool autoDiff);

   /**
    * @param autoDiff flag, if called from @ref autoDiff()
    */
   bool openDiffFile(bool autoDiff);

   /**
    * Inserts a text into the msgstr (into the current form) using undoable commands.
    * if @param clearFirst is set to true, it will clear the contents of msgstr before inserting
    */
   void modifyMsgstrText(const uint offset, const QString& text, bool clearFirst=false);

protected slots:
   bool validateUsingTool( const KDataToolInfo & info, const QString & command );
   void modifyUsingTool( const KDataToolInfo & info, const QString & command );
   void modifyCatalogUsingTool( const KDataToolInfo & info, const QString & command );

private slots:
   void msgstrPluralFormChanged (uint index);
   void autoRemoveFuzzyStatus();

   /** connected to the catalog. it is called when a new file is opened*/
   void newFileOpened(bool readOnly);

   void showError(const QString& message);

   void toggleFuzzyLed(bool on);
   void toggleUntransLed(bool on);
   void toggleErrorLed(bool on);

   void forwardMsgstrEditCmd(KBabel::EditCommand*);

   /**
   * called from a signal from ReplaceDialog to replace the
   * current found string. After that it searches the next string
   */
   void replaceNext();
   /**
   * called from a signal from ReplaceDialog to replace
   * all without asking anymore.
   */
   void replaceAll();
   /**
   * called from a signal from ReplaceDialog to go to next
   * string to replace
   */
   void findNextReplace();

   /**
   * makes some checks like checkings arguments and accels etc
   */
   void autoCheck();

   void autoDiff();

   /**
    * called, when text in msgstrEdit changes to inform
    * the dictionary about the changes
    */
   void informDictionary();
   void setNewLanguage();

   void forwardProgressStart(const QString& msg);
   void forwardSearchStart();
   void forwardSearchStop();

   /**
    * checks if there is are fuzzy entries in front or behind
    * the current entry and emits the appropriate signals
    */
   void checkFuzzies();
   /**
    * checks if there is are untranslated entries in front or behind
    * the current entry and emits the appropriate signals
    */
   void checkUntranslated();

   /** inserts the nth tag from the available tags into msgstr*/
   void insertTag(int n);

   /** visually display the tag to be inserted next */
   void selectTag();

   void updateTags();

   /** inserts the nth argument from the available arguments into msgstr*/
   void insertArg(int n);

   void updateArgs();
   void insertChar(QChar ch);

   void showTryLaterMessageBox();

   void dummy(KSpell*) {}

private:
   static QPtrList<KBabelView> *viewList;

   HidingMsgEdit* msgstrEdit;
   HidingMsgEdit* msgidLabel;
   KBabelDictBox* dictBox;
   GotoDialog* _gotoDialog;
   FindDialog* _findDialog;
   FindDialog* _replaceDialog;
   ReplaceDialog* _replaceAskDialog;

   QPopupMenu* _dropMenu;

   KLed* _fuzzyLed;
   KLed* _untransLed;
   KLed* _errorLed;

   KBCatalog* _catalog;
   uint _currentIndex;
   KBabel::DocPosition _currentPos;

   KBabel::SpellcheckSettings _spellcheckSettings;

   bool _autoSearchTempDisabled;

   QValueList<uint> _backHistory;
   QValueList<uint> _forwardHistory;

   // flag to not beep, when switching to the next entry, because
   // go -> next or prev entry was used.
   bool _dontBeep;

   /**
   * position in document were find or replace function
   * started to search
   */
   KBabel::DocPosition _findStartPos;
   /**
   * the string that was marked during the last search
   */
   QString _lastFoundString;

   /*
   * flag, if internal find functions should break at end or ask for
   * beginning at the other end of the document
   */
   bool _findBreakAtEnd;

   /*
   * flag, if we search backwards and the direction was already
   * changed (see findNext and findPrev)
   */   
   bool _redirectedBackSearch;

   bool _showTryLaterBox;

   KBabel::DocPosition _replacePos;
   int _replaceLen;
   int _replacesTotal;
   bool _replaceWasAtEnd;
   /** contains the diff to the offset, where we started to search */
   int _replaceExtraOffset;

   /** appId for a source of the next files to be searched */
   QCString _fileSource;

   QStringList _tags;
   QPopupMenu *_tagsMenu;

   QStringList _args;
   QPopupMenu *_argsMenu;

   bool _diffEnabled;
   bool _loadingDiffFile;
   bool _diffing;

   /*
    * flag, set if editing KDE documentation PO-file
    */
   bool _editingDocumentation;
   QPtrList<KDataTool> _autocheckTools;

//spellcheck things
private:
   struct Position
   {
      uint index;
      uint form;
      uint pos;
      uint end;
   };

   enum SpellWhat{All,AllMulti,Current,Marked,Begin,End,BeginCurrent};

   struct
   {
      KSpell *kspell;
      KSpellConfig* config;
      QStringList wordList;
      bool active;
      int misspelled;
      int replaced;
      int posCorrection;
      uint lastIndex;
      QPtrList<Position> posDict;
      SpellWhat what2Check;

      // the last word, that was misspelled
      uint lastPos;
      // the position correction in the last word.
      // needed if words with '-' are treated as seperate words
      int inWordCorrection;

      QStringList origWords;
      QStringList newWords;

      QStringList ignoreList;
      QStringList newIgnoreList;
   } spell;

    struct {
        KSpell *kspell;
        KSpellConfig* config;
    } spell2;                   // on-the-fly spellchecking

    //DictSpellChecker * flyspell;


   /**
    * Marks a misspelled word in the editor.
    * After that, the cursor is at the beginning of the
    * marked text
    * @param orig the original word as given from KSpell
    * @param pos the position of the word in the StringList
    * spell.wordList
    *
    * @returns false, if the there is a synchronization error,
    * means the word has not been found in the editor.
    */
   bool markMisspelled(const QString &orig, unsigned int pos);

private slots:
   void spellcheck();
   void cancelSpellcheck();
   void spellStart(KSpell*);
   void spellMisspelled(const QString &orig, const QStringList &sug, unsigned int pos);
   void spellCorrected(const QString &orig, const QString &newWord, unsigned int pos);
   void spellResult(bool);
   void spellCleanDone();
   void spellAddIgnore(const QString &);
   // initialize spellchecking struct
   void cleanUpSpellStruct();
   void slotAutoSaveTimeout( );

private:
   void addSpellcheckWords(uint pos, QString text, uint index, uint form);

private:
   // configuration file
   KSharedConfig::Ptr _config;
   // project file
   KBabel::Project::Ptr _project;
   
   KBabel::RegExpExtractor* _tagExtractor;
   KBabel::RegExpExtractor* _argExtractor;

   QTimer * autoSaveTimer;
   int _autoSaveDelay;

   int _currentTag;
   
   KBabelMW* m_mainwindow;
   CommentView* m_commentview;
   ContextView* m_contextview;
   KBCatalogListView* m_cataloglistview;
   
   CharacterSelectorView* m_charselectorview;
   TagListView* m_taglistview;
   SourceView* m_sourceview;
   
   bool m_overwrite;
};

#endif // KBABELVIEW_H
