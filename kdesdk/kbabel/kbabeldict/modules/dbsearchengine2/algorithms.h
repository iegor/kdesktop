//
// C++ Interface: strategies
//
// Description: 
//
//
// Author: Andrea Rizzi <rizzi@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DBSE2_STRATEGIES_H
#define DBSE2_STRATEGIES_H

#include "database.h"
#include "chunk.h"
#include <qobject.h>

class AbstractSearchAlgorithm : public QObject
{
    Q_OBJECT
    public: 
    
    typedef QValueList<QueryResult> ResultList;
    
    AbstractSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets)
    {
	di=dbi;
	settings=sets;
    }
    /**
      * this contains the algo and return some results.	
      */
    virtual DataBaseInterface::ResultList exec(const QString& query)=0;	
    
    signals:
    void newResult(QueryResult);
    
    protected:
    DataBaseInterface *di;
    DBSESettings *settings;
    
};



class ExactSearchAlgorithm : public AbstractSearchAlgorithm
{
    Q_OBJECT
    public: 
    ExactSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets) : AbstractSearchAlgorithm(dbi,sets) {}
    
    virtual DataBaseInterface::ResultList exec(const QString& query);
};

class AlphaSearchAlgorithm : public AbstractSearchAlgorithm
{
   Q_OBJECT
    public: 
    AlphaSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets) : AbstractSearchAlgorithm(dbi,sets) {}
    
    virtual DataBaseInterface::ResultList exec(const QString& query);
};

class SentenceArchiveSearchAlgorithm : public AbstractSearchAlgorithm
{
   Q_OBJECT
    public: 
    SentenceArchiveSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets) ;
    
    virtual DataBaseInterface::ResultList exec(const QString& query);
};

class ChunkByChunkSearchAlgorithm : public AbstractSearchAlgorithm
{
    Q_OBJECT
    public: 
    ChunkByChunkSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets);
    
    virtual DataBaseInterface::ResultList exec(const QString& query);
    
    void setChunkFactory(AbstractChunkFactory *_factory)
    {
    factory=_factory;
    }
    protected:
    AbstractChunkFactory *factory;
};

class FuzzyChunkSearchAlgorithm : public AbstractSearchAlgorithm
{
    Q_OBJECT
    public: 
    FuzzyChunkSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets);
    
    virtual DataBaseInterface::ResultList exec(const QString& query);
    void setChunkFactory(AbstractChunkFactory *_factory)
    {
    factory=_factory;
    }
    protected:
    AbstractChunkFactory *factory;    
};

class GenericSearchAlgorithm : public AbstractSearchAlgorithm
{
  Q_OBJECT
    public: 
    GenericSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets) ;
    
    virtual ResultList exec(const QString& query);
    
    void addAlgorithm(AbstractSearchAlgorithm *algo);    
    void setMaxResultNumber(uint num){maxResults=num;}

    
    protected:
    QValueList<AbstractSearchAlgorithm *> algoChain;
    uint maxResults;
 };


class CorrelationSearchAlgorithm : public AbstractSearchAlgorithm
{
  Q_OBJECT
    public: 
    CorrelationSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets) : AbstractSearchAlgorithm(dbi,sets) {}
    virtual ResultList exec(const QString& query);
};



class SingleWordSearchAlgorithm : public GenericSearchAlgorithm
{
  Q_OBJECT
    public: 
    SingleWordSearchAlgorithm(DataBaseInterface *dbi,DBSESettings *sets);
    virtual DataBaseInterface::ResultList exec(const QString& query);
  
    private:
    ExactSearchAlgorithm exact;
    AlphaSearchAlgorithm alpha;
    SentenceArchiveSearchAlgorithm sentence;
    CorrelationSearchAlgorithm corr;
  	ChunkByChunkSearchAlgorithm chunk;
	CaseBasedWordChunkFactory casefactory;
};


class UpdateChunkIndexAlgorithm
{
    
};



#endif //DBSE2_STRATEGIES_H
