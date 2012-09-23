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
#include "dbscan.h"

#include <qdir.h>
#include <qfile.h>
#include <kapplication.h>
#include <kurl.h>
#include <kdebug.h>

using namespace KBabel;

PoScanner::PoScanner(DataBaseManager *dbm,
	QObject *parent,const char *name):QObject(parent,name)
{
dm=dbm;
removeOldCatalogTranslation=false; //Check if this flag do something.
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
 
kdDebug(0) << QString("cat: %1, %2").arg(pathName).arg(pattern) << endl;

if(pb)
{emit patternStarted();
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

	  //printf( "%s\n", d[i] );
      
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

	  
emit fileStarted();

InfoItem cinfo;
Catalog * catalog=new Catalog(this,"ScanPoCatalog");
  

QString location=fileName.right(fileName.length()-fileName.findRev("/")-1);
connect(catalog,SIGNAL(signalProgress(int)),this,SIGNAL(fileLoading(int)));
emit filename(location);
emit fileProgress(0);
emit fileLoading(0);

KURL u(fileName);

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
else author=QString("unknown");
 
int catnum=dm->catalogRef(location,author,fileName);

uint i,tot;
tot=catalog->numberOfEntries();
//DataBaseItem dbit;
bool fuzzy;
bool untra;

//kdDebug(0) << QString("Tot: %1").arg(tot) << endl;

for (i=0;i<tot;i++)  //Skip header        = ????
{

 //Faster ?
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
  kdWarning() << "Translation database does not support plural forms" << endl;
  msgstr=catalog->msgstr(i).first();
  res=dm->putNewTranslation(msgid,msgstr,catnum,false);
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
