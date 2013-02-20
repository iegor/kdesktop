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
#include <klocale.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kinstance.h>

#include <qregexp.h>
#include <qtimer.h>
#include <qstylesheet.h>

#include "poauxiliary.h"
#include "preferenceswidget.h"
#include "pa_factory.h"

#include <resources.h>
#include <catalog.h>

using namespace KBabel;

PoAuxiliary::PoAuxiliary(QObject *parent, const char *name)
        : SearchEngine(parent, name)
{
    catalog = new Catalog(this, "PoAuxiliary::catalog");
    prefWidget=0;
    error=false;
    stop=false;
    active=false;
    loading=false;
    initialized=false;

    ignoreFuzzy=true;

    loadTimer = new QTimer(this);
    connect(loadTimer,SIGNAL(timeout()),this,SLOT(loadAuxiliary()));

    msgidDict.setAutoDelete(true);
}

PoAuxiliary::~PoAuxiliary()
{
    results.clear();
}

bool PoAuxiliary::isReady() const
{
    return !error;
}


bool PoAuxiliary::isSearching() const
{
    return active;
}


void PoAuxiliary::saveSettings(KConfigBase *config)
{
    if(autoUpdate && prefWidget && prefWidget->settingsChanged())
    {
        applySettings();
    }
        
    config->writeEntry("Auxiliary", url);
    config->writeEntry("IgnoreFuzzy",ignoreFuzzy);
}

void PoAuxiliary::readSettings(KConfigBase *config)
{    
    bool needLoading=false;

    QString newPath = config->readEntry("Auxiliary"
                    ,"../../../de/messages/@DIR1@/@PACKAGE@.po");
    if(!initialized)
    {
        url = newPath;
    }
    else if(newPath != url)
    {
        url = newPath;
        needLoading = true;
    }
            
    ignoreFuzzy = config->readBoolEntry("IgnoreFuzzy",true);
    
    if(needLoading && !loadTimer->isActive())
    {
        kdDebug(KBABEL_SEARCH) << "readSettings" << endl;
        loadTimer->start(100,true);
    }

    restoreSettings();
}

PrefWidget *PoAuxiliary::preferencesWidget(QWidget *parent)
{
    prefWidget = new AuxiliaryPreferencesWidget(parent,"pocompendium_prefwidget");
    connect(prefWidget, SIGNAL(applySettings()), this, SLOT(applySettings()));
    connect(prefWidget, SIGNAL(restoreSettings())
                    , this, SLOT(restoreSettings()));

    restoreSettings();
    
    return prefWidget;
}

const KAboutData *PoAuxiliary::about() const
{
    return PaFactory::instance()->aboutData();
}


QString PoAuxiliary::name() const
{
    return i18n("PO Auxiliary");
}

QString PoAuxiliary::id() const
{
    return "poauxiliary";
}

QString PoAuxiliary::lastError()
{
    return errorMsg;
}

bool PoAuxiliary::startSearch(const QString& t, uint pluralForm, const SearchFilter*filter)
{
    QString text(t);
    if(autoUpdate && prefWidget && prefWidget->settingsChanged())
    {
        applySettings();
    }

    if(!initialized)
    {
        loadAuxiliary();
    }
    
    if(error)
        return false;
    
    if(isSearching())
        return false;

    stop=false;
    active = true;
    emit started();

    clearResults();
   
    kapp->processEvents(100);

    text.replace("\n","");
    
    Entry *entry = msgidDict[text];
    if(entry)
    {
        if( !(entry->fuzzy && ignoreFuzzy) )
        {
            SearchResult *result = new SearchResult;
            result->requested =QStyleSheet::convertFromPlainText(text);
            result->found = QStyleSheet::convertFromPlainText(text);
            result->translation = 
            QStyleSheet::convertFromPlainText(entry->translation);

            result->plainRequested = text;
            result->plainFound=text;
            result->plainTranslation=entry->translation;
            result->score=100;

            if(entry->fuzzy)
            {
                result->translation="<qt><font color=\"red\">"+i18n("fuzzy")
                    +"</font><hr/>" + result->translation+"</qt>";
            }

            TranslationInfo *info = new TranslationInfo;
            info->location = auxPackage;
            info->translator = auxTranslator;
            info->description = entry->comment;
            info->filePath = auxURL;
            result->descriptions.append(info);

            results.append(result);
            
            emit numberOfResultsChanged(1);
            emit resultFound(result);
        }
    }

    
    active = false;
    stop = false;
    emit finished();
    
    return true;
}


bool PoAuxiliary::startSearchInTranslation(const QString& text)
{
    if(autoUpdate && prefWidget && prefWidget->settingsChanged())
    {
        applySettings();
    }

    if(!initialized)
    {
        loadAuxiliary();
    }
    
    if(error)
        return false;
    
    if(isSearching())
        return false;

    stop=false;
    active = true;
    emit started();

    clearResults();
   
    kapp->processEvents(100);

    Entry *entry = msgstrDict[text];
    if(entry)
    {
        if( !(entry->fuzzy && ignoreFuzzy) )
        {
            SearchResult *result = new SearchResult;
            result->requested =QStyleSheet::convertFromPlainText(entry->orig);
            result->found = QStyleSheet::convertFromPlainText(entry->orig);
            result->translation = 
            QStyleSheet::convertFromPlainText(text);

            result->plainRequested = entry->orig;
            result->plainFound=entry->orig;
            result->plainTranslation=text;
            result->score=100;

            if(entry->fuzzy)
            {
                result->translation="<qt><font color=\"red\">"+i18n("fuzzy")
                    +"</font><hr/>" + result->translation+"</qt>";
            }

            TranslationInfo *info = new TranslationInfo;
            info->location = auxPackage;
            info->translator = auxTranslator;
            info->description = entry->comment;
            info->filePath = auxURL;
            result->descriptions.append(info);

            results.append(result);
            
            emit numberOfResultsChanged(1);
            emit resultFound(result);
        }
    }

    
    active = false;
    stop = false;
    emit finished();
    
    return true;
}

void PoAuxiliary::stopSearch()
{
    stop=true;
}


void PoAuxiliary::applySettings()
{    
    if(!prefWidget)
        return;
    
    bool needLoading=false;
    
    if(isSearching())
        stopSearch();
    
    QString newPath = prefWidget->url();
    if(!initialized)
    {
        url = newPath;
    }
    else if(newPath != url)
    {
        url = newPath;
        needLoading=true;
    }
    
    ignoreFuzzy = prefWidget->ignoreFuzzy();

    if(needLoading && !loadTimer->isActive())
    {
        loadTimer->start(100,true);
    }
}

void PoAuxiliary::restoreSettings()
{
    if(!prefWidget)
        return;
    
    prefWidget->setURL(url);
    prefWidget->setIgnoreFuzzy(ignoreFuzzy);
}
    
void PoAuxiliary::loadAuxiliary()
{
    if(loadTimer->isActive())
        loadTimer->stop();

    if(loading)
        return;

    loading=true;
    error=false;

    QString path=url;
   
    if(path.contains("@LANG@"))
    {
        path.replace("@LANG@",langCode);
    }
    if(path.contains("@PACKAGE@"))
    {
	int pos=package.findRev("/");
	if( pos<0 ) pos=0;
        path.replace("@PACKAGE@",package.mid(pos));
    }
    if(path.contains("@PACKAGEDIR@"))
    {
	QString packagedir;
	int pos=package.findRev("/");
	if( pos > 0 ) packagedir=package.left(pos);
	else packagedir="";
        path.replace("@PACKAGEDIR@",packagedir);
	kdDebug(KBABEL_SEARCH) << "Packagedir found " << packagedir << endl;
    }
    QRegExp reg("@DIR[0-9]+@");
    if(path.contains(reg))
    {
        int pos=reg.search(path);
        int len = reg.matchedLength();
 
        while(pos>=0)
        {
            QString num=path.mid(pos+4,len-5);
            
            bool ok;
            int number=num.toInt(&ok);
            if(ok)
            {
                QString dir=directory(editedFile,number);
                QString s("@DIR%1@");
                path.replace(s.arg(number),dir);

                pos+=dir.length();
            }

            pos=reg.search(path);
	    len = reg.matchedLength();
        }
    }
    
    KURL u;
    QRegExp rel("^[a-zA-Z]+:");
    if(rel.search(path) >= 0)
    {
        u=path;
    }
    else if(path[0] != '/') // relative path
    {
        KURL temp(editedFile);
        QString dir = temp.directory();
        kdDebug(KBABEL_SEARCH) << dir << endl;
        u.setPath(dir+"/"+path);
        u.cleanPath();
        kdDebug(KBABEL_SEARCH) << u.prettyURL() << endl;
    }
    else
    {
        u.setPath(path);
    }
        
    emit progressStarts(i18n("Loading PO auxiliary"));
    connect(catalog, SIGNAL(signalProgress(int))
                        , this, SIGNAL(progress(int)));
      
    ConversionStatus stat = catalog->openURL(u);
    if( stat != OK && stat != RECOVERED_PARSE_ERROR)
    {
        kdDebug(KBABEL_SEARCH) << "error while opening file " << u.prettyURL() << endl;
	
	if( !error )
	{
    	    error = true;
    	    errorMsg = i18n("Error while trying to open file for PO Auxiliary module:\n%1")
                        .arg(u.prettyURL());
    	    emit hasError(errorMsg);
	}
    }
    else
    {
        error = false;
        // build index for fast search
        msgidDict.clear();
        msgstrDict.clear();
        
        emit progressStarts(i18n("Building index"));
        
        int total = catalog->numberOfEntries();
        for(int i=0; i < total; i++)
        {
            if( (100*(i+1))%total < 100 )
            {
                emit progress((100*(i+1))/total);
                kapp->processEvents(100);
            }

            Entry *e = new Entry;
	    // FIXME: should care about plural forms
            e->orig = catalog->msgid(i).first();
	    e->orig.replace("\n","");
	    kdWarning() << "PoAuxialiary does not support plural forms" << endl;
            e->translation = catalog->msgstr(i).first();
            e->comment = catalog->comment(i);
            e->fuzzy = catalog->isFuzzy(i);

            msgidDict.insert(catalog->msgid(i,true).first(),e);
            msgstrDict.insert(e->translation,e);
        }

        auxPackage = catalog->packageName();
        auxURL = catalog->currentURL().url();
        auxTranslator = catalog->lastTranslator();
    }

    disconnect(catalog, SIGNAL(signalProgress(int))
                        , this, SIGNAL(progress(int)));

    emit progressEnds();

    initialized=true;

    loading=false;

    catalog->clear();
}


void PoAuxiliary::setEditedFile(const QString& file)
{
    if(initialized && (url.contains("@DIR") || KURL::isRelativeURL(url)) 
            && file!=editedFile && !loadTimer->isActive() )
    {
        initialized=false;
    }

    editedFile=file;
}


void PoAuxiliary::setEditedPackage(const QString& pack)
{
    if(initialized && url.contains("@PACKAGE@") && pack!=package 
            && !loadTimer->isActive() )
    {
        initialized=false;
    }

    package=pack;
}


void PoAuxiliary::setLanguageCode(const QString& lang)
{
    if(initialized && url.contains("@LANG@") && lang!=langCode 
            && !loadTimer->isActive() )
    {
        initialized=false;
    }
    
    langCode=lang;
}

bool PoAuxiliary::usesRichTextResults()
{
    return true;
}

QString PoAuxiliary::translate(const QString& text, uint pluralForm)
{
    if(!initialized)
    {
        loadAuxiliary();
    }
    
    if(error)
    {
        return QString::null;
    }
    
    Entry *entry = msgidDict[text];
    if(entry)
    {
        return entry->translation;
    }

    return QString::null;
}


#include "poauxiliary.moc"
