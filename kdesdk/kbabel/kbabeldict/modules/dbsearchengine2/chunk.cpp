//
// C++ Implementation: chunk
//
// Description: 
//
//
// Author: Andrea Rizzi <rizzi@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "chunk.h"
#include "algorithms.h"
#include <kdebug.h>



AbstractChunk::~AbstractChunk( )
{
}

WordChunk::WordChunk( DataBaseInterface * di, QString _word ) : AbstractChunk(di)
{
    word=_word;
}

QValueList<QueryResult> WordChunk::translations( )
{
    DataBaseInterface::ResultList r;
    SingleWordSearchAlgorithm sa(di,di->getSettings());
    r=sa.exec(word);
    return r;
}

//QValueList<QueryResult> WordChunk::translationsFromReference( uint reference )
//{
//}

QValueList< uint > WordChunk::locationReferences( )
{
    QValueList<uint> res=di->getWordIndex(word);
    kdDebug(0) << "Number of locations " << res.count() <<endl ;
    return res;
/*    DBItemMainKey *k = new DBItemMainKey(word);	 
    DBItemMultiIndex *d = new DBItemMultiIndex();
    if(wordsindex->get(k,d)!=DB_NOTFOUND)
	return d->getList();
    else
	return QValueList<uint> tmpList;
*/
}

void WordChunk::setLocationReferences( QValueList< uint > )
{
}

SentenceChunk::SentenceChunk( DataBaseInterface * di, QString _sentence ): AbstractChunk(di)
{
    sentence=_sentence;
}

QValueList<QueryResult> SentenceChunk::translations( )
{
    GenericSearchAlgorithm g(di,di->getSettings());
   
    ExactSearchAlgorithm e(di,di->getSettings());
    AlphaSearchAlgorithm a(di,di->getSettings());
    SentenceArchiveSearchAlgorithm s(di,di->getSettings());

    g.addAlgorithm(&e);
    g.addAlgorithm(&a);
    g.addAlgorithm(&s);
    
    return g.exec(sentence);
    
}

//QValueList<QueryResult> SentenceChunk::translationsFromReference( uint reference )
//{
//    
//}

QValueList< uint > SentenceChunk::locationReferences( )
{
}

void SentenceChunk::setLocationReferences( QValueList< uint > )
{
}

QPtrList< AbstractChunk> WordChunkFactory::chunks()
{
    QString str=di->simple(string);
    QPtrList<AbstractChunk> list;
    if(str.isEmpty()) return list;
    _separators.clear();
	kdDebug(0) << "Word chunks of:" <<str << endl;
    int pos;
    QString sep;
    QRegExp r("(\\s)");
    do {
	pos=r.search(str);

	sep=r.cap(1);
	if(!str.left(pos).isEmpty()){
	    //WordChunk *c=new WordChunk(di,di->simple(str.left(pos)))
	    list.append(new WordChunk(di,str.left(pos)));
	    _separators.append(sep);
	}
	else
	{
	    uint current=_separators.count()-1;
	    _separators[current]=_separators[current]+sep;
	}	
	str=str.remove(0,pos+1);
    } while(!str.isEmpty() && pos != -1);

    return list;
}



QPtrList<AbstractChunk> SentenceChunkFactory::chunks()
{
    QString str=string;
    QPtrList<AbstractChunk> list;
    if(str.isEmpty()) return list;

    //  kdDebug(0) << s << endl;

    int pos;


    do {
	QRegExp re("((\\.|;|\\?|\\!|:)( |$|\\\\n\\n))");
	pos=re.search(str);
	QString sep=re.cap(1);

	if(!str.left(pos).isEmpty())
	{
	    list.append(new SentenceChunk(di,str.left(pos).stripWhiteSpace()));
	    _separators.append(sep);
	}
	else
	{
	    uint current=_separators.count()-1;
	    _separators[current]=_separators[current]+sep;
	}

	str=str.remove(0,pos+re.cap(1).length());
    } while(!str.isEmpty() && pos != -1);


  return list;

}
QPtrList< AbstractChunk > CaseBasedWordChunkFactory::chunks( )
{
    QString str=string;
    QPtrList<AbstractChunk> list;
    if(str.isEmpty()) return list;
	uint slen=str.length();
	kdDebug(0) << "CaseWordChunk string:" << str << endl;
	QString tmpWord;
	bool upcase;
	for(uint i=0;i<=slen;i++)
	{
	bool tmpCase=(str[i]==str[i].upper());
		if(upcase!=tmpCase)
		 {
		 	if(!tmpWord.isEmpty() && !tmpWord.isNull()){
			  list.append(new WordChunk(di,tmpWord));
			   _separators.append("");
			  }
			kdDebug(0) << "CaseWordChunk:" << tmpWord << endl;
			tmpWord="";

		 }
		tmpWord+=str[i];
		upcase=tmpCase;
	}

	return list;
}

WordChunkFactory::WordChunkFactory( DataBaseInterface * _di ) : AbstractChunkFactory(_di)
{
}

SentenceChunkFactory::SentenceChunkFactory( DataBaseInterface * _di ): AbstractChunkFactory(_di)
{
}

CaseBasedWordChunkFactory::CaseBasedWordChunkFactory( DataBaseInterface * _di ): AbstractChunkFactory(_di)
{
}







