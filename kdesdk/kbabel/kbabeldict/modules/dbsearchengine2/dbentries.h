#ifndef DBENTRIES_H
#define DBENTRIES_H

#include <db4/db_cxx.h>
#include <qstring.h>
#include <qbuffer.h>
#include <qmap.h>
#include <qstringlist.h>
#include <qdatetime.h>

class CatalogInfo
{

 QString author;
 QDateTime datetime;
 QString filename;
 QString filepath;


};

class ResultInfo
{
public:
//Common info
 QString original;  //placeholder

//
//Multi reference info
 QValueList<CatalogInfo> catalogs;

 QString info;
 
};

class InputInfo
{
public:
 unsigned int ref() {return 1;}
 
};

class DBItem : public  Dbt
{
public:
 
 virtual ~DBItem(){}

 virtual bool isEmpty(){return empty;}
//void fromDbt(Dbt *dbt);

 void set();
 void get();
 

protected:
 virtual void read(QDataStream *s) = 0;
 virtual void write(QDataStream *s) = 0;
 QByteArray mydata;  
 bool empty;
 
};

class DBItemMainKey : public DBItem
{
 public:
  DBItemMainKey();
  DBItemMainKey(QString searchstring);
  
 
 QString getString(){  return sstr;}
 
private:
 
 virtual void read(QDataStream *s);
 virtual void write(QDataStream *s);
 
 QString sstr;
 
};


class DBItemMainData : public DBItem
{
 public:

  typedef QMapIterator<QString,QValueList<unsigned int> > TranslationIterator;
  typedef QMap<QString,QValueList<unsigned int> > TranslationMap;

  DBItemMainData();
 
  QStringList getTranslations();
  QValueList<unsigned int> getReferences(QString str);
  
  void clear();


//Add a translation with  reference ref, if translation exists append
// ref to the list of references
  void addTranslation(QString str,unsigned int ref);       
  void removeTranslation(QString str,unsigned int ref);       

//remove any reference to ref, if ref is the only reference of a translation
// the translation is removed  
  void removeRef(unsigned int ref);  

  unsigned int getIndexnumber(){  return indexnumber; }
  void setIndexnumber(int i){ indexnumber=i;   }
  
private:
 
 virtual void read(QDataStream *s);
 virtual void write(QDataStream *s);

 unsigned int indexnumber;
 TranslationMap translations;
 
};


class DBItemMultiIndex : public DBItem   
{
 public:
  typedef QValueList<unsigned int> IndexList;
 
  DBItemMultiIndex();
  // DBItemMultiIndex(IndexList l);
   
 void addEntry(unsigned int index);
 void removeEntry(unsigned int index);
 
 IndexList getList(){   return list;}
  void clear() {list.clear();}
 
private:
 
 virtual void read(QDataStream *s);
 virtual void write(QDataStream *s);
 
 IndexList list;
 
};


class DBItemNum : public Dbt
{
 public:
  DBItemNum(){ 
  	set_data(&num);
  	set_size(4);
    num=0;
  }
  DBItemNum(unsigned int i){	
    set_data(&num);
  	set_size(4);
    num=i;
  }
 
 unsigned int getNum(){ num=*(unsigned int *)get_data();  return num;}
 
private:
 
 unsigned int num;
 
};




#endif
