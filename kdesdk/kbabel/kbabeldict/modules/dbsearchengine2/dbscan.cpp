/***************************************************************************
                          dbscan.cpp  -  Scan for po files to add in the DB
                         -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/*
 Translation search engine  
 
 
  Copyright  2000
  Andrea Rizzi rizzi@kde.org
 
  License GPL v 2.0

*/
#include "dbscan.h"
#include <kconfig.h>
#include <qdir.h>
#include <qfile.h>
#include <kapplication.h>
#include <kurl.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kurlrequester.h>
#include <kcombobox.h>

using namespace KBabel;

MessagesSource::MessagesSource()
{
    //FIXMR: check if we should put something in that constructor
}
void MessagesSource::writeConfig(KConfigBase *config)
{	
    config->writeEntry("Location",location.url());
    config->writeEntry("LocationType",type);
    config->writeEntry("ProjectName",projectName);
    config->writeEntry("ProjectKeywords",projectKeywords);
    config->writeEntry("Status",status);
}

void MessagesSource::readConfig(KConfigBase *config)
{
    location=config->readEntry("Location");
    type=config->readNumEntry("LocationType",0);
    projectName=config->readEntry("ProjectName");
    projectKeywords=config->readEntry("ProjectKeywords");
    status=config->readEntry("Status");
    
}

void MessagesSource::setDialogValues(SourceDialog *sd)	
{
    sd->projectName->setText(projectName);
    sd->projectKeywords->setText(projectKeywords);
    sd->sourceLocation->setURL(location.url());
    sd->status->setCurrentText(status);
    sd->type->setCurrentItem(type);
}
void MessagesSource::getDialogValues(SourceDialog *sd)
{
    projectName=sd->projectName->text();
    projectKeywords=sd->projectKeywords->text();
    location=sd->sourceLocation->url();
    status=sd->status->currentText();
    type=sd->type->currentItem();
}

QValueList<KURL> MessagesSource::urls()
{
    QValueList<KURL> urlList;
    if(type==0)
	urlList.append(location);
    if(type==1|| type==2)
    	urlList=filesInDir(location,(type==2));    
    
    return urlList;
    
}

QValueList<KURL> MessagesSource::filesInDir(KURL url,bool recursive)
{
    QValueList<KURL> result;
    QDir d(url.path());
    d.setMatchAllDirs(true);
    kdDebug(0) << d.count() << " files in dir "<< url.path()<<endl;
    const QFileInfoList* files = d.entryInfoList();
    kdDebug(0) << files << endl;
    
    //    QPtrListIterator<QFileInfo> it(*files);
    if(files){
	for (QPtrListIterator<QFileInfo> fileit(*files); !fileit.atLast(); ++fileit )
	{   		
	    if ((*fileit)->isDir())
	    {
		if(recursive)
		{
		    if((*fileit)->fileName()!="." && (*fileit)->fileName() !="..")
		    {
			result+=filesInDir(KURL((*fileit)->absFilePath()),recursive); 
			kdDebug(0) << "Recursion done for " << (*fileit)->fileName() << endl;
		    }
		}
		
	    } 
	    else
	    {
		    kdDebug(0) << (*fileit)->fileName() << endl;
		result.append(KURL((*fileit)->absFilePath()));
	    }
	}
    }
    kdDebug(0) << result.count() << endl;
		
    return result;
}

//FIXME: clean this class
PoScanner::PoScanner(DataBaseInterface *dbi,
		     QObject *parent,const char *name):QObject(parent,name)
{
    di=dbi;
    removeOldCatalogTranslation=true;
    count=0;
}

bool PoScanner::scanPattern(QString pathName,QString pattern,bool rec)
{
int tot;
//Only one progress bar!!

bool pb=false;
static bool called=false;
if (!called)
{ pb=true; count=0;}
called=true;
 
kdDebug(0) << QString("Scanning: %1, %2").arg(pathName).arg(pattern) << endl;

if(pb)
{
emit patternStarted();
emit patternProgress(0);
}
  QDir d(pathName,pattern);
  d.setMatchAllDirs(true);
  const QFileInfoList* files = d.entryInfoList();
  tot=files->count();
  QPtrListIterator<QFileInfo> it(*files);
kdDebug(0) << tot << endl;
     for ( int i=0; i<tot; i++ )
        {   
	  if ((*it)->isDir())
	   {
	    if(rec)
              {
	      kdDebug(0) << d[i] << endl;
	      if(d[i]!="." && d[i]!="..")
 	       scanPattern(pathName+"/"+d[i],pattern,true); 
	      }
	   } else
	   {
      	      kdDebug(0) << d[i] << endl;
		   scanFile(pathName+"/"+d[i]);
	   }

	if(pb)	   
  
	   emit patternProgress(100*i/tot);

      
      ++it;
	}
  	


if(pb)	
emit patternProgress(100);	


if(pb)
emit patternFinished();
if(pb){called=false;count=0;}

return true;
}

bool PoScanner::scanFile(QString fileName)
{
    KURL u(fileName);
    return scanURL(u);
}

bool PoScanner::scanURL(KURL u)
{

	  
emit fileStarted();

Catalog * catalog=new Catalog(this,"ScanPoCatalog");
  
QString pretty=u.prettyURL();
QString location=pretty.right(pretty.length()-pretty.findRev("/")-1);

connect(catalog,SIGNAL(signalProgress(int)),this,SIGNAL(fileLoading(int)));
emit filename(location);
emit fileProgress(0);
emit fileLoading(0);

bool error;

ConversionStatus rr=catalog->openURL(u);
if(rr != OK && rr !=RECOVERED_PARSE_ERROR )
{
        delete catalog;
	return false;
}
emit fileLoading(100);

QString author; 
if(rr != HEADER_ERROR)
  author=catalog->lastTranslator();
else author=QString("");

//int catnum=dm->catalogRef(location,author,fileName);
InputInfo ii;


uint i,tot;
tot=catalog->numberOfEntries();

bool fuzzy;
bool untra;


for (i=0;i<tot;i++)  //Skip header        = ????
{

  if(i % 10==0)
  { 
   emit fileProgress(100*i/tot);
   emit added(count);
   kapp->processEvents(100);
  }

  fuzzy=catalog->isFuzzy(i);
  untra=catalog->isUntranslated(i);

  
 if(!fuzzy && !untra)
 {
  int res;
  QString msgid,msgstr;
  msgid=catalog->msgid(i,true).first();
  msgstr=catalog->msgstr(i).first();
  res=di->addEntry(msgid,msgstr,&ii);
  count+=res; 
 }

	
}


//      kdDebug(0) << QString("File finished") << endl;

emit fileProgress(0);	
emit fileLoading(0);
emit fileFinished();
// dm->loadInfo();  // Sync the list of catalogs NOT NEEDED (?)

delete catalog;

//clear();
return true;

}
#include "dbscan.moc"
