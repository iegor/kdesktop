/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2001 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2002 by Stanislav Visnovsky <visnovsky@nenya.ms.mff.cuni.cz>
                    
  based on code of Andrea Rizzi <rizzi@kde.org>

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
#include "regexpextractor.h"

#include <kdebug.h>
#include <qregexp.h>

using namespace KBabel;

RegExpExtractor::RegExpExtractor(const QStringList& regexps) :
    _regExpList( regexps )
{
    _string=QString::null;
    _matches.setAutoDelete(true);
}

void RegExpExtractor::setString(QString string)
{
    _string=string;
    processString();
}

uint RegExpExtractor::countMatches()
{
    return _matches.count();
}

QString RegExpExtractor::firstMatch()
{
    MatchedEntryInfo *ti = _matches.first();

    if(ti)
        return ti->extracted;

    return QString::null;
}

QString RegExpExtractor::nextMatch()
{
    MatchedEntryInfo *ti=_matches.next();
    if(!ti)
        ti=_matches.first();
    
    if(ti)
        return ti->extracted;

    return QString::null;
}

QString RegExpExtractor::match(uint tagnumber)
{
    MatchedEntryInfo *ti=_matches.at(tagnumber);
    if(ti)
        return ti->extracted;

    return QString::null;
}

int RegExpExtractor::matchIndex(uint tagnumber)
{
    MatchedEntryInfo *ti=_matches.at(tagnumber);
    if(ti)
        return ti->index;

    return -1;
}

QString RegExpExtractor::prevMatch()
{
    MatchedEntryInfo *ti=_matches.prev();
    if(ti)
        return ti->extracted;
    
    return QString::null;
}

QString RegExpExtractor::lastMatch()
{
    MatchedEntryInfo *ti=_matches.last();
    if(ti)
        return ti->extracted;
    
    return QString::null;
} 

QStringList RegExpExtractor::matches()
{
    QStringList list;
    MatchedEntryInfo *ti;
    for(ti=_matches.first(); ti!=0; ti = _matches.next())
    {
        list.append(ti->extracted);
    }

    return list;
}

QString RegExpExtractor::plainString(bool keepPos)
{
    QString tmp=_string;

    MatchedEntryInfo *ti;
    for(ti=_matches.first(); ti != 0; ti=_matches.next())
    {
        uint len=ti->extracted.length();
        QString s;
        for(uint i=0; i<len; i++)
        {
             s+=' ';
        }
        tmp.replace(ti->index,len,s);
        
    }
    if(!keepPos)
    {
        tmp=tmp.simplifyWhiteSpace();
    }

    return tmp;
}

QString RegExpExtractor::matchesReplaced(const QString& replace)
{    
    QString tmp=_string;

    int posCorrection=0;
    int replaceLen=replace.length();

    MatchedEntryInfo *ti;
    for(ti=_matches.first(); ti != 0; ti=_matches.next())
    {
        uint len=ti->extracted.length();
        tmp.replace(ti->index + posCorrection,len,replace);
        
        posCorrection+=(replaceLen-len);
    }

    return tmp;
}

void RegExpExtractor::processString()
{
    _matches.clear();
    
    // if there is no regexp to be matched, quit
    if( regExpList().empty() ) return;

    QValueList<MatchedEntryInfo> tmpList;
    
    bool found=false;
    QString tmp=_string;
    do
    {
        found=false;
        QStringList::Iterator it;
        for(it=_regExpList.begin();it!=_regExpList.end();++it)
        {
            int pos=-1;
            QString tag;
	    
	    QRegExp reg = QRegExp((*it));
	    
	    pos = reg.search(tmp);
            int len=reg.matchedLength();
            if(pos>=0)
            {
        	tag=tmp.mid(pos,len);
            }

            if(pos >= 0)
            {
                found=true;
                
                MatchedEntryInfo ti;
                ti.index=pos;
                ti.extracted=tag;
                tmpList.append(ti);
		
                QString s;
                for(uint i=0; i<tag.length(); i++)
                {
                    s+=' ';
                }
                tmp.replace(pos,tag.length(),s);

                break;
            }
        }
    }
    while( found );

    uint num=tmpList.count();
    
    for(uint i=0; i < num; i++)
    {
        uint n= 0;
        uint min=_string.length();
        uint counter=0;
        QValueList<MatchedEntryInfo>::Iterator it;    
        for(it=tmpList.begin();it!=tmpList.end();++it)
        {
            if((*it).index < min)
            {
                min=(*it).index;
                n=counter;
            }
            
            counter++;
        }

        it=tmpList.at(n);
        MatchedEntryInfo *ti = new MatchedEntryInfo;
        ti->index=(*it).index;
        ti->extracted=(*it).extracted;
        _matches.append(ti);
        tmpList.remove(it);
    }
}

QStringList RegExpExtractor::regExpList()
{
    return _regExpList;
}

void RegExpExtractor::setRegExpList( const QStringList& regexps )
{
    _regExpList = regexps;
}

void RegExpExtractor::addRegExpIdentifier(QString regExp)
{
    _regExpList.append(regExp);
}

void RegExpExtractor::deleteRegExpIdentifier(QString regExp)
{
    _regExpList.remove(regExp);
}
 
