//
// C++ Interface: chunk
//
// Description: 
//
//
// Author: Andrea Rizzi <rizzi@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DBSE2_CHUNK_H
#define DBSE2_CHUNK_H
#include <qstring.h>
#include <qvaluelist.h>
#include "database.h"

/**
  * Abstract class for text chunks. 
  * Examples of chunks are "Words" or "Sentences"
  * This abstraction allow to use generic algorithm on chunks,
  * like chunkByChunk translation or chunk indexing.
  */
class AbstractChunk
{
    public:
    AbstractChunk(DataBaseInterface *_di) {di=_di;}
    virtual ~AbstractChunk();
    /**
      * This function should return a list of translation for the current chunk.
      */
    virtual QValueList<QueryResult> translations()=0;
    
    //FIXME: is this in the right place, better in factory? check that stuff
    //virtual QValueList<QueryResult> translationsFromReference(uint reference)=0;
    virtual QValueList<uint> locationReferences()=0;
    virtual void setLocationReferences(QValueList<uint>)=0;
    virtual QString chunkString()=0;
    
    protected:
    DataBaseInterface *di;
};

/**
  * Concrete impl of Chunk, in this case chunks are words.
  */
class WordChunk : public AbstractChunk
{
    public:
    WordChunk(DataBaseInterface *di,QString _word);
    virtual QValueList<QueryResult> translations();
    //virtual QValueList<QueryResult> translationsFromReference(uint reference);
    virtual QValueList<uint> locationReferences();
    virtual void setLocationReferences(QValueList<uint>);
    virtual QString chunkString(){return word;}

    //static QValueList<WordChunk> divide(QString);
    private:
    QString word;
};

/**
  * Concrete impl of Chunk, in this case chunks are sentences.
  */
class SentenceChunk : public AbstractChunk
{
    public:
    SentenceChunk(DataBaseInterface *di,QString _sentence);
    virtual QValueList<QueryResult> translations();
    //virtual QValueList<QueryResult> translationsFromReference(uint reference);
    virtual QValueList<uint> locationReferences();
    virtual void setLocationReferences(QValueList<uint>);
    virtual QString chunkString(){return sentence;}

   // static QValueList<SentenceChunk> divide(QString);
    
    private:
    QString sentence;    
};


/**********************************
  CHUNK FACTORIES
**********************************/


class AbstractChunkFactory 
{
  public:
    AbstractChunkFactory(DataBaseInterface *_di)
    {
	di=_di;
    }
    virtual ~AbstractChunkFactory(){}
    virtual  QPtrList<AbstractChunk> chunks()=0;
    /**
      Change th string and return the chunks
      */
    virtual  QPtrList<AbstractChunk> chunks(const QString& s)
    {
	string=s;
	return chunks();
    }
    /**
      * Returns the list of separators of last @ref chunks() call
      */
    
    virtual  QStringList separators(){ return _separators;}
    void setQuery(const QString& s)
    {
	string=s;
    }
  protected:
    QString string;
    QStringList _separators;
    DataBaseInterface *di;
};

class WordChunkFactory : public AbstractChunkFactory
{
  public:
    WordChunkFactory(DataBaseInterface *_di);
    /**
      YOU SHOULD DELETE THE CHUNKS!!
      */
    virtual  QPtrList<AbstractChunk> chunks();
};

class CaseBasedWordChunkFactory : public AbstractChunkFactory
{
  public:
    CaseBasedWordChunkFactory(DataBaseInterface *_di);
    /**
      YOU SHOULD DELETE THE CHUNKS!!
      */
    virtual  QPtrList<AbstractChunk> chunks();
};

class SentenceChunkFactory : public AbstractChunkFactory
{
  public:
    SentenceChunkFactory(DataBaseInterface *_di);
    
    /**
      YOU SHOULD DELETE THE CHUNKS!!
      */
    virtual  QPtrList<AbstractChunk> chunks();
};


#endif //_CHUNK_H_
