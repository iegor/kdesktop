/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2001 by Matthias Kiefer
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
#include "tmxcompendiumdata.h"

#include <qdom.h>
#include <qfile.h>

#include <tagextractor.h>
#include <resources.h>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kio/netaccess.h>

using namespace KBabel;

TmxCompendiumData::TmxCompendiumData(QObject *parent)
            : QObject(parent)
            , _active(false)
            , _error(false)
            , _initialized(false)
            , _exactDict(9887)
            , _allDict(9887)
            , _wordDict(9887)
{
    _exactDict.setAutoDelete(true);
    _allDict.setAutoDelete(true);
    _wordDict.setAutoDelete(true);
}


bool TmxCompendiumData::load(const KURL& url, const QString& language)
{
    kdDebug(KBABEL_SEARCH) << "Load " << url.url() << " in " << language << endl;
    if(_active)
        return false;


    _error = false;
    _active = true;

    _exactDict.clear();
    _allDict.clear();
    _wordDict.clear();
    

    emit progressStarts(i18n("Loading TMX compendium"));

    QDomDocument doc( "mydocument" );
    
    QString target;
    
    if( KIO::NetAccess::download( url, target ) )
    {
	QFile f( target );
	if ( !f.open( IO_ReadOnly ) )
	{
    	    _error = true;
	    _errorMsg = i18n( "Cannot open the file." );
	}    
	else if ( !doc.setContent( &f ) ) {
	    _error = true;
	    _errorMsg = i18n( "Cannot parse XML data." );
	}
	f.close();
	KIO::NetAccess::removeTempFile(target);
    } else {
	_error = true;
	_errorMsg = i18n( "Cannot open the file." );
    } 
       
    QDomElement docElem = doc.documentElement();
    
    if( docElem.tagName() != "tmx" || !(docElem.hasAttribute("version")
	  && docElem.attribute("version") == "1.4" ) )
    {
	_error = true;
	_errorMsg = i18n( "Unsupported format.");
    }
			      
				    
    if( _error )
    {
        _errorMsg = i18n("Error while trying to read file for TMX Compendium module:\n"
	    "%1\n"
	    "Reason: %2")
            .arg(url.prettyURL()).arg(_errorMsg);

        kdDebug(KBABEL_SEARCH) << "Error:  " << _errorMsg << endl;

        emit progressEnds();

        _active = false;
        _initialized=true;

        return false;
    }

    emit progressStarts(i18n("Building indices"));
    
    QDomNodeList tuTags = docElem.elementsByTagName("tu");
    uint total = tuTags.count();
    
    _originals.clear();
    _originals.resize(total);
    _translations.clear();
    _translations.resize(total);
    
    uint lastindex = 0;
    
    for(uint i=0; i < total; i++)
    { 
        if( (100*(i+1))%total < 100 )
        {
            emit progress((100*(i+1))/total);
            kapp->processEvents(100);
        }
	
    	QDomNodeList tuvTags = tuTags.item(i).toElement().elementsByTagName("tuv");
	QString english, other;
	for( uint j = 0 ; j < tuvTags.count() ; j++ )
        {
	    QDomElement el = tuvTags.item(j).toElement();
	    if( el.attribute("xml:lang").upper() == "EN" ) english = el.text();
	    if( el.attribute("xml:lang").upper().startsWith(language.upper()) )
		other = el.text(); 
	}
	
	if( !english.isEmpty() && !other.isEmpty() )
	{
	    kdDebug(KBABEL_SEARCH) << english << " to " << lastindex << endl;
	    _originals[lastindex] = english;
	    _translations[lastindex] = other;
		
	    QString temp = english;
        
    	    int *index = new int(lastindex);
    	    _exactDict.insert(temp,index);

	    temp = simplify(temp);
    	    temp = temp.lower();

	    if(!temp.isEmpty() && temp.length() > 1)
    	    {
    		QValueList<int> *indexList=_allDict[temp];
            
        	if(!indexList)
        	{
            	    indexList = new QValueList<int>;
	            _allDict.insert(temp,indexList);
    		}

	        indexList->append(lastindex);
	    
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
			
			indexList->append(lastindex);
            	    }
        	}
	    }
	    lastindex++;
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
    
    if( lastindex == 0) 
    {
	_error = true;
	_errorMsg = i18n("Empty database.");
    }
    
    kdDebug(KBABEL_SEARCH) << "load done" << endl;

    _initialized=true;

    emit progressEnds();

    _active = false;

    return true;
}

const int* TmxCompendiumData::exactDict(const QString& text) const
{
    return _exactDict[text];
}

const QValueList<int>* TmxCompendiumData::allDict(const QString& text) const
{
    return _allDict[text];
}

const QValueList<int>* TmxCompendiumData::wordDict(const QString& text) const
{
    return _wordDict[text];
}

const QString TmxCompendiumData::msgid(const int index) const
{
    return _originals[index];
}

const QString TmxCompendiumData::msgstr(const int index) const
{
    return _translations[index];
}


void TmxCompendiumData::registerObject(QObject *obj)
{
    if(!_registered.containsRef(obj))
        _registered.append(obj);
}

bool TmxCompendiumData::unregisterObject(QObject *obj)
{
    _registered.removeRef(obj);

    return _registered.count()==0;
}

bool TmxCompendiumData::hasObjects() const
{
    return _registered.count()==0;
}

QString TmxCompendiumData::simplify(const QString& string)
{ 
    QString result;

    TagExtractor te;
    te.setString(string);
    result=te.plainString();

    result=result.simplifyWhiteSpace();
    result=result.stripWhiteSpace();

    return result;
}

QStringList TmxCompendiumData::wordList(const QString& string)
{    
    QString result=TmxCompendiumData::simplify(string);
   
    return QStringList::split(' ',result);
}

#include "tmxcompendiumdata.moc"
