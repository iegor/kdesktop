/***************************************************************************
                          database.cpp  -
                         -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/*
 Translation search engine


  Copyright  2000
  Andrea Rizzi rizzi@kde.org
  Copyright 2003 Stanislav Visnovsky visnovsky@kde.org

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
#include <stdlib.h>
#include <kdebug.h>
#include <string.h>
#include <resources.h>
#include "database.h"
#include <unistd.h>

#include <qfile.h>

#include <ktempfile.h>
#include <kio/netaccess.h>

WordItem::WordItem (char *data, QString w)
{
    word = w;

    count = *(uint32 *) data;

    data += 4;
//score=*(int *)data;
    data += 4;


    locations = (uint32 *) malloc (4 * count);
    memcpy (locations, data, 4 * count);


}

WordItem::WordItem (QString w)
{
    locations = NULL;
    count = 0;
    word = w;
    score = -1;			// it means no references found.
}

/*

WordItem::WordItem(const WordItem &wi)
{
 count=wi.count;
 score=wi.score;
 word=wi.word;
 locations.duplicate(wi.locations);
 locations.detach();
}

WordItem&  WordItem::operator=(const WordItem & wi )
{
WordItem *i=new WordItem(wi);
i->locations.detach();
return *i;
}	
*/

bool
WordItem::notFound ()
{
    return (score == -1);
}

InfoItem::InfoItem ()
{
    catalogName = "No catalog";
    lastTranslator = "No translator";
    lastFullPath = "";
    charset = "No charset";
    language = "No language";
}

InfoItem::InfoItem (const char *rawData, QString lang)
{
    const char *rd;
    rd = rawData;
    int len;
    unsigned int secs;
    // I'll change the charset handling if needed

    charset = "Utf8";

    catalogName = QString::fromUtf8 (rd);
    len = strlen (rd) + 1;
    rd += len;
    lastTranslator = QString::fromUtf8 (rd);
    len = strlen (rd) + 1;
    rd += len;
    secs = *(unsigned int *) rd;
    revisionDate.setTime_t (secs);
    rd += 4;
    lastFullPath = QString::fromUtf8 (rd);
    len = strlen (rd) + 1;
    rd += len;


    language = lang;
}

void
InfoItem::rawData (char *rawData)
{
    char *rd;
    rd = rawData;

    strcpy (rd, catalogName.utf8 ());
    rd += strlen (rd) + 1;
    strcpy (rd, lastTranslator.utf8 ());
    rd += strlen (rd) + 1;


    //QDate Time problem!!!!!!!!!!!
    QDateTime zeroDate;
    zeroDate.setTime_t (0);
    *(unsigned int *) rd = -revisionDate.secsTo (zeroDate);
    rd += 4;
    strcpy (rd, lastFullPath.utf8 ());
    rd += strlen (rd) + 1;

    *rd = 0;			//Empty for further info

}

int
InfoItem::size ()
{
    int _size;
    _size = 0;
    _size += 1;			// 1 Empty field;
    _size += 3;			// Terminating \0 of next 3 strings
    _size += 4;			// Int size (date)
    _size += strlen (catalogName.utf8 ());
    _size += strlen (lastTranslator.utf8 ());
    _size += strlen (lastFullPath.utf8 ());

    return _size;
}

// this is a quick hack to copy a local file
int
copy_hack (QFile & input, QFile & output)
{
    if (!input.isOpen ())
      {
	  if (!input.open (IO_ReadOnly))
	      return -1;
      }

    if (!output.isOpen ())
      {
	  if (!output.open (IO_WriteOnly))
	      return -1;
      }

    char buffer[10240];
    int s = 0;
    while (!input.atEnd ())
      {
	  s = input.readBlock (buffer, 10240);
	  output.writeBlock (buffer, s);
      }
    output.close ();
    input.close ();
    return 0;
}

DataBaseItem::DataBaseItem ()
{
    numTra = 0;
    location = 0;
}

DataBaseItem::DataBaseItem (char *_key, char *_data)
{

    char *data = _data;
    key = QString::fromUtf8 (_key);

    unsigned int i, r;
    numTra = *(uint32 *) data;
    data += 4;
    location = *(uint32 *) data;
    data += 4;

    for (i = 0; i < numTra; i++)
      {

	  TranslationItem tr;
	  tr.numRef = *(uint32 *) data;

	  data += 4;
	  for (r = 0; r < tr.numRef; r++)
	    {

		int ref;
		ref = *(uint32 *) data;
		data += 4;

		tr.infoRef.append (ref);
	    }
	  tr.translation = QString::fromUtf8 ((const char *) data);
	  translations.append (tr);
	  data += strlen (data) + 1;

      }

}

uint32
DataBaseItem::sizeKey ()
{
    return strlen (key.utf8 ()) + 1;
}

uint32
DataBaseItem::sizeData ()
{
    unsigned int i, _size = 4;
    _size += numTra * 4;
    _size += 4;			// location
    for (i = 0; i < numTra; i++)
      {
	  _size += strlen (translations[i].translation.utf8 ()) + 1;	// +1 is for \0
	  _size += translations[i].numRef * 4;
      }
    return _size;
}

void
DataBaseItem::toRawKey (char *_key)
{
    strcpy (_key, key.utf8 ());
}

void
DataBaseItem::toRawData (char *_data)
{
    char *data = _data;
    unsigned int i, r;

    *(uint32 *) data = numTra;

    data += 4;

    *(uint32 *) data = location;
    data += 4;

    for (i = 0; i < numTra; i++)
      {
	  TranslationItem tr (translations[i]);
	  *(uint32 *) data = tr.numRef;
	  data += 4;
	  for (r = 0; r < tr.numRef; r++)
	    {
		*(uint32 *) data = tr.infoRef[r];	//sub i with r

		data += 4;
	    }
	  strcpy ((char *) data, tr.translation.utf8 ());
	  data += strlen (tr.translation.utf8 ()) + 1;
      }

}


DataBaseManager::DataBaseManager (QString directory, QString lang,
				  QObject * parent, const char *name):
QObject (parent, name)
{
    QString filename;

    language = lang;
    iAmOk = true;
    basedir = directory;
    indexDb = wordDb = infoDb = db = 0;
    openDataBase ();


}

void
DataBaseManager::openDataBase ()
{
    kdDebug () << "Opendatabase" << endl;
    QString directory;
    directory = basedir;
    QString ll = "." + language;
    if (ll == ".")
	ll = ".NOLANG";

    QString transfilename = "%1/translations%2.db";
    transfilename = transfilename.arg (directory).arg (ll);

    QString infofilename = "%1/catalogsinfo%2.db";
    infofilename = infofilename.arg (directory).arg (ll);

    QString wordsfilename = "%1/wordsindex%2.db";
    wordsfilename = wordsfilename.arg (directory).arg (ll);

    QString keysfilename = "%1/keysindex%2.db";
    keysfilename = keysfilename.arg (directory).arg (ll);

    cursor = 0;
    int ret;

    if (!db)
	db_create (&db, 0, 0);

    db_create (&infoDb, 0, 0);
    db_create (&wordDb, 0, 0);
    db_create (&indexDb, 0, 0);

    ret = db->open (db,
#if DB_VERSION_MINOR > 0
		    NULL,
#endif
		    transfilename.local8Bit (), 0, DB_BTREE, 0, 0644);

    if (ret == DB_OLD_VERSION)
      {
	  kdDebug (KBABEL_SEARCH) << "Trying upgrade" << endl;
	  // try upgrade

	  KTempFile transFile, infoFile, keysFile, wordsFile;

	  // do the upgrade on the translation file
	  QFile transfilenameFile (transfilename);

	  if ((ret = copy_hack (transfilenameFile, *transFile.file ())) == 0)
	    {
		ret = db->upgrade (db, transFile.name ().local8Bit (), 0);
	    }

	  if (ret != 0)
	    {
		kdDebug (KBABEL_SEARCH) << "Cannot upgrade translations, " <<
		    ret << endl;
		// cleanup
		transFile.unlink ();
		iAmOk = false;
		emit cannotOpenDB (ret);
		return;
	    }

	  // do the upgrade on the info file
	  QFile infofilenameFile (infofilename);
	  if ((ret = copy_hack (infofilenameFile, *infoFile.file ())) == 0)
	    {
		ret =
		    infoDb->upgrade (infoDb, infoFile.name ().local8Bit (),
				     0);
	    }

	  if (ret != 0)
	    {
		kdDebug (KBABEL_SEARCH) << "Cannot upgrade catalogsinfo" <<
		    endl;
		// cleanup
		transFile.unlink ();
		infoFile.unlink ();
		iAmOk = false;
		emit cannotOpenDB (ret);
		return;
	    }

	  // do the upgrade on the words file
	  QFile wordfilenameFile (wordsfilename);
	  if ((ret = copy_hack (wordfilenameFile, *wordsFile.file ())) == 0)
	    {
		ret =
		    wordDb->upgrade (wordDb, wordsFile.name ().local8Bit (),
				     0);
	    }

	  if (ret != 0)
	    {
		kdDebug (KBABEL_SEARCH) << "Cannot upgrade words" << endl;
		// cleanup
		transFile.unlink ();
		infoFile.unlink ();
		wordsFile.unlink ();
		iAmOk = false;
		emit cannotOpenDB (ret);
		return;
	    }

	  // do the upgrade on the keys file
	  QFile keysfilenameFile (keysfilename);
	  if ((ret = copy_hack (keysfilenameFile, *keysFile.file ())) == 0)
	    {
		ret =
		    indexDb->upgrade (indexDb, keysFile.name ().local8Bit (),
				      0);
	    }

	  if (ret != 0)
	    {
		kdDebug (KBABEL_SEARCH) << "Cannot upgrade keys" << endl;
		// cleanup
		transFile.unlink ();
		infoFile.unlink ();
		wordsFile.unlink ();
		keysFile.unlink ();
		iAmOk = false;
		emit cannotOpenDB (ret);
		return;
	    }

	  kdDebug (KBABEL_SEARCH) << "Files upgraded, copying" << endl;
	  // use temporary file instead
	  if (ret == 0)
	    {
		KIO::NetAccess::del (KURL::fromPathOrURL (transfilename));
		copy_hack (*transFile.file (), transfilenameFile);
		transFile.unlink ();

		KIO::NetAccess::del (KURL::fromPathOrURL (infofilename));
		copy_hack (*infoFile.file (), infofilenameFile);
		infoFile.unlink ();

		KIO::NetAccess::del (KURL::fromPathOrURL (wordsfilename));
		copy_hack (*wordsFile.file (), wordfilenameFile);
		wordsFile.unlink ();

		KIO::NetAccess::del (KURL::fromPathOrURL (keysfilename));
		copy_hack (*keysFile.file (), keysfilenameFile);
		keysFile.unlink ();

		ret = db->open (db,
#if DB_VERSION_MINOR > 0
				NULL,
#endif
				transfilename.local8Bit (), 0, DB_BTREE, 0,
				0644);
		if (ret != 0)
		  {
		      kdWarning (KBABEL_SEARCH) <<
			  "transFilename database can't be opened." << endl;
		      kdWarning (KBABEL_SEARCH) <<
			  "Please, report this incident and how to reproduce it to kbabel@kde.org."
			  << endl;
		      iAmOk = false;
		      emit cannotOpenDB (ret);
		      return;
		  }

	    }
	  kdDebug (KBABEL_SEARCH) << "Upgrade done OK" << endl;
      }

// Open catalogs information database



    ret = infoDb->open (infoDb,
#if DB_VERSION_MINOR > 0
			NULL,
#endif
			infofilename.local8Bit (), 0, DB_RECNO, 0, 0644);
    if (ret != 0)
      {
	  iAmOk = false;
	  emit cannotOpenDB (ret);
	  //Process error here.
      }
    else
	loadInfo ();



// Words index database

    ret = wordDb->open (wordDb,
#if DB_VERSION_MINOR > 0
			NULL,
#endif
			wordsfilename.local8Bit (), 0, DB_BTREE, 0, 0644);
    if (ret != 0)
      {
	  iAmOk = false;
	  emit cannotOpenDB (ret);
	  //Process error here.
      }

//Index of keys.

    ret = indexDb->open (indexDb,
#if DB_VERSION_MINOR > 0
			 NULL,
#endif
			 keysfilename.local8Bit (), 0, DB_RECNO, 0, 0644);
    if (ret != 0)
      {
	  iAmOk = false;
	  emit cannotOpenDB (ret);
	  //Process error here.
      }


}

void
DataBaseManager::closeDataBase ()
{
    if (iAmOk)
      {
	  db->sync (db, 0);
	  db->close (db, 0);

	  infoDb->sync (infoDb, 0);
	  infoDb->close (infoDb, 0);

	  wordDb->sync (wordDb, 0);
	  wordDb->close (wordDb, 0);

	  indexDb->sync (indexDb, 0);
	  indexDb->close (indexDb, 0);

	  // can not be opened again
	  indexDb = wordDb = infoDb = db = 0;

      }

}


// I'm not sure this is a good function !!!

void
DataBaseManager::sync ()
{
// if(iAmOk)  
// { 
//  db->sync(db,0);
//  infoDb->sync(infoDb,0);
//  cursor=0;
// }

    // closeDataBase();
// openDataBase();

    db->sync (db, 0);
    infoDb->sync (infoDb, 0);
    wordDb->sync (wordDb, 0);
    indexDb->sync (indexDb, 0);
    loadInfo ();
}


DataBaseManager::~DataBaseManager ()
{
    closeDataBase ();
}

int
DataBaseManager::putItem (DataBaseItem * item, bool ow)
{
    DBT key, data;

    memset (&key, 0, sizeof (DBT));
    memset (&data, 0, sizeof (DBT));

    bool ret;

    uint32 loc = 0;
    if (item->location == 0)
      {
	  loc = item->location = appendKey (item->key);
// kdDebug(0) << "New key " << item->location <<  endl;
      }
    key.size = item->sizeKey ();
    data.size = item->sizeData ();

    key.data = malloc (key.size);
    data.data = malloc (data.size);


    item->toRawKey ((char *) key.data);
    item->toRawData ((char *) data.data);


    if (ow)
	ret = db->put (db, 0, &key, &data, 0);
    else
	ret = db->put (db, 0, &key, &data, DB_NOOVERWRITE);

//check ret

    if (loc != 0)		//I'm new!
      {
	  uint32 location = loc;

	  QValueList < QString > wlist;

	  wlist = wordsIn (item->key);

	  QValueList < QString >::Iterator wlistit;

	  for (wlistit = wlist.begin (); wlistit != wlist.end (); ++wlistit)
	    {
		addLocation (*wlistit, location);
	    }

      }

    free (key.data);
    free (data.data);		//READ DOCU !!!!

    return ret;

}

DataBaseItem
DataBaseManager::getItem (QString key)
{
    if (!iAmOk)
	return DataBaseItem ();


    DBT _key, data;

    memset (&_key, 0, sizeof (DBT));

    memset (&data, 0, sizeof (DBT));

    int len = strlen (key.utf8 ());
    _key.data = malloc (len + 1);
    _key.size = len + 1;
    strcpy ((char *) _key.data, key.utf8 ());


    int ret;
    ret = db->get (db, 0, &_key, &data, 0);

    if (ret != 0)
      {
	  free (_key.data);
	  return DataBaseItem ();	//return an empty database item 
      }

    DataBaseItem returnItem =
	DataBaseItem ((char *) _key.data, (char *) data.data);

    free (_key.data);
    return returnItem;

}




DataBaseItem
DataBaseManager::cursorGet (uint32 flags)
{

    if (!iAmOk)
	return DataBaseItem ();
    int re;
    DBT key, data;

    memset (&key, 0, sizeof (DBT));

    memset (&data, 0, sizeof (DBT));

    if (cursor == 0)
	re = db->cursor (db, 0, &cursor, 0);

    int ret;
    if ((ret = cursor->c_get (cursor, &key, &data, flags)) == 0)
      {
	  return DataBaseItem ((char *) key.data, (char *) data.data);
      }
    else
      {
	  kdDebug (KBABEL_SEARCH) << QString ("...cursor getting...%1").
	      arg (ret) << endl;

	  return DataBaseItem ();
      }
}


DataBaseItem
DataBaseManager::firstItem ()
{
    return cursorGet (DB_FIRST);
}

DataBaseItem
DataBaseManager::currentItem ()
{
    return cursorGet (DB_CURRENT);
}


DataBaseItem
DataBaseManager::nextItem ()
{
    return cursorGet (DB_NEXT);
}


bool
DataBaseManager::isOk ()
{
    return iAmOk;
}

int
DataBaseManager::count ()
{
    DB_BTREE_STAT *dstat = 0;
#if DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 3
    db->stat (db, NULL, &dstat, DB_FAST_STAT);
#else
    db->stat (db, &dstat, DB_FAST_STAT);
#endif
    int ret = dstat->bt_nkeys;
    free (dstat);

    return ret;
}

int
DataBaseManager::current ()
{
// THIS FUNCTION SEEM TO NOT WORK (not used)
    if (!iAmOk)
	return 0;

    DBT key, data;
    memset (&key, 0, sizeof (DBT));
    memset (&data, 0, sizeof (DBT));

    if (cursor != 0)
	db->cursor (db, 0, &cursor, 0);
    cursor->c_get (cursor, &key, &data, DB_GET_RECNO);
    return *(uint32 *) (data.data);

}

int
DataBaseManager::createDataBase (QString directory,
				 QString language, int mode)
{
    QString filename;
    QString ll = "." + language;
    if (ll == ".")
	ll = ".NOLANG";
    filename = "%1/translations%2.db";
    filename = filename.arg (directory).arg (ll);

    rename (filename.local8Bit (), filename.local8Bit () + ",old");

//kdDebug(0) << QString("Creating %1").arg(filename) << endl;

    iAmOk = true;

    int ret;

    if (!db)
      {
	  if (db_create (&db, 0, 0) != 0)
	    {
		kdDebug() << "db_create db failed" << endl;
		iAmOk = false;
		return false;
	    }
      }

    db->set_flags (db, DB_RECNUM);
    ret = db->open (db,
#if DB_VERSION_MINOR > 0
		    NULL,
#endif
		    filename.local8Bit (), 0, DB_BTREE, DB_CREATE | DB_EXCL,
		    mode);
    if (ret != 0)
      {
	  kdDebug() << "db->open " << filename << " " << mode << " failed" << endl;
	  iAmOk = false;
      }


    filename = "%1/catalogsinfo%2.db";
    filename = filename.arg (directory).arg (ll);
    rename (filename.local8Bit (), filename.local8Bit () + ",old");

    db_create (&infoDb, 0, 0);
    ret = infoDb->open (infoDb,
#if DB_VERSION_MINOR > 0
			NULL,
#endif
			filename.local8Bit (), 0, DB_RECNO, DB_CREATE, mode);
    if (ret != 0)
	iAmOk = false;



    filename = "%1/wordsindex%2.db";
    filename = filename.arg (directory).arg (ll);
    rename (filename.local8Bit (), filename.local8Bit () + ",old");

    db_create (&wordDb, 0, 0);
    ret = wordDb->open (wordDb,
#if DB_VERSION_MINOR > 0
			NULL,
#endif
			filename.local8Bit (), 0, DB_BTREE, DB_CREATE, mode);
    if (ret != 0)
	iAmOk = false;



    filename = "%1/keysindex%2.db";
    filename = filename.arg (directory).arg (ll);
    rename (filename.local8Bit (), filename.local8Bit () + ",old");

    db_create (&indexDb, 0, 0);
    ret = indexDb->open (indexDb,
#if DB_VERSION_MINOR > 0
			 NULL,
#endif
			 filename.local8Bit (), 0, DB_RECNO, DB_CREATE, mode);
    if (ret != 0)
	iAmOk = false;



    if (iAmOk)
	loadInfo ();
    else
	kdDebug (KBABEL_SEARCH) << QString ("I am NOT  ok : %1").
	    arg (ret) << endl;

//THIS IS WRONG, rewrite the error handling. 
    return iAmOk;

}

InfoItem
DataBaseManager::getCatalogInfo (int n)
{

    DBT key;
    DBT data;

    memset (&key, 0, sizeof (DBT));
    memset (&data, 0, sizeof (DBT));

    key.data = &n;
    key.size = 4;

//Check for errors
    int ret = infoDb->get (infoDb, 0, &key, &data, 0);	//DB_SET_RECNO);

    if (ret)
      {
	  return InfoItem ();
      }

// kdDebug(0) << QString("Trad %1").arg(ret) << endl;

    InfoItem it ((char *) data.data, language);
//free(data.data);  // Read docu for this!!!!

    return it;

}

int
DataBaseManager::addCatalogInfo (InfoItem * catInfo, int cat = -1)
{
    DBT data;
    DBT key;

    // clean up data
    memset (&data, 0, sizeof (DBT));
    memset (&key, 0, sizeof (DBT));

    int ret = 0, err;
    if (cat >= 0)
	ret = cat;
    key.size = 4;
    key.data = &ret;
    data.size = catInfo->size ();
    data.data = malloc (data.size);

    catInfo->rawData ((char *) data.data);

    // store the catalog data into database
    if (cat >= 0)
	err = infoDb->put (infoDb, 0, &key, &data, 0);
    else
	err = infoDb->put (infoDb, 0, &key, &data, DB_APPEND);


    ret = *(int *) key.data;

    // Append to the list of catalogInfo
    info.append (*catInfo);

    // cleanup unneeded data memory
    free (data.data);
    return ret;
}

int
DataBaseManager::searchCatalogInfo (QString location)
{
    int n = 0;
    QValueList < InfoItem >::Iterator it;
    for (it = info.begin (); it != info.end (); ++it)
      {
	  n++;
	  if ((*it).catalogName == location)
	      return n;
      }
    return -1;
}

bool
DataBaseManager::putCatalogInfo (int refnum, InfoItem * catInfo)
{
    DBT data;
    DBT key;

    memset (&key, 0, sizeof (DBT));
    memset (&data, 0, sizeof (DBT));

    int ret;
    key.size = 4;
    key.data = &refnum;

    data.size = catInfo->size ();
    data.data = malloc (data.size);
    catInfo->rawData ((char *) data.data);

    ret = infoDb->put (infoDb, 0, &key, &data, 0);

    free (data.data);

    return (ret == 0);
}

void
DataBaseManager::loadInfo ()
{
    int nrec;
    DB_BTREE_STAT *stat;
// memset(&stat,0,sizeof(DB_BTREE_STAT));
#if DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 3
    if (infoDb->stat (infoDb, NULL, &stat, DB_FAST_STAT))
	fprintf (stderr, "Cannot stat\n");
#else
    if (infoDb->stat (infoDb, &stat, DB_FAST_STAT))
	fprintf (stderr, "Cannot stat\n");
#endif
    nrec = stat->bt_nkeys;
    free (stat);

    info.clear ();
    for (int i = 1; i <= nrec; i++)	// I think DB2 Recno are 1 based.
      {
	  info.append (getCatalogInfo (i));
      }

}


QValueList < QString > DataBaseManager::wordsIn (QString string)
{
    QString
	a;
    QValueList < QString > words;
    int
	i,
	l;

    a = string.simplifyWhiteSpace ();
    a = a.stripWhiteSpace ();
    a = a.lower ();
    l = a.length ();

    int
	c = 0;
    //words.setAutoDelete(true);  //Not sure... check if it is en.
    QString
	m;
    for (i = 0; i < l; i++)
	if (a[i].isLetterOrNumber ())
	  {
	      m += a[i];
	  }
	else if (a[i].isSpace ())
	  {
	      words.append (m);
	      c++;		//  C++  ;-)
	      m = "";
	  }
    words.append (m);

    return words;
}



WordItem
DataBaseManager::getWordLocations (QString word)
{

    QString keystring = word.lower ();

    DBT key;
    DBT data;

    char *keydata;

    int intlen = strlen (keystring.utf8 ());

    keydata = (char *) malloc (intlen + 1);
    strcpy (keydata, keystring.utf8 ());

    memset (&key, 0, sizeof (DBT));
    memset (&data, 0, sizeof (DBT));

    key.data = keydata;
    key.size = intlen + 1;

//Check for errors
    int ret = wordDb->get (wordDb, 0, &key, &data, 0);	//DB_SET_RECNO);
//MAYBE THIS WORD IS NO WHERE!!
    if (ret)
      {
	  free (keydata);
	  return WordItem (keystring);
      }


    WordItem it ((char *) data.data, keystring);

    free (keydata);


//  kdDebug(0) << ((uint32 *)it.locations.data())[0] << endl;

    return it;

}


bool
DataBaseManager::addLocation (QString word, unsigned int location)
{

    QString keystring = word.lower ();


    DBT key;
    DBT data;

    char *keydata;
    char *newdata;

    int intlen = strlen (keystring.utf8 ());

    keydata = (char *) malloc (intlen + 1);
    strcpy (keydata, keystring.utf8 ());

    memset (&key, 0, sizeof (DBT));
    memset (&data, 0, sizeof (DBT));

    key.data = keydata;
    key.size = intlen + 1;

    strcpy ((char *) key.data, keystring.utf8 ());

//Check for errors
    int ret = wordDb->get (wordDb, 0, &key, &data, 0);	//DB_SET_RECNO);

//Check if you get something or not
    if (ret == 0)		// CHANGE IT!!!!! if found something
      {
	  uint32 *d;
	  d = (uint32 *) data.data;
	  uint32 num = d[0];
	  uint32 loc = 0;	//Position from 0 to num-1
	  int totalrecord = count ();
	  uint32 step = 1;

	  int antibounce = 0;
//d+=4;
//int score=d[1];
//d+=4; 
	  bool forward, end = false;

	  d[1] = 0;		//Before the begin...  

	  d += 2;		//1 uint32!  


//Try to guess...
	  loc = location * num / totalrecord + 1;
	  if (loc >= num)
	      loc = num - 1;




//Continue if next is smaller or if previous is greater
//before the while check if it is ok
	  if (loc == 0)
	    {
		if (d[loc] > location)
		    end = true;
		else
		    loc = 1;
	    }

	  if ((loc == num) && !end)
	    {
		if (d[loc - 1] < location)
		    end = true;
		else
		    loc = num - 1;
	    }



	  while ((!end) && ((forward = (d[loc]) < location)
			    || ((loc > 0) && (d[loc - 1] > location))))
	    {

		antibounce++;
		//calculate step or use antibounce 
		if (abs ((int) d[loc] - (int) location) < 50
		    || antibounce > 100)
		    step = 1;	//Go linear...
		else
		  {
		      step =
			  (abs (d[loc] - location) * num) / totalrecord + 1;

		  }

		kdDebug (KBABEL_SEARCH) << "Insert:" << location <<
		    " We are at: " << loc << " i.e. " << d[loc] << "  Step:"
		    << step << endl;
		if (loc > 0)
		  {
		      if (forward)
			  kdDebug (KBABEL_SEARCH) << "Go " << "forward" <<
			      " prev is " << d[loc - 1] << endl;
		      else
			  kdDebug (KBABEL_SEARCH) << "Go " << "backward" <<
			      " prev is " << d[loc - 1] << endl;
		  }

		if (forward)
		  {
		      if (loc + step < num)
			  loc += step;	// Go forward
		      else
			  loc = num;	// Go to num          
		  }
		else
		  {
		      if (loc > step)
			  loc -= step;	//Go backward
		      else
			  loc = 0;	// Go to 0
		  }

		//Now check if I am in the right place [THIS IS NOT NECESSARY]

		//check if loc and loc -1 are well defined 1<loc<num-1
		if (loc > num)
		    loc = num;	//Must not happen,       idem

		if (loc == 0)
		  {
		      if (d[loc] > location)
			  end = true;
		      else
			  loc = 1;
		  }

		if ((loc == num) && !end)
		  {
		      if (d[loc - 1] < location)
			  end = true;
		      else
			  loc = num - 1;
		  }

	    }

	  if (loc == num)
	      kdDebug (KBABEL_SEARCH) << "END" << endl;
	  if (loc == 0)
	      kdDebug (KBABEL_SEARCH) << "BEGIN" << endl;

	  if (loc > 0)
	      kdDebug (KBABEL_SEARCH) << location << " inserted between " <<
		  d[loc - 1] << " and " << d[loc] << endl;

	  if ((loc < num && location == d[loc]) || (loc > 0 && location == d[loc - 1]))	//What about word repetition
	    {
		free (keydata);
		return true;	//Why true ??
	    }



//Now insert and put back in the database!
	  newdata = (char *) malloc (4 * (num + 3));	//uint32*(num+score+1..NUM+new)
	  memcpy (newdata, data.data, 4 + 4 + 4 * loc);
	  char *secondpart = (char *) data.data;
	  secondpart += 4 * (loc + 2);
	  memcpy ((newdata + 4 * (loc + 3)), secondpart, (num - loc) * 4);
	  uint32 *intdata = (uint32 *) newdata;
	  intdata[0] = num + 1;
	  //ADD HERE code to recalc score
	  intdata[loc + 2] = location;

//ok send it to database!
	  memset (&data, 0, sizeof (DBT));

	  data.data = newdata;
	  data.size = 4 * (3 + num);
      }				//found sounthing
    else
      {				//found nothing
	  newdata = (char *) malloc (4 * 3);
	  uint32 *intdata = (uint32 *) newdata;
	  intdata[0] = 1;
	  intdata[1] = 1;
	  intdata[2] = location;
	  memset (&data, 0, sizeof (DBT));
	  data.data = newdata;
	  data.size = 4 * 3;
      }

    memset (&key, 0, sizeof (DBT));
//memset(&data,0,sizeof(DBT));

    key.data = keydata;
    key.size = intlen + 1;

    ret = wordDb->put (wordDb, 0, &key, &data, 0);	//DB_SET_RECNO);


    free (newdata);
    free (keydata);
//return it;

    return true;
}


bool
DataBaseManager::removeLocation (QString /*word */ , int /*location */ )
{

//#warning TODO: REMOVE LOCATION
    return true;

}

uint32
DataBaseManager::appendKey (QString _key)
{

    DBT key;
    DBT data;

    memset (&key, 0, sizeof (DBT));
    memset (&data, 0, sizeof (DBT));


    uint32 ret = 0, err;
    key.size = 4;
    key.data = &ret;

    data.size = strlen (_key.utf8 ()) + 1;
    data.data = malloc (data.size);

    strcpy ((char *) data.data, _key.utf8 ());

    err = indexDb->put (indexDb, 0, &key, &data, DB_APPEND);

    if (err)
	ret = 0;
    else
	ret = *(uint32 *) key.data;

//kdDebug(0) << QString("Append result %1,err = %1").arg(ret).arg(err) << endl;


    free (data.data);

    return ret;

}

QString
DataBaseManager::getKey (uint32 n)
{

    DBT key;
    DBT data;

    memset (&key, 0, sizeof (DBT));
    memset (&data, 0, sizeof (DBT));

    key.data = &n;
    key.size = 4;

//Check for errors
    int ret = indexDb->get (indexDb, 0, &key, &data, 0);	//DB_SET_RECNO);
    if (ret)
	return QString::null;

    return QString::fromUtf8 ((char *) data.data);

// kdDebug(0) << QString("Trad %1").arg(ret) << endl;

}

int
DataBaseManager::catalogRef (QString location, QString author, QString path)
{
    InfoItem cinfo;
    int cat, catnum;
    cat = searchCatalogInfo (location);

    if (cat == -1)		//Not exist
      {
	  cinfo.catalogName = location;
	  cinfo.lastTranslator = author;
	  cinfo.lastFullPath = path;
	  //TO DO:
	  // //Add date info

	  kdDebug (0) << "New catalog " << endl;
	  catnum = addCatalogInfo (&cinfo);
	  //sync();
	  kdDebug (0) << "Ref " << catnum << endl;
      }
    else
      {
	  cinfo = getCatalogInfo (cat);
	  //Update date.

	  //last path must be updated
	  cinfo.lastFullPath = path;
	  kdDebug (0) << "New full path " << path << endl;
	  kdDebug (0) << "Ref " << cat << endl;

	  catnum = addCatalogInfo (&cinfo, cat);
	  kdDebug (0) << " must be equal to  " << catnum << endl;

	  catnum = cat;
      }

    return catnum;
}

int
DataBaseManager::putNewTranslation (QString key, QString tran, int catalog,
				    bool ow)
{
    int catnum = catalog;
    int count = 0;
    QString msgid = key;
    DataBaseItem dbit = getItem (msgid);

    if (dbit.numTra == 0)	//Not found
    {
	dbit.numTra += 1;
	  
	// use local variable, dbit.translations is QValueList and
	// will create own copy
	TranslationItem tra;
	tra.numRef = 1;
	tra.translation = tran;
	tra.infoRef.append (catnum);
	dbit.translations.append (tra);
	dbit.key = key;

	//Check ret value
	count++;

	int aa = putItem (&dbit);
	if (aa)
	    kdDebug (0) << QString ("-----------put code ") << aa << endl;
    }
    else
    {
    	// key exists

	QString msgstr = tran;
	bool found_catalog_info = false, foundTr = false, isThisOne = false;

	QValueList < TranslationItem >::Iterator ittr;
	bool rem = false;
	
	// check all translations in the list
	for (ittr = dbit.translations.begin ();
	    ittr != dbit.translations.end (); rem ? ittr : ++ittr)
	{
	    rem = false;
	    found_catalog_info = false;

	    // is the translation one we should put there?
	    isThisOne = (*ittr).translation == msgstr;

	    // is there the catnum we are looking for?
	    if ((*ittr).infoRef.find (catnum) != (*ittr).infoRef.end ())
	    {
		found_catalog_info = true;
		if (ow && !isThisOne)	
		{
		    // I'll look for my catalog reference to del old
		    kdDebug (0) << "Removing the old translation " << endl;
		    (*ittr).numRef -= 1;
		    (*ittr).infoRef.remove (catnum);
		    if ((*ittr).numRef == 0)
		    {
			dbit.numTra -= 1;
			// point the iterator to the next valid item
			ittr = dbit.translations.remove (ittr);
			rem = true;
		    }
		}
	    }

	    if (isThisOne)
	    {
		if (!found_catalog_info)	
		{
		    //There are no reference of this catalog for this translation => add it
		    (*ittr).infoRef.append (catnum);
		    (*ittr).numRef += 1;
		}
		foundTr = true;	// Ok, we found this translation, no need to add it.
	    }
	}

	if (!foundTr)		//This translation is new => Add it !
	{
	    count++;
	    TranslationItem tra;
	    tra.numRef = 1;
	    tra.translation = msgstr;
	    tra.infoRef.append (catnum);

	    dbit.translations.append (tra);
	    dbit.numTra += 1;
	}

	//put the new item in database overwriting the old one.
	//Check ret value
	int aa = putItem (&dbit, true);
	if (aa)
	    kdDebug (0) << QString ("-----------put code ") << aa << endl;

    }
    return count;
}


void
DataBaseManager::rebuildIndexes ()
{
//  uint32 loc;
#if 0
//Reset the 2 databases here.

    while (0 /*browse keys here */ )
      {
	  loc = item->location = appendKey (item->key);

	  uint32 location = loc;

	  QValueList < QString > wlist;

	  wlist = wordsIn (item->key);

	  QValueList < QString >::Iterator wlistit;

	  for (wlistit = wlist.begin (); wlistit != wlist.end (); ++wlistit)
	    {
		addLocation (*wlistit, location);
	    }

      }
#endif
}



#include "database.moc"
