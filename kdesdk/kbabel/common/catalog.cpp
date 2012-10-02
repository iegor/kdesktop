/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2001-2005 by Stanislav Visnovsky <visnovsky@kde.org>
  Copyright (C) 2006 by Nicolas GOUTTE <goutte@kde.org>

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
#include <qtextstream.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qdatetime.h>

#include <kconfig.h>
#include <kdatatool.h>
#include <kglobal.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <kio/netaccess.h>
#include <krfcdate.h>
#include <ktrader.h>
#include <kurl.h>

#include "kbprojectmanager.h"
#include "catalog.h"
#include "catalog_private.h"
#include "catalogitem.h"
#include "diff.h"
#include "findoptions.h"
#include "catalogview.h"
#include "editcmd.h"
#include "kbprojectsettings.h"

#include "resources.h"
#include "version.h"
#include "stringdistance.h"

#include <kmessagebox.h>
using namespace KBabel;

Catalog::Catalog(QObject* parent, const char* name, QString projectFile)
        : QObject(parent,name)
{
   if ( projectFile.isEmpty() )
        projectFile = KBabel::ProjectManager::defaultProjectName();
   d = new CatalogPrivate(ProjectManager::open(projectFile));
   readPreferences();
}

Catalog::Catalog(const Catalog& c): QObject(c.parent(),c.name()
)
{
   kdFatal() << "Copy constructor of Catalog, please report how to reproduce to the authors" << endl;
}

Catalog::~Catalog()
{
    delete d;
}

QString Catalog::msgctxt(uint index) const
{
    if (  d->_entries.isEmpty() )
        return QString::null;
   uint max=d->_entries.count()-1;
   if(index > max)
      index=max;
   return d->_entries[index].msgctxt();
}

QStringList Catalog::msgid(uint index, const bool noNewlines) const
{
   if (  d->_entries.isEmpty() )
        return QString::null;
    uint max=d->_entries.count()-1;
   if(index > max)
      index=max;

   return d->_entries[index].msgid(noNewlines);
}

QStringList Catalog::msgstr(uint index, const bool noNewlines) const
{
    if (  d->_entries.isEmpty() )
        return QString::null;

   uint max=d->_entries.count()-1;
   if(index > max)
      index=max;

   return d->_entries[index].msgstr(noNewlines);
}

QString Catalog::comment(uint index) const
{
    if (  d->_entries.isEmpty() )
        return QString::null;
   uint max=d->_entries.count()-1;
   if(index > max)
      index=max;
   return d->_entries[index].comment();
}

QString Catalog::context(uint index) const
{
    QString c = comment(index);

    QStringList lines = QStringList::split("\n",c);

    QString result;
    for( QStringList::Iterator it=lines.begin(); it!=lines.end(); ++it)
    {
	if( (*it).startsWith( "#:") )
	{
	    result+=(*it)+"\n";
	}
    }
    return result.stripWhiteSpace();
}

CatalogItem Catalog::header() const
{
   return d->_header;
}

QString Catalog::lastTranslator() const
{
    return headerInfo( d->_header ).lastTranslator;
}

int Catalog::indexForMsgid(const QString& id) const
{
    int i=0;
	QValueVector<CatalogItem>::ConstIterator it = d->_entries.begin();

    while(it != d->_entries.end() && !((*it).msgid(true).contains(id)))
    {
        ++it;
        i++;
    }

    if(it == d->_entries.end())
        i=-1;

    return i;
}

QStringList Catalog::tagList(uint index)
{
    if (  d->_entries.isEmpty() )
        return QStringList();

   uint max=d->_entries.count()-1;
   if(index > max)
      index=max;

   return d->_entries[index].tagList(*(d->_tagExtractor));
}


QStringList Catalog::argList(uint index)
{
    if (  d->_entries.isEmpty() )
        return QStringList();

   uint max=d->_entries.count()-1;
   if(index > max)
      index=max;

   return d->_entries[index].argList(*(d->_argExtractor));
}


/*
bool Catalog::setMsgstr(uint index,QString msgstr)
{
    kdWarning() << "Catalog::setMsgstr()" << endl;

   bool untranslatedChanged=false;

   if(_entries[index].isUntranslated() && !msgstr.isEmpty())
   {
      _untransIndex.remove(index);
      untranslatedChanged=true;
   }
   else if(msgstr.isEmpty())
   {
      QValueList<uint>::Iterator it;

      // insert index in the right place in the list
      it = _untransIndex.begin();
      while(it != _untransIndex.end() && index > (*it))
      {
         ++it;
      }
      _untransIndex.insert(it,index);

      untranslatedChanged=true;
   }

   _entries[index].setMsgstr(msgstr);

   setModified(true);

   if(untranslatedChanged)
      emit signalNumberOfUntranslatedChanged(numberOfUntranslated());

   return untranslatedChanged;
}
*/

/*
bool Catalog::setComment(uint index,QString comment)
{
    kdWarning() << "Catalog::setComment()" << endl;
   bool fuzziesChanged=false;


   bool wasFuzzy=_entries[index].isFuzzy();

   _entries[index].setComment(comment);

   bool isFuzzy=_entries[index].isFuzzy();

   if(wasFuzzy && !isFuzzy)
   {
      _fuzzyIndex.remove(index);
      fuzziesChanged=true;
   }
   else if(isFuzzy)
   {
      QValueList<uint>::Iterator it;

      // insert index in the right place in the list
      it = _fuzzyIndex.begin();
      while(it != _fuzzyIndex.end() && index > (*it))
      {
         ++it;
      }
      _fuzzyIndex.insert(it,index);

      fuzziesChanged=true;
   }

   setModified(true);

   if(fuzziesChanged)
      emit signalNumberOfFuzziesChanged(numberOfFuzzies());


   return fuzziesChanged;
}
*/

bool Catalog::setHeader(CatalogItem newHeader)
{
   if(newHeader.isValid())
   {
      // normalize the values - ensure every key:value pair is only on a single line
      QString values = newHeader.msgstr().first();
      values.replace ("\n", "");
      values.replace ("\\n", "\\n\n");
      
      kdDebug () << "Normalized header: " << values << endl;
      
      d->_header=newHeader;
      d->_header.setMsgstr (values);
      
      setModified(true);

      emit signalHeaderChanged();

      return true;
   }

   return false;
}

KURL Catalog::currentURL() const
{
   return d->_url;
}

void Catalog::setCurrentURL(const KURL& url)
{
   d->_url=url;
}


CatalogItem Catalog::updatedHeader(CatalogItem oldHeader, bool usePrefs) const
{
   QStringList headerList=oldHeader.msgstrAsList();
   QStringList commentList=QStringList::split('\n',oldHeader.comment());

   QStringList::Iterator it,ait;
   QString temp;
   bool found;

   const IdentitySettings identityOptions = identitySettings();
   const SaveSettings saveOptions = saveSettings();
   
   if(!usePrefs || saveOptions.updateLastTranslator)
   {
      found=false;

      temp="Last-Translator: "+identityOptions.authorName;
      if(!identityOptions.authorEmail.isEmpty())
      {
         temp+=(" <"+identityOptions.authorEmail+">");
      }
      temp+="\\n";
      for( it = headerList.begin(); it != headerList.end(); ++it )
      {
         if((*it).contains(QRegExp("^ *Last-Translator:.*")))
         {
            (*it) = temp;
            found=true;
            break;
         }
       }
       if(!found)
       {
          headerList.append(temp);
       }
   }
   if(!usePrefs || saveOptions.updateRevisionDate)
   {
      found=false;

      temp="PO-Revision-Date: "+dateTime()+"\\n";

      for( it = headerList.begin(); it != headerList.end(); ++it )
      {
         if((*it).contains(QRegExp("^ *PO-Revision-Date:.*")))
         {
            (*it) = temp;
            found=true;
            break;
         }
       }
       if(!found)
       {
          headerList.append(temp);
       }
   }
   if(!usePrefs || saveOptions.updateProject)
   {
      found=false;

      temp="Project-Id-Version: "+saveOptions.projectString+"\\n";
      temp.replace( "@PACKAGE@", packageName());

      for( it = headerList.begin(); it != headerList.end(); ++it )
      {
         if((*it).contains(QRegExp("^ *Project-Id-Version:.*")))
         {
            (*it) = temp;
            found=true;
            break;
         }
       }
       if(!found)
       {
          headerList.append(temp);
       }
   }
   if(!usePrefs || saveOptions.updateLanguageTeam)
   {
      found=false;

      temp="Language-Team: "+identityOptions.languageName;
      if(!identityOptions.mailingList.isEmpty())
      {
         temp+=(" <"+identityOptions.mailingList+">");
      }
      temp+="\\n";
      for( it = headerList.begin(); it != headerList.end(); ++it )
      {
         if((*it).contains(QRegExp("^ *Language-Team:.*")))
         {
            (*it) = temp;
            found=true;
            break;
         }
       }
       if(!found)
       {
          headerList.append(temp);
       }
   }
    if(!usePrefs || saveOptions.updateCharset)
    {
        
        found=false;
    
        QString encodingStr;
        if(saveOptions.useOldEncoding && d->fileCodec)
        {
            encodingStr = charsetString(d->fileCodec);
        }
        else
        {
            encodingStr=charsetString(saveOptions.encoding);
        }

        temp = "Content-Type: text/plain; charset=" + encodingStr + "\\n";
                
        it = headerList.begin();
        while( it != headerList.end() )
        {
            if( (*it).find( QRegExp( "^ *Content-Type:.*" ) ) != -1 )
            {
                if ( found )
                {
                    // We had already a Content-Type, so we do not need a duplicate
                    it = headerList.remove( it );
                }
                else
                {
                    found=true;
                    QRegExp regexp( "^ *Content-Type:(.*/.*);?\\s*charset=.*$" );
                    QString mimeType;
                    if ( regexp.search( *it ) )
                    {
                        mimeType = regexp.cap( 1 ).stripWhiteSpace();
                    }
                    if ( mimeType.isEmpty() )
                    {
                        mimeType = "text/plain";
                    }
                    temp = "Content-Type: ";
                    temp += mimeType;
                    temp += "; charset=";
                    temp += encodingStr;
                    temp += "\\n";
                    (*it) = temp;
                }
            }
            ++it;
        }
        if(!found)
        {
            headerList.append(temp);
        }
    }
   if(!usePrefs || saveOptions.updateEncoding)
   {
      found=false;

      temp="Content-Transfer-Encoding: 8bit\\n";

      for( it = headerList.begin(); it != headerList.end(); ++it )
      {
         if((*it).contains(QRegExp("^ *Content-Transfer-Encoding:.*")))
         {
            (*it) = temp;
            found=true;
            break;
         }
       }
       if(!found)
       {
          headerList.append(temp);
       }
   }

   temp="X-Generator: KBabel %1\\n";
   temp=temp.arg(VERSION);
   found=false;

   for( it = headerList.begin(); it != headerList.end(); ++it )
   {
      if((*it).contains(QRegExp("^ *X-Generator:.*")))
      {
         (*it) = temp;
         found=true;
         break;
      }
    }
    if(!found)
    {
       headerList.append(temp);
    }
    
   // ensure MIME-Version header
   temp="MIME-Version: 1.0\\n";
   found=false;
   for( it = headerList.begin(); it != headerList.end(); ++it )
   {
	if((*it).contains(QRegExp("^ *MIME-Version:")))
    	{
         (*it) = temp;
         found=true;
         break;
    	}
   }
   if( !found )
   {
	headerList.append(temp);
   }


   temp="Plural-Forms: %1\\n";
   temp=temp.arg(identityOptions.gnuPluralFormHeader);
   found=false;

   // update plural form header
   if( !identityOptions.gnuPluralFormHeader.isEmpty() )
   {
	for( it = headerList.begin(); it != headerList.end(); ++it )
	{
    	    if((*it).contains(QRegExp("^ *Plural-Forms:")))
    	    {
        	(*it) = temp;
        	found=true;
                break;
    	    }
	}
        if( !found )
        {
            headerList.append(temp);
        }
   }

   oldHeader.setMsgstr( headerList.join( "\n" ) );

   //comment = description, copyrights
   if(!usePrefs || (saveOptions.FSFCopyright != ProjectSettingsBase::NoChange))
   {
      found=false;

      for( it = commentList.begin(); it != commentList.end(); ++it )
      {
         // U+00A9 is the Copyright sign
         if ( (*it).find( QRegExp("^# *Copyright (\\(C\\)|\\x00a9).*Free Software Foundation, Inc") ) != -1 )
         {
            found=true;
	    break;
         }
       }
       if(found)
       {
            if ( (*it).find( QRegExp("^# *Copyright (\\(C\\)|\\x00a9) YEAR Free Software Foundation, Inc\\.") ) != -1 )
	    {
		//template string
    		if( saveOptions.FSFCopyright == ProjectSettingsBase::Remove) 
		    (*it).remove(" YEAR Free Software Foundation, Inc");
		else 
		    (*it).replace("YEAR", QDate::currentDate().toString("yyyy"));
	    } else
	    if( saveOptions.FSFCopyright == ProjectSettingsBase::Update )
	    {
		    //update years
		    QString cy = QDate::currentDate().toString("yyyy");
		    if( !(*it).contains( QRegExp(cy)) ) // is the year already included?
		    {
			int index = (*it).findRev( QRegExp("[\\d]+[\\d\\-, ]*") );
			if( index == -1 )
			{
			    KMessageBox::information(0,i18n("Free Software Foundation Copyright does not contain any year. "
			    "It will not be updated."));
			} else {
			    (*it).insert(index+1, QString(", ")+cy);
			}
		    }
	    }
	}
   }

    if ( ( !usePrefs || saveOptions.updateDescription )
           && ( !saveOptions.descriptionString.isEmpty() ) )
    {
        temp = "# "+saveOptions.descriptionString;
        temp.replace( "@PACKAGE@", packageName());
        temp.replace( "@LANGUAGE@", identityOptions.languageName);
        temp = temp.stripWhiteSpace();

        // The description strings has often buggy variants already in the file, these must be removed
        QString regexpstr = "^#\\s+" + QRegExp::escape( saveOptions.descriptionString.stripWhiteSpace() ) + "\\s*$";
        regexpstr.replace( "@PACKAGE@", ".*" );
        regexpstr.replace( "@LANGUAGE@", ".*" );
        //kdDebug() << "REGEXPSTR: " <<  regexpstr << endl;
        QRegExp regexp ( regexpstr );

        // The buggy variants exist in English too (of a time before KBabel got a translation for the corresponding language)
        QRegExp regexpUntranslated ( "^#\\s+Translation of .* into .*\\s*$" );

        kdDebug () << "Temp is '" << temp << "'" << endl;

        found=false;
//not used anyway        bool foundTemplate=false;

        it = commentList.begin();
        while ( it != commentList.end() )
        {
            kdDebug () << "testing '" << (*it) << "'" << endl;
            bool deleteItem = false;

            if ( (*it) == temp )
            {
                kdDebug () << "Match " << endl;
                if ( found )
                    deleteItem = true;
                else
                    found=true;
            }
            else if ( regexp.search( *it ) >= 0 )
            {
                // We have a similar (translated) string (from another project or another language (perhaps typos)). Remove it.
                deleteItem = true;
            }
            else if ( regexpUntranslated.search( *it ) >= 0 )
            {
                // We have a similar (untranslated) string (from another project or another language (perhaps typos)). Remove it.
                deleteItem = true;
            }
            else if ( (*it) == "# SOME DESCRIPTIVE TITLE." )
            {
                // We have the standard title placeholder, remove it
                deleteItem = true;
            }

            if ( deleteItem )
                it = commentList.remove( it );
            else
                ++it;
        }
        if(!found) commentList.prepend(temp);
    }

    // kdDebug() << "HEADER COMMENT: " << commentList << endl;

    if ( (!usePrefs || saveOptions.updateTranslatorCopyright) 
        && ( ! identityOptions.authorName.isEmpty() ) 
        && ( ! identityOptions.authorEmail.isEmpty() ) ) // An email address can be used as ersatz of a name
    {
        QStringList foundAuthors;
    
        temp = "# ";
        temp += identityOptions.authorName;
        if(!identityOptions.authorEmail.isEmpty())
        {
            temp+=(" <"+identityOptions.authorEmail+">");
        }
        temp+=", "+QDate::currentDate().toString("yyyy")+".";

        // ### TODO: it would be nice if the entry could start with "COPYRIGHT" and have the "(C)" symbol (both not mandatory)
        QRegExp regexpAuthorYear( "^#.*(<.+@.+>)?,\\s*([\\d]+[\\d\\-, ]*|YEAR)" );
        QRegExp regexpYearAlone( "^# , \\d{4}.?\\s*$" );
        it = commentList.begin();
        while ( it != commentList.end() )
        {
            bool deleteItem = false;
            if ( (*it).find ( "copyright", 0, false ) != -1 )
            {
                // We have a line with a copyright. It should not be moved.
            }
            else if ( (*it).find ( regexpYearAlone ) != -1 )
            {
                // We have found a year number that is preceeded by a comma.
                // That is typical of KBabel 1.10 (and earlier?) when there is neither an author name nor an email
                // Remove the entry
                deleteItem = true;
            }
            else if ( (*it) == "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR." )
            {
                // Typical placeholder, remove it.
                deleteItem = true;
            }
            else if ( (*it).find ( regexpAuthorYear ) != -1 ) // email address followed by year
            {
                if ( foundAuthors.find( (*it) ) == foundAuthors.end() )
                {
                    // The author line is new (and not a duplicate), so add it to the author line list
                    foundAuthors.append( (*it) );
                }
                // Delete also non-duplicated entry, as now all what is needed will be processed in foundAuthors
                deleteItem = true;
            }
            
            if ( deleteItem )
                it = commentList.remove( it );
            else
                ++it;
        }
    
        if( !foundAuthors.isEmpty() )
        {
            found = false;
            bool foundAuthor = false;
    
            const QString cy = QDate::currentDate().toString("yyyy");

            ait = foundAuthors.end();
            for( it = foundAuthors.begin() ; it!=foundAuthors.end(); ++it )
            {
                if ( (*it).find( QRegExp(
                    QRegExp::escape( identityOptions.authorName )+".*"
                    + QRegExp::escape( identityOptions.authorEmail ) ) ) != -1 )
                {
                    foundAuthor = true;
                    if( (*it).find( cy ) != -1 )
                        found = true;
                    else
                        ait = it;
                }
            }
            if( !found )
            {
                if ( !foundAuthor )
                    foundAuthors.append(temp);
                else if ( ait != foundAuthors.end() )
                {
                    //update years
                    const int index = (*ait).findRev( QRegExp("[\\d]+[\\d\\-, ]*") );
                    if ( index == -1 )
                        (*ait)+=", "+cy;
                    else
                        (*ait).insert(index+1, QString(", ")+cy);
                }
                else
                    kdDebug() << "INTERNAL ERROR: author found but iterator dangling!" << endl;
            }
    
        }
        else
            foundAuthors.append(temp);
        it=commentList.end();
        do
            --it;
        while( ( it != commentList.begin() ) && ( (*it).find( QRegExp( "^#(\\s*$|[:,\\.])" ) ) == -1 ) );
        ++it;
        for( ait = foundAuthors.begin() ; ait != foundAuthors.end() ; ++ait )
        {
            QString s = (*ait);

            // ensure dot at the end of copyright
            if( !s.endsWith(".") ) s += ".";
            commentList.insert(it, s);
        }
    }

   oldHeader.setComment( commentList.join( "\n" ) );

   return oldHeader;
}

void Catalog::setFuzzy(uint index, bool on)
{
    if (  d->_entries.isEmpty() )
        return;

   uint max=d->_entries.count()-1;
   if(index > max)
       return;

   if(d->_entries[index].isFuzzy() != on)
   {
      applyBeginCommand( index, Comment, 0 );

      QPtrList<EditCommand> editList;
      if(on)
      {
          editList=d->_entries[index].addFuzzy(false);
      }
      else
      {
          editList=d->_entries[index].removeFuzzy(false);
          d->_fuzzyIndex.remove(index);
      }

      for ( EditCommand* cmd=editList.first(); cmd != 0; cmd=editList.next() )
      {
         cmd->setIndex(index);
         applyEditCommand(cmd,0);
      }

      setModified(true);

      applyEndCommand( index, Comment, 0 );

      emit signalNumberOfFuzziesChanged(numberOfFuzzies());
   }

}

void Catalog::removeFuzzyStatus(uint index)
{
    setFuzzy(index,false);
}


void Catalog::setModified(bool flag)
{
    bool old=d->_modified;
    d->_modified=flag;

    if(old!=d->_modified)
       emit signalModified(flag);
}


QString Catalog::packageName() const
{
    if( !d->_packageName.isNull() ) return d->_packageName;

    QString package=d->_url.fileName();

    int index=package.find(QRegExp("(\\."+identitySettings().languageCode+")?\\.pot?$"));

    if(index>0)
      package=package.left(index);

    return package;
}

void Catalog::setPackage(const QString& package )
{
    const int pos = package.findRev( '/' );
    if( pos < 0 )
    {
	d->_packageDir = QString();
	d->_packageName = package;
    }
    else
    {
	d->_packageDir = package.left( pos + 1 ); // We want the / included
	d->_packageName = package.mid( pos + 1 ); // We do not want /
    }
    kdDebug() << k_funcinfo << " " << package << " => " << d->_packageDir << " + " << d->_packageName << endl;
}

QString Catalog::packageDir() const
{
    QString result;
    if( !d->_packageDir.isNull() ) result=d->_packageDir;
    else result=d->_url.directory(false);

    return result;
}

QString Catalog::encoding() const
{
    SaveSettings options = saveSettings();
    
    QString encodingStr;
    if(options.useOldEncoding && d->fileCodec)
    {
	    encodingStr = charsetString(d->fileCodec);
    }
    else
    {
        encodingStr= charsetString(options.encoding);
    }

    return encodingStr;
}

ConversionStatus Catalog::openURL(const KURL& url, const QString& package)
{
   QString target;
   ConversionStatus error = OK;

   if(KIO::NetAccess::download(url, target, NULL))
   {
       CatalogImportPlugin* filter=0;

       // gimme plugin for this MIME type
       KMimeType::Ptr mime = KMimeType::findByURL( url, 0, true );
       kdDebug() << "Found mimetype: " << mime->name() <<  endl;
       KTrader::OfferList offers = KTrader::self()->query("KBabelFilter", "('"+mime->name()+"' in [X-KDE-Import])");
       KService::Ptr ptr = offers.first();

       // we have no offer for this MIME type
       if( !ptr )
       {
    	    kdDebug(KBABEL) << "No plugin for this type, will try PO" << endl;
    	    offers = KTrader::self()->query("KBabelFilter", "('application/x-gettext' in [X-KDE-Import])");
    	    ptr = offers.first();
    	    if( !ptr )
	    {
    		kdDebug(KBABEL) << "No plugin for PO files, giving up" << endl;
    		KIO::NetAccess::removeTempFile(target);
    		return NO_PLUGIN;
	    }
       }

       // try to load the library, if unsuccesfull, we have an installation problem
       KLibFactory *factory = KLibLoader::self()->factory( ptr->library().local8Bit() );
       if (!factory)
       {
    	    kdDebug(KBABEL) << "No factory" << endl;
    	    KIO::NetAccess::removeTempFile(target);
    	    return OS_ERROR;
       }

       // create the filter
       filter = static_cast<CatalogImportPlugin*>(factory->create(0, 0));

       // provide progress bar indication
	connect( filter, SIGNAL( signalResetProgressBar(QString,int) ),
	    this, SIGNAL( signalResetProgressBar(QString,int) ));
	connect( filter, SIGNAL( signalProgress(int) ),
	    this, SIGNAL( signalProgress(int) ));
	connect( filter, SIGNAL( signalClearProgressBar() ),
	    this, SIGNAL( signalClearProgressBar() ));

	connect( this, SIGNAL( signalStopActivity() ),
	    filter, SLOT( stop() ));
	    
	// load in the file (target is always local)
	d->_active = true;
	kdDebug(KBABEL) << "openURL active" << endl;
        error = filter->open(target,mime->name(),this);
	// we should be not freed yet
	d->_active = false;
	kdDebug(KBABEL) << "openURL not active" << endl;
	if( error == STOPPED )
	{
	    delete filter;
	    return STOPPED;
	}

	if( error == OK || error == RECOVERED_PARSE_ERROR || error == RECOVERED_HEADER_ERROR )
	{
            const uint entries = numberOfEntries();

            if ( !entries )
            {
                // KBabel cannot work correctly with not any entry
                kdWarning() << k_funcinfo << " No entries! Assuming parse error!" << endl;
                delete filter;
                return NO_ENTRY_ERROR;
            }
            
            //kdDebug( KBABEL ) << k_funcinfo << " Success (full or partial) " << entries << endl;
	    setModified(false);
	    d->_url=url;

            if( package.isEmpty() )
	    {
		d->_packageName=QString::null;
		d->_packageDir=QString::null;
	    }
	    else setPackage(package);

	    emit signalFileOpened(d->_readOnly);
	    emit signalNumberOfFuzziesChanged(numberOfFuzzies());
	    emit signalNumberOfUntranslatedChanged(numberOfUntranslated());
	    emit signalTotalNumberChanged( entries );
	}

	delete filter;

        return error;
   }
   else
   {
      return OS_ERROR;
   }
}

ConversionStatus Catalog::openURL(const KURL& openUrl, const KURL& saveURL, const QString& package)
{
   QString target;
   ConversionStatus error = OK;

   if(KIO::NetAccess::download(openUrl, target, NULL))
   {
       CatalogImportPlugin* filter=0;

       // gimme plugin for this MIME type
       KMimeType::Ptr mime = KMimeType::findByURL( openUrl, 0, true );
       KTrader::OfferList offers = KTrader::self()->query("KBabelFilter", "('"+mime->name()+"' in [X-KDE-Import])");
       KService::Ptr ptr = offers.first();

       // we have no offer for this MIME type
       if( !ptr )
       {
    	    kdDebug(KBABEL) << "No plugin for this type" << endl;
    	    KIO::NetAccess::removeTempFile(target);
    	    return NO_PLUGIN;
       }

       // try to load the library, if unsuccesfull, we have an installation problem
       KLibFactory *factory = KLibLoader::self()->factory( ptr->library().local8Bit() );
       if (!factory)
       {
    	    kdDebug(KBABEL) << "No factory" << endl;
    	    KIO::NetAccess::removeTempFile(target);
    	    return OS_ERROR;
       }

       // create the filter
       filter = static_cast<CatalogImportPlugin*>(factory->create(0, 0));

       // provide progress bar indication
	connect( filter, SIGNAL( signalResetProgressBar(QString,int) ),
	    this, SIGNAL( signalResetProgressBar(QString,int) ));
	connect( filter, SIGNAL( signalProgress(int) ),
	    this, SIGNAL( signalProgress(int) ));
	connect( filter, SIGNAL( signalClearProgressBar() ),
	    this, SIGNAL( signalClearProgressBar() ));

	connect( this, SIGNAL( signalStopActivity() ),
	    filter, SLOT( stop() ));

	// load in the file (target is always local)
	d->_active = true;
	kdDebug(KBABEL) << "openURL - template active" << endl;
        error = filter->open(target,mime->name(),this);
	// we should be not freed yet
	kdDebug(KBABEL) << "openURL - template not active" << endl;
	d->_active = false;
	if( error == STOPPED )
        {
	    delete filter;
            KIO::NetAccess::removeTempFile(target);
	    return STOPPED;
	}

        // Templates should not have recoverable errors (or they are bad templates)
	if( error == OK )
	{
            const uint entries = numberOfEntries();

            if ( !entries )
            {
                // KBabel cannot work correctly with not any entry
                kdWarning() << k_funcinfo << " No entries! Assuming parse error!" << endl;
                delete filter;
                KIO::NetAccess::removeTempFile(target);
                return NO_ENTRY_ERROR;
            }
            
           setModified(false);
           d->_url = saveURL;
           if( package.isEmpty() )
	   {
		d->_packageName=QString::null;
		d->_packageDir=QString::null;
	   }
	   else setPackage(package);

           emit signalFileOpened(d->_readOnly);
           emit signalNumberOfFuzziesChanged(numberOfFuzzies());
           emit signalNumberOfUntranslatedChanged(numberOfUntranslated());
           emit signalTotalNumberChanged( entries );
        }

	delete filter;

        // and remove the temp file
        KIO::NetAccess::removeTempFile(target);

        return error;
   }
   else
   {
      return OS_ERROR;
   }
}

Msgfmt::Status Catalog::checkSyntax(QString& output, bool clearErrors)
{
   if( !d->_mimeTypes.contains( "application/x-gettext" ) )
	return Msgfmt::Unsupported;

   QString filename;
   bool tempFileUsed=false;

   if(d->_url.isLocalFile() && !isModified())
   {
      filename=d->_url.path(0);
   }
   else
   {
      tempFileUsed=true;
      filename=saveTempFile();
   }

   Msgfmt msgfmt;
   Msgfmt::Status result = msgfmt.checkSyntax( filename , output, pluralFormType() != KDESpecific );

   if( clearErrors) clearErrorList();

   if( result==Msgfmt::SyntaxError )
   {
      int currentIndex=-1;
      int currentLine=0;

      if( !d->_header.msgstr().isEmpty() )
         currentLine=d->_header.totalLines()+1;

      // ### KDE4: return "lines" not "output"
      const QStringList lines = QStringList::split("\n",output);
      for ( QStringList::const_iterator it = lines.constBegin(); it != lines.constEnd(); ++it )
      {
         if( (*it).find(QRegExp("^.+:\\d+:")) >= 0 )
         {
            const int begin=(*it).find(":",0)+1;
            const int end=(*it).find(":",begin);

            const QString line=(*it).mid(begin,end-begin);

            while( line.toInt() > currentLine )
            {
               currentIndex++;
               currentLine += ( d->_entries[currentIndex].totalLines() + 1 );
            }

	    if( currentIndex == -1 )
	    {
		// header error
		result = Msgfmt::HeaderError;
		continue;
	    }

            if( !d->_errorIndex.contains(currentIndex) )
            {
               d->_errorIndex.append(currentIndex);
			   d->_entries[currentIndex].setSyntaxError(true);
            }
         }
      }
   }

   if(tempFileUsed)
      QFile::remove(filename);

   return result;
}

void Catalog::clearErrorList()
{
	QValueList<uint>::Iterator it;
	for(it = d->_errorIndex.begin(); it != d->_errorIndex.end(); ++it)
	{
		d->_entries[(*it)].setSyntaxError(false);
		d->_entries[(*it)].clearErrors();
	}

	d->_errorIndex.clear();
}

void Catalog::removeFromErrorList(uint index)
{
	if(d->_errorIndex.contains(index))
	{
		d->_errorIndex.remove(index);
		d->_entries[index].setSyntaxError(false);
		d->_entries[index].clearErrors();
	}
}

QStringList Catalog::itemStatus(uint index, bool recheck, QPtrList<KDataTool> whatToCheck)
{
    if (  d->_entries.isEmpty() )
        return QStringList();

	uint max=d->_entries.count()-1;
	if(index > max)
		index=max;

	CatalogItem& item = d->_entries[index];

	if(recheck)
	{
	    for( KDataTool* t = whatToCheck.first(); t ; t=whatToCheck.next() )
	    {
		t->run("validate", (void*)(&item), "CatalogItem", "application/x-kbabel-catalogitem" );
	    }
	}

	return item.errors();
}

QStringList Catalog::itemStatus(uint index)
{
       if (  d->_entries.isEmpty() )
        return QStringList();

	uint max=d->_entries.count()-1;
	if(index > max)
		index=max;

	CatalogItem& item = d->_entries[index];

	return item.errors();
}

bool Catalog::checkUsingTool(KDataTool* tool, bool clearErrors)
{
	if(clearErrors)
		clearErrorList();

	kdDebug(KBABEL) << "checkUsingTool active" << endl;
	d->_active=true;
	d->_stop=false;
	connect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));

	int index = 0;
	bool hasErrors=false;

	emit signalResetProgressBar(i18n("validating file"),100);

	for ( QValueVector<CatalogItem>::Iterator it = d->_entries.begin();
					it != d->_entries.end(); ++it, index++ )
	{
	    if( !tool->run( "validate", (void*)(&(*it)), "CatalogItem", "application/x-kbabel-catalogitem" ))
	    {
		if( !d->_errorIndex.contains(index) )
		{
			d->_errorIndex.append(index);
			hasErrors=true;
		}
	    }
	    if( d->_stop ) break;
	    emit signalProgress((index*100)/d->_entries.count());
	}

	if( hasErrors && !clearErrors ) qHeapSort(d->_errorIndex);

	kdDebug(KBABEL) << "checkUsingTool not active" << endl;
	d->_active=false;
	d->_stop=false;
	disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));

	emit signalClearProgressBar();

	return !hasErrors;
}

void Catalog::modifyUsingTool(KDataTool* tool, const QString& command)
{
    kdDebug(KBABEL) << "modifyUsingTool active" << endl;
    d->_active=true;
    d->_stop=false;
    connect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));

    int index = 0;
    bool modified = false;

    emit signalResetProgressBar(i18n("applying tool"),100);

    for ( QValueVector<CatalogItem>::Iterator it = d->_entries.begin();
    				it != d->_entries.end(); ++it, index++ )
    {
	CatalogItem dummyItem( *it );

	tool->run( command, (void*)(&dummyItem), "CatalogItem", "application/x-kbabel-catalogitem" );

	if( (*it).msgstr() != dummyItem.msgstr() || (*it).comment() != dummyItem.comment() )
	{
	    if( !modified )
	    {
		applyBeginCommand(0,Msgstr,0);
		modified = true;
	    }

	    if( (*it).msgstr() != dummyItem.msgstr() )
	    {
		uint in = 0; // number of current lural form
		// go over all plural forms and test, which changed
		for ( QStringList::Iterator itorig = (*it).msgstr().begin()
			, itchanged = dummyItem.msgstr().begin()
			; itorig != (*it).msgstr().end()
			; ++itorig, ++itchanged) {
		    if( (*itorig) != (*itchanged) )
		    {
			EditCommand* cmd = new DelTextCmd(0,(*itorig),index);
			cmd->setPart(Msgstr);
			applyEditCommand(cmd,0);
			cmd = new InsTextCmd(0,(*itchanged),index);
			cmd->setPart(Msgstr);
			applyEditCommand(cmd,0);
		    }
		    in++;
		}
	    }

	    if( (*it).comment() != dummyItem.comment() )
	    {
		EditCommand* cmd = new DelTextCmd(0,(*it).comment(),0);
		cmd->setPart(Comment);
		cmd->setIndex(index);
		applyEditCommand(cmd,0);
		cmd = new InsTextCmd(0,dummyItem.comment(),0);
		cmd->setPart(Comment);
		cmd->setIndex(index);
		applyEditCommand(cmd,0);
		kdDebug(KBABEL) << "DummyItem comment is " << dummyItem.comment() << endl;
	    }
	}

	if( d->_stop ) break;
	emit signalProgress((index*100)/d->_entries.count());
    }

    if( modified ) applyEndCommand(0, Msgstr, 0);

    kdDebug(KBABEL) << "modifyUsingTool not active" << endl;
    d->_active=false;
    d->_stop=false;
    disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));

    emit signalClearProgressBar();
}

void Catalog::clear()
{
    d->_errorIndex.clear();
    d->_entries.clear();
    d->_url=KURL();
    d->_obsoleteEntries.clear();

    if(d->_undoList.count() > 0)
       emit signalUndoAvailable(false);
    if(d->_redoList.count() > 0)
       emit signalRedoAvailable(false);

    d->_undoList.clear();
    d->_redoList.clear();

    d->msgidDiffList.clear();
    d->msgstr2MsgidDiffList.clear();
    d->diffCache.clear();
}


uint Catalog::numberOfEntries() const
{
   return d->_entries.count();
}

uint Catalog::numberOfFuzzies() const
{
   return d->_fuzzyIndex.count();
}

uint Catalog::numberOfUntranslated() const
{
   return d->_untransIndex.count();
}


bool Catalog::hasFuzzyInFront(uint index)  const
{
   if(findPrevInList(d->_fuzzyIndex,index)>=0)
   {
      return true;
   }

   return false;
}

bool Catalog::hasFuzzyAfterwards(uint index) const
{
   if(findNextInList(d->_fuzzyIndex,index)>=0)
   {
      return true;
   }

   return false;
}

bool Catalog::hasUntranslatedInFront(uint index) const
{
   if(findPrevInList(d->_untransIndex,index)>=0)
   {
      return true;
   }

   return false;
}

bool Catalog::hasUntranslatedAfterwards(uint index) const
{
   if(findNextInList(d->_untransIndex,index)>=0)
   {
      return true;
   }

   return false;
}

bool Catalog::hasErrorInFront(uint index)  const
{
   if(findPrevInList(d->_errorIndex,index)>=0)
   {
      return true;
   }

   return false;
}

bool Catalog::hasErrorAfterwards(uint index) const
{
   if(findNextInList(d->_errorIndex,index)>=0)
   {
      return true;
   }

   return false;
}

bool Catalog::isFuzzy(uint index) const
{
    if (  d->_entries.isEmpty() )
        return false;

   if(index > numberOfEntries())
      return false;

   return d->_entries[index].isFuzzy();
}


bool Catalog::isUntranslated(uint index) const
{
    if (  d->_entries.isEmpty() )
        return false;

   if(index > numberOfEntries())
      return false;

   return d->_entries[index].isUntranslated();
}

bool Catalog::hasError(uint index, DocPosition& pos) const
{
    if( d->_errorIndex.contains(index) )
    {
	pos.item=index;
	pos.form=0;
	return true;
    }
   return false;
}

PluralFormType Catalog::pluralForm(uint index) const
{
    if (  d->_entries.isEmpty() )
        return NoPluralForm;

    if(index > numberOfEntries())
        return NoPluralForm;

    return static_cast<PluralFormType>(d->_entries[index].pluralForm());
}

PluralFormType Catalog::pluralFormType() const
{
    if (  d->_entries.isEmpty() )
        return NoPluralForm;

    for( uint i = 0 ; i < numberOfEntries(); i++)
    {
	if( d->_entries[i].pluralForm() != NoPluralForm )
	    return d->_entries[i].pluralForm();
    }
    
    return NoPluralForm;
}

int Catalog::nextFuzzy(uint startIndex, DocPosition& pos) const
{
   pos.item=findNextInList(d->_fuzzyIndex,startIndex);
   pos.form=0;
   return pos.item;
}

int Catalog::prevFuzzy(uint startIndex, DocPosition& pos) const
{
   pos.item=findPrevInList(d->_fuzzyIndex,startIndex);
   pos.form=0;
   return pos.item;
}

int Catalog::nextUntranslated(uint startIndex, DocPosition& pos) const
{
   pos.item=findNextInList(d->_untransIndex,startIndex);
   pos.form=0;
   return pos.item;
}

int Catalog::prevUntranslated(uint startIndex, DocPosition& pos) const
{
   pos.item=findPrevInList(d->_untransIndex,startIndex);
   pos.form=0;
   return pos.item;
}


int Catalog::nextError(uint startIndex, DocPosition& pos) const
{
   pos.item=findNextInList(d->_errorIndex,startIndex);
   pos.form=0;
   return pos.item;
}

int Catalog::prevError(uint startIndex, DocPosition& pos) const
{
   pos.item=findPrevInList(d->_errorIndex,startIndex);
   pos.form=0;
   return pos.item;
}


void Catalog::registerView(CatalogView* view)
{
   if(d->_views.containsRef(view)==0)
   {
      d->_views.append(view);
   }
}


void Catalog::removeView(CatalogView* view)
{
   d->_views.removeRef(view);
}


void Catalog::updateViews(EditCommand* cmd,CatalogView* view2exclude)
{
    CatalogView* view;
    for ( view=d->_views.first(); view != 0; view=d->_views.next())
    {
       if(view!=view2exclude)
       {
          view->update(cmd);
       }
    }
}



bool Catalog::hasView() const
{
    if(d->_views.count()==0)
           return false;

    return true;
}

bool Catalog::isLastView() const
{
    if(d->_views.count()<=1)
           return true;

    return false;
}


void Catalog::useProject(Project::Ptr project)
{
    d->_project->config()->sync();
    d->_project = project;
    readPreferences();
    emit signalSettingsChanged(saveSettings());
    emit signalSettingsChanged(identitySettings());
    emit signalSettingsChanged(miscSettings());
    emit signalSettingsChanged(tagSettings());
}

Project::Ptr Catalog::project() const
{
    return d->_project;
}

void Catalog::readPreferences()
{
    getNumberOfPluralForms();
    
    d->_project->config()->setGroup("Tags");
    d->_tagSettings.tagExpressions=d->_project->config()->readListEntry("TagExpressions");
    if( d->_tagSettings.tagExpressions.empty() ) d->_tagSettings.tagExpressions = Defaults::Tag::tagExpressions();
    d->_tagExtractor->setRegExpList(d->_tagSettings.tagExpressions) ;

    d->_tagSettings.argExpressions=d->_project->config()->readListEntry("ArgExpressions");
    if( d->_tagSettings.argExpressions.empty() ) d->_tagSettings.argExpressions = Defaults::Tag::argExpressions();
    d->_argExtractor->setRegExpList(d->_tagSettings.argExpressions) ;
}

void Catalog::savePreferences()
{
   d->_project->config()->setGroup("Tags");

   d->_project->config()->writeEntry( "TagExpressions", d->_tagSettings.tagExpressions );
   d->_project->config()->writeEntry( "ArgExpressions", d->_tagSettings.argExpressions );

   d->_project->config()->sync();
}

IdentitySettings Catalog::identitySettings() const
{
    return d->_project->identitySettings();
}

SaveSettings Catalog::saveSettings() const
{
    return d->_project->saveSettings();

}

MiscSettings Catalog::miscSettings() const
{
    return d->_project->miscSettings();

}

TagSettings Catalog::tagSettings() const
{
    return d->_tagSettings;

}

bool Catalog::isGeneratedFromDocbook() const
{
    return d->_generatedFromDocbook;
}

QString Catalog::package() const
{
    return packageDir()+packageName();
}

bool Catalog::isReadOnly() const
{
    return d->_readOnly;
}

void Catalog::setSettings(SaveSettings settings)
{
   d->_project->setSettings(settings);
}

void Catalog::setSettings(IdentitySettings settings)
{
   IdentitySettings oldsettings = d->_project->identitySettings();
   
   QString oldLanguageCode = oldsettings.languageCode;
   int oldForms = oldsettings.numberOfPluralForms;


   d->_project->setSettings(settings);

   if(oldLanguageCode != settings.languageCode)
   {
       getNumberOfPluralForms();
   }

    if(oldForms != settings.numberOfPluralForms)
    {
        getNumberOfPluralForms();
    }

   emit signalSettingsChanged(settings);
}

void Catalog::setSettings(MiscSettings settings)
{
    d->_project->setSettings(settings);
    emit signalSettingsChanged(settings);
}

void Catalog::setSettings(TagSettings settings)
{
	d->_tagSettings=settings;

	emit signalSettingsChanged(settings);
}

void Catalog::generateIndexLists()
{
   d->_fuzzyIndex.clear();
   d->_untransIndex.clear();
   clearErrorList();

   uint counter=0;
   for ( QValueVector<CatalogItem>::Iterator it = d->_entries.begin(); it != d->_entries.end(); ++it )
   {
       if((*it).isUntranslated())
       {
          d->_untransIndex.append(counter);
       }
       else if((*it).isFuzzy())
       {
          d->_fuzzyIndex.append(counter);
       }

       counter++;
   }

}

int Catalog::findNextInList(const QValueList<uint>& list,uint index) const
{
    QValueList<uint>::ConstIterator it;

    int nextIndex=-1;

    // find index in List
    it=list.find(index);

    // if the given index is found in the list and not the last entry
    // in the list, return the next listentry
    if(it!=list.end() && it!=list.fromLast())
    {
       ++it;
       return (*it);
    }

    // if the index is not in the list, search the index in the list, that
    // is the nearest to the given index
    for( it = list.begin(); it != list.end(); ++it )
    {
       if((*it) > index)
       {
          nextIndex=(*it);
          break;
       }
    }


    return nextIndex;
}

int Catalog::findPrevInList(const QValueList<uint>& list,uint index) const
{
    QValueList<uint>::ConstIterator it;

    int prevIndex=-1;

    it=list.find(index);

    // if the given index is found in the list and not the last entry
    // in the list, return the next listentry
    if(it!=list.end() && it!=list.begin())
    {
       --it;
       return (*it);
    }


    // if the index is not in the list, search the index in the list, that
    // is the nearest to the given index
    for( it = list.fromLast(); it != list.end(); --it )
    {
       if((*it) < index)
       {
          prevIndex=(*it);
          break;
       }
       
       if ( it == list.constBegin() )
       {
           // Decremeniting the iterator at the begin is undefined, so break the loop
           break;
       }
    }


    return prevIndex;
}


QString Catalog::dateTime() const
{
    const QDateTime dt = QDateTime::currentDateTime();
    QString dateTimeString;
    
    const SaveSettings options = d->_project->saveSettings();

    switch(options.dateFormat)
    {
        case Qt::LocalDate:
        {
            dateTimeString = KGlobal::locale()->formatDateTime( dt );
            break;
        }
        case Qt::ISODate:
        {
            dateTimeString = dt.toString("yyyy-MM-dd hh:mm");
            QTime t;
            const int offset = KRFCDate::localUTCOffset();
            const int correction = offset < 0 ? -60 : 60 ;
            t = t.addSecs( offset * correction );
            dateTimeString += ( offset < 0 ? "-" : "+" );
            dateTimeString += t.toString("hhmm");
            break;
        }
        case Qt::TextDate:
        {
            dateTimeString = options.customDateFormat;

            const QDate date = dt.date();
            const QTime time = dt.time();

            // the year
            dateTimeString.replace( "%Y", QString::number( date.year() ) );
            dateTimeString.replace( "%y", QString::number( date.year() ).right(2) );

            // the month
            if(date.month()<10)
            {
                dateTimeString.replace( "%m", "0"+QString::number( date.month() ) );
            }
            else
            {
                dateTimeString.replace( "%m", QString::number( date.month() ) );
            }

            dateTimeString.replace( "%f", QString::number( date.month() ) );

            dateTimeString.replace( "%b", date.longMonthName(date.month()) );
            dateTimeString.replace( "%h", date.longMonthName(date.month()) );

            // the day
            dateTimeString.replace( "%j", QString::number( date.dayOfYear() ) );
            dateTimeString.replace( "%e", QString::number( date.day() ) );
            if(date.day() < 10)
            {
                dateTimeString.replace( "%d", "0"+QString::number( date.day() ) );
            }
            else
            {
                dateTimeString.replace( "%d", QString::number( date.day() ) );
            }

            dateTimeString.replace( "%a", date.longDayName( date.dayOfWeek() ) );


            // hour
            dateTimeString.replace( "%k", QString::number( time.hour() ) );

            if(time.hour() < 10)
            {
                dateTimeString.replace( "%H", "0"+QString::number( time.hour() ) );
            }
            else
            {
                dateTimeString.replace( "%H", QString::number( time.hour() ) );
            }

            QString zone; // AM or PM
            int hour = time.hour();
            if( hour > 12 )
            {
                zone="PM";
                hour -= 12;
            }
            else
            {
                zone="AM";
            }

            dateTimeString.replace( "%I", QString::number( hour ) );

            if(hour < 10)
            {
                dateTimeString.replace( "%i", "0"+QString::number( hour ) );
            }
            else
            {
                dateTimeString.replace( "%i", QString::number( hour ) );
            }

            dateTimeString.replace( "%p", zone );

            // minutes
            if(time.minute() < 10)
            {
                dateTimeString.replace( "%M", "0"+QString::number( time.minute() ) );
            }
            else
            {
                dateTimeString.replace( "%M", QString::number( time.minute() ) );
            }

            // seconds
            if(time.second() < 10)
            {
                dateTimeString.replace( "%S", "0"+QString::number( time.second() ) );
            }
            else
            {
                dateTimeString.replace( "%S", QString::number( time.second() ) );
            }

            // timezone
            dateTimeString.replace( "%Z", d->_project->identitySettings().timeZone );
            QTime t;
            const int offset = KRFCDate::localUTCOffset();
            const int correction = offset < 0 ? -60 : 60;
            t = t.addSecs( offset * correction );
            dateTimeString.replace( "%z", ( offset < 0 ? "-" : "+" ) + t.toString("hhmm") );
            break;
        }
    }
    kdDebug(KBABEL) << "New date: " << dateTimeString << endl;
    return dateTimeString;
}


ConversionStatus Catalog::saveFile()
{
   if(d->_url.isEmpty())
   {
      kdFatal(KBABEL) << "fatal error: empty filename" << endl;
      return NO_FILE;
   }

   return saveFileAs(d->_url,true);
}

ConversionStatus Catalog::saveFileAs(const KURL &url, bool overwrite)
{
   if( d->_active ) return BUSY;

   ConversionStatus status=OK;

   bool newName=false;
   KURL targetURL=d->_url;

   if(url != d->_url)
   {
      newName = true;
      targetURL=url;
   }


   if(d->_project->saveSettings().autoUpdate)
   {
      d->_header=updatedHeader(d->_header);
      emit signalHeaderChanged();
   }


   if(targetURL.isLocalFile())
   {
      // test if the directory exists. If not, create it.
      QDir dir( targetURL.directory());

      QStringList dirList;
      while(!dir.exists() && !dir.dirName().isEmpty())
      {
         dirList.prepend(dir.dirName());
         dir.setPath(dir.path()+"/..");
      }
      for ( QStringList::Iterator it = dirList.begin(); it != dirList.end(); ++it )
      {
         if(!dir.mkdir(*it))
         {
            status=OS_ERROR;
            break;
         }
         dir.cd(*it);
      }

      if(status==OK)
      {
         status=writeFile(targetURL.path(0),overwrite);
      }
   }
   else
   {
      QString tempFile=kapp->tempSaveName(targetURL.path(0));

      status = writeFile(tempFile,overwrite);

      if(status == OK)
      {
         if( !KIO::NetAccess::upload( tempFile, targetURL, NULL ) )
         {
            status = OS_ERROR;
         }
      }

      QFile::remove(tempFile);
   }

   if(status == OK)
   {
      setModified(false);

      if(newName)
      {
         // if we saved a file, the catalog can not be any longer readOnly;
         d->_readOnly=false;

         d->_url=targetURL;

         emit signalFileOpened(d->_readOnly);
      }
   }

   return status;
}

QString Catalog::saveTempFile()
{
   QString filename = kapp->tempSaveName("/temp/kbabel_temp.po");
   if( writeFile(filename) != OK )
   {
      filename = QString::null;
   }

   return filename;
}


ConversionStatus Catalog::writeFile(QString localFile , bool overwrite)
{
   QFileInfo info(localFile);

   if(info.isDir())
      return NO_FILE;

   if(info.exists())
   {
      if(!overwrite || !info.isWritable())
      {
         return NO_PERMISSIONS;
      }
   }
   else // check if the directory is writable
   {
      QFileInfo dir(info.dirPath());
      if(!dir.isWritable())
      {
         return NO_PERMISSIONS;
      }
   }

    ConversionStatus error = OK;
    CatalogExportPlugin* filter=0;

    // gimme plugin for this MIME type
    KMimeType::Ptr mime = KMimeType::findByURL( KURL::fromPathOrURL( localFile  ) );
    KTrader::OfferList offers = KTrader::self()->query("KBabelFilter", "('"+mime->name()+"' in [X-KDE-Export])");
    KService::Ptr ptr = offers.first();

       // we have no offer for this MIME type
    if( !ptr )
    {
    	kdDebug(KBABEL) << "No plugin for this type" << endl;
    	return NO_PLUGIN;
    }

    // try to load the library, if unsuccesfull, we have an installation problem
    KLibFactory *factory = KLibLoader::self()->factory( ptr->library().local8Bit() );
    if (!factory)
    {
    	kdDebug(KBABEL) << "No factory" << endl;
    	return OS_ERROR;
    }

    // create the filter
    filter = static_cast<CatalogExportPlugin*>(factory->create(0, 0));

    // provide progress bar indication
    connect( filter, SIGNAL( signalResetProgressBar(QString,int) ),
	    this, SIGNAL( signalResetProgressBar(QString,int) ));
    connect( filter, SIGNAL( signalProgress(int) ),
	    this, SIGNAL( signalProgress(int) ));
    connect( filter, SIGNAL( signalClearProgressBar() ),
	    this, SIGNAL( signalClearProgressBar() ));

    connect( this, SIGNAL( signalStopActivity() ),
	    filter, SLOT( stop() ));

    // load in the file (target is always local)
    kdDebug(KBABEL) << "writeFile active" << endl;
    d->_active = true;
    error = filter->save(localFile,mime->name(),this);
    // we should be not freed yet
    kdDebug(KBABEL) << "writeFile not active" << endl;
    d->_active = false;
    if( error == STOPPED ) return STOPPED;

    delete filter;

    return error;
}

QTextCodec* Catalog::codecForFile(QString gettextHeader)
{
   QString charset;

   QString head = gettextHeader;

   QRegExp r("Content-Type:\\s*\\w+/[-\\w]+;?\\s*charset\\s*=\\s*[^\\\"\\n]+");
   int begin=r.search(head);
   int len=r.matchedLength();
   if(begin<0) {
   	kdDebug(KBABEL) << "no charset entry found" << endl;
   	return 0;
   }

   head = head.mid(begin,len);

   QRegExp regexp("charset *= *([^\\\\\\\"]+)");
   if( regexp.search( head ) > -1 )
   {
       charset = regexp.cap(1);
   }

   QTextCodec* codec=0;

   if(!charset.isEmpty())
   {
      // "CHARSET" is the default charset entry in a template (pot).
      // characters in a template should be either pure ascii or
      // at least utf8, so utf8-codec can be used for both.
      if( charset == "CHARSET")
      {
          codec=QTextCodec::codecForName("utf8");
          kdDebug(KBABEL)
              << QString("file seems to be a template: using utf8 encoding.")
              << endl;
      }
      else
      {
         codec=QTextCodec::codecForName(charset.latin1());
      }

      if(!codec)
      {
         kdWarning() << "charset found, but no codec available, using UTF8 instead" << endl;
	 codec=QTextCodec::codecForName("utf8");
      }
   }

   return codec;
}

PoInfo Catalog::headerInfo(const CatalogItem headerItem)
{
   QStringList header=headerItem.msgstrAsList();

   QStringList::Iterator it;

   PoInfo info;

   // extract information from the header
   for(it=header.begin();it!=header.end();++it)
   {
      if((*it).contains(QRegExp("^\\s*Project-Id-Version\\s*:\\s*.+\\s*$")))
      {
         info.project=(*it).replace(QRegExp("^\\s*Project-Id-Version\\s*:\\s*"),"");

         if(info.project.right(2)=="\\n")
            info.project.remove(info.project.length()-2,2);

         info.project=info.project.simplifyWhiteSpace();
      }
      else if((*it).contains(QRegExp("^\\s*POT-Creation-Date\\s*:\\s*.+\\s*$")))
      {
         info.creation=(*it).replace(QRegExp("^\\s*POT-Creation-Date\\s*:\\s*"),"");

         if(info.creation.right(2)=="\\n")
            info.creation.remove(info.creation.length()-2,2);

         info.creation=info.creation.simplifyWhiteSpace();
      }
      else if((*it).contains(QRegExp("^\\s*PO-Revision-Date\\s*:\\s*.+\\s*$")))
      {
         info.revision=(*it).replace(QRegExp("^\\s*PO-Revision-Date\\s*:\\s*"),"");

         if(info.revision.right(2)=="\\n")
            info.revision.remove(info.revision.length()-2,2);

         info.revision=info.revision.simplifyWhiteSpace();
      }
      else if((*it).contains(QRegExp("^\\s*Last-Translator\\s*:\\s*.+\\s*$")))
      {
         info.lastTranslator=(*it).replace(QRegExp("^\\s*Last-Translator\\s*:\\s*"),"");

         if(info.lastTranslator.right(2)=="\\n")
            info.lastTranslator.remove(info.lastTranslator.length()-2,2);

         info.lastTranslator=info.lastTranslator.simplifyWhiteSpace();
      }
      else if((*it).contains(QRegExp("^\\s*Language-Team\\s*:\\s*.+\\s*")))
      {
         info.languageTeam=(*it).replace(QRegExp("^\\s*Language-Team\\s*:\\s*"),"");

         if(info.languageTeam.right(2)=="\\n")
            info.languageTeam.remove(info.languageTeam.length()-2,2);

         info.languageTeam=info.languageTeam.simplifyWhiteSpace();
      }
      else if((*it).contains(QRegExp("^\\s*MIME-Version\\s*:\\s*.+\\s*")))
      {
         info.mimeVersion=(*it).replace(QRegExp("^\\s*MIME-Version\\s*:\\s*"),"");

         if(info.mimeVersion.right(2)=="\\n")
            info.mimeVersion.remove(info.mimeVersion.length()-2,2);

         info.mimeVersion=info.mimeVersion.simplifyWhiteSpace();
      }
      else if((*it).contains(QRegExp("^\\s*Content-Type\\s*:\\s*.+\\s*")))
      {
         info.contentType=(*it).replace(QRegExp("^\\s*Content-Type\\s*:\\s*"),"");

         if(info.contentType.right(2)=="\\n")
            info.contentType.remove(info.contentType.length()-2,2);

         info.contentType=info.contentType.simplifyWhiteSpace();
      }
      else if((*it).contains(QRegExp("^\\s*Content-Transfer-Encoding\\s*:\\s*.+\\s*")))
      {
         info.encoding=(*it).replace(QRegExp("^\\s*Content-Transfer-Encoding\\s*:\\s*"),"");

         if(info.encoding.right(2)=="\\n")
            info.encoding.remove(info.encoding.length()-2,2);

         info.encoding=info.encoding.simplifyWhiteSpace();
      }
      else
      {
          QString line=(*it);

         if(line.right(2)=="\\n")
            line.remove(line.length()-2,2);

         line=line.simplifyWhiteSpace();
         if(!info.others.isEmpty())
             info.others+='\n';

         info.others+=line;
      }


   }

   info.headerComment=headerItem.comment();

   return info;
}

bool Catalog::isUndoAvailable()
{
   return !d->_undoList.isEmpty();
}

bool Catalog::isRedoAvailable()
{
   return !d->_redoList.isEmpty();
}

int Catalog::undo()
{
   if(!isUndoAvailable())
      return -1;

   int macroLevel = 0;

   EditCommand *command=0;
   do
   {
      command = d->_undoList.take();
      if ( !command )
      {
         kdError() << "undo command is NULL?" << endl;
         return -1;
      }

      processCommand( command, 0, true );

      macroLevel += command->terminator();

      if ( d->_undoList.isEmpty() )
      {
         emit signalUndoAvailable( false );
      }
      if(d->_redoList.isEmpty())
      {
         emit signalRedoAvailable(true);
      }
      d->_redoList.append(command);

    }
    while(macroLevel != 0);

    return command->index();
}

int Catalog::redo()
{
   if(!isRedoAvailable())
      return -1;

   int macroLevel = 0;
   EditCommand *command=0;

   do
   {
      command = d->_redoList.take();
      if ( !command )
      {
         kdError() << "undo command is NULL?" << endl;
         return -1;
      }

      processCommand( command, 0,false );

      macroLevel += command->terminator();
      if ( d->_redoList.isEmpty() )
      {
         emit signalRedoAvailable( false );
      }
      if ( d->_undoList.isEmpty() )
      {
         emit signalUndoAvailable( true );
      }

      d->_undoList.append( command );
    }
    while (macroLevel != 0);

    return command->index();
}

void Catalog::applyEditCommand(EditCommand* cmd, CatalogView* view)
{

    processCommand(cmd,view);
    setModified(true);

    if ( d->_undoList.isEmpty() )
    {
       emit signalUndoAvailable(true);
    }
    else if ( cmd->merge( d->_undoList.last() ) )
    {
       delete cmd;
       return;
    }


    d->_undoList.append(cmd);


    if ( !d->_redoList.isEmpty() )
    {
       d->_redoList.clear();
       emit signalRedoAvailable( false );
    }

}

void Catalog::applyBeginCommand(uint index, Part part, CatalogView* view)
{
    applyEditCommand( new BeginCommand(index,part), view );
}

void Catalog::applyEndCommand(uint index, Part part, CatalogView* view)
{
    applyEditCommand( new EndCommand(index,part), view );
}

void Catalog::processCommand(EditCommand* cmd,CatalogView* view, bool undo)
{
    kdDebug(KBABEL) << "Catalog::processCommand()" << endl;
    if(cmd->terminator()==0)
    {
       bool checkUntranslated=false;
       bool checkFuzzy=false;
       bool wasFuzzy=false;

       CatalogItem &item=d->_entries[cmd->index()];

       if(cmd->part() == Msgstr)
       {
          if( item.isUntranslated() )
          {
             d->_untransIndex.remove(cmd->index());

             emit signalNumberOfUntranslatedChanged(numberOfUntranslated());
          }
          else
          {
             checkUntranslated=true;
          }
       }
       else if(cmd->part() == Comment)
       {
          checkFuzzy=true;
          wasFuzzy=item.isFuzzy();
       }



       item.processCommand(cmd,undo);

       if(undo)
       {
          EditCommand* tmpCmd=0;
          DelTextCmd* delcmd = (DelTextCmd*) cmd;
          if (delcmd->type() == EditCommand::Delete )
          {
             tmpCmd = new InsTextCmd(delcmd->offset,delcmd->str,delcmd->pluralNumber);
          }
          else
          {
             tmpCmd = new DelTextCmd(delcmd->offset,delcmd->str,delcmd->pluralNumber);
          }

          tmpCmd->setIndex(cmd->index());
          tmpCmd->setPart(cmd->part());

          updateViews(tmpCmd,view);

          delete tmpCmd;
       }
       else
       {
          updateViews(cmd,view);
       }

       if(checkUntranslated && item.isUntranslated())
       {
          QValueList<uint>::Iterator it;

          // insert index in the right place in the list
          it = d->_untransIndex.begin();
          while(it != d->_untransIndex.end() && cmd->index() > (int)(*it))
          {
             ++it;
          }
          d->_untransIndex.insert( it,(uint)(cmd->index()) );

          emit signalNumberOfUntranslatedChanged(numberOfUntranslated());
       }
       else if(checkFuzzy)
       {
          if(wasFuzzy != item.isFuzzy())
          {
             if(wasFuzzy)
             {
                d->_fuzzyIndex.remove(cmd->index());
                emit signalNumberOfFuzziesChanged(numberOfFuzzies());
             }
             else
             {
                QValueList<uint>::Iterator it;

                // insert index in the right place in the list
                it = d->_fuzzyIndex.begin();
                while(it != d->_fuzzyIndex.end() && cmd->index() > (int)(*it))
                {
                   ++it;
                }
                d->_fuzzyIndex.insert( it,(uint)(cmd->index()) );

                emit signalNumberOfFuzziesChanged(numberOfFuzzies());
             }
          }
       }

    }
}

bool Catalog::findNext(const FindOptions* findOpts, DocPosition& docPos, int& len)
{
	bool success = false; // true, when string found
	bool endReached=false;

       kdDebug(KBABEL) << "findNext active" << endl;
	d->_active=true;
	d->_stop=false;
	connect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
	
	MiscSettings miscOptions = miscSettings();

	len=0;
	int pos=0;

	QString searchStr = findOpts->findStr;
	QRegExp regexp(searchStr);

	if( findOpts->isRegExp ) {
		regexp.setCaseSensitive(findOpts->caseSensitive);
	}

	if( docPos.item == numberOfEntries()-1) {
		switch(docPos.part) {
			case Msgid:
				// FIXME: we should search in plurals as well
				if(!findOpts->inMsgstr && !findOpts->inComment
					&& docPos.offset >= msgid(docPos.item).first().length() ) {
					endReached=true;
				}
				break;
			case Msgstr:
				if(!findOpts->inComment && (int)(docPos.form+1) >= numberOfPluralForms(docPos.item)
				    && docPos.offset >= msgstr(docPos.item).last().length() ) {
					endReached=true;
				}
				break;
			case Comment:
				if(docPos.offset >= comment(docPos.item).length() ) {
					endReached=true;
				}
				break;
			case UndefPart:
				break;
		}
	}

	while(!success) {
        int accelMarkerPos = -1;
        int contextInfoLength = 0;
        int contextInfoPos = -1;
		QString targetStr;

		kapp->processEvents(10);

		if( d->_stop || endReached)
		{
		    kdDebug(KBABEL) << "FindNext: endReached or stopped" << endl;
		    disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
		    kdDebug(KBABEL) << "findNext not active" << endl;
		    d->_active=false;
		    d->_stop=false;
		    return false;
		}

		switch(docPos.part) {
			case Msgid:
				// FIXME: should care about plural forms in msgid
				targetStr = msgid(docPos.item).first();
				break;
			case Msgstr:
				targetStr = *(msgstr(docPos.item).at(docPos.form));
				break;
			case Comment:
				targetStr = comment(docPos.item);
				break;
			case UndefPart:
				break;
		}

        if(findOpts->ignoreContextInfo)
        {
            contextInfoPos = miscOptions.contextInfo.search(targetStr);
            contextInfoLength = miscOptions.contextInfo.matchedLength();
            if(contextInfoPos >= 0)
            {
                targetStr.remove(contextInfoPos,contextInfoLength);

                if(docPos.offset > (uint)contextInfoPos)
                    docPos.offset -= contextInfoLength;
            }
        }

        if(findOpts->ignoreAccelMarker
                && targetStr.contains(miscOptions.accelMarker))
        {
            accelMarkerPos = targetStr.find(miscOptions.accelMarker);
            targetStr.remove(accelMarkerPos,1);

            if(docPos.offset > (uint)accelMarkerPos)
                docPos.offset--;
        }

		if( findOpts->isRegExp ) {
			if ((pos=regexp.search(targetStr,docPos.offset)) >= 0 ) {
			    len = regexp.matchedLength();
				if(findOpts->wholeWords) {
					QString pre=targetStr.mid(pos-1,1);
					QString post=targetStr.mid(pos+len,1);
					if(!pre.contains(QRegExp("[a-zA-Z0-9]")) && !post.contains(QRegExp("[a-zA-Z0-9]")) ){
						success=true;
						docPos.offset=pos;
					}
				}
				else {
					success=true;
					docPos.offset=pos;
				}
			}
		}
		else {
			if( (pos=targetStr.find(searchStr,docPos.offset,findOpts->caseSensitive)) >= 0 ) {
				len=searchStr.length();

				if(findOpts->wholeWords) {
					QString pre=targetStr.mid(pos-1,1);
					QString post=targetStr.mid(pos+len,1);
					if(!pre.contains(QRegExp("[a-zA-Z0-9]")) && !post.contains(QRegExp("[a-zA-Z0-9]")) ){
						success=true;
						docPos.offset=pos;
					}
				}
				else {
					success=true;
					docPos.offset=pos;
				}
			}
		}


		if(!success) {
			docPos.offset=0;
			switch(docPos.part) {
				case Msgid:
				{
					if(findOpts->inMsgstr) {
						docPos.part = Msgstr;
						docPos.form = 0;
					}
					else if(findOpts->inComment) {
						docPos.part = Comment;
					}
					else
					{
						if(docPos.item >= numberOfEntries()-1)
						{
							endReached=true;
						}
						else
						{
							docPos.item++;
							docPos.form = 0;
						}
					}
					break;
				}
				case Msgstr:
					if( (int)docPos.form < numberOfPluralForms(docPos.item)-1 && pluralForm(docPos.item)==Gettext) {
					    docPos.form++;
					}
					else
					if(findOpts->inComment) {
						docPos.part = Comment;
					}
					else if(findOpts->inMsgid) {
						if(docPos.item >= numberOfEntries()-1)
						{
							endReached=true;
						}
						else
						{
							docPos.part = Msgid;
							docPos.item++;
						}
					}
					else {
						if(docPos.item >= numberOfEntries()-1)
						{
							endReached=true;
						}
						else
						{
							docPos.item++;
							docPos.form = 0;
						}
					}
					break;
				case Comment:
					if(findOpts->inMsgid) {
						if(docPos.item >= numberOfEntries()-1)
						{
							endReached=true;
						}
						else
						{
							docPos.part = Msgid;
							docPos.item++;
							docPos.form = 0;
						}
					}
					else if(findOpts->inMsgstr){
						if(docPos.item >= numberOfEntries()-1)
						{
							endReached=true;
						}
						else
						{
							docPos.part = Msgstr;
							docPos.form = 0;
							docPos.item++;
						}
					}
					else {
						if(docPos.item >= numberOfEntries()-1)
						{
							endReached=true;
						}
						else
						{
							docPos.item++;
							docPos.form = 0;
						}
					}
					break;
				case UndefPart:
					break;
			}
		}
        else
        {
            if(accelMarkerPos >= 0)
            {
                if(docPos.offset >= (uint)accelMarkerPos)
                {
                    docPos.offset++;
                }
                else if(docPos.offset+len > (uint)accelMarkerPos)
                {
                    len++;
                }
            }

            if(contextInfoPos >= 0)
            {
                if(docPos.offset >= (uint)contextInfoPos)
                {
                    docPos.offset+=contextInfoLength;
                }
                else if(docPos.offset+len > (uint)contextInfoPos)
                {
                    len+=contextInfoLength;
                }

            }
        }
	}

	disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
	kdDebug(KBABEL) << "findNext not active" << endl;
	d->_active=false;
	d->_stop=false;

	return true;
}

bool Catalog::findPrev(const FindOptions* findOpts, DocPosition& docPos, int& len)
{
	bool success = false;  // true, when found
	bool beginReached = false;

	kdDebug(KBABEL) << "findPrev active" << endl;
	d->_active=true;
	d->_stop=false;
	connect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
	
	MiscSettings miscOptions = miscSettings();

	len=0;
	int pos=0;

	QString searchStr = findOpts->findStr;
	QRegExp regexp(searchStr);

	if( findOpts->isRegExp ) {
		regexp.setCaseSensitive(findOpts->caseSensitive);
	}
	while(!success) {
        int accelMarkerPos = -1;
        int contextInfoLength = 0;
        int contextInfoPos = -1;
		QString targetStr;

		kapp->processEvents(10);

		if( d->_stop || beginReached)
		{
		    kdDebug(KBABEL) << "FindNext: endReached or stopped" << endl;
		    disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
		    kdDebug(KBABEL) << "findPrev active" << endl;
		    d->_active=false;
		    d->_stop=false;
		    return false;
		}

		switch(docPos.part) {
			case Msgid:
				// FIXME: should care about plural forms in msgid
				targetStr = msgid(docPos.item).first();
				break;
			case Msgstr:
				targetStr = *(msgstr(docPos.item).at(docPos.form));
				break;
			case Comment:
				targetStr = comment(docPos.item);
				break;
			case UndefPart:
				break;
		}

        if(findOpts->ignoreContextInfo)
        {
            contextInfoPos = miscOptions.contextInfo.search(targetStr);
	    contextInfoLength = miscOptions.contextInfo.matchedLength();
            if(contextInfoPos >= 0)
            {
                targetStr.remove(contextInfoPos,contextInfoLength);

                if(docPos.offset > (uint)contextInfoPos)
                    docPos.offset -= contextInfoLength;
            }
        }

        if(findOpts->ignoreAccelMarker
                && targetStr.contains(miscOptions.accelMarker))
        {
            accelMarkerPos = targetStr.find(miscOptions.accelMarker);
            targetStr.remove(accelMarkerPos,1);

            if(docPos.offset > (uint)accelMarkerPos)
                docPos.offset--;
        }

		if(docPos.offset <= 0) {
			success=false;
		}
		else if( findOpts->isRegExp ) {
			/*
			don't work!?
			if((pos=targetStr.findRev(regexp,docPos.offset)) >= 0 ) {
				regexp.match(targetStr,pos,&len); // to get the length of the string
			*/
			bool found=false;
			int tmpPos=docPos.offset;
			while(!found && tmpPos>=0)
			{
				if( (pos=regexp.search(targetStr,tmpPos)) >= 0 && (uint)pos < docPos.offset)
					found=true;
				else
					tmpPos--;
				len = regexp.matchedLength();
			}
			if(found) {
				if(findOpts->wholeWords) {
					QString pre=targetStr.mid(pos-1,1);
					QString post=targetStr.mid(pos+len,1);
					if(!pre.contains(QRegExp("[a-zA-Z0-9]")) && !post.contains(QRegExp("[a-zA-Z0-9]")) ){
						success=true;
						docPos.offset=pos;
					}
				}
				else {
					success=true;
					docPos.offset=pos;
				}
			}
		}
		else if( (pos=targetStr.findRev(searchStr,docPos.offset-1,findOpts->caseSensitive)) >= 0
		          && (uint)pos < docPos.offset) {
			len=searchStr.length();
			if(findOpts->wholeWords) {
				QString pre=targetStr.mid(pos-1,1);
				QString post=targetStr.mid(pos+len,1);
				if(!pre.contains(QRegExp("[a-zA-Z0-9]")) && !post.contains(QRegExp("[a-zA-Z0-9]")) ){
					success=true;
					docPos.offset=pos;
				}
			}
			else {
				success=true;
				docPos.offset=pos;
			}
		}

		if(!success) {
			switch(docPos.part) {
				case Comment:
				{
					if(findOpts->inMsgstr) {
						docPos.part = Msgstr;
						docPos.form = msgstr(docPos.item).count()-1;
						docPos.offset = msgstr(docPos.item).last().length();
					}
					else if(findOpts->inMsgid) {
						docPos.part = Msgid;
						// FIXME: should care about plural forms in msgid
						docPos.offset = msgid(docPos.item).first().length();
					}
					else
					{
						if(docPos.item <= 0)
						{
							beginReached=true;
						}
						else
						{
							docPos.item--;
							docPos.offset = comment(docPos.item).length();
						}
					}
					break;
				}
				case Msgstr:
					if(docPos.form != 0 ) {
					    docPos.form--;
					    docPos.offset = (*msgstr(docPos.item).at(docPos.form)).length();
					}
					else if(findOpts->inMsgid) {
						docPos.part = Msgid;
						// FIXME: should care about plural forms in msgid
						docPos.offset = msgid(docPos.item).first().length();
					}
					else if(findOpts->inComment) {
						if(docPos.item <= 0)
						{
							beginReached=true;
						}
						else
						{
							docPos.part = Comment;
							docPos.item--;
							docPos.offset = comment(docPos.item).length();
						}
					}
					else {
						if(docPos.item <= 0)
						{
							beginReached=true;
						}
						else
						{
							docPos.item--;
							docPos.offset = msgstr(docPos.item).last().length();
							docPos.form = msgstr(docPos.item).count()-1;
						}
					}
					break;
				case Msgid:
					if(findOpts->inComment) {
						if(docPos.item <= 0 )
						{
							beginReached=true;
						}
						else
						{
							docPos.part = Comment;
							docPos.item--;
							docPos.offset = comment(docPos.item).length();
						}
					}
					else if(findOpts->inMsgstr){
						if(docPos.item <= 0)
						{
							beginReached=true;
						}
						else
						{
							docPos.part = Msgstr;
							docPos.item--;
							docPos.offset = msgstr(docPos.item).last().length();
							docPos.form = msgstr(docPos.item).count()-1;
						}
					}
					else {
						if(docPos.item <= 0)
						{
							beginReached=true;
						}
						else
						{
							docPos.item--;
							// FIXME: should care about plural forms in msgid
							docPos.offset = msgid(docPos.item).first().length();
						}
					}
					break;
				case UndefPart:
					break;
			}
		}
        else
        {
            if(accelMarkerPos >= 0)
            {
                if(docPos.offset >= (uint)accelMarkerPos)
                {
                    docPos.offset++;
                }
                else if(docPos.offset+len > (uint)accelMarkerPos)
                {
                    len++;
                }
            }

            if(contextInfoPos >= 0)
            {
                if(docPos.offset >= (uint)contextInfoPos)
                {
                    docPos.offset+=contextInfoLength;
                }
                else if(docPos.offset+len > (uint)contextInfoPos)
                {
                    len+=contextInfoLength;
                }

            }
        }
	}

	disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
	kdDebug(KBABEL) << "findPrev active" << endl;
	d->_active=false;
	d->_stop=false;

	return true;
}


Catalog::DiffResult Catalog::diff(uint entry, QString *result)
{
    if(!result)
    {
        kdWarning() << "0 pointer for result" << endl;
        return DiffNotFound;
    }

    if( d->msgidDiffList.isEmpty() )
    {
        return DiffNeedList;
    }

    // first look if the diff for this entry is in the cache
    QString *s = d->diffCache[entry];
    if(s)
    {
        if(s->isEmpty())
            return DiffNotFound;


        *result = *s;
        return DiffOk;
    }

    // then look if the same msgid is contained in the diff file
    // FIXME: should care about plural forms in msgid
    QString id = msgid(entry).first();
    id.replace( "\n","");
    if(d->msgidDiffList.contains(id))
    {
	// FIXME:: should care about plural forms in msgid
        *result = msgid(entry).first();

        return DiffOk;
    }

    connect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
    kdDebug(KBABEL) << "diffv active" << endl;
    d->_active=true;
    d->_stop=false;

    QString idForDiff;


    // then look if there are entries with the same translation
    kdWarning() << "Diff feature (2) does not work with plural forms" << endl;
    QString str = msgstr(entry).first();
    str.replace("\n","");
    if(d->msgstr2MsgidDiffList.contains(str))
    {
        QStringList list = d->msgstr2MsgidDiffList[str];

        if(list.count() == 1)
        {
            idForDiff = list.first();
        }
        else
        {
            // find the best matching id
            double bestWeight = 0.6;
            QString bestId;

            QStringList::ConstIterator it;
            for(it = list.begin(); it != list.end(); ++it)
            {
                double weight = LevenshteinDistance()(id, (*it));
                if(weight > bestWeight)
                {
                    bestWeight = weight;
                    bestId = (*it);
                }
            }

            if( !bestId.isEmpty() )
            {
                idForDiff = bestId;
            }
        }
    }
    else
    {
        emit signalResetProgressBar(i18n("searching matching message")
                ,100);

        // find the best matching id
        double bestWeight = 0.6;
        QString bestId;

        int counter=0;
        int oldPercent=0;
        int max = QMAX( d->msgidDiffList.count()-1, 1);

        QStringList::ConstIterator it;
        for(it = d->msgidDiffList.begin();
                it != d->msgidDiffList.end(); ++it)
        {
            counter++;
            int  percent = 100*counter/max;
            if(percent > oldPercent)
            {
                oldPercent = percent;
                emit signalProgress(percent);
            }

            double weight = LevenshteinDistance()( id, (*it) );
            if(weight > bestWeight)
            {
                bestWeight = weight;
                bestId = (*it);
            }

            kapp->processEvents(10);

	    if( d->_stop )
	    {
		disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
		kdDebug

		(KBABEL) << "diffv not active" << endl;
		d->_active=false;
		d->_stop=false;
		return DiffNotFound;
	    }
        }

        if( !bestId.isEmpty() )
        {
            idForDiff = bestId;
        }

        emit signalClearProgressBar();

    }

    if( idForDiff.isEmpty() )
    {
        s = new QString(*result);
        if( !d->diffCache.insert(entry,s) )
            delete s;

	kdDebug	(KBABEL) << "diffv not active" << endl;
	d->_active=false;
	d->_stop=false;
        return DiffNotFound;
    }

    QString r = wordDiff(idForDiff,id);

    //esp for plural forms
    *result = r.replace("\\n<KBABELADD>" + QString(QChar(0x00B6)) + "</KBABELADD>", "\\n\n");

    s = new QString(*result);
    if( !d->diffCache.insert(entry,s) )
        delete s;

    disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
    kdDebug(KBABEL) << "diffv not active" << endl;
    d->_active=false;
    d->_stop=false;

    return DiffOk;
}

void Catalog::setDiffList( const QValueList<DiffEntry>& list)
{
    connect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
    kdDebug(KBABEL) << "setDiffList active" << endl;
    d->_active=true;
    d->_stop=false;

    emit signalResetProgressBar(i18n("preparing messages for diff"),100);

    d->msgidDiffList.clear();
    d->msgstr2MsgidDiffList.clear();
    d->diffCache.clear();

    uint max = QMAX(list.count()-1,1);
    int oldPercent=0;
    uint counter=0;
    QValueList<DiffEntry>::ConstIterator it;
    for(it = list.begin(); it != list.end(); ++it)
    {
        int percent = (100*counter)/max;
        counter++;
        if(percent > oldPercent)
        {
            oldPercent = percent;
            emit signalProgress(percent);
            kapp->processEvents(10);
        }

        QString id = (*it).msgid;
        id.replace("\n","");
        QString str = (*it).msgstr;
        str.replace("\n","");
        d->msgidDiffList.append(id);

        if(!str.isEmpty())
        {
            if(d->msgstr2MsgidDiffList.contains(str))
            {
                QStringList sl = d->msgstr2MsgidDiffList[str];
                sl.append(id);
            }
            else
            {
                QStringList sl;
                sl.append(id);
                d->msgstr2MsgidDiffList.insert(str,sl);
            }
        }
    }

    emit signalClearProgressBar();

    disconnect( this, SIGNAL( signalStopActivity() ), this, SLOT( stopInternal() ));
    kdDebug(KBABEL) << "setDiffList not active" << endl;
    d->_active=false;
    d->_stop=false;
}

QValueList<DiffEntry> Catalog::asDiffList()
{
    QValueList<DiffEntry> list;

    for ( QValueVector<CatalogItem>::Iterator it = d->_entries.begin();
					it != d->_entries.end(); ++it)
	{
        DiffEntry e;
        e.msgid = (*it).msgid().first();
	kdWarning() << "Diff feature does not support plural forms" << endl;
        e.msgstr = (*it).msgstr().first();

        list.append(e);
    }

    return list;

}

int Catalog::defaultNumberOfPluralForms() const
{
    return d->numberOfPluralForms;
}

void Catalog::getNumberOfPluralForms()
{
    IdentitySettings options = identitySettings();
    
    if(options.numberOfPluralForms > 0)
    {
        d->numberOfPluralForms = options.numberOfPluralForms;
        return;
    }

    QString lang=options.languageCode;
    if(lang.isEmpty())
    {
        d->numberOfPluralForms=-1;
        return;
    }

    d->numberOfPluralForms = getNumberOfPluralForms(lang);
}

int Catalog::getNumberOfPluralForms(const QString& lang)
{
    int nr=-1;

    KLocale locale("kdelibs");
    locale.setLanguage(lang);

    const char* formsString =
    "_: Dear translator, please do not translate this string in any form, but "
    "pick the _right_ value out of NoPlural/TwoForms/French... If not sure what "
    "to do mail thd@kde.org and coolo@kde.org, they will tell you. Better leave "
    "that out if unsure, the programs will crash!!\n"
    "Definition of PluralForm - to be set by the translator of kdelibs.po";

    QString formsTranslation = locale.translate(formsString);

    // no translation found
    if(formsTranslation == formsString || formsTranslation.isEmpty())
    {
        kdDebug(KBABEL) << "no translation of PluralForms found" << endl;
        return -1;
    }
    if ( formsTranslation == "NoPlural" )
      nr = 1;
    else if ( formsTranslation == "TwoForms" )
      nr = 2;
    else if ( formsTranslation == "French" )
      nr = 2;
    else if ( formsTranslation == "Gaeilge" || formsTranslation == "OneTwoRest" )
      nr = 3;
    else if ( formsTranslation == "Russian" )
      nr = 3;
    else if ( formsTranslation == "Polish" )
      nr = 3;
    else if ( formsTranslation == "Slovenian" )
      nr = 4;
    else if ( formsTranslation == "Lithuanian" )
      nr = 3;
    else if ( formsTranslation == "Czech" )
      nr = 3;
    else if ( formsTranslation == "Slovak" )
      nr = 3;
    else if ( formsTranslation == "Maltese" )
      nr = 4;
    else if ( formsTranslation == "Arabic" )
      nr = 4;
    else if ( formsTranslation == "Balcan" )
      nr = 3;
    else
    {
        kdDebug(KBABEL) << "unknown translation of PluralForms: "
            << formsTranslation << endl;
        nr=-1;
    }

    return nr;
}

int Catalog::numberOfPluralForms( uint index ) const
{
    if( index > numberOfEntries() ) return -1;

    if (  d->_entries.isEmpty() )
        return -1;
    if( d->_entries[index].pluralForm() == NoPluralForm  ) return 1;

    if( d->numberOfPluralForms > 0 )  return d->numberOfPluralForms;

    return 2; //default
}

bool Catalog::isModified() const
{
    return d->_modified;
}

void Catalog::setEntries(QValueVector<CatalogItem> entries)
{
    d->_entries=entries;
    
    // update the project for entries
    for ( QValueVector<CatalogItem>::Iterator it = d->_entries.begin();
					it != d->_entries.end(); ++it)
    {
	it->setProject( d->_project );
    }
}

void Catalog::setObsoleteEntries(QValueList<CatalogItem> entries)
{
    d->_obsoleteEntries=entries;
}

QValueList<CatalogItem> Catalog::obsoleteEntries() const
{
    return d->_obsoleteEntries;
}

void Catalog::setCatalogExtraData(const QStringList& data)
{
    d->_catalogExtra = data;
}

QStringList Catalog::catalogExtraData() const
{
    return d->_catalogExtra;
}

QString Catalog::importPluginID() const
{
    return d->_importID;
}

QTextCodec* Catalog::fileCodec() const
{
    return d->fileCodec;
}

void Catalog::setGeneratedFromDocbook(const bool generated)
{
    d->_generatedFromDocbook = generated;
}

void Catalog::setFileCodec( QTextCodec* codec )
{
    d->fileCodec = codec;
}

void Catalog::setErrorIndex( const QValueList<uint>& list )
{
    d->_errorIndex = list;
}

void Catalog::setImportPluginID( const QString& id )
{
    d->_importID = id;
}

void Catalog::stop()
{
    if( d->_active )
	emit signalStopActivity();
}

void Catalog::stopInternal()
{
    d->_stop = true;
}

bool Catalog::isActive()
{
    return  d->_active;
}

void Catalog::setMimeTypes( const QString& mimeTypes )
{
    d->_mimeTypes = mimeTypes;
}

QString Catalog::mimeTypes() const
{
    return d->_mimeTypes;
}

void Catalog::wordCount (uint &total, uint &fuzzy, uint &untranslated) const
{
    total = 0;
    fuzzy = 0;
    untranslated = 0;
    
    QRegExp separator( "[ \n\t]+" );
    
    for ( QValueVector<CatalogItem>::Iterator it = d->_entries.begin();
					it != d->_entries.end(); ++it)
    {
	// find out the number of words for this message
	
	// join all forms together
	QString message = (*it).msgid ().join (" ");
	
	// remove tags first
	d->_tagExtractor->setString( message );
	message = d->_tagExtractor->plainString(false);

	QStringList words = QStringList::split ( separator, message );
	
	total += words.count();

	if ( (*it).isFuzzy() )
	    fuzzy += words.count();
	else if ( (*it).isUntranslated() )
	    untranslated += words.count();
    }    
}

#include "catalog.moc"

// kate: space-indent on; indent-width 4; replace-tabs on;
