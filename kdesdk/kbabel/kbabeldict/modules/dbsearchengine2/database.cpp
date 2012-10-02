/*
  
DBSE 3
(c) 2000-2003 Andrea Rizzi
License: GPLv2 
	 
*/
#include <math.h>
#include "database.h"

#include <qregexp.h>
#include <qdict.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kmessagebox.h>

#define i18n (const char*)






DataBase::DataBase(QString dbpath,QString dbname, QString dblang) : Db(0,DB_CXX_NO_EXCEPTIONS)
{
    
    filename=dbpath+"."+dblang+".db";
    database=dbname;
    
}

int DataBase::open(DBTYPE type,unsigned int flags)
{
    int ret;
    ret = Db::open(
#if DB_VERSION_MINOR > 0
              NULL,
#endif
    (const char*)filename.local8Bit(),(const char *)database.local8Bit(),type,flags,0644);
    mytype=type;
    return ret;
}

unsigned int DataBase::getLast()
{
    if(mytype!=DB_RECNO)
	return 0;
    
    Dbc *cur;
    cursor(0,&cur,0);
    DBItemNum index;
    DBItemMainKey key;
    cur->get(&index,&key,DB_LAST);
    return index.getNum();
    
}





QueryResult::QueryResult(QString r)
{
    res=r;
}
QueryResult::QueryResult(QString r,QString o,int s)
{
    res=r;
    richr=r;
    orig=o;  
    richo=o;
    sco=s;
}

QueryResult::QueryResult()
{
    res="";
}




DataBaseInterface::DataBaseInterface(QString dir, DBSESettings *sets)
{
    
    //FIXME Better db names!!
    main = openMyDataBase(dir+"/testm","main","it",DB_BTREE);
    alpha = openMyDataBase(dir+"/testa","alpha","it",DB_BTREE);
    numindex = openMyDataBase(dir+"/testn","numindex","it",DB_RECNO);
    wordsindex = openMyDataBase(dir+"/testw","wordsindex","it",DB_BTREE);
    sentence = openMyDataBase(dir+"/tests","sentence","it",DB_BTREE);
    corr = openMyDataBase(dir+"/testc","corr","it",DB_BTREE);
    transword = openMyDataBase(dir+"/testt","transword","it",DB_RECNO);
    
    // kdDebug(0) << main << endl;
    // kdDebug(0) << alpha << endl;
    settings=sets;
    _stopNow=false;
}

DataBaseInterface::~DataBaseInterface()
{
    
    if(main){
	main->close(0); 
	delete main;
    }    
    if(numindex){
	numindex->close(0); 
	delete numindex; 
    }
    
    if(alpha){
	alpha->close(0); 
	delete alpha; 
    } 
    if(wordsindex){
	wordsindex->close(0); 
	delete wordsindex; 
    } 
    if(sentence){
	sentence->close(0); 
	delete sentence; 
    }
    
}

DataBase *DataBaseInterface::openMyDataBase(const QString& prefix,const QString& name,const QString& l,DBTYPE tt)
{

    DataBase *aDb = new DataBase(prefix,name,l);
    if(aDb==0){
	return 0;
    }
    else
    {
	if(aDb->open(tt)!=0)
	{
	    kdDebug(0) << "Database '"<< name <<"'do not exist, I try to create it.." << endl;
		//ask only the first time.
	   	static bool create=(	KMessageBox::questionYesNo(0,"Database do not exist. Do you want to create it now?",
		i18n("Create Database"), i18n("Create"), i18n("Do Not Create"))==KMessageBox::Yes);
		if(create)
		if(aDb->open(tt,DB_CREATE)!=0)
		{
		kdDebug(0) << "...cannot create!!"<< endl;
		return 0;
	    }
	    else
	    {
		kdDebug(0) << "...done!" << endl;
		return aDb;
	    }
	}
	
    } 
    return aDb;
}

/*
 *   query functions.
 *
 */


DataBaseInterface::MainEntry DataBaseInterface::get(const QString& query,SearchFilter *filter)
{
    static int counter=1;
    counter++;
    DBItemMainKey k(query);
    DBItemMainData d;
    //int r=
    main->get(&k,&d);
    //  kdDebug(0) << "MAINDB->GET returned: " << r << endl;
    if(counter%5==0) kapp->processEvents(100);
    //  kdDebug(0) << "events processed" << endl;
    return qMakePair(k,d);
    
}

/*
 * put functions
 *                                              *
 */


bool DataBaseInterface::addEntry(QString original,QString translated,InputInfo *info)
{
    DBItemMainKey  mk(original);
    DBItemMainData  md;
    QMap<QString, int> correlationDiff;
    bool newentry=false;
    //try to get
    kdDebug(0) << "Inserting the pair:" << endl;
    kdDebug(0) << "ORIGINAL:" << original << endl;
    kdDebug(0) << "TRANSLATED:" << translated << endl;
    
    if(main->get(&mk,&md)==DB_NOTFOUND)
    {
	kdDebug(0) << "new entry" << endl;
	newentry=true;
	//This is a new entry, create index entry
	DBItemNum *nind;
	int newid=numindex->getLast()+1;
	nind=new DBItemNum(newid);
	numindex->put(nind,&mk);
	
	delete nind;
	
	md.clear();
	md.setIndexnumber(newid);
	
	
	//Update secondary index alpha
	DBItemMainKey ka(simple(original));
	DBItemMultiIndex in;
	if(alpha->get(&ka,&in)==DB_NOTFOUND) in.clear()  ;
	//alpha->get(&ka,&in);
	in.addEntry(newid);
	alpha->put(&ka,&in);
	kdDebug(0) << "Updating the word index " << endl;
	//Update words index
	QStringList ws=words(original);
	for(QStringList::iterator it = ws.begin(); it!=ws.end(); ++it)
	{
	    DBItemMainKey word(*it);
	    DBItemMultiIndex win;
	    if(wordsindex->get(&word,&win)==DB_NOTFOUND) win.clear();
	    win.addEntry(newid);
	    wordsindex->put(&word,&win);
	}
	
	kdDebug(0) << "new entry preparation DONE" << endl;
    } 
    else
    {
	
	kdDebug(0) << "It exists!" <<endl;
    }

    
    //Update sentence index
    QStringList so=sentences(original);
    QStringList st=sentences(translated);
    if(so.count()==st.count() && st.count() >1 ) //we already hav a database for single string.
    {
	kdDebug(0) << "inside sentence loop" << endl;
	for(int i=0; i< so.count() ; i++)
	{
	    DBItemMainKey sk(so[i]);
	    DBItemMainData sd;
	    if(sentence->get(&sk,&sd)==DB_NOTFOUND&&!newentry)
		kdDebug(0) << "Warning: new sentence for old entry, do we changed sentence definition? " << endl;
	    
	    kdDebug(0) << "here alive" << endl;
	    
	    //	    if(clean)
	    sd.removeRef(info->ref());
	    kdDebug(0) << "now alive" << endl;
	    sd.addTranslation(st[i],info->ref());
	    kdDebug(0) << "still alive" << endl;
	    
	    sentence->put(&sk,&sd);
	    
	}
	
	
	
    }
    kdDebug(0) << "Fuzzy sentence archive updated" << endl;
    
    
    
    //Add that translation, link to ref for information on that translation
    
    if(!translated.isEmpty())
    {
	//loop on all translations to update correlation
	QStringList tmpTranslations=md.getTranslations();
	for(QStringList::iterator otIt=tmpTranslations.begin(); otIt!=tmpTranslations.end();++otIt)
	{
	    QStringList wt=words(*otIt);
	    for(QStringList::iterator it = wt.begin(); it!=wt.end(); ++it)
	    {
		if(correlationDiff.contains(*it))
		    correlationDiff[*it]--;
		else
		    correlationDiff[*it]=-1;
	    }
	}

	//clean so that we have only one translation per catalog.
	md.removeRef(info->ref());
	md.addTranslation(translated,info->ref());

	tmpTranslations=md.getTranslations();
	for(QStringList::iterator otIt=tmpTranslations.begin(); otIt!=tmpTranslations.end();++otIt)
	{
	    QStringList wt=words(*otIt);
	    for(QStringList::iterator it = wt.begin(); it!=wt.end(); ++it)
	    {
		if(correlationDiff.contains(*it))
		    correlationDiff[*it]++;
		else
		    correlationDiff[*it]=1;
	    }
	}

	//FIXME: use the correlationDIff map somehow

    }

    //finally put!
    return (main->put(&mk,&md)==0);

}


bool DataBaseInterface::removeEntry(QString original)
{
    DBItemMainKey  mk(original);
    DBItemMainData  md;
    
    //FIXME implement remove 
    //try to get
    if(main->get(&mk,&md)==DB_NOTFOUND)
    {			
	/*      //This is a new entry, create index entry
      DBItemNum *nind;
      int newid=numindex->getLast()+1;
      nind=new DBItemNum(newid);
      numindex->put(nind,&mk); 
      
      delete nind;
      
      md.clear();
      md.setIndexnumber(newid); 
      
      
      //Update secondary index alpha
      DBItemMainKey ka(simple(original));
      DBItemMultiIndex in;
      if(alpha->get(&ka,&in)==DB_NOTFOUND) in.clear()  ;
      //alpha->get(&ka,&in);
      in.addEntry(newid);
      alpha->put(&ka,&in);
      
      //Update words index
      QStringList ws=words(original);
      for(QStringList::iterator it = ws.begin(); it!=ws.end(); it++)
	{
	   DBItemMainKey word(*it);
	   DBItemMultiIndex win;
	   if(wordsindex->get(&word,&win)==DB_NOTFOUND) win.clear();
	   win.addEntry(newid);
	   wordsindex->put(&word,&win);    
	}
	
      //Update sentence index
      QStringList so=sentences(original);
      QStringList st=sentences(translated);
      if(so.count()==st.count() && st.count() >1 ) //we already hav a database for single string.
      {
	  for(int i=0; i< so.count() ; i++)
	  {
	      DBItemMainKey sk(so[i]);
	      DBItemMainKey sd(st[i]); //should be a list i.e. main data?
	      sentence->put(&sk,&sd);    
	      
	  } 
      }
      
*/      
    }
    
    
    return false;
    
}



QMap<QString,float> DataBaseInterface::correlation(QString word,SearchFilter *filter,bool notify, float minSign)
{
    QDict<unsigned int> res;
    // res.setAutoDelete(true);
    QMap<QString, float>final;
    DBItemMultiIndex::IndexList il;
    unsigned int tot=0;
    unsigned int background=0;
    unsigned int nocck;
    QString sword=simple(word);
    DBItemMainKey *k = new DBItemMainKey(sword);	 
    DBItemMultiIndex *d = new DBItemMultiIndex();	 
    if(wordsindex->get(k,d)!=DB_NOTFOUND)
    {
	
	il=d->getList();
	kdDebug(0) << il.count()<<endl;
	tot=0;
	for(QValueList<unsigned int>::iterator it=il.begin();it!=il.end();++it)
	{
	    numindex->get(*it,k);
	    
	    
	    // QValueList<QueryResult> trad=exactMatch(k->getString(),filter);	   
	    
	    MainEntry e=get(k->getString(),filter);
	    QStringList trad=e.second.getTranslations();      
	    
	    nocck=words(k->getString()).contains(sword);
	    for( QStringList::iterator it2=trad.begin();it2!=trad.end();++it2)
	    {
		
		QStringList w=words(*it2);
		unsigned int numWords = w.count()*10+1;
		unsigned int wei=100000/sqrt(numWords);  //weight (is the best one?)
		
		background+=(numWords-nocck)*wei; 	
		QDict<uint> count;
		//count.setAutoDelete(true);
		//FIXME:SET AUTODELETE FOR ALL DICTS
		for(QStringList::iterator it1=w.begin();it1!=w.end();it1++)
		{
		    uint *ip;   
		    if(!(ip=count[*it1])) count.insert(*it1,new uint(1));
		    else
			(*ip)++;  
		}
		
		for(QStringList::iterator it1=w.begin();it1!=w.end();it1++)
		{
		    uint *ip;
		    if(*(count[*it1])==nocck) //add only if same number of entry (it cuts articles)
			if(!(ip=res[*it1])) res.insert(*it1,new uint(wei));
		    else
			(*ip)+=wei;  
		}
		
	    }
	}
	
	unsigned int sqrBG=sqrt((1.0*background+1)/10000);
	
	for(QDictIterator<uint> it(res) ; it.current(); ++it)
	{
	    float sign=1.0*(*(it.current()))/(10000.0*sqrBG);
	    if(sign >minSign){
		final[it.currentKey()]=sign;
		kdDebug(0) << it.currentKey() <<" Score:" << 1.0*(*(it.current()))/10000 << "/" <<sqrBG << " = " <<sign << endl;
	    }
	}
	
	kdDebug(0) << "final count " <<final.count()<< endl;  
    }
    
    return final;
}

QStringList DataBaseInterface::words(QString s)
{
    QString str=simple(s);
    QStringList list;

    int pos;

    do {
	pos=str.find(QRegExp("\\s"));
	//   if(!simple(str.left(pos)).isEmpty())
	//     list.append(simple(str.left(pos)));
	if(!str.left(pos).isEmpty())
	    list.append(str.left(pos));
	str=str.remove(0,pos+1);
    } while(!str.isEmpty() && pos != -1);
    
    return list;
}

QString DataBaseInterface::simple(QString str,bool ck)
{
    QString res;
    if(ck)
	res=str;  //case keep
    else
	res=str.lower();   //lowercase
    //FIXME: uncoment the foll. line (check speed)
    res=res.replace(QRegExp("(<(.*)>)(.*)(</\\2>)"),"\\3"); //remove enclosing tags


	//Try to get rid of regexps.
    //    res=res.replace(QRegExp("(('|-|_|\\s|[^\\w%])+)")," "); //strip non-word char
    //    res=res.replace(QRegExp("(('|-|_)+)")," "); //strip non-word char
    //    res=res.replace(QRegExp("[^\\w\\s%]"),""); //strip non-word char

    QString r;
    QChar c;
    bool wasSpace=true;
    uint len=res.length();
    for(uint i=0; i<len;i++)
    {
	c=res[i];
	if(c.isLetterOrNumber())
	{
	    r+=c;
	    wasSpace=false;
	}
	    else
	{
	    if(!wasSpace && c.isSpace())
	    {
		r+=' ';
		wasSpace=true;
	    }
	    else
	    {
		    if(!wasSpace && (c=='-' || c=='\'' || c=='_'))
		    {
			r+=' ';
			wasSpace=true;
		    }
		    else
		    {
			if(c=='%'){
			    r+=c;
			    wasSpace=false;
			}
		    }
		}
	}
	    //	wasSpace=c.isSpace();
	}
    if(r[len-1].isSpace())
	r.truncate(len-1);
    res=r;
    //kdDebug(0) << "Simple: "<<res<< endl;
    //res=res.simplifyWhiteSpace(); //remove double spaces
    //res=res.stripWhiteSpace(); //"  as " -> "as"
    
    // kdDebug(0) << res << endl;
    return res;
}

QStringList DataBaseInterface::sentences(QString s)
{
    QString str=s;
    QStringList list;
    
    //  kdDebug(0) << s << endl;
    
    int pos;
    
    
    do {
	QRegExp re("((\\.|;|\\?|\\!|:)( |$|\\\\n\\n))");
	pos=re.search(str); 
	if(!str.left(pos).isEmpty())
	    list.append(str.left(pos).stripWhiteSpace());
	
	kdDebug(0) << str.left(pos) << endl;
	
	str=str.remove(0,pos+re.cap(1).length());
    } while(!str.isEmpty() && pos != -1);
    
    
    return list;
}

QStringList DataBaseInterface::sentencesSeparator(QString s)
{
    QString str=s;
    QStringList list;
    
    //  kdDebug(0) << s << endl;
    
    int pos;
    
    do {
	QRegExp re;
	re.setPattern("([.:?!;]( |$|\\\\n\\n))");
	pos = re.search(str);
	QString separator=re.cap(1);
	if(pos!=-1){
	    list.append(separator);
	}
	
	str=str.remove(0,pos+1);
    } while(!str.isEmpty() && pos != -1);
    
    return list;
}

bool DataBaseInterface::isUpper(QChar s)
{
    return s==s.upper();
}

bool DataBaseInterface::isLower(QChar s)
{
    return s==s.lower();
}



QString DataBaseInterface::format(QString _s,QString t)
{
    //FIXME use settings
    //FIXME use regexp

    QString s=_s;
	QString noTagT=t.replace(QRegExp("(<(.*)>)(.*)(</\\2>)"),"\\3");
    QChar first=noTagT[noTagT.find(QRegExp("\\w"))];
    bool firstCapital=isUpper(first);

    /*
bool dotsAtEnd=(t.find("...")+3==t.length());
bool gtgtAtEnd=(t.find(">>")+2==t.length());
bool ltltAtEnd=(t.find("<<")==t.length()-2);

bool columnAtEnd=(t.find(":")+1==t.length());
*/
    
    bool allupper=(t.upper()==t);
    
    
    if(firstCapital)
	s[0]=s[0].upper();
    else
	s[0]=s[0].lower();
    
    //if(dotsAtEnd)
    // s+="...";
    
    /*if(gtgtAtEnd)
 s+=">>";
 
if(ltltAtEnd)
 s+="<<";
 
if(columnAtEnd)
 s+=":";
*/
    
    if(allupper)
	s=s.upper();
    
    int pos=t.find(QRegExp("&"));
    if(pos>=0) {
	QChar accel=t[t.find(QRegExp("&"))+1];
	if(accel!='&')
	{
	    
	    pos=s.find(accel,false);
	    if(pos<0)
		pos=0;
	    s.insert(pos,"&");
	}
    }
    s=formatRegExp(s,t,".*(\\.\\.\\.|:|>>|<<|\\.|\\?)$",
		   "^(.*)$",
		   "\\1@CAP1@");
    s=formatRegExp(s,t,"(<(.*)>).*(\\.\\.\\.|:|>>|<<|\\.|\\?)*(</\\2>)$",
		   "^(.*)$",
		   "@CAP1@\\1@CAP3@@CAP4@");
    
    return s;
    
}


QString DataBaseInterface::formatRegExp(QString _s, QString t, QString tre,QString stringSearch,QString stringReplace)
{   
    QString s=_s;
    QRegExp templateRegExp(tre);
    //QString stringSearch = "(.*)!@CAP1@$";  // use @CAP1.. fot caps in templates
    //QString stringReplace = "\\1@CAP1@"; // use \1, \2 for caps in str and @CAP1 fot caps in template
    
    
    if(templateRegExp.exactMatch(t))
    {
	QStringList caps=templateRegExp.capturedTexts();
	int i=0;
	for(QStringList::iterator capit=caps.begin();capit!=caps.end();++capit)
	{
	    QString phRegExp="(?!\\\\)@CAP"+QString::number(i)+"@";
	    //kdDebug(0) << "phRegExp: " << phRegExp << endl;
	    //kdDebug(0) << "cap[" << i << "]: "<< *capit<< endl;
	    
	    stringReplace = stringReplace.replace(QRegExp(phRegExp),*capit);
	    stringSearch = stringSearch.replace(QRegExp(phRegExp),*capit);
	    i++;
	    
	}
	// kdDebug(0) << "stringSearch " << stringSearch << endl;      
	//  kdDebug(0) << "stringReplace " << stringReplace << endl;
	QRegExp stringSearchRegExp = QRegExp(stringSearch);
	//  kdDebug(0) << "before: "<<s<<endl;
	s = s.replace(stringSearchRegExp,stringReplace);
	//  kdDebug(0) << "after: "<<s<<endl;
	
    }
    
    return s;
}

DBItemMultiIndex::IndexList DataBaseInterface::getAlpha( const QString & query )
{
    DBItemMainKey *k = new DBItemMainKey(simple(query));
    DBItemMultiIndex *d = new DBItemMultiIndex();
    alpha->get(k,d);    
    
    return d->getList();
}

DataBaseInterface::MainEntry DataBaseInterface::getFromIndex( uint i )
{
    DBItemMainKey k;
    numindex->get(i,&k);
    return get(k.getString(),0); //FIXME: this is a BUG right now but the filter should be removed
}

DataBaseInterface::MainEntry DataBaseInterface::getSentence( const QString & query )
{ 
    
    static int counter=1;
    counter++;
    DBItemMainKey k(query);
    DBItemMainData d;
    sentence->get(&k,&d);
    if(counter%5==0) kapp->processEvents(100);
    return qMakePair(k,d);
    
}

DBItemMultiIndex::IndexList DataBaseInterface::getWordIndex( const QString & query )
{
    DBItemMainKey k = DBItemMainKey(query);	 
    DBItemMultiIndex d =  DBItemMultiIndex();	 
    if(wordsindex->get(&k,&d)!=DB_NOTFOUND){
	return d.getList();
    }
    else
    {	
	QValueList<unsigned int> tmpList;
	return tmpList; 
    }
    
}



//#include "database.moc.cpp"

