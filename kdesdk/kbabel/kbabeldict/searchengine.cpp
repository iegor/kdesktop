/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

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


#include "searchengine.h"

#include <kdebug.h>

SearchResult::SearchResult()
{
    score=0;
    descriptions.setAutoDelete(true);
}

SearchResult::SearchResult(const SearchResult& r)
{
    requested=r.requested;
    found=r.found;
    translation=r.translation;
    plainTranslation=r.plainTranslation;
    plainFound=r.plainFound;
    plainRequested=r.plainRequested;
    descriptions.setAutoDelete(true);
    score=r.score;

    QPtrListIterator<TranslationInfo> it(r.descriptions);
    for ( ; it.current(); ++it )
    {
        TranslationInfo *ti=new TranslationInfo(*(*it));
        descriptions.append(ti);
    }
        
}

PrefWidget::PrefWidget(QWidget *parent, const char *name)
		  : QWidget(parent,name)
{
}

PrefWidget::~PrefWidget()
{
}


SearchEngine::SearchEngine(QObject *parent, const char *name)
		: QObject(parent, name)
{
    autoUpdate=false;
    results.setAutoDelete(true);
}

SearchEngine::~SearchEngine()
{
}

bool SearchEngine::startSearchInTranslation(const QString&, unsigned int, const SearchFilter*)
{
    return true;
}

bool SearchEngine::usesRichTextResults()
{
	return false;
}

bool SearchEngine::isEditable()
{
	return false;
}

void SearchEngine::edit()
{
}

void SearchEngine::setAutoUpdateOptions(bool activate)
{
	autoUpdate=activate;
}

SearchResult *SearchEngine::result(int n) 
{
	SearchResult *r=0;
	
	if(n >= 0 && n < (int)results.count())
		r = results.at(n);
		
	return r;
}

int SearchEngine::numberOfResults() const
{
	return results.count();
}

SearchResult *SearchEngine::nextResult() 
{
	SearchResult *r= results.next();
	if(r)
	   return r;
	else
	{
		return 0;
	}
}

SearchResult *SearchEngine::prevResult()
{
	SearchResult *r= results.prev();
	if(r)
	   return r;
	else
	{
		return 0;
	}
}

SearchResult *SearchEngine::firstResult()
{
	SearchResult *r= results.first();
	if(r)
	   return r;
	else
	{
	   return 0;
	}
}


SearchResult *SearchEngine::lastResult()
{
	SearchResult *r= results.last();
	if(r)
	   return r;
	else
	{
	   return 0;
	}
}

void SearchEngine::clearResults()
{
	results.clear();
	emit numberOfResultsChanged(0);
}



void SearchEngine::setEditedFile(const QString&)
{
}


void SearchEngine::setEditedPackage(const QString&)
{
}

void SearchEngine::setLanguageCode(const QString&)
{
}

void SearchEngine::setLanguage(const QString&,const QString&)
{
}



void SearchEngine::stringChanged(const QStringList&,const QString&,unsigned int, const QString&)
{
}


QString SearchEngine::directory(const QString& path, int n)
{
	if(n<0)
		return QString::null;

	int begin=path.findRev('/');
	int end=path.length();

	if(begin < 0)
	{
		kdDebug() << "not found" << endl;
		if(n==0)
			return path;
		else
			return QString::null;
	}

	for(int i=0; i < n; i++)
	{
		if(begin==0)
		{
			return QString::null;
		}

		end = begin;
		begin = path.findRev('/', end-1);

		if(begin < 0)
		{
			return QString::null;
		}
	}
	
	begin++;
	return path.mid(begin,end-begin);
}


uint SearchEngine::score(const QString& orig, const QString& found)
{
    if(orig == found)
        return 100;
  
    uint scoreA = ngramMatch(found, orig);
    uint scoreB = ngramMatch(orig,found);
    uint score = QMIN(scoreA, scoreB);

    QString f = found.lower();
    QString o = orig.lower();

    scoreA = ngramMatch(f, o);
    scoreB = ngramMatch(o, f);
    score = (QMIN(scoreA, scoreB)+score)/2;
  
    return score;
}

uint SearchEngine::ngramMatch (const QString& text1, const QString& text2, 
			       uint ngram_len)
{
  if (text1.length() < ngram_len || text2.length() < ngram_len)
    return 0;

  uint cnt=0;
  uint ngram_cnt = text1.length() - ngram_len + 1;

  for (uint i = 0; i < ngram_cnt; i++) {
    if (text2.find(text1.mid(i,ngram_len)) >= 0)
      cnt++;
  }

  return (cnt*100) / ngram_cnt;
}


bool SearchFilter::match( const TranslationInfo& toCheck )
{
    // FIXME: not implemented yet
    return true;
}

#include "searchengine.moc"
