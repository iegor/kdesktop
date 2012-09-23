/***************************************************************************
			  KDBSearchEngine2.cpp  -  description
			     -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000-2003 by Andrea Rizzi
    email                : rizzi@kde.org
***************************************************************************/

/*
  Translation search engine. Version II
  
  
  Copyright  2000-2003
  Andrea Rizzi rizzi@kde.org
  
  License GPL v 2.0

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
  
*/
#include "algorithms.h"
#include "chunk.h"
#include "database.h"
#include "KDBSearchEngine2.h"
#include "dbscan.h"
#include <klineedit.h>
#include <kapplication.h>
#include <qpushbutton.h>
#include <kurlrequester.h>
#include <qcheckbox.h>
#include <knuminput.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

#include <qmap.h>

#include <kdebug.h>
#define i18n (const char *)

KDBSearchEngine2::KDBSearchEngine2(QObject *parent,const char*name)
    : SearchEngine(parent,name)
    {
    pw=0;
    dbDirectory=".";
    
    di=0;   //Database Interface is not yet initialized
    
    connect(this,SIGNAL(hasError(QString)),SLOT(setLastError(QString)));
    
    searching=false;  // i'm not searching
    iAmReady=true;  //there are no reason to say I'm not ready.
    
}


KDBSearchEngine2::~KDBSearchEngine2()
{
    if(di)
	delete di;  //delete database interface
}

bool KDBSearchEngine2::startSearch(QString str)
{
    kdDebug(0) << "Start a new search. Looking for: " << str << endl;
    
    static QString queryString;
    
    queryString=str;  //set the latest query string (note: it is static)
    
    if(autoupdate)
    {
	updateSettings();
    }
    
    
    if(!init()) return false;    //-check initialization
    di->stop(true); //stop all new emits from database interface
    emit started(); //say everybody we are starting
    
    if(searching) return true; //We already have a search loop, as soon as we come back
    //on the search loop we will start the new search (queryString).
    
    searching=true;  //really start searching
    
    QString searchingString;
    
    do //Search loop, it stops only when finished and latest searched string is the actual query string.
    {
	searchingString=queryString; //-set new search string
	di->stop(false);  	   //-unlock searching
	
	if(numberOfResult<1) numberOfResult=1;

//	di->singleWordMatch(searchingString,0,true);

	GenericSearchAlgorithm strategy(di,&settings);

	//Let's create a search sequence:
	ExactSearchAlgorithm exact(di,&settings);
	AlphaSearchAlgorithm alpha(di,&settings);
	SentenceArchiveSearchAlgorithm sent(di,&settings);
	ChunkByChunkSearchAlgorithm sbys(di,&settings);
	ChunkByChunkSearchAlgorithm wbyw(di,&settings);
	CorrelationSearchAlgorithm corr(di,&settings);
	FuzzyChunkSearchAlgorithm fs(di,&settings);
	FuzzyChunkSearchAlgorithm fw(di,&settings);

	SentenceChunkFactory sf(di);
	sbys.setChunkFactory(&sf);
	fs.setChunkFactory(&sf);


	WordChunkFactory wf(di);
	wbyw.setChunkFactory(&wf);
	fw.setChunkFactory(&wf);

	strategy.addAlgorithm(&exact);
	strategy.addAlgorithm(&alpha);
	strategy.addAlgorithm(&sent);
	strategy.addAlgorithm(&sbys);
	//strategy.addAlgorithm(&fs);
	strategy.addAlgorithm(&fw);
	strategy.addAlgorithm(&corr);
	strategy.addAlgorithm(&wbyw);


	connect(&strategy,SIGNAL(newResult(QueryResult)),this,SLOT(receiveResult(QueryResult)));
	strategy.exec(searchingString);	disconnect(&strategy,SIGNAL(newResult(QueryResult)),this,SLOT(receiveResult(QueryResult)));


	kdDebug(0) << "End of search for " << searchingString  << endl;
    }
    while(searchingString!=queryString);
    kdDebug(0) << "Exiting the search loop" << endl;
    //if != someone asked a different string when we was searching
    //so we restart our search (maybe a cleanresult is needed?).
    
    
    di->stop(false);  //-clean searching locks
    searching=false;  //-clean searching locks
    emit finished();  //Finished
    
    return true;
    
}





bool KDBSearchEngine2::startSearchInTranslation(QString str)
{
    if(autoupdate)
    { 
	updateSettings();
    }

    //TODO!!!!
    return true; 
    
}



bool KDBSearchEngine2::messagesForPackage(const QString& package
					  , QValueList<Message>& resultList, QString& error)
{
    //FIXME implement this (needs filters)
    return true;
}


void  KDBSearchEngine2::setLastError(QString er)
{
    lasterror=er;
}

QString KDBSearchEngine2::translate(const QString text)
{
	ExactSearchAlgorithm exact(di,&settings);

	return exact.exec(text)[0].result();

}


void KDBSearchEngine2::receiveResult(QueryResult r)
{

    SearchResult se; // Create a new SearchResult for our QueryResult

    se.translation=r.richResult();
    se.found=r.richOriginal();
    
    se.plainTranslation=r.result();
    se.plainFound=r.original();
    se.score=r.score();

    emit resultFound(&se); // dispatch the new SearchResult
    
}


/*  A SEARCH RESULT CONTAINS (see searchengine.h)
    QString requested;
    QString found;
    QString translation;
    QString plainTranslation;
    QString plainFound;
    QString plainRequested;
    int score;
    QPtrList<TranslationInfo> descriptions;
*/



bool KDBSearchEngine2::init()
{
    if(di!=0) return true; //if there already is a DBinterface we are ok
    else
    {
	di = new DataBaseInterface(dbDirectory,&settings);
	connect(di,SIGNAL(newResult(QueryResult)),this,SLOT(receiveResult(QueryResult)));
	//FIXME: what wbout ready()
	if(!di->mainOk()) return false;  //check if the main DB is OK.

	return true;
    }

}
const KAboutData *KDBSearchEngine2::about() const
{
    return DbSe2Factory::instance()->aboutData();
}

void KDBSearchEngine2::stringChanged( QString orig, QString translated
				      , QString description)
{

    if(!init()) return;
    //if(translated.isEmpty()) return;
    InputInfo ii;
    if(description.find("fuzzy",false)==-1)
	di->addEntry(orig,translated,&ii);
    
}

PrefWidget * KDBSearchEngine2::preferencesWidget(QWidget *parent)
{
    
    pw =  new KDB2PreferencesWidget(parent);
    kdDebug(0) << "new KDB2 preferences widget" << endl;
    setSettings();
    connect(pw,SIGNAL(restoreNow()),this,SLOT(setSettings()));
    connect(pw,SIGNAL(applyNow()),this,SLOT(updateSettings()));
    connect(pw,SIGNAL(destroyed()),this,SLOT(prefDestr()));

    
    connect(pw->dbpw->scanAll,SIGNAL(clicked()),this,SLOT(scanAllPressed()));
    connect(pw->dbpw->scanSource,SIGNAL(clicked()),this,SLOT(scanNowPressed()));
    connect(pw->dbpw->addSource,SIGNAL(clicked()),this,SLOT(addSource()));
    connect(pw->dbpw->editSource,SIGNAL(clicked()),this,SLOT(editSource()));
    connect(pw->dbpw->removeSource,SIGNAL(clicked()),this,SLOT(removeSource()));
    
    
    return pw;
}

void KDBSearchEngine2::saveSettings(KConfigBase *config)
{
 KConfigGroupSaver cgs(config,"KDBSearchEngine2");
    
 
 
 config->writeEntry("DBDirectory",dbDirectory);
 config->writeEntry("AutoAdd",autoAdd);
 config->writeEntry("UseSentence",useSentence);
 config->writeEntry("UseGlossary",useGlossary);
 config->writeEntry("UseExact",useExact);
 config->writeEntry("UseDivide",useDivide);
 config->writeEntry("UseAlpha",useAlpha);
 config->writeEntry("UseWordByWord",useWordByWord);
 config->writeEntry("UseDynamic",useDynamic);
 config->writeEntry("NumberOfResults",numberOfResult);
 
 config->writeEntry("ScoreDivide",settings.scoreDivide);
 config->writeEntry("ScoreExact",settings.scoreExact);
 config->writeEntry("ScoreSentence",settings.scoreSentence);
 config->writeEntry("ScoreWordByWord",settings.scoreWordByWord);
 config->writeEntry("ScoreGlossary",settings.scoreGlossary);
 config->writeEntry("ScoreAlpha",settings.scoreAlpha);
 config->writeEntry("ScoreDynamic",settings.scoreDynamic);
 config->writeEntry("MinimumScore",settings.minScore);
 config->writeEntry("FirstCapital",settings.firstCapital);
 config->writeEntry("AllCapital",settings.allCapital);
 config->writeEntry("Accelerator",settings.accelerator);
 config->writeEntry("SameLetter",settings.sameLetter);

 uint sourceNumber=0;
 config->writeEntry("NumberOfDBImportSources",sources.count());
 
   for(QMap<QString,MessagesSource>::iterator sourceIt=sources.begin() ; sourceIt!=sources.end(); ++sourceIt)
    {
     sourceNumber++;
     config->setGroup("DBImportSource-"+QString::number(sourceNumber));
     config->writeEntry("Name",sourceIt.key());
     sourceIt.data().writeConfig(config);
    }
}

void KDBSearchEngine2::readSettings(KConfigBase *config)
{
    
    /*QString defaultDir;
 KStandardDirs * dirs = KGlobal::dirs();
 if(dirs)
 {
     defaultDir = dirs->saveLocation("data");
     if(defaultDir.right(1)!="/")
	 defaultDir+="/";
     defaultDir += "kbabeldict/dbsearchengine2";
 }
*/
    KConfigGroupSaver cgs(config,"KDBSearchEngine2");
    
    // dbDirectory=config->readEntry("DBDirectory",defaultDir);
    autoAdd=config->readBoolEntry("AutoAdd",true);
    useSentence=config->readBoolEntry("UseSentence",true);
    useGlossary=config->readBoolEntry("UseGlossary",true);
    useExact=config->readBoolEntry("UseExact",true);
    useDivide=config->readBoolEntry("UseDivide",true);
    useAlpha=config->readBoolEntry("UseAlpha",true);
    useWordByWord=config->readBoolEntry("UseWordByWord",true);
    useDynamic=config->readBoolEntry("UseDynamic",true);
    numberOfResult=config->readNumEntry("NumberOfResults",5);

    settings.scoreDivide=config->readNumEntry("ScoreDivide",90);
    settings.scoreExact=config->readNumEntry("ScoreExact",100);
    settings.scoreSentence=config->readNumEntry("ScoreSentence",90);
    settings.scoreWordByWord=config->readNumEntry("ScoreWordByWord",70);
    settings.scoreGlossary=config->readNumEntry("ScoreGlossary",98);
    settings.scoreAlpha=config->readNumEntry("ScoreAlpha",98);
    settings.scoreDynamic=config->readNumEntry("ScoreDynamic",80);
    settings.minScore=config->readNumEntry("MinimumScore",60);
    settings.firstCapital=config->readBoolEntry("FirstCapital",true);
    settings.allCapital=config->readBoolEntry("AllCapital",false);
    settings.accelerator=config->readBoolEntry("Accelerator",true);
    settings.sameLetter=config->readBoolEntry("SameLetter",true);
 
    uint numberOfSources=config->readNumEntry("NumberOfDBImportSources",0);
    kdDebug(0) << "Found "<< numberOfSources << " sources" << endl;
    for(uint sourceNumber=1;sourceNumber<=numberOfSources;sourceNumber++)
    {
     config->setGroup("DBImportSource-"+QString::number(sourceNumber));
     QString name=config->readEntry("Name");
     sources[name].readConfig(config);
    }
    if(pw)
	setSettings();
    
}

void KDBSearchEngine2::setSettings()
{
    if(pw) {
	pw->dbpw->dbDirectory->setURL(dbDirectory);
	pw->dbpw->autoUpdate->setChecked(autoAdd);

	pw->dbpw->useSentence->setChecked(useSentence);
	pw->dbpw->useGlossary->setChecked(useGlossary);
	pw->dbpw->useExact->setChecked(useExact);
	pw->dbpw->useDivide->setChecked(useDivide);
	pw->dbpw->useAlpha->setChecked(useAlpha);
	pw->dbpw->useWordByWord->setChecked(useWordByWord);
	pw->dbpw->useDynamic->setChecked(useDynamic);
	pw->dbpw->numberOfResult->setValue(numberOfResult);
	pw->dbpw->scoreDivide->setValue(settings.scoreDivide);
	pw->dbpw->scoreExact->setValue(settings.scoreExact);
	pw->dbpw->scoreSentence->setValue(settings.scoreSentence);
	pw->dbpw->scoreWordByWord->setValue(settings.scoreWordByWord);
	pw->dbpw->scoreGlossary->setValue(settings.scoreGlossary);
	pw->dbpw->scoreAlpha->setValue(settings.scoreAlpha);
	pw->dbpw->scoreDynamic->setValue(settings.scoreDynamic);
	pw->dbpw->minScore->setValue(settings.minScore);
	pw->dbpw->firstCapital->setChecked(settings.firstCapital);
	pw->dbpw->allCapital->setChecked(settings.allCapital);
	pw->dbpw->accelerator->setChecked(settings.accelerator);
	pw->dbpw->sameLetter->setChecked(settings.sameLetter);
	
	//pw->dbpw->checkLang->setChecked(true);
	//pw->dbpw->useFilters->setChecked(false);
	//pw->dbpw->dateToday->setChecked(false);
	pw->dbpw->sourceList->clear();	
	for(QMap<QString,MessagesSource>::iterator sourceIt=sources.begin() ; sourceIt!=sources.end(); ++sourceIt)
	{
	    pw->dbpw->sourceList->insertItem(sourceIt.key());
	}
    }

}

void KDBSearchEngine2::updateSettings()
{
    if(!pw) return;
    
    QString newdb = pw->dbpw->dbDirectory->url();
    if(newdb !=dbDirectory)
    {
	kdDebug(0) << "Recreate DB-Interface cause dbdir is changed" << endl;
	dbDirectory=newdb;
	if (di!=0) delete di;
	di= new DataBaseInterface(dbDirectory,&settings);
    }
    
    autoAdd=pw->dbpw->autoUpdate->isChecked();
    
    useSentence=pw->dbpw->useSentence->isChecked();
    useGlossary=pw->dbpw->useGlossary->isChecked();
    useExact=pw->dbpw->useExact->isChecked();
    useDivide=pw->dbpw->useDivide->isChecked();
    useAlpha=pw->dbpw->useAlpha->isChecked();
    useWordByWord=pw->dbpw->useWordByWord->isChecked();
    useDynamic=pw->dbpw->useDynamic->isChecked();
    
    numberOfResult=pw->dbpw->numberOfResult->value();
    
    settings.scoreWordByWord=pw->dbpw->scoreWordByWord->value();
    settings.scoreGlossary=pw->dbpw->scoreGlossary->value();
    settings.scoreDivide=pw->dbpw->scoreDivide->value();
    settings.scoreExact=pw->dbpw->scoreExact->value();
    settings.scoreSentence=pw->dbpw->scoreSentence->value();
    settings.scoreAlpha=pw->dbpw->scoreAlpha->value();
    settings.scoreDynamic=pw->dbpw->scoreDynamic->value();
    
    settings.minScore=pw->dbpw->minScore->value();
    
    settings.firstCapital=pw->dbpw->firstCapital->isChecked();
    settings.allCapital=pw->dbpw->allCapital->isChecked();
    settings.accelerator=pw->dbpw->accelerator->isChecked();
    settings.sameLetter=pw->dbpw->sameLetter->isChecked();
    
    //pw->dbpw->editRule->
    //pw->dbpw->deleteRule->
    //pw->dbpw->customRules->

    /*
pw->dbpw->checkLang->
pw->dbpw->useFilters->
pw->dbpw->dateToday->

pw->dbpw->removeSource->
pw->dbpw->scanSource->
pw->dbpw->addSource->
pw->dbpw->sourceDir->
pw->dbpw->scanAll->
pw->dbpw->sourceList->
*/

}


/*void  KDBSearchEngine2::scanSource(QString sourceName)
{
    //FIXME: an error here would be nice
    if(!init()) return;
    
    
    for(QValueList<MessagesSource>::iterator sourceIt=sources.begin() ; sourceIt!=sources.end(); sourceIt++)
    {
	if((*sourceIt).getName()==sourceName)
	{
	    QValueList<KURL> urls=(*sourceIt).urls();
	    PoScanner ps(di);
	    for(QValueList<KURL>::iterator urlIt=urls.begin();urlIt!=urls.end();urlIt++)
		ps.scanFile(*urlIt);

	    //We suppose name are unique so no need for further scrolling
	    return ;
	}	
    }
}*/
void  KDBSearchEngine2::scanNowPressed()
{
    if(!pw)
    {
	kdDebug(0) << "We should not be here, scanNow called without a pw!" << endl;
	return; 
    }
    QString sourceName;	
    sourceName=pw->dbpw->sourceList->currentText();
    if(!init()) return;
    if(sources.contains(sourceName))
    {
	QValueList<KURL> urls=sources[sourceName].urls();
	PoScanner ps(di);
	for(QValueList<KURL>::iterator urlIt=urls.begin();urlIt!=urls.end();++urlIt)
	    ps.scanURL(*urlIt);	
	
    }
    else
    {	
	kdDebug(0) << "source name not found in our MAP. This is a bug." << endl;	
    }	
}

void  KDBSearchEngine2::scanAllPressed()
{
    //FIXME: an error here would be nice too
    if(!init()) return;
    PoScanner ps(di);
    
    
    for(QMap<QString,MessagesSource>::iterator sourceIt=sources.begin() ; sourceIt!=sources.end(); ++sourceIt)
    {
	QValueList<KURL> urls=(*sourceIt).urls();
	for(QValueList<KURL>::iterator urlIt=urls.begin();urlIt!=urls.end();++urlIt)
	    ps.scanURL(*urlIt);	
    }	
}

void  KDBSearchEngine2::editSource()
{
    if(!pw)
    {
	kdDebug(0) << "We should not be here, editSource called without a pw!" << endl;
	return; 
    }
    QString sourceName;	
    sourceName=pw->dbpw->sourceList->currentText();
 
    if(sources.contains(sourceName))
    {
	bool nameIsNew;
	QString newName;
	SourceDialog sd;
	do{
	    sources[sourceName].setDialogValues(&sd);
	    sd.sourceName->setText(sourceName);
	    if(sd.exec()==QDialog::Accepted)
	    {
		sources[sourceName].getDialogValues(&sd);
		newName= sd.sourceName->text();
	    } else
	    {
		return;
	    }
	    nameIsNew=sources.contains(newName);
	    if(newName!=sourceName && !nameIsNew)
	    {

		KMessageBox::error(0,
		i18n("The name you chose is already used.\nPlease change the source name."),
		i18n("Name is Not Unique"));
		kdDebug(0) << "Name is not uniqe" << endl;
	    }
	}while(newName!=sourceName && !nameIsNew);

	if(newName!=sourceName){
	  sources[newName]=sources[sourceName];
	  sources.remove(sourceName);
	}
	pw->dbpw->sourceList->changeItem(newName,pw->dbpw->sourceList->currentItem());

    }
    else
    {
	kdDebug(0) << "source name not found in our MAP. This is a bug." << endl;
    }

}
void  KDBSearchEngine2::removeSource()
{

    if(!pw)
    {
	kdDebug(0) << "We should not be here, delteSource called without a pw!" << endl;
	return; 
    }
    QString sourceName;	
    sourceName=pw->dbpw->sourceList->currentText();
    sources.remove(sourceName);
    pw->dbpw->sourceList->removeItem(pw->dbpw->sourceList->currentItem());

}

void  KDBSearchEngine2::addSource()
{
    QString newName;
    SourceDialog sd;
    bool nameIsNew;
    do{
	if(sd.exec()==QDialog::Accepted)
	{
	    newName= sd.sourceName->text();
	    nameIsNew=!sources.contains(newName);
	}
	else
	{
	    return;
	}
	// nameIsNew=sources.contains(newName);
	if(!nameIsNew)
	{
		KMessageBox::error(0,i18n("The name you chose is already used.\nPlease change the source name."),
		i18n("Name is Not Unique"));
	    kdDebug(0) << "Name is not uniqe" << endl;
	}
	else
	{
	    	    sources[newName].getDialogValues(&sd);
	}
    }while(!nameIsNew);

    pw->dbpw->sourceList->insertItem(newName);


}
QString KDBSearchEngine2::searchTranslation( const QString text, int & score )
{
	GenericSearchAlgorithm strategy(di,&settings);
	strategy.setMaxResultNumber(1);

	ExactSearchAlgorithm exact(di,&settings);
	AlphaSearchAlgorithm alpha(di,&settings);
	SentenceArchiveSearchAlgorithm sent(di,&settings);

	strategy.addAlgorithm(&exact);
	strategy.addAlgorithm(&alpha);
	strategy.addAlgorithm(&sent);

	QueryResult firstRes=strategy.exec(text)[0];
	score=firstRes.score();
	return firstRes.result();

}

QString KDBSearchEngine2::fuzzyTranslation( const QString text, int & score )
{
	GenericSearchAlgorithm strategy(di,&settings);
	strategy.setMaxResultNumber(1);
	ExactSearchAlgorithm exact(di,&settings);
	AlphaSearchAlgorithm alpha(di,&settings);
	SentenceArchiveSearchAlgorithm sent(di,&settings);
	ChunkByChunkSearchAlgorithm sbys(di,&settings);
	ChunkByChunkSearchAlgorithm wbyw(di,&settings);
	FuzzyChunkSearchAlgorithm fs(di,&settings);
	FuzzyChunkSearchAlgorithm fw(di,&settings);

	SentenceChunkFactory sf(di);
	sbys.setChunkFactory(&sf);
	fs.setChunkFactory(&sf);


	WordChunkFactory wf(di);
	wbyw.setChunkFactory(&wf);
	fw.setChunkFactory(&wf);

	strategy.addAlgorithm(&exact);
	strategy.addAlgorithm(&alpha);
	strategy.addAlgorithm(&sent);
	strategy.addAlgorithm(&sbys);
	//strategy.addAlgorithm(&fs);
	strategy.addAlgorithm(&fw);
	strategy.addAlgorithm(&wbyw);


	QueryResult firstRes=strategy.exec(text)[0];
	score=firstRes.score();
	return firstRes.result();


}

//FIXME: ProgressBasr, progressbar,progressbasr
#include "KDBSearchEngine2.moc"

