/***************************************************************************
                          KDBSearchEngine.cpp  -  description
                             -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000 by Andrea Rizzi
			   (C) 2005 by Stanislav Visnovsky
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

#include <qtextedit.h>
#include <qprogressdialog.h>

#include <qinputdialog.h>
#include <kdeversion.h>
#include <klocale.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kurlrequester.h>
#include <kstandarddirs.h>

#include "kapplication.h"
#include "KDBSearchEngine.h"

#include "dbscan.h"

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include <sys/time.h>
#include "preferenceswidget.h"
#include "dbse_factory.h"
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <klineedit.h>
#include <kconfig.h>

#include <qdir.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qmemarray.h>

#include "database.h"

#include "catalogsettings.h"

#define BIGNUMBER 400000000

using namespace KBabel;

KDBSearchEngine::KDBSearchEngine (QObject * parent, const char *name):
SearchEngine (parent, name)
{
    edited = "unknown";
    dm = 0;			//Database Manager
    pw = 0;			//Preference widget
    lang = "";
    dbOpened = false;
    dbname = "";
    lasterror = i18n ("No error");
    connect (this, SIGNAL (hasError (const QString &)),
	     SLOT (setLastError (const QString &)));

    IAmReady = true;		//  I don't know if it is a good idea, no DB loaded!!!

    scanInProgress = false;
    searching = false;
    stopNow = false;

    norm = false;		// Normalize white space = FALSE
    comm = true;		// Remove Comments = TRUE

}


KDBSearchEngine::~KDBSearchEngine ()
{
}



bool
KDBSearchEngine::loadDatabase (QString database, bool noask = false)
{
    bool ret = true;
    bool asked = false;
    if (noask)
	asked = true;
    if (dm != 0)
      {
	  delete dm;
	  dm = 0;
      }

    QDir dir (database);
    if (!dir.exists ())
      {
	  if (asked
	      || KMessageBox::questionYesNo (0,
					     i18n
					     ("Database folder does not exist:\n%1\n"
					      "Do you want to create it now?").
					     arg (database), QString::null, i18n("Create Folder"), i18n("Do Not Create")) ==
	      KMessageBox::Yes)
	    {
		asked = true;

		QStringList dirList;
		while (!dir.exists () && !dir.dirName ().isEmpty ())
		  {
		      dirList.prepend (dir.dirName ());
		      dir.setPath (dir.path () + "/..");
		  }

		for (QStringList::Iterator it = dirList.begin ();
		     it != dirList.end (); ++it)
		  {
		      if (!dir.mkdir (*it))
			{
			    KMessageBox::sorry (0,
						i18n
						("It was not possible to create folder %1").
						arg (dir.path () + "/" +
						     (*it)));
			    ret = false;
			    break;
			}
		      dir.cd (*it);
		  }
	    }
	  else
	    {
		ret = false;
	    }
      }

    if (ret)
      {
	  // test, if there are both of ,old and standard databases
	  QString transFile = database + "/translations." + lang + ".db";

	  bool oldstuff = QFile::exists (transFile + ",old");
	  bool newstuff = QFile::exists (transFile);

	  if (oldstuff && newstuff)
	    {
		// there is an old db2 database, ask user

		if (KMessageBox::
		    questionYesNo (0,
				   i18n
				   ("<p>There are backup database files from previous versions "
				    "of KBabel. However, another version of KBabel (probably from KDE 3.1.1 or 3.1.2) "
				    "created a new database. As a result, your KBabel installation contains two versions "
				    "of database files. Unfortunatelly, the old and new version "
				    "can not be merged. You need to choose one of them.<br/><br/>"
				    "If you choose the old version, the new one will be removed. "
				    "If you choose the new version, the old database files will be left alone "
				    "and you need to remove them manually. Otherwise this message will be displayed "
				    "again (the old files are at $KDEHOME/share/apps/kbabeldict/dbsearchengine/*,old).</p>"),
				   i18n ("Old Database Found"),
				   i18n ("Use &Old Database"),
				   i18n ("Use &New Database")) ==
		    KMessageBox::Yes)
		  {
		      // remove the new files
		      QFile::remove (transFile);
		      QFile::remove (database + "/wordsindex." + lang +
				     ".db");
		      QFile::remove (database + "/keysindex." + lang + ".db");
		      QFile::remove (database + "/catalogsinfo." + lang +
				     ".db");

		      // rename the old files
		      KIO::NetAccess::copy (KURL (transFile + ",old"),
					    KURL (transFile), 0);
		      KIO::NetAccess::
			  copy (KURL
				(database + "/wordsindex." + lang +
				 ".db,old"),
				KURL (database + "/wordsindex." + lang +
				      ".db"), 0);
		      KIO::NetAccess::
			  copy (KURL
				(database + "/keysindex." + lang + ".db,old"),
				KURL (database + "/keysindex." + lang +
				      ".db"), 0);
		      KIO::NetAccess::
			  copy (KURL
				(database + "/catalogsinfo." + lang +
				 ".db,old"),
				KURL (database + "/catalogsinfo." + lang +
				      ".db"), 0);

		      QFile::remove (transFile + ",old");
		      QFile::remove (database + "/wordsindex." + lang +
				     ".db,old");
		      QFile::remove (database + "/keysindex." + lang +
				     ".db,old");
		      QFile::remove (database + "/catalogsinfo." + lang +
				     ".db,old");
		  }
	    }
	  else if (oldstuff)
	    {
		// rename the old files
		KIO::NetAccess::copy (KURL (transFile + ",old"),
				      KURL (transFile), 0);
		KIO::NetAccess::
		    copy (KURL (database + "/wordsindex." + lang + ".db,old"),
			  KURL (database + "/wordsindex." + lang + ".db"), 0);
		KIO::NetAccess::
		    copy (KURL (database + "/keysindex." + lang + ".db,old"),
			  KURL (database + "/keysindex." + lang + ".db"), 0);
		KIO::NetAccess::
		    copy (KURL
			  (database + "/catalogsinfo." + lang + ".db,old"),
			  KURL (database + "/catalogsinfo." + lang + ".db"), 0);

		QFile::remove (transFile + ",old");
		QFile::remove (database + "/wordsindex." + lang + ".db,old");
		QFile::remove (database + "/keysindex." + lang + ".db,old");
		QFile::remove (database + "/catalogsinfo." + lang +
			       ".db,old");
	    }

	  dm = new DataBaseManager (database, lang, this, "Database manager");

	  if (!dm->isOk ())
	    {
		if (asked
		    || KMessageBox::questionYesNo (0,
						   i18n
						   ("Database files not found.\nDo you want to create them now?"), QString::null, i18n("Create"), i18n("Do Not Create"))
		    == KMessageBox::Yes)
		  {
		      //fprintf(stderr,"SI\n");
		      ret = dm->createDataBase (database, lang);
		  }
		else
		    ret = false;
	    }
	  else
	      ret = true;
      }

//Wrong errore hangdling

    if (ret)
	totalRecord = dm->count ();
    else
	totalRecord = 0;

    return ret;
}


/*
   Set if the research have to consider multiple spaces as a single one.
 */

void
KDBSearchEngine::setNormalizeSpace (bool normalize)
{
    norm = normalize;
}


void
KDBSearchEngine::setRemoveInternalComment (bool internalcomment)
{
    comm = internalcomment;
}

/*
   Set if the research have to be Case Sensitive or not
 */

void
KDBSearchEngine::setCaseSensitive (bool sensitive)
{
    sens = sensitive;
}

/*
   Set the a string containing all char that must be ignored
   during the search.
 */

void
KDBSearchEngine::setRemoveCharString (QString chartoremove)
{
    remchar = chartoremove;
}

/*
   Return true if there's a search in progress.
  */
bool
KDBSearchEngine::isSearching () const
{
    return searching;
}




/*
   Add a search string in the list of the string to search for.
   Returns the ID of the string in the list.
   Returns -1 if there is a problem (may be search in progress)
  */

int
KDBSearchEngine::addSearchString (QString searchString, int rule)
{
    if (searching || scanInProgress)
	return -1;
    SearchEntry e;
    e.string = QString (searchString);
    e.rules = rule;
    searchStringList.append (e);
    return searchStringList.count ();
}


/*
   Start the research in the database of all the string in the list

  */

bool
KDBSearchEngine::startSearch (const QString & str, uint pluralForm,
			      const SearchFilter * filter)
{

    if (autoUpdate)
      {
	  updateSettings ();
      }


    int l1 = 0, l2 = 0;
    if (defSub1)
	l1 = defLimit1;
    if (defSub2)
	l2 = defLimit2;

    return startSingleSearch (str, l1, l2);


}

bool
KDBSearchEngine::startSearchInTranslation (QString s)
{

    if (autoUpdate)
      {
	  updateSettings ();
      }


    int l1 = 0, l2 = 0;
    if (defSub1)
	l1 = defLimit1;
    if (defSub2)
	l2 = defLimit2;


    return startSingleSearch (s, l1, l2, true);

}


bool
KDBSearchEngine::openDb (bool noask = false)
{
    if (!dbOpened)
      {
	  dbOpened = loadDatabase (dbname, noask);	//Try first to open it now
	  if (!dbOpened)	// Still not opened!!
	    {
		hasError (i18n ("Cannot open the database"));
		return false;
	    }
      }

    return true;
}




bool
KDBSearchEngine::messagesForFilter (const SearchFilter * filter,
				    QValueList < SearchResult > &resultList,
				    QString & error)
{

    int count = 0;
    stopNow = false;		// Remove dirty.
    SearchResult m;

    if (searching)
      {
	  error = i18n ("Another search has already been started");
	  return false;
      }

    if (scanInProgress)
      {
	  error =
	      i18n ("Unable to search now: a PO file scan is in progress");
	  return false;
      }


    if (!openDb ())
      {
	  error = i18n ("Unable to open the database");
	  return false;
      }

    if (totalRecord <= 0)
      {
	  error = i18n ("Database empty");
	  return false;
      }

    QString package = filter->location ();

    int step = (totalRecord / 30) + 1;
    int ntra, nref;
    int req = dm->searchCatalogInfo (package);
    if (req == -1)
      {
	  error = i18n ("No entry for this package in the database.");
	  return false;
      }
    DataBaseItem item;
    int i, h;
    kdDebug (0) << "looking for catalog " << req << endl;

    progressStarts (i18n ("Searching for %1 in database").arg (package));

    for (item = dm->firstItem (); !item.isNull (); item = dm->nextItem ())
      {
	  count++;
	  if (count % step == 0)
	    {
		emit progress (100 * count / totalRecord);
		kapp->processEvents (100);
	    }
	  if (stopNow)
	    {
		stopNow = false;
		searching = false;
		emit finished ();
		return true;	// No error, stopped!
	    }

	  ntra = item.numTra;
	  for (i = 0; i < ntra; i++)
	    {
		nref = item.translations[i].numRef;

		for (h = 0; h < nref; h++)
		  {
		      if (item.translations[i].infoRef[h] == req)
			{
			    m.found = item.key;
			    m.translation = item.translations[i].translation;
			    resultList.append (m);
			}
		  }
	    }

      }

    return true;
}

void
KDBSearchEngine::repeat ()
{

    int count = 0;
    stopNow = false;		// Remove dirty.

    if (searching)
      {
	  return;
      }

    if (scanInProgress)
      {
	  return;
      }


    if (!openDb ())
      {
	  return;
      }

    if (totalRecord <= 0)
      {
	  return;
      }

    int step = (totalRecord / 30) + 1;
    int ntra, nref;

    DataBaseItem item;
    int i, h, tot;

    int req = dm->searchCatalogInfo ("kdelibs.po");
    if (req == -1)
	kdDebug (0) << "No kdelibs.po found!" << endl;


    QProgressDialog *pd =
	new QProgressDialog (i18n ("Looking for repetitions"), i18n ("Stop"),
			     100);

    connect (this, SIGNAL (progress (int)), pd, SLOT (setProgress (int)));
    connect (this, SIGNAL (finished ()), pd, SLOT (close ()));
    connect (pd, SIGNAL (cancelled ()), this, SLOT (stopSearch ()));


    QString txt = "// %1 repetitions, %2 translation(s)\ni18n(\"%3\");\n";
    QString id;
    int min;
    bool ok = false;

    min =
	QInputDialog::getInteger (i18n ("Minimum Repetition"),
				  i18n
				  ("Insert the minimum number of repetitions for a string:"),
				  2, 1, 999999, 1, &ok);

    if (!ok)
	return;

    pd->show ();

    progressStarts (i18n ("Searching repeated string"));

    static QTextEdit *mle = new QTextEdit ();
    mle->clear ();

    bool inlibs;

    for (item = dm->firstItem (); !item.isNull (); item = dm->nextItem ())
      {
	  count++;
	  if (count % step == 0)
	    {
		emit progress (100 * count / totalRecord);
		kapp->processEvents (100);
	    }
	  if (stopNow)
	    {
		stopNow = false;
		searching = false;
		emit finished ();
		return;		// No error, stopped!
	    }
	  tot = 0;
	  inlibs = false;
	  ntra = item.numTra;
	  for (i = 0; i < ntra; i++)
	    {
		nref = item.translations[i].numRef;
		for (h = 0; h < nref; h++)
		    if (item.translations[i].infoRef[h] == req)
			inlibs = true;

		tot += nref;
	    }

	  if (tot >= min && !inlibs)
	    {
		id = item.key;
		id = id.replace ("\n", "\"\n\"");
		mle->append (txt.arg (tot).arg (ntra).arg (id));

	    }
      }


    emit progress (100);
    emit finished ();
    mle->resize (400, 400);
    mle->show ();

    delete pd;
    return;
}



bool
KDBSearchEngine::startSearchNow (int searchmode)
{
    if (searchmode == -1)
	searchmode = mode;
    int count = 0;
    stopNow = false;		// Remove dirty.
    clearResults ();


    if (searching)
      {
	  hasError (i18n ("Another search has already been started"));
	  return false;
      }

    if (scanInProgress)
      {
	  hasError (i18n
		    ("Unable to search now: a PO file scan is in progress"));
	  return false;
      }


    if (!openDb ())
	return false;


    if (totalRecord <= 0)
      {
	  hasError (i18n ("Database empty"));
	  return false;
      }



    searching = true;

    emit started ();

    bool allkey = (searchmode == MD_ALL_GOOD_KEYS);

    bool equal, contains, contained, regexp, intra;

    intra = searchmode & MD_IN_TRANSLATION;

    QString msgIdFound;
    QString msgId;
    QString msgStr;
    //QString msgIdRequested;
    SearchResult *aresult;
    TranslationInfo *adescription;
    SearchList searchList;
    int i, len, files, h;
    len = remchar.length ();

    int n, m;			//,word;
    QString *id;

    QString mainRequest = searchStringList[0].string;


    SearchList::Iterator it, it1;
    QString *idMod;
    bool foundSomething = false;

    searchList = searchStringList;	//Create a copy and modify it
    if (!allkey)
      {
	  for (it = searchList.begin (); it != searchList.end (); ++it)
	    {
		idMod = &((*it).string);
		int pos;
		for (i = 0; i < len; i++)
		  {
		      while ((pos = idMod->find (remchar.at (i))) != -1)
			  idMod->remove (pos, 1);
		  }

		if (comm)
		    idMod->replace (QRegExp ("\\_\\:.*\\\\n"), "");	//Read it from catalog !!! (NOT ONLY HERE)


		if (norm)
		    idMod->simplifyWhiteSpace ();
		if (!sens)
		    *idMod = idMod->upper ();
	    }

      }

    timeval now;
    gettimeofday (&now, NULL);
    //fprintf(stderr,"\n%ld.%ld\n",now.tv_sec,now.tv_usec);
    //double tim=1.0*now.tv_usec/1000000.0+now.tv_sec;
    int pos;



    DataBaseItem item;


    //Now we can browse the whole db or the "good keys"
    QValueList < KeyAndScore > goodkeys;
    int totalprogress;

    bool gk = (searchmode == MD_GOOD_KEYS) || allkey;
    int k = 0;

    if (gk)
      {
	  goodkeys = searchWords (mainRequest, thre, threorig, listmax);	//FIX IT, mainReq?
	  if (stopNow)
	  {
		stopNow = false;
		searching = false;
		emit finished ();
		return false;
	  }
	  if (goodkeys.count () == 0)
	      gk = false;	// if no good keys, use the whole database
      }
      
    // prepare progress values
    totalprogress = gk ? goodkeys.count () : totalRecord;
    int step = (totalprogress / 30) + 1;
    if( step > 100 )
	step = 100;

    emit progress (0);
    kapp->processEvents (100);
    if (stopNow)
    {
	stopNow = false;
	searching = false;
	emit finished ();
	return true;	// No error, stopped!
    }
    
    
    for (item = gk ? (dm->getItem (goodkeys[0])) : (dm->firstItem ());
	 !item.isNull ();
	 item = gk ? (dm->getItem (goodkeys[++k])) : (dm->nextItem ()))
      {

//   Emit progress, process event and check stop now
	  if (count % step == 0)
	    {
		emit progress (100 * count / /*QMAX( */
			       totalprogress /*,1) */ );
		kapp->processEvents (100);

		if (stopNow)
		{
	    	    stopNow = false;
		    searching = false;
		    emit finished ();
		    return true;	// No error, stopped!
		}
	    }

	  //  fprintf(stderr,"%s\n",(const char *)item.key.utf8());
	  msgIdFound = item.key;	//Check if this is OK with UTF8

//     searchmode && MD_IN_TRANSLATION)

	  count++;



	  msgId = msgIdFound;

	  if (!allkey)
	    {

		//Remove character in list of character to be ignored
		for (i = 0; i < len; i++)
		    while ((pos = msgId.find (remchar.at (i))) != -1)
			msgId.remove (pos, 1);

		//Remove context information from id found
		if (comm)
		    msgId.replace (QRegExp ("\\_\\:.*\\\\n"), "");


		if (norm)
		    msgId.simplifyWhiteSpace ();
		if (!sens)
		    msgId = msgId.upper ();
	    }



	  it = searchList.begin ();
	  idMod = &((*it).string);
	  bool foundExact = false;
	  
	  for (it1 = searchStringList.begin ();
	       it1 != searchStringList.end (); it1++)
	    {

		id = &((*it1).string);
		uint nn = 0;
		do
		  {
		      if (intra)
			{
			    msgId = item.translations[nn].translation;
			    if (!allkey)
			      {
				  //Remove character in list of character to be ignored
				  for (i = 0; i < len; i++)
				      while ((pos =
					      msgId.find (remchar.at (i))) !=
					     -1)
					  msgId.remove (pos, 1);

				  //Remove context information from id found
				  if (comm)
				      msgId.
					  replace (QRegExp ("\\_\\:.*\\\\n"),
						   "");


				  if (norm)
				      msgId.simplifyWhiteSpace ();
				  if (!sens)
				      msgId = msgId.upper ();
			      }


			}


		      int rules = (*it).rules;

		      if (rules & Equal)
			  equal = (*idMod == msgId);
		      else
			  equal = false;

		      if (rules & Contains)
			  contains = idMod->contains (msgId);
		      else
			  contains = false;

		      if (rules & Contained)
			  contained = msgId.contains (*idMod);
		      else
			  contained = false;

		      if (!foundExact && (rules & RegExp))
			{
			    QRegExp reg (*idMod);
			    regexp = (reg.search (msgId) != -1);
			}
		      else
			  regexp = false;
		      nn++;
		  }
		while (intra && nn < item.numTra);

		if (equal || contains || contained || regexp || allkey)
		  {

		      if (equal)
			  foundExact = true;

		      m = item.numTra;	//Translations found.

		      for (n = 0; n < m; n++)
			{

			    foundSomething = true;


			    msgStr = item.translations[n].translation;

			    files = item.translations[n].numRef;

			    aresult = new SearchResult ();

			    results.setAutoDelete (true);
			    if (!gk)
				aresult->score =
				    score (mainRequest, msgIdFound);
			    else
				aresult->score = goodkeys[k].score;

			    if (intra)
				aresult->score =
				    score (mainRequest,
					   item.translations[n].translation);


			    SearchResult *s = 0;
			    for (s = results.first (); s != 0;
				 s = results.next ())
			      {
				  if (s->score > aresult->score)
				    {
					results.insert (results.at (),
							aresult);
					break;
				    }

			      }
			    if (s == 0)	//no break or empty list
				results.append (aresult);


/*		  if(*id==msgIdFound)  //Put it first of the list
		  results.prepend(aresult);
		    else
		  results.append(aresult);
*/
			    aresult->requested = *id;
			    aresult->found = msgIdFound;
			    aresult->translation = msgStr;
			    aresult->descriptions.setAutoDelete (true);
			    for (h = 0; h < files; h++)
			      {

				  aresult->descriptions.append (adescription =
								new
								TranslationInfo
								());
				  int rr = item.translations[n].infoRef[h];

				  InfoItem info = dm->getCatalogInfo (rr);


				  adescription->location = info.catalogName;
				  adescription->translator =
				      info.lastTranslator;
				  adescription->filePath = info.lastFullPath;
			      }

			    emit numberOfResultsChanged (results.count ());
			    emit resultFound (aresult);

//                if(*id==msgIdFound)  //Put it first of the list so th order change
			    emit resultsReordered ();



			}
		  }
		//        idMod=searchList.next();
		it++;
		idMod = &((*it).string);
	    }


      }
    gettimeofday (&now, NULL);
    //fprintf(stderr,"%ld.%ld\n",now.tv_sec,now.tv_usec);

    //fprintf(stderr,"Finish, %d (of %d) records in %f seconds!!\n",count,totalRecord, 1.0*now.tv_usec/1000000.0+now.tv_sec-tim);
    emit progress (100);
    emit finished ();

    searching = false;
    return true;		//foundSomething;
}

/*
  Start a search for a single string
 */

bool
KDBSearchEngine::startSingleSearch (QString searchString,
				    unsigned int pattern1Limit,
				    unsigned int /*pattern2Limit */ ,
				    bool inTranslation)
{
    /*
       Check Ret
       value.
     */

    unsigned int nw = 0;
    int in = 0, len = 0;
    clearList ();
    addSearchString (searchString, defRule);



    QRegExp reg ("[a-zA-Z0-9_%" /*+remchar */  + regaddchar + "]+");
    while ((in = reg.search (searchString, in + len)) != -1)
      {
	  nw++;
	  len = reg.matchedLength ();
      }
    in = 0;
    len = 0;
    // fprintf(stderr,"asas %d\n",nw);

    if (mode == MD_ALL_GOOD_KEYS && !inTranslation)
	return startSearchNow ();



    if ((nw < pattern1Limit) && (nw > 1))
	for (unsigned int k = 0; k < nw; k++)
	  {
	      in = reg.search (searchString, in + len);
	      len = reg.matchedLength ();
	      QString regToAdd = searchString;
	      regToAdd.replace (in, len, "[a-zA-Z0-9_%" + regaddchar + "]*");
	      regToAdd.append ("$");
	      regToAdd.prepend ("^");
//      fprintf(stderr,"%s",(const char *)regToAdd.local8Bit());
	      addSearchString (regToAdd, RegExp);
	  }

    if (inTranslation)
	return startSearchNow (MD_IN_TRANSLATION);
    else
	return startSearchNow ();

    return false;

}


/*
  Start a search for a list of string
 */

//bool KDBSearchEngine::startListSearch(QPtrList<QString> searchStrList)
//{
  //  searchStringList=searchStrList;
//  return startSearchNow();
//}
/*
  Stop the current search
  */



void
KDBSearchEngine::setLanguageCode (const QString & ll)
{
    if (ll == lang)
	return;

    lang = ll;
    if (dbOpened)		//if opened open it again with new code, what about close before open ?
	dbOpened = loadDatabase (dbname);

}

void
KDBSearchEngine::setLanguage (const QString & languageCode,
			      const QString & /*languageName */ )
{
    setLanguageCode (languageCode);
}

void
KDBSearchEngine::stopSearch ()
{
    stopNow = true;
}


void
KDBSearchEngine::clearList ()
{
    searchStringList.clear ();
}

bool
KDBSearchEngine::isReady () const 
{
    return IAmReady;
}


void
KDBSearchEngine::saveSettings (KConfigBase * config)
{
// updateSettings();  //maybe with autoupdate
    KConfigGroupSaver cgs (config, "KDBSearchEngine");
#if KDE_IS_VERSION(3,1,3)
    config->writePathEntry ("Filename", dbname);
#else
    config->writeEntry ("Filename", dbname);
#endif
    config->writeEntry ("Language", lang);


    config->writeEntry ("CaseSensitive", sens);
    config->writeEntry ("Normalize", norm);
    config->writeEntry ("RemoveContext", comm);

    config->writeEntry ("Rules", defRule);
    config->writeEntry ("Limit1", defLimit1);
    config->writeEntry ("Limit2", defLimit2);
    config->writeEntry ("Substitution1", defSub1);
    config->writeEntry ("Substitution2", defSub2);

    config->writeEntry ("RegExp", regaddchar);
    config->writeEntry ("RemoveCharacter", remchar);

    config->writeEntry ("Threshold1", thre);
    config->writeEntry ("Threshold2", threorig);
    config->writeEntry ("ListMax", listmax);
    config->writeEntry ("Mode", mode);
    config->writeEntry ("CommonThrs", commonthre);
    config->writeEntry ("ReturnNothing", retnot);

    config->writeEntry ("AutoAuthor", autoauthor);
    config->writeEntry ("AutoUp", autoup);

}


void
KDBSearchEngine::readSettings (KConfigBase * config)
{
    QString newName;

    KConfigGroupSaver cgs (config, "KDBSearchEngine");

    QString defaultLang;
    QString oldLang = lang;
    Defaults::Identity def;
    defaultLang = def.languageCode ();
    lang = config->readEntry ("Language", defaultLang);

    QString defaultDir;
    KStandardDirs *dirs = KGlobal::dirs ();
    if (dirs)
      {
	  defaultDir = dirs->saveLocation ("data");
	  if (defaultDir.right (1) != "/")
	      defaultDir += "/";
	  defaultDir += "kbabeldict/dbsearchengine";
      }

    newName = config->readPathEntry ("Filename", defaultDir);

    if (newName != dbname || oldLang != lang)
      {
	  dbname = newName;
	  if (dbOpened)		//Reload only if it is opened
	      dbOpened = loadDatabase (dbname);
      }

    sens = config->readBoolEntry ("CaseSensitive", false);
    norm = config->readBoolEntry ("Normalize", true);
    comm = config->readBoolEntry ("RemoveContext", true);

    defRule = config->readNumEntry ("Rules", 1);
    defLimit1 = config->readNumEntry ("Limit1", 20);
    defLimit2 = config->readNumEntry ("Limit2", 8);

    thre = config->readNumEntry ("Threshold1", 50);
    threorig = config->readNumEntry ("Threshold2", 50);
    listmax = config->readNumEntry ("ListMax", 500);
    mode = config->readNumEntry ("Mode", MD_GOOD_KEYS);

    defSub1 = config->readBoolEntry ("Substitution1", true);
    defSub2 = config->readBoolEntry ("Substitution2", false);

    regaddchar = config->readEntry ("RegExp");
    remchar = config->readEntry ("RemoveCharacter", "&.:");
    commonthre = config->readNumEntry ("CommonThrs", 300);
    retnot = config->readBoolEntry ("ReturnNothing", false);
    autoauthor = config->readEntry ("AutoAuthor");
    autoup = config->readBoolEntry ("AutoUp", true);

    setSettings ();
}

PrefWidget *
KDBSearchEngine::preferencesWidget (QWidget * parent)
{

    pw = new PreferencesWidget (parent);
    setSettings ();
    connect (pw, SIGNAL (restoreNow ()), this, SLOT (setSettings ()));
    connect (pw, SIGNAL (applyNow ()), this, SLOT (updateSettings ()));
    connect (pw, SIGNAL (destroyed ()), this, SLOT (prefDestr ()));
    connect (pw->dbpw->scanPB_2, SIGNAL (clicked ()), this, SLOT (scan ()));
    connect (pw->dbpw->scanrecPB, SIGNAL (clicked ()), this,
	     SLOT (scanRecur ()));
    connect (pw->dbpw->scanFilePB, SIGNAL (clicked ()), this,
	     SLOT (scanFile ()));
    connect (pw->dbpw->repeatPB, SIGNAL (clicked ()), this, SLOT (repeat ()));


    return pw;
}

void
KDBSearchEngine::scanRecur ()
{
    if (scanInProgress)
	return;
    updateSettings ();

    if (!openDb ())
	return;
    scanInProgress = true;
    PoScanner *sca = new PoScanner (dm, this, "Po Scanner");
    QString cvsdir;
    cvsdir =
	KFileDialog::getExistingDirectory ("", 0,
					   i18n
					   ("Select Folder to Scan Recursively"));

    if (cvsdir.isEmpty ())
      {
	  scanInProgress = false;
	  return;
      }
    if (pw)
      {
	  connect (sca, SIGNAL (patternProgress (int)), pw->dbpw->totalPB,
		   SLOT (setProgress (int)));
	  connect (sca, SIGNAL (fileLoading (int)), pw->dbpw->loadingPB,
		   SLOT (setProgress (int)));
	  connect (sca, SIGNAL (fileProgress (int)), pw->dbpw->processPB,
		   SLOT (setProgress (int)));
      }

    connect (sca, SIGNAL (patternProgress (int)), SIGNAL (progress (int)));	//Kbabel progress bar

    connect (sca, SIGNAL (added (int)), pw, SLOT (setEntries (int)));
    connect (sca, SIGNAL (filename (QString)), pw, SLOT (setName (QString)));


    progressStarts (i18n ("Scanning folder %1").arg (cvsdir));
    connect (sca, SIGNAL (patternFinished ()), SIGNAL (progressEnds ()));

    sca->scanPattern (cvsdir, "*.po", true);
    disconnect (this, SIGNAL (progress (int)));
//disconnect(SIGNAL(patternStarted()),this,SIGNAL(started()) );
    disconnect (this, SIGNAL (progressEnds ()));
    if (pw)
      {
	  disconnect (pw->dbpw->totalPB, SLOT (setProgress (int)));
	  disconnect (pw->dbpw->loadingPB, SLOT (setProgress (int)));
	  disconnect (pw->dbpw->processPB, SLOT (setProgress (int)));
      }
    totalRecord = dm->count ();

    scanInProgress = false;
    dm->sync ();
    delete sca;
}


void
KDBSearchEngine::scan ()
{
    if (scanInProgress)
	return;
    updateSettings ();

    if (!openDb ())
	return;
    scanInProgress = true;
    PoScanner *sca = new PoScanner (dm, this, "Po Scanner");
    QString cvsdir;

    cvsdir =
	KFileDialog::getExistingDirectory ("", 0,
					   i18n ("Select Folder to Scan"));
    if (cvsdir.isEmpty ())
      {
	  scanInProgress = false;
	  return;
      }
    if (pw)
      {
	  connect (sca, SIGNAL (patternProgress (int)), pw->dbpw->totalPB,
		   SLOT (setProgress (int)));
	  connect (sca, SIGNAL (fileLoading (int)), pw->dbpw->loadingPB,
		   SLOT (setProgress (int)));
	  connect (sca, SIGNAL (fileProgress (int)), pw->dbpw->processPB,
		   SLOT (setProgress (int)));
      }
    connect (sca, SIGNAL (patternProgress (int)), SIGNAL (progress (int)));
    progressStarts (i18n ("Scanning folder %1").arg (cvsdir));
    connect (sca, SIGNAL (patternFinished ()), SIGNAL (progressEnds ()));

    connect (sca, SIGNAL (added (int)), pw, SLOT (setEntries (int)));
    connect (sca, SIGNAL (filename (QString)), pw, SLOT (setName (QString)));



    sca->scanPattern (cvsdir, "*.po", false);

    disconnect (this, SIGNAL (progress (int)));
//disconnect(SIGNAL(patternStarted()),this,SIGNAL(started()) );
    disconnect (this, SIGNAL (progressEnds ()));
    if (pw)
      {
	  disconnect (pw->dbpw->totalPB, SLOT (setProgress (int)));
	  disconnect (pw->dbpw->loadingPB, SLOT (setProgress (int)));
	  disconnect (pw->dbpw->processPB, SLOT (setProgress (int)));
      }
    totalRecord = dm->count ();

    scanInProgress = false;

    dm->sync ();
    delete sca;
}

void
KDBSearchEngine::scanFile ()
{
    if (scanInProgress)
	return;
    updateSettings ();

    if (!openDb ())
	return;
    scanInProgress = true;
    PoScanner *sca = new PoScanner (dm, this, "Po Scanner");
    QString cvsdir;
    pw->dbpw->totalPB->setProgress (0);

    cvsdir =
	KFileDialog::getOpenFileName ("", "*.po", 0,
				      i18n ("Select PO File to Scan"));
    if (cvsdir.isEmpty ())
      {
	  scanInProgress = false;
	  return;
      }
    if (pw)
      {
	  connect (sca, SIGNAL (fileLoading (int)), pw->dbpw->loadingPB,
		   SLOT (setProgress (int)));
	  connect (sca, SIGNAL (fileProgress (int)), pw->dbpw->processPB,
		   SLOT (setProgress (int)));
      }
    connect (sca, SIGNAL (fileProgress (int)), SIGNAL (progress (int)));
    progressStarts (i18n ("Scanning file %1").arg (directory (cvsdir, 0)));
    connect (sca, SIGNAL (fileFinished ()), SIGNAL (progressEnds ()));

    connect (sca, SIGNAL (added (int)), pw, SLOT (setEntries (int)));
    connect (sca, SIGNAL (filename (QString)), pw, SLOT (setName (QString)));



    sca->scanFile (cvsdir);

    sca->disconnect (SIGNAL (fileProgress (int)), this,
		     SIGNAL (progress (int)));
//disconnect(SIGNAL(patternStarted()),this,SIGNAL(started()) );
    sca->disconnect (SIGNAL (fileFinished ()), this,
		     SIGNAL (progressEnds ()));
    if (pw)
      {
	  disconnect (pw->dbpw->loadingPB, SLOT (setProgress (int)));
	  disconnect (pw->dbpw->processPB, SLOT (setProgress (int)));
      }

    totalRecord = dm->count ();

    scanInProgress = false;

    dm->sync ();
    delete sca;
}

const KAboutData *
KDBSearchEngine::about () const
{

    return DbSeFactory::instance ()->aboutData ();

}

QString
KDBSearchEngine::name () const
{
    return i18n ("Translation Database");
}

QString
KDBSearchEngine::id () const
{
    return QString ("KDBSearchEngine");
}

QString
KDBSearchEngine::lastError ()
{
    return lasterror;
}

void
KDBSearchEngine::prefDestr ()
{
    pw = 0;
}


void
KDBSearchEngine::setSettings ()
{

    if (pw == 0)
	return;

    pw->dbpw->dirInput->setURL (dbname);
    pw->dbpw->caseSensitiveCB->setChecked (sens);
    pw->dbpw->normalizeCB->setChecked (norm);
    pw->dbpw->removeContextCB->setChecked (comm);

    pw->dbpw->oneWordSubCB->setChecked (defSub1);
    pw->dbpw->twoWordSubCB->setChecked (defSub2);


    if (defRule == 8)
	pw->dbpw->RegExpRB->setChecked (true);
    else
      {
	  pw->dbpw->normalTextRB->setChecked (true);
	  pw->dbpw->equalCB->setChecked (defRule & Equal);
	  pw->dbpw->containsCB->setChecked (defRule & Contains);
	  pw->dbpw->containedCB->setChecked (defRule & Contained);
      }

    pw->dbpw->oneWordSubSB->setValue (defLimit1);
    pw->dbpw->twoWordSubSB->setValue (defLimit2);

    pw->dbpw->maxSB->setValue (listmax);
    pw->dbpw->thresholdSL->setValue (thre);
    pw->dbpw->thresholdOrigSL->setValue (threorig);

    pw->dbpw->allRB->setChecked (mode == MD_ALL_DB);
    pw->dbpw->slistRB->setChecked (mode == MD_GOOD_KEYS);
    pw->dbpw->rlistRB->setChecked (mode == MD_ALL_GOOD_KEYS);

    pw->dbpw->nothingCB->setChecked (retnot);
    pw->dbpw->freqSB->setValue (commonthre);

    pw->dbpw->regExpLE->setText (regaddchar);
    pw->dbpw->ignoreLE->setText (remchar);

    pw->dbpw->authorLE->setText (autoauthor);
    pw->dbpw->autoAddCB_2->setChecked (autoup);



}


void
KDBSearchEngine::updateSettings ()
{
    if (pw == 0)
	return;

    QString newName = pw->dbpw->dirInput->url ();

    if (newName != dbname)
      {
	  kdDebug (0) << "Database changed" << endl;
	  dbname = newName;
	  if (dbOpened)
	      dbOpened = loadDatabase (dbname);
      }

    sens = pw->dbpw->caseSensitiveCB->isChecked ();
    norm = pw->dbpw->normalizeCB->isChecked ();
    comm = pw->dbpw->removeContextCB->isChecked ();

    int tmpRule = 0;
    if (pw->dbpw->RegExpRB->isChecked ())
	tmpRule = RegExp;
    else
      {
	  if (pw->dbpw->equalCB->isChecked ())
	      tmpRule += Equal;
	  if (pw->dbpw->containsCB->isChecked ())
	      tmpRule += Contains;
	  if (pw->dbpw->containedCB->isChecked ())
	      tmpRule += Contained;
      }

    defRule = tmpRule;

    defLimit1 = pw->dbpw->oneWordSubSB->text ().toInt ();
    defLimit2 = pw->dbpw->twoWordSubSB->text ().toInt ();
    defSub1 = pw->dbpw->oneWordSubCB->isChecked ();
    defSub2 = pw->dbpw->twoWordSubCB->isChecked ();

    listmax = pw->dbpw->maxSB->value ();
    thre = pw->dbpw->thresholdSL->value ();
    threorig = pw->dbpw->thresholdOrigSL->value ();

    if (pw->dbpw->allRB->isChecked ())
	mode = MD_ALL_DB;
    if (pw->dbpw->slistRB->isChecked ())
	mode = MD_GOOD_KEYS;
    if (pw->dbpw->rlistRB->isChecked ())
	mode = MD_ALL_GOOD_KEYS;


    regaddchar = pw->dbpw->regExpLE->text ();
    remchar = pw->dbpw->ignoreLE->text ();

    retnot = pw->dbpw->nothingCB->isChecked ();
    commonthre = pw->dbpw->freqSB->value ();

    autoauthor = pw->dbpw->authorLE->text ();
    autoup = pw->dbpw->autoAddCB_2->isChecked ();


}

void
KDBSearchEngine::setLastError (const QString & er)
{
    lasterror = er;
}

QString
KDBSearchEngine::translate (const QString & text, const uint pluralForm)
{
    if (!openDb ())
	return QString::null;
/*

if(!dbOpened)
    {
    dbOpened=loadDatabase(dbname);  //Try first to open it now
    if(!dbOpened) // Still not opened!!
        {
         //emit anerror
	 hasError(i18n("Database not opened"));
         return QString::null;
       }
     }
*/

    DataBaseItem dbit = dm->getItem (text);

    if (dbit.isNull ())
	return QString::null;
    if (dbit.numTra == 1)
	return dbit.translations[0].translation;

    uint32 n = dbit.numTra;
    uint32 max = 0, nmax = 0;
    for (uint32 i = 0; i < n; i++)
	if (dbit.translations[i].numRef > max)
	  {
	      nmax = i;
	      max = dbit.translations[i].numRef;
	  }

    return dbit.translations[nmax].translation;


}

QValueList < KeyAndScore > KDBSearchEngine::searchWords (QString phrase,
							 int threshold,
							 int thresholdorig,
							 uint32 max)
{
    QValueList < QString > wordlist;

    if (!openDb ())
      {
	  QValueList < KeyAndScore > a;
	  return a;
      }

    progressStarts (i18n ("Searching words"));

    QValueList < QString >::Iterator wlit;
    wordlist = dm->wordsIn (phrase);
    int
	nw = wordlist.count ();
//QMemArray<WordItem> wi(nw);
    QMemArray < uint32 > numofloc (nw), currentloc (nw);
    QMemArray < int >
    score (nw);
    QMemArray < uint32 * >loc (nw), locorig (nw);
    QValueList < uint32 > resloc;
    QValueList < int >
	resfound;
    QValueList < KeyAndScore > keylist;
//wi.resize(wordlist.count());
    int
	totalprogress = 0;
    int
	totrec = dm->count ();
    uint32
	cthre = totrec * commonthre / 10000;
    int
	i = 0, common = 0;
    for (wlit = wordlist.begin (); wlit != wordlist.end (); ++wlit)
      {
	  WordItem
	      wi = dm->getWordLocations (*wlit);
	  if (!wi.notFound ())
	    {
		if (wi.count < cthre)
		    score[i] = 1;
		else
		  {
		      score[i] = 0;
		      common++;
		  }

		locorig[i] = loc[i] = wi.locations;
		totalprogress += numofloc[i] = wi.count;
		currentloc[i] = 0;
//         score[i]=wi.score;
		//wi[i]=WordItem(wi[i]);
		//wi[i].locations.detach();
		i++;
//     }
		//   else
		//  common++;



	    }
      }
    bool
	cs = (common == nw);	//All words are common;
    if (totalprogress == 0)
	totalprogress = 1;
    int
	step = totalprogress / 30 + 1;
    int
	count = 0;
    int
	thrs = (wordlist.count () * threshold) / 100;
    if (thrs < 1)
	thrs = 1;		// whole database ???

    int
	tot = i;
//nt32 jmin=0;
    int
	found;
    uint32
	min;			//Big ?
    bool
	empty = false;


    while (!empty)
      {

	  empty = true;
	  found = retnot ? common : 0;
	  if (thrs <= found)
	      thrs = found + 1;	// whole database ???

	  min = BIGNUMBER;
	  for (int j = 0; j < tot; j++)
	      if (cs || score[j])
		{
		    if (numofloc[j] > currentloc[j])	// Check if there's still something to do.
			empty = false;
		    if (loc[j][0] < min)	//Found the minimum head
			min = loc[j][0];


		}
	  if (min != BIGNUMBER)
	    {
		for (int j = 0; j < tot; j++)
		    if (cs || score[j])
		      {
			  if (loc[j][0] == min)	//Count the heads, move forward
			    {
				found++;
				count++;
				//check stopnow  here
				if (count % step == 0)
				  {
				      emit
				      progress (100 * count / totalprogress);
				      kapp->processEvents (100);
				  }
				  if (stopNow)
				  {
					return keylist;
				  }

				currentloc[j]++;
				if (numofloc[j] == currentloc[j])	//End reached
				    loc[j][0] = BIGNUMBER;	//so set head to a big number
				else	//Go on..
				  {
				      loc[j]++;
				  }
			    }
		      }		//end of for
		bool
		    inserted = false;



		if (found >= thrs)
		  {
		      //Words count in key.
		      int
			  nword = 0;

		      int
			  in = 0, len = 0;
		      QString
			  keyst = dm->getKey (min);
		      QRegExp
		      reg ("[a-zA-Z0-9_%" /*+remchar */  + regaddchar + "]+");
		      while ((in = reg.search (keyst, in + len)) != -1)
			{
			    nword++;
			    len = reg.matchedLength ();
			}

		      if (found >= nword * thresholdorig / 100)	//
			{

			    if (resfound.count () <= max
				|| (*resfound.end () < found))
				if ((*resfound.end ()) >= found)
				  {
				      inserted = true;
				      resloc.append (min);
				      resfound.append (found);

				  }
				else
				    for (uint32 j = 0; j < resloc.count ();
					 j++)
				      {
					  if (resfound[j] < found || (resfound[j] == found && 0))	//Orig word
					    {
						resloc.insert (resloc.at (j),
							       min);
						resfound.insert (resfound.
								 at (j),
								 found);
						inserted = true;
						break;
					    }
				      }
			    if (!inserted)
			      {
				  resloc.append (min);
				  resfound.append (found);
			      }

			}
		  }

	    }

      }
    int
	nres = (resloc.count () < max) ? resloc.count () : max;

    for (int j = 0; j < nres; j++)
      {
	  QString
	      strkey = dm->getKey (resloc[j]);
	  int
	      stdscore = KDBSearchEngine::score (phrase, strkey);
	  int
	      sc = 0;

	  if (stdscore < 99)
	    {
		int
		    in = 0, len = 0, nword = 0;
		int
		    remove = retnot ? common : 0;
		QRegExp
		reg ("[a-zA-Z0-9_%" /*+remchar */  + regaddchar + "]+");
		while ((in = reg.search (strkey, in + len)) != -1)
		  {
		      nword++;
		      len = reg.matchedLength ();
		  }

//    kdDebug(0) << nword << "NWORD  " << resfound[j] << "FOUND  "
//    << resfound[j]-remove << "REAL  " << remove << "to be remove   " << endl;
		if (nword < 1)
		    nword = 1;
		sc = 70 - resfound[j] * 70 / nw + abs (nword -
						       (resfound[j] -
							remove)) * 30 /
		    nword + 2;
		sc = 100 - sc;
//    kdDebug(0) <<" Score : " << sc << endl;

	    }
	  else
	      sc = stdscore;

	  KeyAndScore
	  key (strkey, sc);

//  kdDebug(0) << (QString) key << " [" << key.score << "]" << endl;
	  keylist.append (key);
      }

//kdDebug(0) << "Here!" << endl;

    for (int j = 0; j < tot; j++)
      {
	  free (locorig[j]);
      }
    progressStarts (i18n ("Process output"));
    return keylist;
}

void
KDBSearchEngine::stringChanged (const QStringList & o,
				const QString & translated, const uint,
				const QString &)
{

    QString orig = o.first ();	// FIXME: plural forms

    // skip empty originals or translated texts
    if (orig.isEmpty () || translated.isEmpty ())
	return;
    
    if (autoup)
      {
	  if (openDb (true))	//true= no ask
	    {

		dm->putNewTranslation (orig, translated,
			dm->catalogRef (directory (edited, 0),
			autoauthor, edited));
		//kdDebug(0) << "Changed " << orig << " " << translated << endl;
		dm->sync ();
	    }
      }

}

void
KDBSearchEngine::setEditedFile (const QString & file)
{

    edited = file;		//kdDebug(0) << edited << endl;
}

KeyAndScore::KeyAndScore (const QString & a, int sc):
QString (a)
{
    score = sc;
}

KeyAndScore::KeyAndScore ():QString ()
{
    score = 0;
}

#include "KDBSearchEngine.moc"
