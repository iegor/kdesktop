/*

  DBSE 3
  (c) 2000-2003 Andrea Rizzi
  License: GPLv2 

*/
#ifndef DATABASE_2_H
#define DATABASE_2_H

#include <qstring.h>
#include <qvaluelist.h>
#include <db4/db_cxx.h>
#include <qobject.h>
#include <qregexp.h>

#include "dbentries.h"


class DBSESettings
{
public:
  //DatabaseInterface Settings
  uint scoreWordByWord;
  uint scoreGlossary;
  uint scoreDivide;
  uint scoreExact;
  uint scoreSentence;
  uint scoreAlpha;
  uint scoreDynamic;
  uint scoreChunkByChunk;
  uint minScore;
  bool firstCapital;
  bool allCapital;
  bool accelerator;
  bool sameLetter;
  


};


class DataBase : Db
{
 public:
    DataBase(QString dbpath, QString dbname, QString dblang);
    
    int open(DBTYPE type,unsigned int flags=0);

//Standard access (overload std functions)
    


int del(DBItem * key){
     key->set(); 
     int r = Db::del(0,key,0);
     key->get();
     return r;
     }
    


int get(DBItem * key,DBItem *data){
     key->set(); 
     data->set();
     int r = Db::get(0,key,data,0);
     key->get();
     data->get();
     return r;
     }
    int put(DBItem * key,DBItem *data,int fl=0){
     key->set();
     data->set();
     int r= Db::put(0,key,data,0);
     key->get();
     data->get();
     return r;
    }

    int del(DBItemNum * key){
     int r = Db::del(0,key,0);
     return r;
    }
    
    int get(DBItemNum * key,DBItem *data){
     data->set();
     int r = Db::get(0,key,data,0);
     data->get();
     return r;
    }
    int put(DBItemNum * key,DBItem *data)
    {
     data->set();
     int r= Db::put(0,key,data,0);
     data->get();
     return r;
    }
  
//Overload, using this you loose the Key!!
    int del(int i){DBItemNum n(i);  return del(&n);}  
    int get(int i,DBItem *data){DBItemNum n(i);  return get(&n,data);}
    int put(int i,DBItem *data){DBItemNum n(i);  return put(&n,data);}
	
    unsigned int getLast();
    int close(unsigned int i) {return Db::close( i); }	    

//For scrolling
   // int getFirst(DBItem *key,DBItem *data,QString table);
   // int getNext(DBItem *key,DBItem *data,QString table);
   // bool isEnd(QString table);
private:
    QString filename;
    QString database;
    DBTYPE mytype;  
};





class QueryResult  //from DBInterface to KDBSearchEngine
{
public:
  QueryResult();
  QueryResult(QString r);
  QueryResult(QString r,QString o,int s);
  void setRichResult(QString r) { richr=r;  }
  void setRichOriginal(QString r) { richo=r;  }
  
  QString richResult() {return richr;}
  QString richOriginal() {return richo;}
  
  QString result(){ return res; }
  QString original() {return orig; }
  int score() {return sco;}
//info contains originalkey,catalog,date,author etc...
  ResultInfo info(){ResultInfo i; i.info="no info"; return i;}


private:
   QString res;
   QString orig;
   QString richr;
   QString richo;
   int sco;

};

class SearchFilter
{
  int filter; //placeholder
};



class DataBaseInterface : public QObject
{

 public:
    //move result list typedef to AbstractAlgorthm or somewhere else
    typedef QValueList<QueryResult> ResultList; 
    typedef QPair<DBItemMainKey,DBItemMainData> MainEntry;
    
    DataBaseInterface( QString dir, DBSESettings *sets); 
    ~DataBaseInterface();
    
    //Ask the Database to stop now.
    void stop(bool b=true) {_stopNow=b;}
    
    //Search words
    ResultList wordsMatch(QString query,SearchFilter *filter=0,bool notify=true);
   
    //Edit database functions.
    //addEntry and sync all the tables
    bool addEntry(QString original,QString translated,InputInfo *info);
    //FIXME:implement me!
    bool removeEntry(QString original);
    
    //FIXME: go persistent!
    QMap<QString,float> correlation(QString word,SearchFilter *filter=0,bool notify=true,float minSign=0.2);
    
    
    // Read the database
    MainEntry get(const QString& query,SearchFilter *filter=0);
    MainEntry getFromIndex(uint i);
    DBItemMultiIndex::IndexList getAlpha(const QString& query);
    DBItemMultiIndex::IndexList getWordIndex(const QString& query);
    MainEntry getSentence(const QString& query);
   
    //Database status check functions
     bool mainOk() {return  main!=0;} 
     bool catalogOk() {return  catalog!=0;} 
     bool alphaOk() {return  alpha!=0;} 
     bool sentenceOk() {return  sentence!=0;}
     bool numindexOk() {return  numindex!=0;} 
     bool wordsindexOk() {return  wordsindex!=0;} 
     bool externalOk() {return  external!=0;}
     bool wordOk() {return  word!=0;}
     bool transwordOk() {return  transword!=0;}
     bool correlationOk() {return  corr!=0;}
     bool stopNow() {return _stopNow;}
  
    // signals:
    // void  newResult(QueryResult); 
     
     DBSESettings *getSettings() {return settings;}
     
     private:       
     DataBase * openMyDataBase(const QString& prefix,const QString& name,const QString& l,DBTYPE tt);
     DataBase *main;
     DataBase *numindex;
     DataBase *wordsindex;
     DataBase *catalog;
     DataBase *alpha;
     DataBase *sentence;
     DataBase *word;
     DataBase *external; 
     DataBase *transword;
     DataBase *corr;
     bool _stopNow;
     DBSESettings *settings;
     
     //FIXME:Move to KBabel common library.
     public:     
     QString format( QString _s,QString t);
     QString formatRegExp(QString _s, QString t, QString tre,QString stringSearch,QString stringReplace);
     static QStringList words(QString s);
     static QStringList sentences(QString s);
     static QStringList sentencesSeparator(QString s);
     static QString simple(QString str,bool ck=false);
     static bool isUpper(QChar s);
     static bool isLower(QChar s);
     
 };

#endif
 
