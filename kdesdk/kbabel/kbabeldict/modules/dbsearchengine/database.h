/***************************************************************************
                          database.h  - 
                             -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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


#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <config.h>

#ifdef USE_DB_H_PATH
#include USE_DB_H_PATH
#else
#ifdef HAVE_DB4_DB_H
#include <db4/db.h>
#else
#include <db.h>
#endif
#endif

#define uint32 u_int32_t

#include <qvaluelist.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qobject.h>
#include <qdatetime.h>
#include <qcstring.h>  //bytearray


class WordItem   // Returned by getWordLocations
{
 public:
  
  WordItem(char *data,QString w);
  WordItem(QString w);
  //WordItem(const WordItem &wi);
  //WordItem& operator=(const WordItem & wi );
       	 
  //The word (key in database)
  QString word;
  //Sorted locations
  //QByteArray locations;  //too many problems with this..
  //NOTE:
  //This is allocated only if you call WordItem(char *data,QString w);
  //YOU MUST FREE IT when you do not need it anymore
  //No destructor will do it !!!

  uint32 *locations; 
  
  
  uint32 count;
  //Is this word common ? 
  int score;
  
  bool notFound();
};


class InfoItem
{
 public:

  //Create the NO INFO item 
  InfoItem();
  
  // Create an info item from raw data
  InfoItem(const char *rawData,QString lang);

  QString catalogName;  
  QString lastFullPath;   

  QString lastTranslator;
  QDateTime revisionDate;
  QString charset;
  QString language;

  
  int size();
  void rawData(char *);
  
};

class TranslationItem
{
 public:
  QString translation;
  QValueList<int> infoRef;
  uint32 numRef;
 
};

class DataBaseItem 
{
 public:
  /*
    Create empty class;
  */
  DataBaseItem();
 
  /*
    Create a DataBaseItem from raw data
  */
  DataBaseItem(char *_key,char *_data);

  /*
    return the size (in raw data) of this item.
  */
  uint32 sizeData();
  uint32 sizeKey();
 
  bool isNull() { return (numTra==0); }
 
  /*
    You MUST allocate data of sizeData() byte.
  */
  void toRawData(char *_data);
  void toRawKey(char *_key);

  QString key;
  QValueList<TranslationItem> translations;
  uint32 numTra;
  uint32 location; 
};

class DataBaseManager : public QObject
{

  Q_OBJECT

    public:
  /*
    Main constructor.
    directory is the name of the directory where the database structre
    is stored.
   
    DIRECTORY/translations.$LANG.db       The trasnaltion archive 
    DIRECTORY/catalogsinfo.$LANG.db      Info about catalogs

    //Not yet implemented
    DIRECTORY/wordsindex.$LANG.db      Index of words
     
   
  */
 
  DataBaseManager(QString directory,QString lang,QObject *parent=0,const char *name=0);
  ~DataBaseManager();
  /*
    Create a new databse structure.
   
  */
  int createDataBase(QString directory,QString language,int mode=0664);




  /*
    Put a DataBaseItem into the database.
  
    if ow==false enter the new DataBaseItem only
    if the key of DataBaseItem does not previously exist  

    It also update the wordIndex.

  */
 
  int putItem(DataBaseItem *item,bool ow=false);

  DataBaseItem getItem(QString key);

  /*
    @return the first entry in the database.
  */
 
  DataBaseItem firstItem(); 
 
  /*
    @return the current entry of the database.
  */
 
  DataBaseItem currentItem(); 

  /*
    @return the next entry in the database.
  */

  DataBaseItem nextItem(); 
 

  /*
   * Add a new translation to the database
   * catalog is a valid catalog refnum (use catalogRef to get one)
   * if ow is true a translation of a key coming from catalog is
   * overwritten if you provide a new translation
   */

  int putNewTranslation(QString key,QString tran,int catalog,bool ow=true);
  
  /*
    @return info about catalog n
  */
 
  InfoItem getCatalogInfo(int n);
 
  /*
    Add an entry to catalogsinfo database and
    @return a refnum for the new added item 
  */
 
  int addCatalogInfo(InfoItem *catInfo,int);
 
  /*
    Search an Item with the same "location" and 
    @return its refnum.
  */

  int searchCatalogInfo(QString location);
 
  /* Get a catalog info for location,
   * if it doesn't exist it will create one.
   * @return the refnum
   */

  int catalogRef(QString location,QString author,QString path);
   
  /*
    Put at refnum the catInfo
    @return true if everything is OK
  */

  bool putCatalogInfo(int refnum, InfoItem *catInfo); 

  /*
    Get word info 
  */
  WordItem getWordLocations(QString word);
 
  /*
    Add a location for word
  */
  bool addLocation(QString word, unsigned int location);

  /*
    Remove location for word
  */
  bool removeLocation(QString word, int location);


  /*
   * Rebuild location and word indexes
   */
  void rebuildIndexes();
  
  uint32 appendKey(class QString);
 
  QString getKey(uint32 n);

  /*
    Load the catalogs info.
  */
  void loadInfo();


  void sync();

  bool isOk();
  int count();
  int current();

  void openDataBase();
  void closeDataBase();

  static  QValueList<QString> wordsIn(QString string);

 signals:

  void cannotOpenDB(int);


 protected:
  DataBaseItem cursorGet(uint32 flags); 
 
  QString language;
  QString basedir;
  QValueList<InfoItem> info; 
 
  DB *db,*infoDb,*wordDb,*indexDb;
  DBC *cursor;
  bool iAmOk; 
  bool indexOk;  //Database could word without word index
};





#endif
