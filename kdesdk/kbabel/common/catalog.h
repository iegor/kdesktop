/*****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2004 by Stanislav Visnovsky
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
#ifndef CATALOG_H
#define CATALOG_H

#include "pluralforms.h"
#include "itempart.h"
#include "msgfmt.h"
#include "poinfo.h"
#include "catalogfileplugin.h"
#include "kbproject.h"

#include <qptrlist.h>
#include <qvaluevector.h>
#include <kdemacros.h>
#include <kstandarddirs.h>

class KDataTool;
class QString;
class QTextStream;
class KFileInfo;
class QTextCodec;
class QFile;
class KDataTool;
class KURL;

namespace KBabel
{

class CatalogView;
class EditCommand;
class FindOptions;
class CatalogItem;
class IdentitySettings;
class MiscSettings;
class SaveSettings;
class TagSettings;

class CatalogPrivate;

/**
* This struct represents a position in a catalog.
* A position is a tuple (index,pluralform,textoffset).
*
* @short Structure, that represents a position in a catalog.
* @author Matthias Kiefer <matthias.kiefer@gmx.de>
* @author Stanislav Visnovsky <visnovsky@kde.org>
*/
struct DocPosition
{
	DocPosition( ) : offset(0), part(UndefPart), item(0), form(0) { }
	uint offset;
	
	Part part;
	uint item;	
	uint form;
};

/**
* This struct represents an entry in diff-algorithm sresults.
*
* @short Structure, that represents a diff result.
*/
struct DiffEntry
{
    QString msgid;
    QString msgstr;
};

/**
* This class represents a catalog, saved in a po-file.
* It has the ability to load from and save to a po-file.
* Also it defines all necessary functions to set and get the entries
*
* @short Class, that represents a translation catalog(po-file)
* @author Matthias Kiefer <matthias.kiefer@gmx.de>
*/
class KDE_EXPORT Catalog : public QObject
{
    Q_OBJECT

public:
   enum DiffResult{DiffOk, DiffNotFound, DiffNeedList};

   /**
   * reads header information from the file and searches for charset
   * information.
   * @param gettextHeader text containing gettext headers
   *
   * @return Codec for found charset or 0, if no information has been found
   */
   static QTextCodec* codecForFile(QString gettextHeader);
   
   static PoInfo headerInfo(const CatalogItem headerItem);

   /**
   * A constructor for an empty message catalog.
   * @param parent parent @ref QObject for this catalog
   * @param name   unique name for this object
   * @param configFile configuration file to read config from
   */
   Catalog(QObject* parent=0, const char* name=0, QString projectFile = QString() );

   /**
   * Deprecated. A copy constructor. Do not use since each Catalog registers
   * its views and there should be no more than one Catalog for a given file.
   */
   Catalog(const Catalog&);
   virtual ~Catalog();

   /**
   * Get the message context for a given message.
   *
   * @param index  index of the requested message
   * @return       context for the given message
   */
   QString msgctxt(uint index) const;
   
   /**
   * Get list of texts for a given message in original language. Each entry in the list
   * represents a single singular/plural form.
   *
   * @param index  index of the requested message
   * @param noNewLines set true if the new lines should be eliminated in result
   *        (useful for searching etc)
   * @return list of singular/plural forms of the original text
   */
   QStringList msgid(uint index, const bool noNewlines=false) const;

   /**
   * Get list of translated texts for a given message. Each entry in the list
   * represents a single singular/plural form.
   *
   * @param index  index of the requested message
   * @param noNewLines set true if the new lines should be eliminated in result
   *        (useful for searching etc)
   * @return list of translated singular/plural forms
   */
   QStringList msgstr(uint index, const bool noNewlines=false) const;

   /**
   * Get the comment for a given message.
   *
   * @param index  index of the requested message
   * @return       comment for the given message
   */
   QString comment(uint index) const;

   /**
   * Get the context information for a given message. This works
   * for GNU gettext PO files comments only. Finds and returns comment lines,
   * which start with #:
   *
   * @param index  index of the requested message
   * @return       context information found in the message comment
   * @deprecated
   */
   QString context(uint index) const KDE_DEPRECATED;

   /**
   * Get the header for the current file.
   *
   * @return       @ref CatalogItem representing the header
   */
   CatalogItem header() const;

   /**
   * Get the name and e-mail address of the last translator of the current file.
   *
   * @return       string representation of the name and e-mail address
   */
   QString lastTranslator() const;
   
   /**
    * @return The list of obsolete entries. These cannot be changed at
    * all and are stored for import/export purposes.
    */
   QValueList<CatalogItem> obsoleteEntries() const;

   /**
    * @return The index of the item, that has the msgid id.
    * If no item is found, -1 is returned.
    */
   int indexForMsgid(const QString& id) const;
   
   /** @return a list of tags in entry #index */
   QStringList tagList(uint index);

   /** @return a list of arguments in entry #index */
   QStringList argList(uint index);
   
   /**
    * @return the list of errors found for a given item index
    * @param index index of the item to check
    */
   QStringList itemStatus(uint index);

   /**
    * @return the list of errors found for a given item index
    * @param index index of the item to check
    * @param recheck flag, if the item status should be checked now
    * @param whatToCheck what checks to do (a list of tools to be used
    */
   QStringList itemStatus(uint index, bool recheck, QPtrList<KDataTool> whatToCheck);

   /**
   * replaces msgstr in catalog item at index index with msgstr
   * returns true, if untranslated status changed
   */
   //bool setMsgstr(uint index,QString msgstr);
   /**
   * replaces comment in catalog item at index index with comment
   * returns true, if fuzzy status changed
   */
   //bool setComment(uint index,QString comment);
   /**
   * replaces header with given String
   * @return false if the header is not valid
   */
   bool setHeader(CatalogItem header);

   /** removes string ", fuzzy" from comment */
   void removeFuzzyStatus(uint index);

   /** sets or unsets fuzzy status of entry index */
   void setFuzzy(uint index, bool on);

   /**
   * used for session managment when modified file is
   * saved in a temp file
   */
   void setCurrentURL(const KURL& url);
   /**
   * @return URL currently represented by the catalog
   */
   KURL currentURL() const;

   /**
   * @return package name for the current catalog
   */
   QString packageName() const;
   /**
   * @return directory for the package of the current catalog
   */
   QString packageDir() const;
   /**
   * @return package name and directory for the current catalog
   */
   QString package() const;

   /**
   * Setup the package for the catalog. It tries to parse the
   * package and split it to name and directory
   * @param package the name and directory to be set as package
   */
   void setPackage(const QString& package);

   /**
   * @return encoding for the current catalog
   */
   QString encoding() const;

   /**
   * opens file url by using KDE's network downlad and calls
   * openFile with a local filename
   */
   ConversionStatus openURL(const KURL& url, const QString& package=QString::null);

   /**
   * opens file openURL by using KDE's network downlad and calls
   * openFile with a local filename
   * sets current URL to saveURL
   */
   ConversionStatus openURL(const KURL& openURL, const KURL& saveURL, const QString& package=QString::null);

   /** save the file under the old filename */
   ConversionStatus saveFile();
   /** saves the file under a new filename */
   ConversionStatus saveFileAs(const KURL& url,bool overwrite=false);

   /**
   * saves the current catalog in a temporary file and
   * returns the name and path of the file.
   */
   QString saveTempFile();

   /**
    * @return extra data for the catalog as set by the import filter
    */
   QStringList catalogExtraData() const;
   /**
    * @return ID string of the used import filter
    */
   QString importPluginID() const;
   /**
    * @return list of MIME types (separated by comma) this catalog
    * can be viewed as. It is set by the used import filter.
    */
   QString mimeTypes() const;
   /**
    * @return the file codec used for the current catalog
    */
   QTextCodec* fileCodec() const;

   /**
    * Check syntax of the GNU gettext PO file using 'msgfmt'.
    * 
    * @param output the result as returned by msgfmt
    * @param clearError should the errors be cleared before running msgfmt
    * @return the file codec used for the current catalog
    */
   Msgfmt::Status checkSyntax(QString& output, bool clearErrors=true);

   /**
    * checks using an external tool. The tool must provide the "validate" command
    * with the datatype expected to be CatalogItem *. The MIME type
    * is application/x-kbabel-catalogitem.
    */
   bool checkUsingTool(KDataTool* tool, bool clearErrors=true);
   
   /**
    * modifies catalog by applying an external tool on all items. The tool must provide the "validate" command
    * with the datatype expected to be CatalogItem *. The MIME type
    * is application/x-kbabel-catalogitem.
    */
   void modifyUsingTool(KDataTool* tool, const QString& command);
   
   /**
    * Returns true, if there is an ongoing activity, such as load/save
    */
   bool isActive();

   /**
    * Stop the current activity, for example load/save or validation
    */
   void stop();

   /** closes the file and deletes all entries */
   void clear();

   /** @return whether the catalog has been modified since the last save */
   bool isModified() const;
   /** sets modified flag */
   void setModified(bool flag);

   /** @return whether the catalog is read-only */
   bool isReadOnly() const;

   /** set if file is generated from a docbook source file (with xml2pot) */
   bool isGeneratedFromDocbook() const;

   /** @return current number of entries */
   uint numberOfEntries() const;
   /** @return current number of fuzzies */
   uint numberOfFuzzies() const;
   /** @return current number of untranslated entries */
   uint numberOfUntranslated() const;
   
   /** returns the number of words total, in fuzzy and in untranslated messages */
   void wordCount (uint &total, uint &fuzzy, uint &untranslated) const;

   bool hasFuzzyInFront(uint index) const;
   bool hasFuzzyAfterwards(uint index) const;
   bool hasUntranslatedInFront(uint index) const;
   bool hasUntranslatedAfterwards(uint index) const;
   bool hasErrorInFront(uint index) const;
   bool hasErrorAfterwards(uint index) const;
   bool isFuzzy(uint index) const;
   bool isUntranslated(uint index) const;
   /** @return whether the entry has an error
    *  @param index index of the queried entry
    *  @param pos   position of the error (currently only form is filled)
    */
   bool hasError(uint index, DocPosition& pos) const;
   /** @return type of plural forms for the given entry
    *  @param index index of the queried entry
    */
   PluralFormType pluralForm(uint index) const;

   /** @return type of plural forms this file uses.
    */
   PluralFormType pluralFormType() const;

   /** @return number of the plural forms for a given entry
    *  @param index index of the queried entry
    */
   int numberOfPluralForms(uint index) const;
   
   /**
   * returns the expected number of plural forms
   */
   int defaultNumberOfPluralForms() const;

   /**
   * returns index of next fuzzy entry behind startIndex
   * -1 if there is no entry behind. Further, it fills the
   * pos parameter with correct index and sets the form to 0.
   */
   int nextFuzzy(uint startIndex, DocPosition &pos) const;
   /**
   * returns index of previous fuzzy entry before startIndex
   * -1 if there is no entry before. Further, it fills the
   * pos parameter with correct index and sets the form to 0.
   */
   int prevFuzzy(uint startIndex, DocPosition &pos) const;
   /**
   * returns index of next untranslated entry behind startIndex
   * -1 if there is no entry behind. Further, it fills the
   * pos parameter with correct index and the form.
   */
   int nextUntranslated(uint startIndex, DocPosition &pos) const;
   /**
   * returns index of previous untranslated entry before startIndex
   * -1 if there is no entry before. Further, it fills the
   * pos parameter with correct index and the form.
   */
   int prevUntranslated(uint startIndex, DocPosition &pos) const;
   /**
   * returns index of next error entry behind startIndex
   * -1 if there is no entry behind
   */
   int nextError(uint startIndex, DocPosition &pos) const;
   /**
   * returns index of previous error entry before startIndex
   * -1 if there is no entry before
   */
   int prevError(uint startIndex, DocPosition &pos) const;

   /**
   * returns the header with Information updated, but does _not_ change
   * the header in the catalog
   */
   CatalogItem updatedHeader(CatalogItem oldHeader, bool usePrefs=true) const;

   /** read settings from the apps config file/project */
   void readPreferences();
   /** save settings to the apps config file/project */
   void savePreferences();
   
   void useProject(Project::Ptr project);
   Project::Ptr project() const;

   SaveSettings saveSettings() const;
   IdentitySettings identitySettings() const;
   MiscSettings miscSettings() const;
   TagSettings tagSettings() const;

   /**
   * reads the header from QTextstream and puts it in header
   * I made it static to be able to use this function for
   * the search in po-files. This way, I can easily find the first
   * catalog entry in the textstream
   */
//   static ConversionStatus readHeader(QTextStream& stream,CatalogItem& header);


   void registerView(CatalogView* view);
   void removeView(CatalogView* view);
   bool hasView() const;
   bool isLastView() const;

   bool isUndoAvailable();
   bool isRedoAvailable();
   /**
   * undo the last edit command
   * @return the index of the item, which was modified or -1, if no undo available
   */
   int undo();
   /**
   * redo the last undo command
   * @return the index of the item, which was modified or -1, if no redo available
   */
   int redo();

   /**
   * finds next occurence of searchstring using opts, starting at pos.
   * @param pos starting position and if successful contains position
   * of found string.
   * @param len contains len of found string
   * @ return true, if a string was found. false, if end was reached
   */
   bool findNext(const FindOptions* opts, DocPosition& pos, int& len);
   /**
   * finds previous occurence of searchstring using opts, starting at pos.
   * @param pos starting position and if successful contains position
   * of found string.
   * @param len contains len of found string
   * @ return true, if a string was found. false, if begin was reached
   */
   bool findPrev(const FindOptions* opts, DocPosition& pos, int& len);

   /**
    * Remove the index from the error list 
    * @param index index to be removed
    */
   void removeFromErrorList(uint index);

   /**
    * tries to find a corresponding entry for entry entry 
    * from the list of old messages and calculates the diff for it
    */
   DiffResult diff(uint entry, QString* result);

   /**
    * sets a list of entries to generate a diff from
    */
   void setDiffList( const QValueList<DiffEntry>& );

   /**
    * @return the contents of this catalog as list for diffs
    */
   QValueList<DiffEntry> asDiffList();
   
   /**
    * @return how many plural forms are used in language lang.
    * If nothing is found -1 is returned.
    */
   static int getNumberOfPluralForms(const QString& lang);
   
public slots:
   /** 
    * Update the save setting using this one.
    * @param settings the new settings.
    */
   void setSettings(KBabel::SaveSettings settings);
   /** 
    * Update the identity setting using this one.
    * @param settings the new settings.
    */
   void setSettings(KBabel::IdentitySettings settings);
   /** 
    * Update the miscelanous setting using this one.
    * @param settings the new settings.
    */
   void setSettings(KBabel::MiscSettings settings);
   /** 
    * Update the tag setting using this one.
    * @param settings the new settings.
    */
   void setSettings(KBabel::TagSettings settings);

   /**
    * Apply the given edit command.
    * 
    * @param cmd the command to be applied
    * @param souce the view which applies the command
    */
   void applyEditCommand(EditCommand* cmd,CatalogView* source);
   /**
    * Convenience method. Apply a begin edit command for a given message and catalog
    * source.
    * 
    * @param index index of to be changed message
    * @param part  part (msgid,msgstr,comment) of to be changed message
    * @param source the view which applies the command
    */
   void applyBeginCommand(uint index, Part part, CatalogView* source);
   /**
    * Convenience method. Apply an end edit command for a given message and catalog
    * source.
    * 
    * @param index index of to be changed message
    * @param part  part (msgid,msgstr,comment) of to be changed message
    * @param source the view which applies the command
    */
   void applyEndCommand(uint index, Part part, CatalogView* source);

private slots:
   /** slot used for internal long-term activity */
   void stopInternal();

private:
   /** calls @ref CatalogView::update(EditCommand*) */
   void updateViews(EditCommand* cmd,CatalogView* view2exclude=0);

   /**
   * takes over changes, but does not change undo/redo list
   */
   void processCommand(EditCommand* cmd, CatalogView* view2exclude=0, bool undo=false);

   /** do the actual file write using plugin */
   ConversionStatus writeFile(QString localfile, bool overwrite=false);

   /**
   * generates  lists that contain indexes of all fuzzy and untranslated entries
   */
   void generateIndexLists();

   /**
   * returns value in list that is lower than parameter index
   * or -1 if there is none
   */
   int findPrevInList(const QValueList<uint>& list,uint index) const;
   /**
   * returns value in list that is bigger than parameter index
   * or -1 if there is none
   */
   int findNextInList(const QValueList<uint>& list,uint index) const;

   /** returns the current date and time in the format of the users choice */
   QString dateTime() const;

   /** clear the list of all errors */
   void clearErrorList();

   /**
    * Tries to find out how many plural forms are used in this language and
    * sets numberOfPluralForms accordingly. If nothing is found,
    * numberOfPLuralForms is set to -1.
    */
   void getNumberOfPluralForms();
   
   // this class can fill our contents
   friend class CatalogImportPlugin;
   
   /** set the flag that the catalog is generated from docbook */
   void setGeneratedFromDocbook(const bool generated);
   /** set the entries of the catalog */
   void setEntries( QValueVector<CatalogItem> entries);
   /** set the obsolete entries of the catalog */
   void setObsoleteEntries( QValueList<CatalogItem> entries);
   /** set extra data for the catalog as defined by import plugin */
   void setCatalogExtraData(const QStringList& data);
   /** set file codec for the catalog */
   void setFileCodec(QTextCodec* codec);
   /** set the list of import errors */
   void setErrorIndex(const QValueList<uint>&errors);
   /** set ID of the used import plugin */
   void setImportPluginID(const QString& id);
   /** set the MIME types for the current catalog as defined by import plugin */
   void setMimeTypes(const QString& mimeTypes);
   
private:
   CatalogPrivate* d;
   
signals:
   void signalError(QString);
   void signalResetProgressBar(QString,int);
   void signalProgress(int);
   void signalClearProgressBar();

   void signalModified(bool);
   /** emitted when the header was changed, maybe when saving */
   void signalHeaderChanged();
   /** emitted when a file was opened or saved under another name */
   void signalFileOpened(bool readOnly);

   void signalNumberOfFuzziesChanged(uint number);
   void signalNumberOfUntranslatedChanged(uint number);
   void signalTotalNumberChanged(uint number);

   void signalSettingsChanged(KBabel::SaveSettings);
   void signalSettingsChanged(KBabel::IdentitySettings);
   void signalSettingsChanged(KBabel::MiscSettings);
   void signalSettingsChanged(KBabel::TagSettings);

   void signalUndoAvailable(bool);
   void signalRedoAvailable(bool);
   
   /** internal signal sent to external part to stop current ongoing activity*/
   void signalStopActivity();
};

}

#endif //CATALOG_H
