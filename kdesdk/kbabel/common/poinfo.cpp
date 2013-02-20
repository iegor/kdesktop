/*  
  This file is part of KBabel
  Copyright (C) 2002 Stefan Asserhäll <stefan.asserhall@telia.com>
		2003-2005 Stanislav Visnovsky <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
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

*/

#include "poinfo.h"

#include "catalogitem.h"
#include "findoptions.h"
#include "msgfmt.h"
#include "resources.h"

#include <kapplication.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <ksavefile.h>

#include <qdatastream.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qtextcodec.h>

#include "libgettext/pofiles.h"
#include "libgettext/tokens.h"

#include <fstream>

using namespace KBabel;

// A PO-file cache item
struct poInfoCacheItem
{
    PoInfo info;
    QDateTime lastModified;
};

inline QDataStream& operator << ( QDataStream& stream, poInfoCacheItem* item )
{
    // Note: if you change anything here, do not forget to increase the #define POINFOCACHE_VERSION
    stream << item->info.total;
    stream << item->info.fuzzy;
    stream << item->info.untranslated;
    stream << item->info.project;
    stream << item->info.creation;
    stream << item->info.revision;
    stream << item->info.lastTranslator;
    stream << item->info.languageTeam;
    stream << item->info.mimeVersion;
    stream << item->info.contentType;
    stream << item->info.encoding;
    stream << item->info.others;
    stream << item->info.headerComment;
    stream << item->lastModified;
    return stream;
}

inline QDataStream& operator >> ( QDataStream& stream, poInfoCacheItem* item )
{
    stream >> item->info.total;
    stream >> item->info.fuzzy;
    stream >> item->info.untranslated;
    stream >> item->info.project;
    stream >> item->info.creation;
    stream >> item->info.revision;
    stream >> item->info.lastTranslator;
    stream >> item->info.languageTeam;
    stream >> item->info.mimeVersion;
    stream >> item->info.contentType;
    stream >> item->info.encoding;
    stream >> item->info.others;
    stream >> item->info.headerComment;
    stream >> item->lastModified;
    return stream;
}

// Cache of PO-file items
static QDict<poInfoCacheItem> _poInfoCache;

// File name of cache
static QString _poInfoCacheName;

// flag to stop current reading
bool PoInfo::stopStaticRead;

bool PoInfo::_gettextPluralForm;

// Note: We only read the cache file if the data seems usable. If not, we will re-generate the data.
void PoInfo::cacheRead()
{
    QFile cacheFile( _poInfoCacheName );
    if( cacheFile.open( IO_ReadOnly ) ) {
	QDataStream s( &cacheFile );

        // Check the file cache version.
        // If it is not the current version, we do not read the cache file
        Q_UINT32 version;
	s >> version;
        if( version != POINFOCACHE_VERSION ) {
            // Wrong POINFOCACHE_VERSION, so abort
            kdDebug(KBABEL) << "Wrong cache file version: " << version << endl;
            return;
        }

        /*
         * Check the version of the QDataStream with which the cache file was written
         *
         * If the cache file was written by an incompatible future version of Qt,
         * the cache file will not be read.
         *
         * On the other side, a cache file written by a previous version of Qt can be read,
         * by setting the version of the QDataStream used.
         */
        Q_INT32 qdatastreamVersion;
        s >> qdatastreamVersion;
        if( qdatastreamVersion > 0 &&  qdatastreamVersion <= s.version() ) {
            s.setVersion( qdatastreamVersion );
        }
        else {
            // QDataStream version seems stupid, so abort
            kdDebug(KBABEL) << "Wrong QDataStream version: " << qdatastreamVersion << endl;
            return;
        }

        QString url;
        while( !s.atEnd() ) {
            poInfoCacheItem* item = new poInfoCacheItem;
            s >> url;
            s >> item;
            _poInfoCache.insert( url, item );
        }
        cacheFile.close();
    }
}

void PoInfo::cacheWrite()
{
    // We use KSaveFile as otherwise we have no management about the cache file's integrity
    // (especially if two instances would write into the same cache file)
    KSaveFile cacheFile( _poInfoCacheName );
    
    QDataStream* stream = cacheFile.dataStream();
    
    if( stream ) {

        // Write the cache file version
        // We choose to fix a format (Q_UINT32) for compatibility (Qt version, platforms, architectures)
	const Q_UINT32 version = POINFOCACHE_VERSION;
	*stream << version;

        // Write the version of the QDataStream
        // Here too we choose a fixed format (Q_INT32) for compatibility
        const Q_INT32 qdatastreamVersion = stream->version();
        *stream << qdatastreamVersion;
        
        QDictIterator<poInfoCacheItem> it( _poInfoCache ); // iterator for dict
        for ( ; it.current(); ++it ) {
	    if( QFile::exists( it.currentKey() ) ) {
		*stream << it.currentKey();
                *stream << it.current();
	    }
	}
        if ( !cacheFile.close() ) {
            kdWarning(KBABEL) << "Could not write cache file: " << _poInfoCacheName << endl;
        }
    }
    else {
        kdWarning(KBABEL) << "Could not create QDataStream for cache file: " << _poInfoCacheName << endl;
        cacheFile.abort();
    }
}

bool PoInfo::cacheFind(const QString url, PoInfo& info)
{
    // Read cache if it has not been read, and set up post routine to write it
    static bool _cacheIsRead = false;
    if( !_cacheIsRead ) {
	_cacheIsRead = true;
        _poInfoCacheName = locateLocal("cache", "kbabel/poinfocache");
	cacheRead();
    }

    poInfoCacheItem *item = _poInfoCache.find( url );
    if( item ) {
	QFileInfo fi( url );

	if( fi.lastModified() == item->lastModified ) {
	    info = item->info;
	    return true;
	}
    }
    return false;
}

void PoInfo::cacheSave(const QString url, PoInfo& info)
{
    poInfoCacheItem *item = new poInfoCacheItem;
    QFileInfo fi( url );

    item->info = info;
    item->lastModified = fi.lastModified();
    _poInfoCache.insert( url, item );
}

QTextCodec* PoInfo::codecForFile(QString gettextHeader)
{
   QRegExp regexp("Content-Type:\\s*\\w+/[-\\w]+;?\\s*charset\\s*=\\s*(\\S+)\\s*\\\\n");
   if( regexp.search(gettextHeader) == -1 )
   {
       kdDebug(KBABEL) << "no charset entry found" << endl;
       return 0;
   }
   
   const QString charset = regexp.cap(1);
   kdDebug(KBABEL) << "charset: " << charset << endl;

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
         kdWarning(KBABEL) << "charset found, but no codec available, using UTF8 instead" << endl;
	 codec=QTextCodec::codecForName("utf8");
      }
   }
   else
   {
      // No charset? So it is probably ASCII, therefore UTF-8
       kdWarning(KBABEL) << "No charset defined! Assuming UTF-8!" << endl;
       codec=QTextCodec::codecForName("utf8");
   }

   return codec;
}

PoInfo PoInfo::headerInfo(const CatalogItem& headerItem)
{
   // A header of a Gettext .po/.pot file is made of entries of the kind:
   // key:value\n
   // Note that the "line" defined by the \n can be different than the line of the file.

   // We join all lines of the header and then split the result again at the \n sequence
   const QStringList header=QStringList::split("\\n",headerItem.msgstrAsList().join(QString()));

   PoInfo info;

   // extract information from the header
   QStringList::const_iterator it;

   // The header of a Gettext .po file is consisted of lines of key and value
   for(it=header.begin();it!=header.end();++it)
   {
      bool knownKey=false;
      // We search for the : character, which is the separator between key and value
      const int res=(*it).find(':');
      if (res>=0)
      {
         knownKey=true; // We know most keys, if not it will be changed to false in the "else" case
         const QString key=(*it).left(res).simplifyWhiteSpace();
         QString value=(*it).mid(res+1);
         // "Chop" the \n at the end
         if (value.endsWith("\\n"))
            value.remove(value.length()-2,2); // ### Qt4: use  value.chop(2)
         value=value.simplifyWhiteSpace();
         kdDebug(KBABEL) << "Header key: " << key << " value: " << value << endl;
         if (key=="Project-Id-Version")
            info.project=value;
         else if (key=="POT-Creation-Date")
            info.creation=value;
         else if (key=="PO-Revision-Date")
            info.revision=value;
         else if (key=="Last-Translator")
            info.lastTranslator=value;
         else if (key=="Language-Team")
            info.languageTeam=value;
         else if (key=="MIME-Version")
            info.mimeVersion=value;
         else if (key=="Content-Type")
            info.contentType=value;
         else if (key=="Content-Transfer-Encoding")
            info.encoding=value;
         else
         {
            kdDebug(KBABEL)<<"Unknown key: "<<key<<endl;
            knownKey=false;
         }
      }
      if (!knownKey)
      {
         QString line=(*it);

         if(line.right(2)=="\\n")
            line.remove(line.length()-2,2); // ### Qt4: use  value.chop(2)

         if(!info.others.isEmpty())
            info.others+='\n';

         info.others+=line.simplifyWhiteSpace();
      }
   }

   info.headerComment=headerItem.comment();

   return info;
}


ConversionStatus PoInfo::info(const QString& url, PoInfo& info, QStringList &wordList, bool updateWordList, bool interactive)
{
    return PoInfo::info( url, info, wordList, updateWordList, interactive, true);
}

ConversionStatus PoInfo::info(const QString& url, PoInfo& info, QStringList &wordList, bool updateWordList, bool interactive, bool msgfmt)
{
   stopStaticRead = false;

   if( !updateWordList && PoInfo::cacheFind( url, info ) )
       return OK;

   QString target;
   if(KIO::NetAccess::download(KURL( url ), target, 0))
   {
       QFile file(target);

       if ( msgfmt )
       {
            // First check file with msgfmt to be sure, it is syntactically correct
            Msgfmt msgfmt;
            QString output;
            Msgfmt::Status stat = msgfmt.checkSyntax( target , output );
            if(stat == Msgfmt::SyntaxError)
            {
                KIO::NetAccess::removeTempFile(target);
                return PARSE_ERROR;
            }
       }


       std::ifstream* stream = new std::ifstream( file.name().local8Bit());
       if( stream->is_open() )
       {
           CatalogItem temp;

           info.total=0;
           info.fuzzy=0;
           info.untranslated=0;

	   GettextFlexLexer* lexer = new GettextFlexLexer( stream  );

	   lexer->yylex();
	   
           // now parse the rest of the file
           ConversionStatus success=OK;
	   
           while( lexer->lastToken != T_EOF && success==OK)
           {
               if( interactive ) kapp->processEvents(10);
	       
	       if( stopStaticRead )
	       {
                    KIO::NetAccess::removeTempFile(target);
                    delete lexer;
		    delete stream;
		    return OK;
		}
	       
	       success=fastRead(temp,lexer,false);
	       
               if(success==OK || success==RECOVERED_PARSE_ERROR)
               {
		  success=OK;
		  
		  if( temp.comment().contains("\n#~") ) continue; // skip obsolete
		  
                  if( temp.msgid().first().isEmpty()) //header
		  {
		      if( temp.isFuzzy() )  temp.removeFuzzy();
		      
		      //find out the codec
		      QTextCodec* codec = codecForFile( temp.msgstr().first() );
		      if( !codec ) return PARSE_ERROR;
		      
		      // convert from UTF-8 using codec
		      temp.setComment( codec->toUnicode(temp.comment().utf8()) );
		      temp.setMsgstr( codec->toUnicode(temp.msgstr().first().utf8()) );
		      
		      PoInfo infoCounts = info;
		      info=PoInfo::headerInfo(temp);
		      info.total = infoCounts.total;
		      info.fuzzy = infoCounts.fuzzy;
		      info.untranslated = infoCounts.untranslated;
		      continue; // do not update counters and word list for header
		  }
		  		  
                  info.total++;

                  if(temp.isFuzzy())
                     info.fuzzy++;
                  else if(temp.isUntranslated())
                     info.untranslated++;
		     
		  if( updateWordList )
		  {
		    // FIXME: should care about plural forms in msgid
		    QString st = temp.msgid().first().simplifyWhiteSpace().lower();
		    QStringList sl = QStringList::split( ' ', st );
		    while(!sl.isEmpty())
		    {
			QString w = sl.first();
			sl.pop_front();
			if( !wordList.contains(w) ) wordList.append( w );
		    }
		    st = temp.msgstr().join(" " ).simplifyWhiteSpace().lower();
		    sl = QStringList::split( ' ', st );
		    while(!sl.isEmpty())
		    {
			QString w = sl.first();
			sl.pop_front();
			if( !wordList.contains(w) ) wordList.append( w );
		    }
		    st = temp.comment().simplifyWhiteSpace().lower();
		    sl = QStringList::split( ' ', st );
		    while(!sl.isEmpty())
		    {
			QString w = sl.first();
			sl.pop_front();
			if( !wordList.contains(w) ) wordList.append( w );
		    }
                }
	      }
           }

	   delete lexer;
	   delete stream;

           if(success==PARSE_ERROR)
           {
	       KIO::NetAccess::removeTempFile(target);
               return PARSE_ERROR;
           }
       }
       else
       {
          delete stream;
          KIO::NetAccess::removeTempFile(target);
          return NO_PERMISSIONS;
       }

	KIO::NetAccess::removeTempFile(target);
	if( target == url )
	    PoInfo::cacheSave( url, info );
        return OK;
   }
   else
   {
      return OS_ERROR;
   }

   return OK;
}

bool PoInfo::findInFile( const QString& url, FindOptions options )
{
   enum {Begin, Comment, Msgid, Msgstr, Msgctxt} part = Begin;
   
   stopStaticRead = false;
   QString target;
   if(KIO::NetAccess::download(KURL( url ), target, 0))
   {
       std::ifstream* stream = new std::ifstream( target.local8Bit()); 
       if(stream->is_open())
       {
           KIO::NetAccess::removeTempFile(target);
	   
	   GettextFlexLexer* lexer = new GettextFlexLexer( stream );

	   lexer->yylex();

           // prepare the search
	   
	   QString searchStr = options.findStr;
	   QRegExp regexp( searchStr );
	   
	   if( options.isRegExp ) 
		regexp.setCaseSensitive( options.caseSensitive );

           // first read header
	   CatalogItem temp;
	   
	   ConversionStatus status = fastRead( temp, lexer, true );
	   if( status != OK || !temp.msgid().first().isEmpty() ) 
	   {
		delete lexer;
		delete stream;
		return false; // header is not at the beginning, broken file
	   }

	   QTextCodec* codec = codecForFile( temp.msgstr().first() );
	   if( !codec ) 
	   {
		return false;
	   }
	   
	   // now parse the rest of the file
	   QString text;
	   int pos,len;
	   
           while(lexer->lastToken != T_EOF)
           {
	       switch( lexer->lastToken ) {
	           case T_COMMENT: {
			part = Comment;
			if( !options.inComment ) break;
			text = codec->toUnicode(lexer->YYText()); 
			if( options.isRegExp )
			    pos=regexp.search(text, 0 );
			else 
			    pos=text.find(searchStr,0,options.caseSensitive);
			if( pos >= 0)
			{
			    if( options.wholeWords) {
				len = searchStr.length();
				QString pre = text.mid(pos-1,1);
				QString post = text.mid(pos+len,1);
				if( !pre.contains( QRegExp("[a-zA-Z0-9]")) &&
				    !post.contains( QRegExp("[a-zA-Z0-9]") )
				) {
				    delete lexer;
				    delete stream;
				    return true;
				}
			    }
			    else {
				delete lexer;
				delete stream;
				return true;
			    };
			}
			break;
		   }
		   case T_STRING: {
			if( part == Msgid && !options.inMsgid ) break;
			else if( part == Msgstr && !options.inMsgstr ) break;
                        // HACK: We ignore any string following a msgctxt, as it does not change a statistic
                        else if( part == Msgctxt ) break;
			
			text = codec->toUnicode(lexer->YYText()); 
			
			if( options.ignoreContextInfo )
			{
			    pos = options.contextInfo.search(text);
			    len = options.contextInfo.matchedLength();
			    if( pos >= 0 )
		    		text.remove( pos, len );
			}
		    
			if( options.ignoreAccelMarker )
			{
			    pos = text.find( options.accelMarker );
			    if( pos >= 0 )
				text.remove( pos, 1 );
			}
			
			if( options.isRegExp )
			    pos=regexp.search(text, 0 );
			else 
			    pos=text.find(searchStr,0,options.caseSensitive);

			if( pos >= 0)
			{
			    if( options.wholeWords) {
				len = searchStr.length();
				QString pre = text.mid(pos-1,1);
				QString post = text.mid(pos+len,1);
				if( !pre.contains( QRegExp("[a-zA-Z0-9]")) &&
				    !post.contains( QRegExp("[a-zA-Z0-9]") )
				) {
				    delete lexer;
				    delete stream;
				    return true;
				}
			    }
			    else {
				delete lexer;
				delete stream;
				return true;
			    };
			}
			break;
		   }
		   case T_MSGSTR: {
			part = Msgstr;
			break;
		   }
		   case T_MSGID: 
		   case T_MSGIDPLURAL: {
	    		kapp->processEvents(10);
			
			// if stopped, return not found
			if( stopStaticRead ) 
			{
			    delete lexer;
			    delete stream;
			    return false;
			}
			part = Msgid;
			break;
		   }
                   case T_MSGCTXT: {
                        part = Msgctxt;
                        break;
                   }
	       }
	       lexer->yylex();
           }
	   delete lexer;
	   delete stream;
       }
    }
    return false;
}

// this does not like any incorrect files
ConversionStatus PoInfo::fastRead( CatalogItem& item, GettextFlexLexer *lexer, bool storeText)
{
   item.clear();
   _gettextPluralForm = false;

    // comment
    if( lexer->lastToken == T_COMMENT )
    {
	QString _comment = QString::fromUtf8(lexer->YYText());
	while( lexer->yylex() == T_COMMENT )
	    _comment += "\n"+QString::fromUtf8(lexer->YYText());
	item.setComment( _comment );
//	kdDebug(KBABEL) << "Comment: " << _comment << endl;
    }

    //obsolete
    if( lexer->lastToken == T_OBSOLETE ) {
	lexer->yylex();
	item.setComment("#~\n#~");
	return OK;
    }

    // msgctxt
    if( lexer->lastToken == T_MSGCTXT ) {
        // HACK: we simply ignore the context, as it does not change a statistic
        do {
            lexer->yylex();
        } while ( lexer->lastToken == T_STRING );
    }
        
    // msgid
    if( lexer->lastToken != T_MSGID ) return PARSE_ERROR;
    
    if( lexer->yylex() != T_STRING ) return PARSE_ERROR;
    QStringList msgids = item.msgid();
    QStringList::Iterator it = msgids.begin();
    *it = QString::fromUtf8(lexer->YYText());
    if( storeText )
	while( lexer->yylex() == T_STRING )
    	    (*it) += ("\n"+ QString::fromUtf8(lexer->YYText()) );
    else {
	if( lexer->yylex() == T_STRING ) // this is not header
	{
	    *it = "SKIPPED";
	    while( lexer->yylex() == T_STRING );
	}
    }
    item.setMsgid( msgids );
    
//    kdDebug(KBABEL) << "Msgid: " << *it << endl;

    if( lexer->lastToken == T_MSGIDPLURAL ) 
    {
	_gettextPluralForm = true;
	if( lexer->yylex() != T_STRING ) return PARSE_ERROR;
	QStringList msgids = item.msgid();
	it = msgids.fromLast();
	*it = QString::fromUtf8(lexer->YYText());
	if( storeText ) 
	    while( lexer->yylex() == T_STRING )
    		(*it)+="\n"+ QString::fromUtf8(lexer->YYText());
	else while( lexer->yylex() == T_STRING );
	item.setMsgid( msgids );
//	kdDebug(KBABEL) << "Msgid_plural: " << *it << endl;
    }
    
    // msgstr
    if( lexer->lastToken != T_MSGSTR ) return PARSE_ERROR;

    if( !_gettextPluralForm )
    {
	if( lexer->yylex() != T_STRING ) return PARSE_ERROR;
	
	QStringList msgstrs = item.msgstr();
	it = msgstrs.begin();
	*it = QString::fromUtf8(lexer->YYText());
	if( storeText || item.msgid().first().isEmpty() ) // if we should store the text or it is a header
	    while( lexer->yylex() == T_STRING )
    		(*it)+= ("\n"+ QString::fromUtf8(lexer->YYText()));
	else 
	if( lexer->yylex() == T_STRING ) // check next token, whether it is really translated
	{
	    *it = "SKIPPED";
	    while( lexer->yylex() == T_STRING );
	}
	item.setMsgstr( msgstrs );
//	kdDebug(KBABEL) << "Msgstr: " << *it << endl;
    } 
    else 
    {
	QStringList msgstrs = item.msgstr();
	QString s = QString::fromUtf8(lexer->YYText());
	while( lexer->lastToken == T_MSGSTR && s.contains( QRegExp("^msgstr\\[[0-9]+\\]" ) ) )
	{
	    if( lexer->yylex() != T_STRING ) return PARSE_ERROR;
	    it = msgstrs.fromLast();
	    *it = QString::fromUtf8(lexer->YYText());
	    
	    if( storeText )
		do {
    		    (*it)+="\n"+QString::fromUtf8(lexer->YYText());
		} while( lexer->yylex() == T_STRING );
	    else while( lexer->yylex() == T_STRING );
//	    kdDebug(KBABEL) << "Msgstr: " << *it << endl;
	    s = QString::fromUtf8(lexer->YYText());
	} 
	item.setMsgstr( msgstrs );
    }
    
    return OK;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
