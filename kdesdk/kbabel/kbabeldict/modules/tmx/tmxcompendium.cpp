/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002 by Stanislav Visnovsky
			    <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */
#include <klocale.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kstaticdeleter.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kinstance.h>
#include <kio/netaccess.h>

#include <qtextstream.h>
#include <qtimer.h>

#include "tmxcompendiumdata.h"
#include "tmxcompendium.h"
#include "preferenceswidget.h"
#include "pc_factory.h"

#include <catalogitem.h>

// ngram length and minimal matching for fuzzy search
#define NGRAM_LEN 3
#define LIM_NGRAM 50  

static KStaticDeleter< QDict<TmxCompendiumData> > compDataDeleter;
QDict<TmxCompendiumData> *TmxCompendium::_compDict = 0;

TmxCompendium::TmxCompendium(QObject *parent, const char *name)
        : SearchEngine(parent, name)
{
    prefWidget=0;
    data=0;
    error=false;
    stop=false;
    active=false;
    initialized=false;
    loading=false;

    langCode = KGlobal::locale()->language();

    caseSensitive = false;
    wholeWords=true;

    matchEqual = true;
    matchNGram = true;
    matchIsContained = false;
    matchContains = true;
    matchWords=true;


    loadTimer = new QTimer(this);
    connect(loadTimer,SIGNAL(timeout()),this,SLOT(slotLoadCompendium()));
}

TmxCompendium::~TmxCompendium()
{
    if(isSearching())
    {
        stopSearch();
    }

    unregisterData();
}

bool TmxCompendium::isReady() const
{
    return (isSearching() || !error);
}


bool TmxCompendium::isSearching() const
{
    return (active || loading);
}


void TmxCompendium::saveSettings(KConfigBase *config)
{
    if(autoUpdate && prefWidget && prefWidget->settingsChanged())
    {
        applySettings();
    }
        
    config->writeEntry("CaseSensitive",caseSensitive);
    config->writeEntry("WholeWords", wholeWords);

    config->writeEntry("MatchEqual", matchEqual);
    config->writeEntry("MatchIsContained",matchIsContained);
    config->writeEntry("MatchContains", matchContains);
    config->writeEntry("MatchWords", matchWords);
    config->writeEntry("MatchNGram", matchNGram);  

    config->writeEntry("Compendium", url);
}

void TmxCompendium::readSettings(KConfigBase *config)
{    
    caseSensitive = config->readBoolEntry("CaseSensitive", false);
    wholeWords = config->readBoolEntry("WholeWords",true);

    matchEqual = config->readBoolEntry("MatchEqual", true);
    matchIsContained = config->readBoolEntry("MatchIsContained", false);
    matchContains = config->readBoolEntry("MatchContains",true);
    matchWords = config->readBoolEntry("MatchWords",true);
    matchNGram = config->readBoolEntry("MatchNGram",true);  

    QString newPath = config->readEntry("Compendium","http://i18n.kde.org/po_overview/@LANG@.messages");
    if(!initialized)
    {
        url = newPath;
    }
    else if(newPath != url)
    {
        url = newPath;
        loadCompendium();
    }

            
    restoreSettings();
}

PrefWidget *TmxCompendium::preferencesWidget(QWidget *parent)
{
    prefWidget = new TmxCompendiumPreferencesWidget(parent,"tmxcompendium_prefwidget");
    connect(prefWidget, SIGNAL(applySettings()), this, SLOT(applySettings()));
    connect(prefWidget, SIGNAL(restoreSettings())
                    , this, SLOT(restoreSettings()));

    restoreSettings();
    
    return prefWidget;
}

const KAboutData *TmxCompendium::about() const
{
    return PcFactory::instance()->aboutData();
}


QString TmxCompendium::name() const
{
    return i18n("TMX Compendium");
}

QString TmxCompendium::id() const
{
    return "tmxcompendium";
}

QString TmxCompendium::lastError()
{
    return errorMsg;
}


bool TmxCompendium::startSearch(const QString& text, uint pluralForm, const SearchFilter*)
{
    if(autoUpdate && prefWidget && prefWidget->settingsChanged())
    {
        applySettings();
    }
 
    if(isSearching())
        return false;

    clearResults();
    stop = false;
    active = true;

    if(!initialized)
    {
        if(loadTimer->isActive())
            loadTimer->stop();

        slotLoadCompendium();
    }
    
    if(error || !data)
    {
        active = false;
        return false;
    }

    if(data->active())
    {
        active = false;
        return true;
    }
   
    emit started();


    QValueList<int> foundIndices;
    QValueList<int> checkedIndices;
    uint checkCounter=0;
    
    const int *index = data->exactDict(text);
    if(index)
    {
        foundIndices.append(*index);

        SearchResult *result = new SearchResult;
        result->requested = text;
        result->found = data->msgid(*index);
        result->translation = data->msgstr(*index);
        result->score = 100;

        TranslationInfo *info = new TranslationInfo;
        info->location = directory(realURL,0);
        info->translator = QString::null;
        info->description = QString::null;
        result->descriptions.append(info);

        results.append(result);

        emit numberOfResultsChanged(results.count());
        emit resultFound(result);
    }

    QString searchStr=TmxCompendiumData::simplify(text);
    
           
    if(!caseSensitive)
    {
         searchStr = searchStr.lower();
    }

    QString temp = searchStr;
    temp = temp.lower();

    const QValueList<int> *indexList = data->allDict(temp);
    if(indexList)
    {
        QValueList<int>::ConstIterator it;
        for( it = indexList->begin(); it != indexList->end(); ++it )
        {
            if(foundIndices.contains(*it))
            {
                continue;
            }

            QString origStr = data->msgid(*it);
            origStr = TmxCompendiumData::simplify(origStr);
        
           
            if(!caseSensitive)
            {
                origStr = origStr.lower();
            }

            if(origStr==searchStr)
            {
                foundIndices.append(*it);

                SearchResult *result = new SearchResult;
                result->requested = text;
                result->found = data->msgid(*it);
                result->translation = data->msgstr(*it);
		// FIXME: handle plural forms properly
                result->score = score(result->requested,result->found.first());

                TranslationInfo *info = new TranslationInfo;
                info->location = directory(realURL,0);
                info->translator = QString::null;
                info->description = QString::null;
                result->descriptions.append(info);
 
                addResult(result);
            }
        }
    }
 
    QStringList wList = TmxCompendiumData::wordList(searchStr);
    for ( QStringList::Iterator wit = wList.begin()
               ; wit != wList.end(); ++wit ) 
    {
        if(stop)
            break;

        indexList = data->wordDict((*wit).lower());
        if(indexList)
        {
            QValueList<int>::ConstIterator it;
            for( it = indexList->begin(); it != indexList->end(); ++it )
            {
                if(stop)
                    break;
                
                if(foundIndices.contains(*it))
                {
                    continue;
                }
              
                if(checkedIndices.contains(*it))
                {
                    continue;
                }
                
                checkedIndices.append(*it);
                checkCounter++;

                if( (100*(checkCounter+1))%data->numberOfEntries() < 100)
                {
                    emit progress( (100*(checkCounter+1))/data->numberOfEntries());
                }
            
                kapp->processEvents(100);

                QString origStr = data->msgid(*it);
                origStr = TmxCompendiumData::simplify(origStr);

           
                if(!caseSensitive)
                {
                    origStr = origStr.lower();
                }

                bool found = false;
                if(matchWords)
                {
                    if(!caseSensitive)
                    {
                        found=true;
                    }
                    else
                    {
                        QString s=*wit;
                        QString o=origStr;
                        
                        if(wholeWords)
                        {
                            s=" "+*wit+" ";
                            o=" "+origStr+" ";
                        }
                        if(o.contains(s))
                        {
                            found=true;
                        }
                    }
                }

                
                if(!found && origStr==searchStr)
                {
                    found =true;
                }
                 
                // if there is an string which just contains ignored characters,
                // continue
                if(!found && origStr.isEmpty() || 
                    (origStr.length() == 1 && text.length() > 1))
                {
                    continue;
                }
            

                if(!found && matchContains && !wholeWords)
                {
                    QString s=maskString(searchStr);
                    QRegExp searchReg(s);

                    if(searchReg.search(origStr) >= 0)
                        found=true;
                }
            
                if(!found && matchIsContained && !wholeWords)
                {
                    QString s=maskString(origStr);
                    QRegExp reg(s);

                    if(reg.search(searchStr) >= 0)
                    {
                        found = true;
                    }
                }
                if(!found && matchWords && !wholeWords)
                {
                    QStringList list = TmxCompendiumData::wordList(searchStr);
                    
                    for ( QStringList::Iterator wit2 = list.begin()
                       ; wit2 != list.end(); ++wit2 ) 
                    {
                        QString s=maskString(*wit2);
                        QRegExp reg(s);

                        if(reg.search(origStr) >= 0)
                        {
                            found = true;
                        }
                    }
                }

                if(found)
                {
                    foundIndices.append(*it);

                    SearchResult *result = new SearchResult;
                    result->requested = text;
                    result->found = data->msgid(*it);
                    result->translation = data->msgstr(*it);
		    // FIXME: handle plural forms properly
                    result->score = score(result->requested,(result->found.first()));

                    TranslationInfo *info = new TranslationInfo;
                    info->location = directory(realURL,0);
                    info->translator = QString::null;
                    info->description = QString::null;
                    result->descriptions.append(info);
 
                    addResult(result);
                }
            }
        }
    }
 
    
    if( matchNGram || 
	(!wholeWords && (matchContains || matchIsContained || matchWords))
	)
      {
        QRegExp searchReg;
        if(matchContains)
        {
            QString s=maskString(searchStr);
            searchReg.setPattern(s);
        }

        
        bool breakLoop=false;
        int i=-1;
        while(!breakLoop)    
        {
            if(stop)
            {
                breakLoop=true;
                break;
            }

            i++;
            
            if(checkedIndices.contains(i))
            {
                continue;
            }

            checkedIndices.append(i);
            checkCounter++;
  
            if(foundIndices.contains(i))
            {
                 continue;
            }
       
            kapp->processEvents(100);

            if(i >= data->numberOfEntries())
            {
                 breakLoop = true;
                 break;
            }
        
            if( (100*(checkCounter+1))%data->numberOfEntries() < 100)
            {
                emit progress( (100*(checkCounter+1))/data->numberOfEntries());
            }

             QString origStr = data->msgid(i);
             origStr = TmxCompendiumData::simplify(origStr);
            
             if(!caseSensitive)
             {
                 origStr = origStr.lower();
             }

 
            // if there is an string which just contains ignored characters,
            // continue
            if(origStr.isEmpty() || 
                    (origStr.length() == 1 && text.length() > 1))
            {
                continue;
            }
            
            bool found = false;
            if(matchContains && searchReg.search(origStr) >= 0)
            {
                found=true;
            }

                    
            if(!found && matchIsContained)
            {
                QString s=maskString(origStr);
                QRegExp reg(s);

                if(reg.search(searchStr) >= 0)
                {
                    found = true;
                }
            }
            if(!found && matchWords)
            {
                QStringList list = TmxCompendiumData::wordList(searchStr);
                    
                for ( QStringList::Iterator wit2 = list.begin()
                   ; wit2 != list.end(); ++wit2 ) 
                {
                    QString s=maskString(*wit2);

                    if(wholeWords)
                    {
                        origStr = " "+origStr+" ";
                        s=" "+s+" ";
                    }
                    QRegExp reg(s);
                    
                    if(reg.search(origStr) >= 0)
                    {
                        found = true;
                    }
                }
             }

	    if(!found && matchNGram)
	      {
		// to get more results one could
		// interchange searchStr and origStr when
		// the latter is shorter

		found = ( ngramMatch(searchStr,origStr,NGRAM_LEN) 
			  > LIM_NGRAM );
	      }
       
            if(found)
            {
                 foundIndices.append(i);

                 SearchResult *result = new SearchResult;
                 result->requested = text;
                 result->found = data->msgid(i);
                 result->translation = data->msgstr(i);
		 // FIXME: handle plural forms properly
                 result->score = score(result->requested,(result->found.first()));

                 TranslationInfo *info = new TranslationInfo;
                 info->location = directory(realURL,0);
                 info->translator = QString::null;
                 info->description = QString::null;
                 result->descriptions.append(info);
    
                 addResult(result);
             }
        }
    } 
    
    if( (100*(checkCounter+1))/data->numberOfEntries() < 100 && !stop)
    {
        for(int i=(100*(checkCounter+1))/data->numberOfEntries(); i<=100; i++)
        {
            emit progress(i);
        }
    }
    

    active = false;
    stop = false;
    emit finished();
    
    return true;
}

void TmxCompendium::stopSearch()
{
    stop=true;
}


void TmxCompendium::applySettings()
{    
    if(!prefWidget)
        return;
    
    if(isSearching())
        stopSearch();
    
    caseSensitive = prefWidget->caseSensitive();
    wholeWords = prefWidget->wholeWords();

    matchEqual = prefWidget->matchEqual();
    matchNGram = prefWidget->matchNGram();
    matchIsContained = prefWidget->matchIsContained();
    matchContains = prefWidget->matchContains();
    matchWords = prefWidget->matchWords();

    
    bool needLoading=false;
    
    
    QString newPath = prefWidget->url();
    if(!initialized)
    {
        url = newPath;
    }
    else if(newPath != url)
    {
        url = newPath;
        needLoading = true;
    }

    if(needLoading)
    {
        loadCompendium();
        initialized=false;
    }
}

void TmxCompendium::restoreSettings()
{
    if(!prefWidget)
        return;
    
    prefWidget->setCaseSensitive(caseSensitive);
    prefWidget->setWholeWords(wholeWords);
    prefWidget->setURL(url);

    prefWidget->setMatchEqual(matchEqual);
    prefWidget->setMatchNGram(matchNGram);
    prefWidget->setMatchIsContained(matchIsContained);
    prefWidget->setMatchContains(matchContains);
    prefWidget->setMatchWords(matchWords);
}
    
void TmxCompendium::loadCompendium()
{
    if(!loading && !loadTimer->isActive())
        loadTimer->start(100,true);
}

void TmxCompendium::slotLoadCompendium()
{
    if(loading)
        return;

    if(loadTimer->isActive())
        loadTimer->stop();
    
    loading = true;

    if(data)
    {
        unregisterData();
    }


    QString path=url;
   
    if(path.contains("@LANG@"))
    {
        path.replace("@LANG@",langCode);
    }
    KURL u=KCmdLineArgs::makeURL(path.local8Bit());
    realURL = u.url();
  
    registerData();
    

    if(!data)
    {
        kdError() << "no data object in tmxcompendium?" << endl;

        loading=false;
        return;
    }

    if(!data->initialized())
    {
        if(!data->active())
        {
            data->load(u,langCode);
            recheckData();
            if(error)
            {
                emit hasError(errorMsg);
            }
        }
        else
        {
            connect(data, SIGNAL(progressEnds()), this, SLOT(recheckData()));
        }
    }
    else
    {
        recheckData();
        if(error)
        {
            emit hasError(errorMsg);
        }
    }

    initialized=true;
}

void TmxCompendium::recheckData()
{
    if(data)
    {
        disconnect(data, SIGNAL(progressEnds()), this, SLOT(recheckData()));

        error = data->hasErrors();
        errorMsg = data->errorMsg();
    }

    loading=false;
}

QString TmxCompendium::maskString(QString s) const
{
    s.replace('\\',"\\\\");
    s.replace('?',"\\?");
    s.replace('[',"\\[");
    s.replace('.',"\\.");
    s.replace('*',"\\*");
    s.replace('+',"\\+");
    s.replace('^',"\\^");
    s.replace('$',"\\$");
    s.replace('(',"\\(");
    s.replace(')',"\\)");
    s.replace('{',"\\{");
    s.replace('}',"\\}");
    s.replace('|',"\\|");
    
    return s;
}

void TmxCompendium::addResult(SearchResult *result)
{
    if(results.last() && results.last()->score >= result->score)
    {
        results.append(result);
    }
    else
    {
        SearchResult *sr;
        for(sr = results.first(); sr != 0; sr=results.next())
        {
            if(sr->score < result->score)
            {
                results.insert(results.at(),result);
                emit resultsReordered();
                break;
            }
        }

        if(!sr)
        {
             results.append(result);
        }
    }

    emit numberOfResultsChanged(results.count());
    emit resultFound(result);
}


void TmxCompendium::setLanguageCode(const QString& lang)
{   
    if(initialized && url.contains("@LANG@") && lang!=langCode 
            && !loadTimer->isActive() )
    {
        initialized=false;
    }
    
    langCode=lang;
}

QString TmxCompendium::translate(const QString& text, uint pluralForm)
{
    if(!initialized)
    {
        if(loadTimer->isActive())
            loadTimer->stop();

        slotLoadCompendium();
    }
    
    if(error || !data || data->active())
    {
        return QString::null;
    }
   

    const int *index = data->exactDict(text);

    if(index)
    {
        return data->msgstr(*index);
    }
   
    return QString::null;
}

QString TmxCompendium::fuzzyTranslation(const QString& text, int &score, uint pluralForm)
{
  if(!initialized)
    {
      if(loadTimer->isActive())
	loadTimer->stop();

      slotLoadCompendium();
    }
    
  if(error || !data || data->active())
    {
      return QString::null;
    }
    
  // try to find fuzzy string
  bool breakLoop = false;
  stop = false;
  int i=-1;
  int best_matching = -1;
  int best_match = 0;
  int total = data->numberOfEntries();
 
  QString searchStr = TmxCompendiumData::simplify(text);

  //kdDebug(750) << "find best match for " << searchStr << endl;

  while(!breakLoop)
    {

      // progress and loop control
      if(stop)
	{
	  breakLoop=true;
	  break;
	}

      i++;

      if(i >= total)
	{
	  breakLoop = true;
	  break;
	}

      if( (100*(i+1))%total < 100)
	{
	  emit progress( (100*(i+1))/total);
	}

      // get a message from the catalog
      QString origStr = data->msgid(i);
      origStr = TmxCompendiumData::simplify(origStr);

      // don't match too long strings for short search string
      if (origStr.length() > 2*searchStr.length())
	continue;
      //      kdDebug(750) << i << ": matching " << origStr << endl;

      int ngram_result = ngramMatch(searchStr,origStr);

      if (ngram_result > best_match) {
	best_match = ngram_result;
	best_matching = i;

	// kdDebug(750) << "[" << ngram_result << "] " << text << "-"
	// << origStr << endl;
      }
    }

  if (best_match > LIM_NGRAM) {
    score = best_match;
    return data->msgstr(best_matching);
  }
 
  return QString::null;
}


void TmxCompendium::unregisterData()
{
    if(data)
    { 
        disconnect(data, SIGNAL(progressStarts(const QString&)), this
            , SIGNAL(progressStarts(const QString&)));
        disconnect(data, SIGNAL(progressEnds()), this , SIGNAL(progressEnds()));
        disconnect(data, SIGNAL(progress(int)), this , SIGNAL(progress(int)));

        if(data->active())
        {
            disconnect(data,SIGNAL(progressEnds()),this,SLOT(recheckData()));
        }

        if(data->unregisterObject(this))
        {
            if(!data->active())
            {
                compendiumDict()->remove(realURL);
            }
            else
            {
                connect(data,SIGNAL(progressEnds()),this,SLOT(removeData()));
            }
        }

        data=0;
    }
}

void TmxCompendium::registerData()
{
    data = compendiumDict()->find(realURL);
    if(!data)
    {
        data = new TmxCompendiumData;
        compendiumDict()->insert(realURL,data);
    }

    data->registerObject(this);
    
    if(data->active())
    {
        emit progressStarts(i18n("Loading PO compendium"));
    }
    
    connect(data, SIGNAL(progressStarts(const QString&)), this
            , SIGNAL(progressStarts(const QString&)));
    connect(data, SIGNAL(progressEnds()), this , SIGNAL(progressEnds()));
    connect(data, SIGNAL(progress(int)), this , SIGNAL(progress(int)));
}

void TmxCompendium::removeData()
{
    const QObject *s=sender();
    if(s && s->inherits("TmxCompendiumData"))
    {
        const TmxCompendiumData *d=static_cast<const TmxCompendiumData*>(s);
        if(d)
        {
            QDictIterator<TmxCompendiumData> it(*compendiumDict());
            while(it.current())
            {
                if(it.current() == d)
                {
                    if(!d->hasObjects())
                    {
                        compendiumDict()->remove(it.currentKey());
                    }

                    break;
                }

                ++it;
            }
        }
        
    }
}

QDict<TmxCompendiumData> *TmxCompendium::compendiumDict()
{
    if(!_compDict)
    {
        _compDict=compDataDeleter.setObject( new QDict<TmxCompendiumData> );
        _compDict->setAutoDelete(true);
    }

    return _compDict;
}



#include "tmxcompendium.moc"
