/* ****************************************************************************
   
DBSE2  
Andrea Rizzi

**************************************************************************** */

#ifndef DBSEARCH_ENGINE2_H
#define DBSEARCH_ENGINE2_H

#include <searchengine.h>
#include <qdialog.h>

#include "database.h"
#include "dbse2_factory.h"
#include "preferenceswidget.h"
#include "dbscan.h"
#include "sourcedialog.h"

//#include "DBSESettings.h"

class KDBSearchEngine2 : public SearchEngine
{
    Q_OBJECT
    
    public:
    
    KDBSearchEngine2(QObject *parent=0, const char *name=0);
    virtual ~KDBSearchEngine2();
    
    //init if needed.
    bool init();
    
    /** @return true, if a search is currently active */
    bool isSearching() const {return searching;}
    
    void stopSearch()  { if(di) di->stop();}
    
    
    
    /** @returns true, if it was initialized correctly */
    bool isReady() const {return iAmReady; }
    

    void saveSettings(KConfigBase *config);
    void readSettings(KConfigBase *config);


    QString translate(const QString text);

    QString fuzzyTranslation(const QString text, int &score);
    QString searchTranslation(const QString, int &score );

    /**
     * Finds all messages belonging to package package. If nothing is found,
     * the list is empty.
     * @param package The name of the file, something like "kbabel.po"
     * @param resultList Will contain the found messages afterwards
     * @param error If an error occured, this should contain a description
     *
     * @return true, if successfull
     */
    bool messagesForPackage(const QString& package
			    , QValueList<Message>& resultList, QString& error);

    /** 
     * @returns true, if the searchresults are given as rich text
     * the default implementation returns false
     */ 
    bool usesRichTextResults(){return true;}
    
    /** @returns true, if the the entries in the database can be edited */
    bool isEditable(){return false;}
    
    /** 
     * @returns a widget which lets the user setup all preferences of this
     * search engine. The returned widget should not be bigger than
     * 400x400. If you need more space, you maybe want to use
     * a tabbed widget.
     * @param parent the parent of the returned widget
     */
    virtual PrefWidget* preferencesWidget(QWidget *parent);
    
    /** @returns information about this SearchEngine */
    virtual const KAboutData *about() const;
    
    /** @returns the i18n name of this search engine */
    QString name() const {return i18n("DB SearchEngine II");}
    
    /** @returns a untranslated name of this engine */
    QString id() const {return "dbse2";}
    
    /** @returns the last error message */
    QString lastError() {return lasterror;}
    
    
    /**
     * sets the engine to always ask the preferences dialog for settings
     * if is existing before starting the search.
     */
    virtual void setAutoUpdateOptions(bool activate)
    {
	autoupdate=activate;
    }
    
    
    
    public slots:	
	
	void receiveResult(QueryResult r);
    
    /**
     * starts a search for string s in the original text
     * @returns false, if an error occured. Use @ref lastError
     * to get the last error message
     */
    bool startSearch(QString s);
    
    /**
     * starts a search for string s in the translated text
     * @returns false, if an error occured. Use @ref lastError
     * to get the last error message
     */
    bool startSearchInTranslation(QString s);
    
    
    /** stops a search */
    //  virtual void stopSearch() ;
    
    
    /**
     * This method allows a search engine to use different settings depending
     * on the edited file. The default implementation does nothing.
     * @param file The edited file with path
     */
    //    virtual void setEditedFile(QString file);
    
    /**
     * This method allows a search engine to use different settings depending
     * on the edited package. The default implementation does nothing.
     * @param package The name of the package, that is currently translated.
     */
    //    virtual void setEditedPackage(QString package);
    
    /**
     * This method allows a search engine to use different settings depending
     * on the language code. The default implementation does nothing.
     * @param lang The current language code (e.g. de).
     */	
    //    virtual void setLanguageCode(QString lang);
    //    virtual void setLanguage(QString languageCode, QString languageName);
    
    
    
    /**
     * This method is called, if something has been changed in the
     * current file. See @ref setEditedFile if you want to know the file
     * name
     * @param orig the original string
     * @param translation the translated string  
     */
    
    void stringChanged( QString orig, QString translated
			, QString description);
    
    //void scan();
    
    void  setLastError(QString er);
    
    //Slots for preference dialog
  //  void  scanSource(QString sourceName);
    void  scanNowPressed();
    void  scanAllPressed();
    void  editSource();
    void  removeSource();
    void  addSource();
    
	
    private:
    DataBaseInterface *di;
    bool searching;
    bool iAmReady;
    bool autoupdate;
    QString lasterror;
    KDB2PreferencesWidget *pw;
    
    //PrefWidg -> DBSE
    void updateSettings();
    //DBSE -> PrefWidg
    void    setSettings();
    
    DBSESettings settings;
    QString dbDirectory;
    bool autoAdd,useSentence,useGlossary,useExact;
    bool useDivide,useAlpha,useWordByWord,useDynamic;
    uint numberOfResult;
    QMap<QString,MessagesSource> sources;
    
};


#endif // SEARCH_ENGINE2_H
