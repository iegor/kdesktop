/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "cachedcatalog.moc"

#include <qstringlist.h>
#include <qstring.h>

#include "cachedcatalog.h"
#include "katapultitem.h"
#include "match.h"
#include "status.h"

CachedCatalog::CachedCatalog()
	: KatapultCatalog()
{
	cache.setAutoDelete(TRUE);
	results.setAutoDelete(FALSE);
}

CachedCatalog::~CachedCatalog()
{
}

unsigned int CachedCatalog::minQueryLen() const
{
	return 3;
}

void CachedCatalog::queryChanged()
{
	int newStatus = 0;
	if(query() == "")
	{
		results.clear();
		setBestMatch(Match());
	} else {
		if(query().length() >= minQueryLen())
		{
			Match newBestMatch;
		
			if(status() & S_Active)
			{
				QPtrListIterator<KatapultItem> it(results);
				KatapultItem *item;
				
				while((item = it.current())!=0)
				{
					++it;
					Match match = queryItem(item, query());
					if(match.isNull())
						results.removeRef(item);
					else if(newBestMatch.isNull() || match.rank() > newBestMatch.rank())
						newBestMatch = match;
				}
			} else {
				results.clear();
			
				QPtrListIterator<KatapultItem> it(cache);
				KatapultItem *item;
				while((item=it.current())!=0)
				{
					++it;
					Match match = queryItem(item, query());
					if(!match.isNull()) {
						results.append(item);
						if(newBestMatch.isNull() || match.rank() > newBestMatch.rank())
							newBestMatch = match;
					}
				}
			}
			
			newStatus |= S_Active;
			if(results.count() > 0)
			{
				newStatus |= S_HasResults;
				if(results.count() > 1)
					newStatus |= S_Multiple;
			} else
				newStatus |= S_NoResults;
				
			setBestMatch(newBestMatch);
		}
	}
	setStatus(newStatus);
}

Match CachedCatalog::queryItem(const KatapultItem *item, QString query) const
{
	int wordNo = 0;
	int _rank = 0;
	unsigned int _matched = 0;
	QString text = item->text().lower();
	QStringList queryWords = QStringList::split(" ", query.lower());
	int wordMax = queryWords.count()-1;
	QStringList words = QStringList::split(" ", text);
	QStringList::Iterator wit = words.begin();
	for(QStringList::Iterator qit = queryWords.begin(); qit != queryWords.end(); ++qit) {
		QString queryWord = *qit;
		bool didMatch = FALSE;
		for(; wit != words.end(); ++wit) {
			QString word = *wit;
			if(word.startsWith(queryWord)) {
				if(_matched != 0)
					_matched++;
				if(wordNo == wordMax) {
					_matched += queryWord.length();
				} else {
					_matched += word.length();
				}
				didMatch = TRUE;
				break;
			}
			if(wordNo == 0) {
				if(_matched != 0)
					_matched++;
				_matched += word.length();
			}
		}
		if(didMatch == FALSE) {
			return Match();
		}
		wordNo++;
	}
	
	if(_matched > text.length())
		return Match();

	_rank = 100*query.length()/text.length();
	
	if(_rank == 0)
		return Match();
	else
		return Match(item, _rank, _matched);
}

const KatapultItem * CachedCatalog::findExact(QString text) const
{
	text = text.lower();
	KatapultItem *item;
	QPtrListIterator<KatapultItem> it(cache);
	while((item=it.current())!=0)
	{
		++it;
		if(item->text().lower() == text)
			return item;
	}
	return 0;
}

void CachedCatalog::addItem(KatapultItem *item)
{
	if(findExact(item->text())) {
		qDebug("Ignored duplicate item: %s", item->text().ascii());
		delete item;
	} else
		cache.append(item);
}
