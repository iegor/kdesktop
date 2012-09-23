#include "dbentries.h"
#include <qdatastream.h>


void DBItem::set()
{
 QBuffer b(mydata);
 b.open( IO_WriteOnly );
 QDataStream s(&b);
 write(&s);
 b.close();
 set_data(mydata.data());
 set_size(mydata.size());
 
}
 void DBItem::get()
 {
	mydata.resize(get_size());
	mydata.duplicate((const char *)get_data(),(unsigned int)get_size());

	QDataStream *s = new QDataStream(mydata,IO_ReadOnly);
	read(s);
    delete s;
 }

  

DBItemMainKey::DBItemMainKey()
{
 sstr="";
 //set();
}

DBItemMainKey::DBItemMainKey(QString searchstring)
{
 sstr=searchstring;
 //set(); 
}
 
void DBItemMainKey::read(QDataStream *s)
{
*s >> sstr;
}

void DBItemMainKey::write(QDataStream *s)
{
 *s << sstr;
 }
 
 

  

DBItemMainData::DBItemMainData()
{
 clear();
 //set();
}

void DBItemMainData::clear()
{
 indexnumber=0;
 translations.clear();
 //set();
}
 
void DBItemMainData::read(QDataStream *s)
{
*s >> indexnumber >> translations;
}

void DBItemMainData::write(QDataStream *s)
{
 *s << (Q_UINT32)indexnumber << translations;
}

void DBItemMainData::addTranslation(QString str, unsigned int ref)
{
//get();
 if(translations[str].find(ref)==translations[str].end())  // If it do not exists
   translations[str].append(ref);  //add a new reference.
  else
  {

  }
//set();
    
}

void DBItemMainData::removeTranslation(QString str, unsigned int ref)
{
//get();
 translations[str].remove(ref);
 if(translations[str].isEmpty())
   translations.remove(str);
//set();

}

void DBItemMainData::removeRef( unsigned int ref)
{
//get();
QMapIterator<QString,QValueList<unsigned int> > it2;
 for(QMapIterator<QString,QValueList<unsigned int> > it = translations.begin();
     it!= translations.end(); /* it++*/)
	 {  //Dirty hack
	  it2=it;
	  it++;
	  if(it2.data().find(ref)!=it2.data().end())
      removeTranslation(it2.key(),ref);
	 }

//set();
}

QStringList  DBItemMainData::getTranslations()
{
//get();
QStringList result;
 for(QMapIterator<QString,QValueList<unsigned int> > it = translations.begin();
     it!= translations.end(); it++)
     result.append(it.key());

return result;

}

QValueList<unsigned int>  DBItemMainData::getReferences(QString str)
{
//get();
return translations[str];  //this also add a "str" entry but we do not call set()!
}



DBItemMultiIndex::DBItemMultiIndex()
{
list.clear();
//set();
}
  
void DBItemMultiIndex::addEntry(unsigned int index)
{
// get();
 if(list.find(index)==list.end())
 {
   list.append(index);
   qHeapSort(list);
 //  set();

 }
 
}

void DBItemMultiIndex::removeEntry(unsigned int index)
{
// get();
 list.remove(index);
// set();
}

 
void DBItemMultiIndex::read(QDataStream *s)
{
*s >> list;
}

void DBItemMultiIndex::write(QDataStream *s)
{
 *s << list;
 }
