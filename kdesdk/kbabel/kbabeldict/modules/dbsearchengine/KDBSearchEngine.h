
/***************************************************************************
                          KDBSearchEngine.h  -  description
                             -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   In addition, as a special exception, the copyright holders give       *
 *   permission to link the code of this program with any edition of       *
 *   the Qt library by Trolltech AS, Norway (or with modified versions     *
 *   of Qt that use the same license as Qt), and distribute linked         *
 *   combinations including the two.  You must obey the GNU General        *
 *   Public License in all respects for all of the code used other than    *
 *   Qt. If you modify this file, you may extend this exception to         *
 *   your version of the file, but you are not obligated to do so.  If     *
 *   you do not wish to do so, delete this exception statement from        *
 *   your version.                                                         *
 ***************************************************************************/
/*
 Translation search engine


  Copyright  2000
  Andrea Rizzi rizzi@kde.org

*/


/*
 * This is a database engine specific
 * structure, I think I must found a way to
 * make it more "standard"
 */

#ifndef _KDBSEARCH_ENGINE_
#define _KDBSEARCH_ENGINE_
#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qmemarray.h>
#include <qvaluelist.h>


#include "database.h"
#include "searchengine.h"
#include "preferenceswidget.h"


//value of mode
#define MD_ALL_DB          1
#define MD_GOOD_KEYS	   2
#define MD_ALL_GOOD_KEYS   3
#define MD_IN_TRANSLATION  4

class SearchEntry
{
  public:
  QString string;
  int rules;
};

class KeyAndScore : public QString
{
 public:
   KeyAndScore(); 
   KeyAndScore(const QString &a,int sc);
   int score;
};

typedef QValueList<SearchEntry> SearchList;

/*  USE searchengine.h structure
class CatalogDescription
{
 public:
  char language[6];     // How many character  chinese language needs?
                        // QString is better ?
  QString filename;
  QString dateOfScan;   //When you add this to database (last time)
  QString authorOfScan;
  QString fileHeader;   //msgid ""
};

  */

class KDBSearchEngine : public SearchEngine
{
 Q_OBJECT

public:


 KDBSearchEngine(QObject *parent=0,const char *name=0);
 ~KDBSearchEngine();
 enum Rules {Equal = 1, Contains = 2, Contained = 4, RegExp = 8};
 enum FormatRules { Dots = 1, Ampersand = 2, FirstWordUpper =  4,   //Not yet used
	  AllWordUpper = 8, AmpersanAlwaysFirst = 16 };

 /*
   Set the directory where db file are located
 */
 
 void setDBName(QString filename); 
	  
 /*
   Set if the research have to consider multiple spaces as a single one.
 */
 
 void setNormalizeSpace(bool normalize); 

 /*
   Set if the research have to be Case Sensitive or not
 */
 
 void setCaseSensitive(bool sensitive); 


 /*
	 ignore ":_  bla bla  \n" in msgid

 */

 void setRemoveInternalComment(bool internalcomment);

 /*
   Set the a string containing all char that must be ignored
   during the search.
 */


 void setRemoveCharString(QString chartoremove);

 /*
	 Enable an output filter that format Uppercase/lowercase
	 method could be;
   0 = Disabled;
   1 = English pattern;
   2 = Only first;
 */

 void enableCapitalFilter(int method);

 /*
	 Enable an output filter that format special characters
	 method could be;
   0 = Disabled;
   1 = Put ... at the end if needed (remove if present but no needed);
   2 = Put an accelerator (or remove);
   4 = Remove %* if not present in msgid.
   8 = Change %d,%s etc with %1 %2 etc if needed.
 */

 void enableCharFilter(int method);

 /*
	  Sets the rules to decide when 2 string match (Uppercase and remove char
    are not set here)
   1 = EQUAL
   2 = SEARCH STRING IS CONTAINED IN DATABASE STRING (use with care, if you search nothing
			  it will produce a dangerouse output)
   4 = DATABASE STRING IS CONTAINED IN SEARCH STRING (it exclude msgid "")
   8 = String is a regexp. (exclude 1 2 4)
 */

 void setMatchingRules(int rules);

 /*
   Search Engine methods reimplemented
 */



 virtual bool messagesForFilter(const SearchFilter* filter
                         , QValueList<SearchResult>& resultList, QString& error);


 virtual void setLanguageCode(const QString& lang);
 virtual void setLanguage(const QString& languageCode, const QString& languageName);

 virtual void setEditedFile(const QString& file);
    
 virtual bool isReady() const ;
 virtual bool isSearching() const;

 virtual void saveSettings(KConfigBase *config);
 virtual void readSettings(KConfigBase *config);

 virtual PrefWidget *preferencesWidget(QWidget *parent);

 virtual const KAboutData *about() const;

 virtual QString name() const;

 virtual QString id() const;

 virtual QString lastError();

 virtual QString translate(const QString& text, uint pluralForm);

 virtual void stringChanged( const QStringList& orig, const QString& translated
        , const uint translationPluralForm, const QString& description);

 
public slots:

 void scan();
 void scanRecur();
 void scanFile();
 /*
   Add a search string in the list of the string to search for.
   Returns the ID of the string in the list.
   Returns -1 if there is a problem (may be search in 
   progress)
  */ 
 
 int addSearchString(QString searchString, int rule=Equal);

 /*
   Start the research in the database of all the string in the list    
   search mode -1 means use global mode;
  */
 bool startSearchNow(int searchmode=-1);   

 /* 
  Start a search for a single string
  If the number of word of your string is less than patternXlimit
  the KDBSearchEngine will search for all string with X * substituted to
	X word.
  For example if pattern1limit is 4 and you search "My name is Andrea"
  This string will also match with:
  "Your name is Andrea", "My dog is Andrea", "My name was Andrea", "My name is Joe"

	Do not set pattern2limit too high.

  If a patternlimit is > 0 rules 8 (* means any word) is added.

 */

 bool startSingleSearch(QString searchString,unsigned int pattern1Limit,unsigned int pattern2Limit,
                         bool inTranslation=false);

 /* 
  Start a search for a list of string
 */
 
// bool startListSearch(QPtrList<QString> searchStrList); 

/* 
 *  Return a list of key in database that contains some
 *  words of the given string phrase.
 *  thresholdin is a number from 0 to 100 a key is good 
 *  if at least the threshold% of words are found
 *  thresholdout a key is good if the found words represnt
 *  at least the thresholdorig% of the words in the key
 *  max is the maximum number of results
 */

QValueList<KeyAndScore> searchWords(QString phrase,int threshold,
				int thresholdorig ,uint32 max);

 
 /* 
  Stop the current search
  */

 virtual void stopSearch();
 virtual bool startSearch(const QString& text, uint pluralForm, const SearchFilter* filter);
 virtual bool startSearchInTranslation(QString s);

 void clearList();
signals:
 void found(SearchResult *);
private slots: 
  void updateSettings();  //Use widget settings
  void setSettings();  //Fill widget with actual settings
  void prefDestr();
  void setLastError(const QString&);
  void repeat();    
private:
 /*
   Reload database info (and keep the dbase opened).
  */
  bool loadDatabase(QString database,bool);

 bool openDb(bool);
  
 PreferencesWidget *pw;
 bool IAmReady;
 int methodFilterChar;
 int methodFilterCapital;
 int defRule;
 int defLimit1;
 int defLimit2;
 int thre;
 int threorig;
 int commonthre;
 int listmax;
 int mode;
 bool retnot;
 bool defSub1;
 bool defSub2;
 bool stopNow;
 bool searching;
 bool norm,sens,comm;
 int numofresult;
 char * filename;
 QString remchar;
 QString regaddchar;
 QString dbname;
 bool dbOpened;
// GDBM_FILE db;
 DataBaseManager * dm;
// datum key, value;
// QMemArray<CatalogDescription *> catinfo;
 SearchList searchStringList;
 int totalRecord;
 QString lasterror;
 QString lang;
 bool scanInProgress;
 QString edited;
 bool autoup;
 QString autoauthor;
};

#endif
