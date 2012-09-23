//
// C++ Implementation: algorithms
//
// Description: 
//
//
// Author: Andrea Rizzi <rizzi@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "algorithms.h"
#include <qstringlist.h>
#include <kdebug.h>

//FIXME: remove
#define i18n (const char*)

DataBaseInterface::ResultList ExactSearchAlgorithm::exec(const QString& query )
{
    DataBaseInterface::ResultList res;
    DataBaseInterface::MainEntry e=di->get(query,0);
    
    QStringList trs=e.second.getTranslations();
    
    for(QStringList::iterator it=trs.begin();it!=trs.end();++it)
    {	
	
	emit newResult(QueryResult(*it,e.first.getString(),settings->scoreExact));
	
	res.push_back(QueryResult(*it));
    }
    kdDebug(0) <<"Exact algo found " << res.count() << "entries" << endl;
     return res;
}


DataBaseInterface::ResultList GenericSearchAlgorithm::exec(const QString& query )
{
    DataBaseInterface::ResultList res;
   // ExactSearchAlgorithm exact(query,settings);
    uint countResults=0;
    for(QValueList<AbstractSearchAlgorithm *>::iterator algoit = algoChain.begin(); algoit!=algoChain.end() && countResults < maxResults; algoit++)
	{
	connect(*algoit,SIGNAL(newResult(QueryResult)),this,SIGNAL(newResult(QueryResult)));
	kdDebug(0) << "Algo pointer" << (*algoit) << endl;
	res+=(*algoit)->exec(query);
	countResults=res.count();
	kdDebug(0) << "Count = " << countResults << endl;
	disconnect(*algoit,SIGNAL(newResult(QueryResult)),this,SIGNAL(newResult(QueryResult)));
    }
    return res;
}

void GenericSearchAlgorithm::addAlgorithm( AbstractSearchAlgorithm * algo )
{
    algoChain.append(algo);
}

DataBaseInterface::ResultList AlphaSearchAlgorithm::exec( const QString & query )
{
    DataBaseInterface::ResultList res;
    DBItemMultiIndex::IndexList il=di->getAlpha(query);

    for(DBItemMultiIndex::IndexList::iterator it=il.begin();it!=il.end()&&!di->stopNow();++it)
    {
	DataBaseInterface::MainEntry e=di->getFromIndex(*it);
	QStringList trs=e.second.getTranslations();
	for(QStringList::iterator it=trs.begin();it!=trs.end() && !di->stopNow();++it)
	{
	    QueryResult r(di->format(di->simple(*it,true),query),e.first.getString(),settings->scoreAlpha);
	    emit newResult(r); 
	    res.push_back(r);
	}
    }
    kdDebug(0) <<"Alpha algo found " << res.count() << "entries" << endl;
    
    return res;
}

DataBaseInterface::ResultList SentenceArchiveSearchAlgorithm::exec( const QString & query )
{
    DataBaseInterface::ResultList res;
    
    DataBaseInterface::MainEntry e = di->getSentence(query);
    
    QStringList trs=e.second.getTranslations();
    
    kdDebug(0) << "Count in sentence archive " << trs.count()<< endl;
    
    for(QStringList::iterator it=trs.begin();it!=trs.end();++it)
    {
	QueryResult r(di->format(di->simple(*it,true),query),e.first.getString(),settings->scoreSentence);
	emit newResult(r);  
	
	res.push_back(r);
    }
    kdDebug(0) <<"Sentence algo found " << res.count() << "entries" << endl;

    return res;
}

DataBaseInterface::ResultList ChunkByChunkSearchAlgorithm::exec( const QString & query )
{
    ResultList res;
    factory->setQuery(query);
    QPtrList<AbstractChunk> chunks=factory->chunks();
    kdDebug(0) << "Number of chunks " << chunks.count() << endl;
    chunks.setAutoDelete(true); //I should delete the chunks myself
    QStringList querySeparators=factory->separators();

	//This prevents recursive loop.
	if (chunks.count()<=1) return res;

	QStringList translations,tmpTranslations;

    translations.push_back("");   //FIXME this is needed to start  , but is not good
    int finalscore=0;
    int i=0;
    QMap<QString,bool> translationUsed;

    //Loop on all chunk
    for(AbstractChunk *it=chunks.first();it && !di->stopNow(); it=chunks.next())
    {
	kdDebug(0) << "Process next chunk" << endl;
 	int chunkscore=0;
	QValueList<QueryResult> r=it->translations();
	 kdDebug(0) << "Number of results for this chunk " << r.count() << endl;

	if(r.count()<1) {
	    // kdDebug(0) << "Nothing found for:" << it->translations() << endl;
	    chunkscore=-10;

	}
	else
	{
	    //FIXME: check this, why 0? it is the best one?
	    chunkscore=r[0].score();
	    kdDebug(0) << "ChunkScore " << chunkscore << endl;
	    tmpTranslations.clear();


	    //Loop on results
	    translationUsed.clear();
	    for(ResultList::iterator it1=r.begin();it1!=r.end() &&!di->stopNow(); ++it1)
	    {
		QString chunkTranslation= (*it1).result();
		if(!translationUsed.contains(chunkTranslation))
		{
		    translationUsed[chunkTranslation]=true;
		    kdDebug(0) << "a translation is: " << chunkTranslation << endl;
		    for(QStringList::iterator it2=translations.begin();it2!=translations.end() && !di->stopNow() ; it2++)
		    {
			QString prevTranslation=*it2;
			tmpTranslations.push_back(prevTranslation+chunkTranslation+querySeparators[i]);
			kdDebug(0) << "..appending it to " << prevTranslation << endl;
		    }
		}
	    } 
	    
	    translations=tmpTranslations; 
	
	}  
	
	//kdDebug(0) << it-> << r[0].result() << "#" << querySeparators[i] << endl;
	i++; 
	finalscore+=chunkscore;
	
	kdDebug(0) << "partial score " << finalscore;
    }
    kdDebug(0) << "this is finishd" << endl;
       if(settings->scoreChunkByChunk==0) 
	settings->scoreChunkByChunk=1;
// FIXME:fix the score system
//    finalscore/=(i*100*100/settings->scoreChunkByChunk);  //change 100 to 120(?) to lower this result (done)
    
    if (finalscore<50) return res;
    
    for(QStringList::iterator it2=translations.begin();it2!=translations.end() && !di->stopNow() ; it2++)
    {
	QString theTranslation=*it2;
	QueryResult qr(di->format(theTranslation,query),i18n("CHUNK BY CHUNK"),finalscore);
	qr.setRichOriginal(i18n("<h3>Chunk by chunk</h3>CHANGE THIS TEXT!!!!This translation is"
				"obtained translating the  sentences and using a"
				"fuzzy sentence translation database.<br>"
				" <b>Do not rely on it</b>. Translations may be fuzzy.<br>"));
	qr.setRichResult("<font color=#800000>"+theTranslation+"</font>") ;    
	emit newResult(qr); 
	res.push_back(qr);
    }
    
    
    return res;
    
    
}

ChunkByChunkSearchAlgorithm::ChunkByChunkSearchAlgorithm( DataBaseInterface * dbi, DBSESettings * sets ): AbstractSearchAlgorithm(dbi,sets) , factory(0) 
{
    
}


SentenceArchiveSearchAlgorithm::SentenceArchiveSearchAlgorithm( DataBaseInterface * dbi, DBSESettings * sets ): AbstractSearchAlgorithm(dbi,sets) 
{
}

FuzzyChunkSearchAlgorithm::FuzzyChunkSearchAlgorithm( DataBaseInterface * dbi, DBSESettings * sets ) : AbstractSearchAlgorithm(dbi,sets) 
{
    
}


DataBaseInterface::ResultList FuzzyChunkSearchAlgorithm::exec( const QString & query )
{
    //FIXME: this code is shit too
    ResultList res;
    factory->setQuery(query);
    QPtrList<AbstractChunk> querychunks = factory->chunks();
    querychunks.setAutoDelete(true);
    
    typedef QMap<QString,QValueList<unsigned int> > ResultMap;
    ResultMap rmap;  //result of words index query
    unsigned int notfound=0,frequent=0,nchunks = querychunks.count();
    
    //Get index list for each word
    for(AbstractChunk *it=querychunks.first(); it &&!di->stopNow() ;  it=querychunks.next()  )
    { 
	QValueList<uint> locations = (*it).locationReferences();
	
	if(locations.count()>0)
	{
	    rmap[(*it).chunkString()] = locations;
	    
	    if(locations.count()>1000)  //FIXME NORMALIZE THIS!!!
	    {
		frequent++;
		kdDebug(0) << "\""<<(*it).chunkString()  << "\" is frequent" <<endl;
	    }
	}
	else
	    notfound++;
	
    }
    
    
    //Now we have a map (rmap)  "word in query->list of occurency"
    
    QValueList<unsigned int>::iterator countpos[nchunks+1];
    
    
    QValueList<unsigned int> il;
    for(int i = 0;i<=nchunks&&!di->stopNow();i++) 
	countpos[i]=il.end();
    
    unsigned int bestcount=0;
    while(!rmap.isEmpty())
    {
	unsigned int ref,count;
	ref=(unsigned int)-1; 
	count=0;
	
	
	//This will find the min head and count how many times it occurs
	for(ResultMap::iterator it = rmap.begin();it!=rmap.end()&&!di->stopNow();++it)
	{
	    unsigned int thisref=it.data().first();
	    if(thisref<ref)
	    {
		ref=thisref;
		count=0;
	    }
	    if(thisref==ref)
	    {
		count++;
	    }
	    
	}
	
	
	for(ResultMap::iterator it = rmap.begin();it!=rmap.end()&&!di->stopNow();)
	{
	    it.data().remove(ref);
	    
	    //kdDebug(0)<< ((frequent<(nwords-notfound)) && (it.data().count()>350)) <<endl;
	    //FIXME: I think the frequent word check is not in the right place
	    if(it.data().isEmpty() || (((frequent+notfound)<nchunks) && (it.data().count()>1000)))  
		//very dirty hack...
	    {
		
		ResultMap::iterator it2=it;
		it++;
		rmap.remove(it2);
	    }
	    else it++;
	    
	}
	
	//This should be configurable or optimized:
	if(count>=(nchunks-notfound)*0.50 && count!=0)
	{
	    il.insert(countpos[count],ref);
	     for(unsigned int i = nchunks;i>=count;i--) 
		if(countpos[i]==countpos[count])		    
		    countpos[i]--;
	}
    }     
    
    //loop on number of words found
    int bestscore=0;
    
    for(unsigned int wf=nchunks;wf>0;wf-- ){
	for(QValueList<unsigned int>::iterator it=countpos[wf];it!=countpos[wf-1] ;++it)
	{ //loop on entries with same number of word found
	    DataBaseInterface::MainEntry e;
	    e=di->getFromIndex(*it);
	      QStringList trs=e.second.getTranslations();
	    for(QStringList::iterator it=trs.begin();it!=trs.end()&&!di->stopNow();++it)
	    {		
		unsigned int cinr=factory->chunks(*it).count(); //chunk in result
		//compute a score, lets kbabel sort now, it should be fast...
		int score=90*wf/nchunks-(signed int)90*(((nchunks-cinr)>0)?(nchunks-cinr):(cinr-nchunks))/(nchunks*10);
		if(score>bestscore) bestscore=score;
		if(score>bestscore*0.40)
		{
		    // kdDebug(0) << "s: "<<score << "  wf: "<<wf<<"  nwords: "<<nwords<<" winr:  "<<winr
		    //    <<" 90*wf/nwords: "<<90*wf/nwords << "  -:" <<  90*(((nwords-winr)>0)?(nwords-winr):(winr-nwords))/(nwords*10)<< endl;
		    // FIXME: format better the richtext
		    QString ori=e.first.getString();
		    QString re=di->format(di->simple(*it,true),query);
		    QueryResult r(re,ori,score);
		    for(QPtrListIterator<AbstractChunk> it(querychunks); it.current() && di->stopNow() ; ++it){
			ori=ori.replace(QRegExp((*it)->chunkString(),false),"<font color=#000080><u><b>"+(*it)->chunkString()+"</b></u></font>");
		    }
		    r.setRichOriginal(ori);	
		    if(!di->stopNow())
			emit newResult(r); 
		    res.push_back(r);
		}
	    }	      
	}
    }	
    return res;
    
}

DataBaseInterface::ResultList CorrelationSearchAlgorithm::exec( const QString & query )
{
    //FIXME, this code is shit.
    DataBaseInterface::ResultList res;
    if(di->words(query).count()>1) return res;
    QMap<QString,float> corRes = di->correlation(query,0,false);
    float max=0,max1=0,max2=0;
    QString best,best1,best2;
    
    for(QMap<QString,float>::iterator it = corRes.begin(); it !=corRes.end(); ++it)
    {
	if(it.data()>max) 
	{
	    max2=max1;
	    best2=best1;
	    max1=max;
	    best1=best;
	    best = it.key(); 
	    max=it.data();
	    
	}	
	
	
    }
    if(!best.isEmpty()) 
    {
	double myscore=0.01*max*settings->scoreDynamic;
	QueryResult r(di->format(best,query),i18n("DYNAMIC DICT:"),myscore);
	r.setRichOriginal(i18n("<h3>Dynamic Dictionary</h3>This is a dynamic dictionary created"
			       " looking for correlation of original and translated words.<br>"
			       " <b>Do not rely on it</b>. Translations may be fuzzy.<br>"));
	r.setRichResult("<font size=+2 color=#A00000>"+di->format(best,query)+"</font>") ;    
	res.push_back(r);
	if(!di->stopNow()) 
	    emit newResult(r); 	
    }	
    if(!best1.isEmpty()) 
    {
	double myscore=0.01*max1*settings->scoreDynamic;     
	QueryResult r(di->format(best1,query),i18n("DYNAMIC DICT:"),myscore);
	r.setRichOriginal(i18n("<h3>Dynamic Dictionary</h3>This is a dynamic dictionary created"
			       " looking for correlation of original and translated words.<br>"
			       " <b>Do not rely on it</b>. Translations may be fuzzy.<br>"));
	r.setRichResult("<font size=+2 color=#800000>"+di->format(best1,query)+"</font>") ;    
	res.push_back(r);
	if(!di->stopNow()) 
	    emit newResult(r); 	
    }	
    
    kdDebug(0) << "Correlation algorithm found" << res.count() << "results";
    return res;
    
}

GenericSearchAlgorithm::GenericSearchAlgorithm( DataBaseInterface * dbi, DBSESettings * sets ): AbstractSearchAlgorithm(dbi,sets) 
{
    maxResults = 5; //FIXME use as default somthing from DBSESettings
}

SingleWordSearchAlgorithm::SingleWordSearchAlgorithm( DataBaseInterface * dbi, DBSESettings * sets ) : GenericSearchAlgorithm(dbi,sets),
    exact(dbi,sets), alpha(dbi,sets), sentence(dbi,sets), corr(dbi,sets), chunk(dbi,sets),casefactory(dbi)
    {
    addAlgorithm(&exact);
    addAlgorithm(&alpha);
    addAlgorithm(&sentence);
	chunk.setChunkFactory(&casefactory);
	addAlgorithm(&chunk);
	addAlgorithm(&corr);
}

DataBaseInterface::ResultList SingleWordSearchAlgorithm::exec( const QString & query )
{
    if(di->words(query).count()>1)
	return ResultList();
    return GenericSearchAlgorithm::exec(query);
}


//#include "algorithms.moc"
