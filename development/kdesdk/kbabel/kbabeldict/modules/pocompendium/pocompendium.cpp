/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2003 by Stanislav Visnovsky
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

#include <qregexp.h>
#include <qtextstream.h>
#include <qtimer.h>

#include "resources.h"
#include "compendiumdata.h"
#include "pocompendium.h"
#include "preferenceswidget.h"
#include "pc_factory.h"

#include <catalog.h>
#include <catalogitem.h>

// ngram length and minimal matching for fuzzy search
#define NGRAM_LEN 3
#define LIM_NGRAM 50  

using namespace KBabel;

static KStaticDeleter< QDict<CompendiumData> > compDataDeleter;
QDict<CompendiumData> *PoCompendium::_compDict = 0;

PoCompendium::PoCompendium(QObject *parent, const char *name)
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
    ignoreFuzzy=true;
    wholeWords=true;

    matchEqual = true;
    matchNGram = true;
    matchIsContained = false;
    matchContains = true;
    matchWords=true;


    loadTimer = new QTimer(this);
    connect(loadTimer,SIGNAL(timeout()),this,SLOT(slotLoadCompendium()));
}

PoCompendium::~PoCompendium()
{
    if(isSearching())
    {
        stopSearch();
    }

    unregisterData();
}

bool PoCompendium::isReady() const
{
    return (isSearching() || !error);
}


bool PoCompendium::isSearching() const
{
    return (active || loading);
}


void PoCompendium::saveSettings(KConfigBase *config )
{
    if(autoUpdate && prefWidget && prefWidget->settingsChanged())
    {
        applySettings();
    }
        
    config->writeEntry("CaseSensitive",caseSensitive);
    config->writeEntry("IgnoreFuzzy", ignoreFuzzy);
    config->writeEntry("WholeWords", wholeWords);

    config->writeEntry("MatchEqual", matchEqual);
    config->writeEntry("MatchIsContained",matchIsContained);
    config->writeEntry("MatchContains", matchContains);
    config->writeEntry("MatchWords", matchWords);
    config->writeEntry("MatchNGram", matchNGram);  

    config->writeEntry("Compendium", url);
}

void PoCompendium::readSettings(KConfigBase *config)
{    
    caseSensitive = config->readBoolEntry("CaseSensitive", false);
    ignoreFuzzy = config->readBoolEntry("IgnoreFuzzy",true);
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

PrefWidget *PoCompendium::preferencesWidget(QWidget *parent)
{
    prefWidget = new CompendiumPreferencesWidget(parent,"pocompendium_prefwidget");
    kdDebug(KBABEL_SEARCH) << "PreferencesWidget is " << prefWidget << endl;
    connect(prefWidget, SIGNAL(applySettings()), this, SLOT(applySettings()));
    connect(prefWidget, SIGNAL(restoreSettings())
                    , this, SLOT(restoreSettings()));

    restoreSettings();
    
    return prefWidget;
}

const KAboutData *PoCompendium::about() const
{
    return PcFactory::instance()->aboutData();
}


QString PoCompendium::name() const
{
    return i18n("PO Compendium");
}

QString PoCompendium::id() const
{
    return "pocompendium";
}

QString PoCompendium::lastError()
{
    return errorMsg;
}

bool PoCompendium::searchExact(const QString& text, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& )
{
    const int *index = data->exactDict(text);
    if(index)
    {
        foundIndices.append(*index);

        SearchResult *result = new SearchResult;
        result->requested = text;
        result->found = data->catalog()->msgid(*index);
#warning	kdWarning() << "PoCompendium can't return plural form translation" << endl;
        result->translation = data->catalog()->msgstr(*index).first();
        result->score = 100;

        TranslationInfo *info = new TranslationInfo;
        info->location = directory(realURL,0);
        info->translator = catalogInfo.lastTranslator;
        info->description = data->catalog()->comment(*index);
        result->descriptions.append(info);

	addResult(result, results);
	return true;
    }
    
    return false;

}

bool PoCompendium::searchCaseInsensitive(const QString& text, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& )
{
    QString searchStr = text.lower();
    
    const QValueList<int> *indexList = data->allDict(text.lower());
    if(indexList)
    {
        QValueList<int>::ConstIterator it;
        for( it = indexList->begin(); it != indexList->end(); ++it )
        {
            if(foundIndices.contains(*it))
            {
                continue;
            }

            if(ignoreFuzzy && data->catalog()->isFuzzy(*it))
            {
                continue;
            }


            QString origStr = data->catalog()->msgid(*it).first();
            origStr = CompendiumData::simplify(origStr);
        
           
            if(!caseSensitive)
            {
                origStr = origStr.lower();
            }

            if(origStr==searchStr)
            {
                foundIndices.append(*it);

                SearchResult *result = new SearchResult;
                result->requested = text;
                result->found = data->catalog()->msgid(*it);
                result->translation = *(data->catalog()->msgstr(*it).at(pluralForm));
                result->score = score(result->requested,*(result->found.at(pluralForm)));

                TranslationInfo *info = new TranslationInfo;
                info->location = directory(realURL,0);
                info->translator = catalogInfo.lastTranslator;
                info->description = data->catalog()->comment(*it);
                result->descriptions.append(info);
		
		addResult(result, results);
		return true;
            }
        }
    }
    return false;
}

bool PoCompendium::searchTextOnly(const QString& text, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& )
{
    QString searchStr = text.lower();
    QString t = text;
    t.remove( " " );

    const QValueList<int> *indexList = data->textonlyDict(t.lower());
    if(indexList)
    {
        QValueList<int>::ConstIterator it;
        for( it = indexList->begin(); it != indexList->end(); ++it )
        {
            if(foundIndices.contains(*it))
            {
                continue;
            }

            if(ignoreFuzzy && data->catalog()->isFuzzy(*it))
            {
                continue;
            }


            QString origStr = data->catalog()->msgid(*it).first();
            origStr = CompendiumData::simplify(origStr);
        
           
            foundIndices.append(*it);

            SearchResult *result = new SearchResult;
            result->requested = text;
            result->found = data->catalog()->msgid(*it).first();
#warning	    kdWarning() << "PoCompendium can't return plural form translation" << endl;
            result->translation = data->catalog()->msgstr(*it).first();
            result->score = score(result->requested,*(result->found.at(pluralForm)));

            TranslationInfo *info = new TranslationInfo;
            info->location = directory(realURL,0);
            info->translator = catalogInfo.lastTranslator;
            info->description = data->catalog()->comment(*it);
            result->descriptions.append(info);
		
	    addResult(result, results);
	    return true;
        }
    }
    return false;
}

bool PoCompendium::searchWords(const QString& searchStr, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& checkedIndices )
{
    uint checkCounter = 0;
    bool foundResults = false;
    
    QStringList wList = CompendiumData::wordList(searchStr);
    for ( QStringList::Iterator wit = wList.begin()
               ; wit != wList.end(); ++wit ) 
    {
        if(stop)
            break;

        const QValueList<int> *indexList = data->wordDict((*wit).lower());
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

                if(ignoreFuzzy && data->catalog()->isFuzzy(*it))
                {
                    continue;
                }
 
                if( (100*(checkCounter+1))%catalogInfo.total < 100)
                {
                    emit progress( (50*checkCounter+1)/catalogInfo.total);
                }
            
                kapp->processEvents(100);

                QString origStr = data->catalog()->msgid(*it).first();
                origStr = CompendiumData::simplify(origStr);

           
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
                    (origStr.length() == 1 && searchStr.length() > 1))
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
                    QStringList list = CompendiumData::wordList(searchStr);
                    
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
                    result->requested = searchStr; // FIXME:here should be the original text
                    result->found = data->catalog()->msgid(*it).first();
#warning		    kdWarning() << "PoCompendium can't return plural form translation" << endl;
                    result->translation = data->catalog()->msgstr(*it).first();
                    result->score = score(result->requested,*(result->found.at(pluralForm)));

                    TranslationInfo *info = new TranslationInfo;
                    info->location = directory(realURL,0);
                    info->translator = catalogInfo.lastTranslator;
                    info->description = data->catalog()->comment(*it);
                    result->descriptions.append(info);
 
                    addResult(result, results);
		    
		    foundResults = true;
                }
            }
        }
    }
    
    return foundResults;
}

bool PoCompendium::searchNGram(const QString& searchStr, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& checkedIndices )
{
    uint checkCounter = 0;
    bool foundResults = false;
    
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
            
        if(i >= catalogInfo.total)
        {
            breakLoop = true;
            break;
        }
        
        if( (100*(checkCounter+1))%catalogInfo.total < 100)
        {
            emit progress( 50+(50*(checkCounter+1))/catalogInfo.total);
        }

        if(checkedIndices.contains(i))
        {
            continue;
        }

        checkedIndices.append(i);
        checkCounter++;
  
        if(ignoreFuzzy && data->catalog()->isFuzzy(i))
        {
    	     continue;
        }
        
        if(foundIndices.contains(i))
        {
            continue;
        }
       
        kapp->processEvents(100);

        QString origStr = data->catalog()->msgid(i).first();
        origStr = CompendiumData::simplify(origStr);
            
        if(!caseSensitive)
        {
            origStr = origStr.lower();
        }

 
        // if there is an string which just contains ignored characters,
        // continue
        if(origStr.isEmpty() || 
                    (origStr.length() == 1 && searchStr.length() > 1))
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
            QStringList list = CompendiumData::wordList(searchStr);
                    
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
            result->requested = searchStr;
            result->found = data->catalog()->msgid(i).first();
            result->translation = data->catalog()->msgstr(i).first();
#warning	    kdWarning() << "PoCompendium can't return plural form translation" << endl;
            result->score = score(result->requested,*(result->found.at(pluralForm)));

            TranslationInfo *info = new TranslationInfo;
            info->location = directory(realURL,0);
            info->translator = catalogInfo.lastTranslator;
            info->description = data->catalog()->comment(i);
            result->descriptions.append(info);
    
            addResult(result, results);
	    
	    foundResults = true;
        }
    }
    
    return foundResults;
}

bool PoCompendium::startSearch(const QString& text, uint pluralForm, const SearchFilter* filter)
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
    
    // first, exact search
    searchExact(text, pluralForm, results, foundIndices, checkedIndices);
    
    QString searchStr=CompendiumData::simplify(text);
    
           
    if(!caseSensitive)
    {
         searchStr = searchStr.lower();

	// do case insensite search if necessary
	searchCaseInsensitive(searchStr, pluralForm, results, foundIndices, checkedIndices);
    }
    
    // search without whitespace
    searchTextOnly(searchStr, pluralForm, results, foundIndices, checkedIndices);
    
    // now, search based on words (contains, is contained, etc)
    searchWords(searchStr, pluralForm, results, foundIndices, checkedIndices);
 
    
    if( matchNGram || 
	(!wholeWords && (matchContains || matchIsContained || matchWords))
	)
    {
	// search based on ngrams
	searchNGram(searchStr, pluralForm, results, foundIndices, checkedIndices);
    } 
    
    emit progress(100);

    active = false;
    stop = false;
    emit finished();
    
    return true;
}

void PoCompendium::stopSearch()
{
    stop=true;
}


void PoCompendium::applySettings()
{    
    if(!prefWidget)
        return;
    
    if(isSearching())
        stopSearch();
    
    caseSensitive = prefWidget->caseSensitive();
    ignoreFuzzy = prefWidget->ignoreFuzzy();
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

void PoCompendium::restoreSettings()
{
    if(!prefWidget)
        return;
    
    prefWidget->setCaseSensitive(caseSensitive);
    prefWidget->setIgnoreFuzzy(ignoreFuzzy);
    prefWidget->setWholeWords(wholeWords);
    prefWidget->setURL(url);

    prefWidget->setMatchEqual(matchEqual);
    prefWidget->setMatchNGram(matchNGram);
    prefWidget->setMatchIsContained(matchIsContained);
    prefWidget->setMatchContains(matchContains);
    prefWidget->setMatchWords(matchWords);
}
    
void PoCompendium::loadCompendium()
{
    if(!loading && !loadTimer->isActive())
        loadTimer->start(100,true);
}

void PoCompendium::slotLoadCompendium()
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
        kdError() << "no data object in pocompendium?" << endl;

        loading=false;
        return;
    }

    if(!data->initialized())
    {
        if(!data->active())
        {
            data->load(u);
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

void PoCompendium::recheckData()
{
    if(data)
    {
        disconnect(data, SIGNAL(progressEnds()), this, SLOT(recheckData()));

        error = data->hasErrors();
        errorMsg = data->errorMsg();

        if(!error)
        {
            catalogInfo = Catalog::headerInfo(data->catalog()->header());
            catalogInfo.total = data->catalog()->numberOfEntries();
            catalogInfo.fuzzy = data->catalog()->numberOfFuzzies();
            catalogInfo.untranslated = data->catalog()->numberOfUntranslated();
        }
    }

    loading=false;
}

QString PoCompendium::maskString(QString s)
{
    s.replace("\\","\\\\");
    s.replace("\?","\\?");
    s.replace("[","\\[");
    s.replace(".","\\.");
    s.replace("*","\\*");
    s.replace("+","\\+");
    s.replace("^","\\^");
    s.replace("$","\\$");
    s.replace("(","\\(");
    s.replace(")","\\)");
    s.replace("{","\\{");
    s.replace("}","\\}");
    s.replace("|","\\|");
    
    return s;
}

void PoCompendium::addResult(SearchResult *result, QPtrList<SearchResult>& res)
{
    if(res.last() && res.last()->score >= result->score)
    {
        res.append(result);
    }
    else
    {
        SearchResult *sr;
        for(sr = res.first(); sr != 0; sr=res.next())
        {
            if(sr->score < result->score)
            {
		int pos = res.at();
		if( pos < 0 ) pos = 0;
                res.insert( pos,result);
                emit resultsReordered();
                break;
            }
        }

        if(!sr)
        {
             res.append(result);
        }
    }

    emit numberOfResultsChanged(res.count());
    emit resultFound(result);
}


void PoCompendium::setLanguageCode(const QString& lang)
{   
    if(initialized && url.contains("@LANG@") && lang!=langCode 
            && !loadTimer->isActive() )
    {
        initialized=false;
    }
    
    langCode=lang;
}

QString PoCompendium::translate(const QString& text, uint pluralForm)
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
#warning	kdWarning() << "PoCompendium can't return plural form translation" << endl;
        return data->catalog()->msgstr(*index).first();
    }
   
    return QString::null;
}

QString PoCompendium::fuzzyTranslation(const QString& text, int &score, const uint pluralForm)
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
  int total = data->catalog()->numberOfEntries();
 
  QString searchStr = CompendiumData::simplify(text);

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

      // get a message from the catalog FIXME: plurals
      QString origStr = data->catalog()->msgid(i).first();
      origStr = CompendiumData::simplify(origStr);

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
#warning    kdWarning() << "PoCompendium can't return plural form translation" << endl;
    return data->catalog()->msgstr(best_matching).first();
  }
 
  return QString::null;
}


QString PoCompendium::searchTranslation(const QString& text, int &sc, const uint pluralForm)
{
    if(autoUpdate && prefWidget && prefWidget->settingsChanged())
    {
        applySettings();
    }
 
    if(isSearching())
        return QString::null;

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
        return QString::null;
    }

    if(data->active())
    {
        active = false;
        return QString::null;
    }
   
    emit started();

    QPtrList<SearchResult> res;
    
    QValueList<int> foundIndices;
    QValueList<int> checkedIndices;
    
    // first, exact search
    if( searchExact(text, pluralForm, res, foundIndices, checkedIndices) )
    {
	active = false;
	stop = false;
	emit finished();

	// found, this is the best
	sc = res.first()->score;
	return res.first()->translation;
    }
    
    QString searchStr=CompendiumData::simplify(text);
    
           
    if(!caseSensitive)
    {
        searchStr = searchStr.lower();

	// do case insensite search if necessary
	if( searchCaseInsensitive(searchStr, pluralForm, res, foundIndices, checkedIndices) )
	{
	    active = false;
	    stop = false;
	    emit finished();
	    
	    // found, return this one
	    sc = res.first()->score;
	    return res.first()->translation;
	}
    }
    
    // search without whitespace
    QString s = searchStr;
    s.remove( ' ' );
    if( searchTextOnly(s, pluralForm, res, foundIndices, checkedIndices) )
    {
	active = false;
	stop = false;
	emit finished();

	// found, return this one
	sc = res.first()->score;
	return res.first()->translation;
    }
    
    // now, search based on words (contains, is contained, etc)
    searchWords(searchStr, pluralForm, res, foundIndices, checkedIndices);
    
    if( matchNGram || 
	(!wholeWords && (matchContains || matchIsContained || matchWords))
	)
    {
	// search based on ngrams
	searchNGram(searchStr, pluralForm, res, foundIndices, checkedIndices);
    }
    
    active = false;
    stop = false;

    // now, pick up the best one from not exact translations
    if( res.count() > 0 ) {

	emit finished();

	sc = res.first()->score;
	return res.first()->translation;
    }
    
    sc = 0;
    
    return QString::null;
}


void PoCompendium::unregisterData()
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

void PoCompendium::registerData()
{
    data = compendiumDict()->find(realURL);
    if(!data)
    {
        data = new CompendiumData;
        compendiumDict()->insert(realURL,data);
    }

    data->registerObject(this);
    
    if(data->active())
    {
        emit progressStarts(i18n("Loading PO compendium"));
    }
    
    connect(data, SIGNAL(
    progressStarts(const QString&)), this
            , SIGNAL(progressStarts(const QString&)));
    connect(data, SIGNAL(progressEnds()), this , SIGNAL(progressEnds()));
    connect(data, SIGNAL(progress(int)), this , SIGNAL(progress(int)));
}

void PoCompendium::removeData()
{
    const QObject *s=sender();
    if(s && s->inherits("CompendiumData"))
    {
        const CompendiumData *d=static_cast<const CompendiumData*>(s);
        if(d)
        {
            QDictIterator<CompendiumData> it(*compendiumDict());
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

QDict<CompendiumData> *PoCompendium::compendiumDict()
{
    if(!_compDict)
    {
        _compDict=compDataDeleter.setObject( new QDict<CompendiumData> );
        _compDict->setAutoDelete(true);
    }

    return _compDict;
}



#include "pocompendium.moc"
