/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2001 by Matthias Kiefer
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
#include "compendiumdata.h"

#include <resources.h>
#include <catalog.h>
#include <tagextractor.h>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>

using namespace KBabel;

CompendiumData::CompendiumData(QObject *parent)
            : QObject(parent)
            , _active(false)
            , _error(false)
            , _initialized(false)
            , _catalog(0)
            , _exactDict(9887)
            , _allDict(9887)
            , _wordDict(9887)
            , _textonlyDict(9887)
{
    _catalog = new Catalog(this, "CompendiumData::catalog", QString::null);
    _exactDict.setAutoDelete(true);
    _allDict.setAutoDelete(true);
    _wordDict.setAutoDelete(true);
    _textonlyDict.setAutoDelete(true);
}


bool CompendiumData::load(KURL url)
{
    if(_active)
        return false;


    _error = false;
    _active = true;

    _exactDict.clear();
    _allDict.clear();
    _wordDict.clear();
    _textonlyDict.clear();
    

    emit progressStarts(i18n("Loading PO compendium"));
    connect(_catalog, SIGNAL(signalProgress(int)), this, SIGNAL(progress(int)));

    ConversionStatus stat=_catalog->openURL(url);
        
    disconnect(_catalog, SIGNAL(signalProgress(int))
            , this, SIGNAL(progress(int)));
        

    if( stat!= OK && stat != RECOVERED_PARSE_ERROR)
    {
        kdDebug(KBABEL_SEARCH) << "error while opening file " << url.prettyURL() << endl;
                
        _error = true;
        _errorMsg = i18n("Error while trying to read file for PO Compendium module:\n%1")
            .arg(url.prettyURL());

        emit progressEnds();

        _active = false;
        _initialized=true;

        return false;
    }

    emit progressStarts(i18n("Building indices"));

    int total = _catalog->numberOfEntries();
    for(int i=0; i < total; i++)
    { 
        if( (100*(i+1))%total < 100 )
        {
            emit progress((100*(i+1))/total);
            kapp->processEvents(100);
        }

	// FIXME: shoudl care about plural forms
        QString temp = _catalog->msgid(i,true).first();
        
        int *index = new int(i);
        _exactDict.insert(temp,index);


        temp = simplify(temp);
        temp = temp.lower();

        if(!temp.isEmpty() && temp.length() > 1)
        {
	    // add to allDict
            QValueList<int> *indexList=_allDict[temp];
            
            if(!indexList)
            {
                indexList = new QValueList<int>;
                _allDict.insert(temp,indexList);
            }

            indexList->append(i);

	    // add to textonlyDict
	    QString temp1 = temp;
	    temp1.remove( ' ' );
	    
            indexList=_textonlyDict[temp1];
            
            if(!indexList)
            {
                indexList = new QValueList<int>;
                _textonlyDict.insert(temp1,indexList);
		kdDebug() << "Adding " << temp1 << endl;
            }

            indexList->append(i);

	    // add to wordDict
            QStringList wList = wordList(temp);
            for ( QStringList::Iterator it = wList.begin()
                    ; it != wList.end(); ++it ) 
            {
                if( (*it).length() > 1)
                {
                    indexList=_wordDict[*it];
            
                    if(!indexList)
                    {
                        indexList = new QValueList<int>;
                        _wordDict.insert(*it,indexList);
                    }

                    indexList->append(i);
                }
            }
        }
    }

    // remove words, that are too frequent
    uint max=_allDict.count()/10;
    QDictIterator< QValueList<int> > it(_wordDict);
    while ( it.current() ) 
    {
        if(it.current()->count() > max)
        {
            _wordDict.remove(it.currentKey());
        }
        else
        {
            ++it;
        }
    }
    

    _initialized=true;

    emit progressEnds();



    _active = false;

    return true;
}

const int* CompendiumData::exactDict(const QString text) const
{
    return _exactDict[text];
}

const QValueList<int>* CompendiumData::allDict(const QString text) const
{
    return _allDict[text];
}

const QValueList<int>* CompendiumData::wordDict(const QString text) const
{
    return _wordDict[text];
}

const QValueList<int>* CompendiumData::textonlyDict(const QString text) const
{
    return _textonlyDict[text];
}


void CompendiumData::registerObject(QObject *obj)
{
    if(!_registered.containsRef(obj))
        _registered.append(obj);
}

bool CompendiumData::unregisterObject(QObject *obj)
{
    _registered.removeRef(obj);

    return _registered.count()==0;
}

bool CompendiumData::hasObjects() const
{
    return _registered.count()==0;
}

QString CompendiumData::simplify(const QString string)
{ 
    QString result;

    TagExtractor te;
    te.setString(string);
    result=te.plainString();

    result=result.simplifyWhiteSpace();
    result=result.stripWhiteSpace();

    return result;
}

QStringList CompendiumData::wordList(const QString string)
{    
    QString result=CompendiumData::simplify(string);
   
    return QStringList::split(' ',result);
}

#include "compendiumdata.moc"
