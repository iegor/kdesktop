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

#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include <qdatetime.h>
#include <qptrlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <kaboutdata.h>
#include <kconfigbase.h>

 
/*
 * This class gives information about the translator, etc.  
 * Information, that is not available in a specific implementation of
 * the SearchEngine, should be left empty.
 */
class KDE_EXPORT TranslationInfo
{
public:
    /** 
     * Information about the location, where this entry was found.
     * For example the PO-file it was found in, etc. 
     * */
    QString location; 

    /**
     * The complete path of the file, where this entry was found
     */
    QString filePath;


    /** The date of the last change */
    QDateTime lastChange;
    
    /** The language, the translation belongs to */
    QString languageCode;

    /**
     * The translator of this string
     * For example the translator found in the header of the PO-file.
     */
    QString translator;
    
    /**
     * The name of a project this translation is a part of.
     */
    QString projectName;
    
    /**
     * Keywords defined for @ref projectName. For example KDE_3_1_BRANCH (project branch)
     */
    QStringList projectKeywords;
    
    /**
     * Part/context in a project, for example "multimedia", "admin", etc.
     */
    QString projectContext;
    
    /**
     * Status of the translation, for example "approved", "spellchecked", "unknown"
     */
    QString status;

    /**
     * Additional information to be presented to the user, for example a comment
     */
    QString description;    
};


/** 
 * This class contains a result from the search
 * plus additional information where it was found,
 * the date and time of the last change, the translator, etc.
 */
class KDE_EXPORT SearchResult
{
public:
    SearchResult();
    SearchResult(const SearchResult&);
        
    /** The requested string to search for */
    QString requested;
    
    /** The string that, was found (a list if it is a plural form) */
    QStringList found;

    /** The number of a plural form to search for */
    uint requestedPluralForm;

    /** The translation of the found string */
    QString translation;

    /** The number of a plural form of the translated string found */
    uint translationPluralForm;

    /** 
     * This string contains the plain translation if you are
     * using rich text. If you don't use rich text, you can leave
     * it empty
     */
    QString plainTranslation;

    /** 
     * This string contains the plain string, that was found, if you are
     * using rich text. If you don't use rich text, you can leave
     * it empty
     */
    QString plainFound;
    
    /** 
     * This string contains the plain requested string if you are
     * using rich text. If you don't use rich text, you can leave
     * it empty
     */
    QString plainRequested;
    
    /**
     * Constains a score for the found translation. 0 means exact matching.
     * The higher the score is, the worse is the matching.
     * How to compute the score is not yet really clear :-(
     */
    int score;

    QPtrList<TranslationInfo> descriptions;
};

/**
 *
 */
class KDE_EXPORT SearchFilter 
{
public:
    SearchFilter() : 
	_projects()
	, _location( QString::null )
	, _languageCode( QString::null )
	, _origLanguageCode( QString::null )
	, _translators()
	, _projectKeywords()
	, _projectContexts()
	, _translationStatus()
    {}
    
    virtual ~SearchFilter() {}

    void setProjectName( const QString& project ) { _projects = project; }
    void setProjectName( const QStringList& projects ) { _projects = projects; }

    /** 
     * Information about the location, where this entry was found.
     * For example the PO-file it was found in, etc. 
     * */
    void setLocation( const QString& location) { _location = location; }
    QString location() const { return _location; }

    /** The original language, the translation was made from */
    void setOriginalLanguage( const QString& languageCode) { _origLanguageCode = languageCode; }

    /** The language, the translation belongs to */
    void setTranslationLanguage( const QString& languageCode) { _languageCode = languageCode; }

    /**
     * The translator of this string
     * For example the translator found in the header of the PO-file.
     */
    void setTranslator( const QString& translator) { _translators = translator ; }
    void setTranslator( const QStringList& translators) { _translators = translators ; }
    
    /**
     * Keywords defined for @ref projectName. For example KDE_3_1_BRANCH (project branch)
     */
    void setProjectKeywords( const QStringList& projectKeywords ) { _projectKeywords = projectKeywords; }
    
    /**
     * Part/context in a project, for example "multimedia", "admin", etc.
     */
    void setProjectContext( const QString& projectContext) { _projectContexts = projectContext; } 
    void setProjectContext( const QStringList& projectContexts) { _projectContexts = projectContexts; }
    
    /**
     * Status of the translation, for example "approved", "spellchecked", "unknown"
     */
    void setStatus( const QString& translationStatus) { _translationStatus = translationStatus; }
    void setStatus( const QStringList& translationStati) { _translationStatus = translationStati; }

    /**
     *  The key method of the class - check, if the argument
     *  matches this filter.
     */
    virtual bool match( const TranslationInfo& toCheck );

private:
    QStringList _projects;
    QString _location; 
    QString _languageCode;
    QString _origLanguageCode;
    QStringList _translators;
    QStringList _projectKeywords ;
    QStringList _projectContexts;
    QStringList _translationStatus;
};

/**
 * This class is the base class for the preferences widget used to
 * setup the search engines. Inherit from this and reimplement all
 * necessary function. The widget should not be bigger than 400x400.
 * If you need more space, you maybe want to use a tabbed widget.
 */
class KDE_EXPORT PrefWidget : public QWidget
{
    Q_OBJECT
            
public:
    PrefWidget(QWidget *parent, const char* name=0);
    virtual ~PrefWidget();

public slots:
    /**
     * apply changes of the settings
     */
    virtual void apply()=0;
    /**
     * revert made changes
     */
    virtual void cancel()=0;
    /**
     * set settings to the standard options
     */
    virtual void standard()=0;
};


class KDE_EXPORT SearchEngine : public QObject
{
    Q_OBJECT
            
public:
    SearchEngine(QObject *parent=0, const char *name=0);
    virtual ~SearchEngine();

    

    /** @return the search result number n */
    virtual SearchResult *result(int n);
    
    /** @return the number of search results */
    virtual int numberOfResults() const;

    /** @return true, if a search is currently active */
    virtual bool isSearching() const = 0;
    

    /** save the settings in the given config object */
    virtual void saveSettings(KConfigBase *config) = 0;
    virtual void readSettings(KConfigBase *config) = 0;

    /** @returns true, if it was initialized correctly */
    virtual bool isReady() const =0;


    /** 
     * @returns the exact translation of text or a empty string
     * if no exact match was found.
     */
    virtual QString translate(const QString& text, const uint pluralForm = 0)=0;
 
    /** 
     * @returns the translation of text according to the plugin settings or a empty string
     * if no match was found.
     */
    virtual QString searchTranslation(const QString&, int &score, const uint pluralForm = 0) {
        Q_UNUSED(pluralForm);

	score = 0; return QString::null;
    }

    /** 
     * @returns a fuzzy translation of text or a empty string
     * if no good match was found.
     */
    virtual QString fuzzyTranslation(const QString& /*text*/, int &score, const uint pluralForm = 0) { 
        Q_UNUSED(pluralForm);

      score = 0; return QString::null; };
   

    /**
     * Finds all messages belonging to package package. If nothing is found,
     * the list is empty.
     * @param package The name of the file, something like "kbabel.po"
     * @param resultList Will contain the found messages afterwards
     * @param error If an error occured, this should contain a description
     * 
     * @return true, if successfull
     */
    virtual bool messagesForFilter(const SearchFilter* filter
                         , QValueList<SearchResult>& resultList, QString& error) 
    {
        Q_UNUSED(filter);
        Q_UNUSED(resultList);

	error = i18n("not implemented");
	return false;
    }
    
    /** 
     * @returns true, if the searchresults are given as rich text
     * the default implementation returns false
     */ 
    virtual bool usesRichTextResults();

    /** @returns true, if the the entries in the database can be edited */
    virtual bool isEditable();
    
    /** 
     * @returns a widget which lets the user setup all preferences of this
     * search engine. The returned widget should not be bigger than
     * 400x400. If you need more space, you maybe want to use
     * a tabbed widget.
     * @param parent the parent of the returned widget
     */
    virtual PrefWidget* preferencesWidget(QWidget *parent)=0;

    /** @returns information about this SearchEngine */
    virtual const KAboutData *about() const= 0;

    /** @returns the i18n name of this search engine */
    virtual QString name() const= 0;

    /** @returns a untranslated name of this engine */
    virtual QString id() const= 0;

    /** @returns the last error message */
    virtual QString lastError() = 0;
            

    /**
     * sets the engine to always ask the preferences dialog for settings
     * if is existing before starting the search.
     */
    virtual void setAutoUpdateOptions(bool activate);


    /**
     *  @returns how good @param text1 matches @param text2
     *  comparing 3-grams .. n-grams, the returned value is
     *  a number between 0 and 100. @param ngram_len should be
     *  a value between 3 and 5. 
     */
    static uint ngramMatch (const QString& text1, const QString& text2, 
			    uint ngram_len=3);

public slots:
    /**
     * starts a search for string s in the original text
     * @returns false, if an error occured. Use @ref lastError
     * to get the last error message
     */
    virtual bool startSearch(const QString& s, uint pluralForm = 0, const SearchFilter* filter = 0) = 0;

    /**
     * starts a search for string s in the translated text
     * @returns false, if an error occured. Use @ref lastError
     * to get the last error message
     */
    virtual bool startSearchInTranslation(const QString& s, uint pluralForm = 0, const SearchFilter* filter = 0);


    /** stops a search */
    virtual void stopSearch() = 0;

    /** @return the next search result */
    virtual SearchResult *nextResult();
    
    /** @return the previous search result */
    virtual SearchResult *prevResult();

    /** @return the first search result */
    virtual SearchResult *firstResult();

    /** @return the last search result */
    virtual SearchResult *lastResult();
    
    /** clears the result list */
    virtual void clearResults();
    

    /**
     * This method allows a search engine to use different settings depending
     * on the edited file. The default implementation does nothing.
     * @param file The edited file with path
     */
    virtual void setEditedFile(const QString& file);

    /**
     * This method allows a search engine to use different settings depending
     * on the edited package. The default implementation does nothing.
     * @param package The name of the package, that is currently translated.
     */
    virtual void setEditedPackage(const QString& package);

    /**
     * This method allows a search engine to use different settings depending
     * on the language code. The default implementation does nothing.
     * @param lang The current language code (e.g. de).
     */
    virtual void setLanguageCode(const QString& lang);
    virtual void setLanguage(const QString& languageCode, const QString& languageName);



    /**
     * This method is called, if something has been changed in the
     * current file. See @ref setEditedFile if you want to know the file
     * name
     * @param orig the original string (list of all plural forms)
     * @param translation the translated string  
     * @param pluralForm the number of the plural form of the translation
     * @param description the additional description, e.g., a PO comment
     */
     virtual void stringChanged( const QStringList& orig, const QString& translated
            , const uint translationPluralForm, const QString& description);
    
    /**
     * If the database is editable this slot should open an dialog to let
     * the user edit the contents of the database.
     */
    virtual void edit();

    /************ convenience functions for distring normalization, etc.**********/

    /** 
     * @returns the n-th directory name of path. Counting is started at
     * end of path. Example: directory("/usr/local/src/foobar, 0")
     * returns "foobar", while n=1 would return "src"
     * FIXME: isn't it a code duplication?
     */
    static QString directory(const QString& path, int n);

    /**
     * computes a score to assess the match of the two strings:
     * 0 means exact match, bigger means worse
     */
    static uint score(const QString& orig, const QString& found);
     
signals:
    /** signals, that a new search started */
    void started();
    
    /** 
     * signals progress in searching 
     * @param p should be between 0 and 100
     * */
    void progress(int p);
    
    /**
     * Use this, if you want to set your own message on the
     * progressbar or if you do something else then searching,
     * maybe loading a big file
     */
    void progressStarts(const QString&);

    void progressEnds();
    
    /** 
     * signals, that the number of search results has changed 
     * @param n the new number of results
     * */
    void numberOfResultsChanged(int n);

    /**
     * signals, that the order of the results in the list has changed.
     * For example because a better matching string has been prepended.
     */
    void resultsReordered();

    /**
     * signals, that a new result was found
     */
    void resultFound(const SearchResult*);

    /** 
     * signals, that search has finished
     */
    void finished();
    
    /**
     * signals, that an error occured, for example, that you wasn't
     * able to open a database.      
     */ 
     void hasError(const QString& error);

protected:
    QPtrList<SearchResult> results;
    bool autoUpdate;
};


#endif // SEARCH_ENGINE_H
