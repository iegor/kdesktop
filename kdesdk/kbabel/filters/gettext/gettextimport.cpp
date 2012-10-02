// kate: space-indent on; indent-width 3; replace-tabs on;

/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2001-2003 by Stanislav Visnovsky <visnovsky@kde.org>
  Copyright (C) 2006 by Nicolas GOUTTE <nicolasg@snafu.de>

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

#include "gettextimport.h"

#include <catalogitem.h>
#include <resources.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qtextcodec.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klocale.h>

K_EXPORT_COMPONENT_FACTORY( kbabel_gettextimport, KGenericFactory<GettextImportPlugin> ( "kbabelgettextimportfilter" ) )

using namespace KBabel;

GettextImportPlugin::GettextImportPlugin(QObject* parent, const char* name, const QStringList &) : CatalogImportPlugin(parent,name)
{
}

ConversionStatus GettextImportPlugin::load(const QString& filename, const QString&)
{
   kdDebug( KBABEL ) << k_funcinfo << endl;
   
   if ( filename.isEmpty() ) {
      kdDebug(KBABEL) << "fatal error: empty filename to open" << endl;
      return NO_FILE;
   }

   QFileInfo info(filename);

   if(!info.exists() || info.isDir())
      return NO_FILE;

   if(!info.isReadable())
      return NO_PERMISSIONS;

   QFile file(filename);

   if ( !file.open( IO_ReadOnly ) )
      return NO_PERMISSIONS;
   
   uint oldPercent = 0;
   emit signalResetProgressBar(i18n("loading file"),100);

   QByteArray ba = file.readAll();
   file.close();

   // find codec for file
   bool hadCodec;
   QTextCodec* codec=codecForArray( ba, &hadCodec );
   
   bool recoveredErrorInHeader = false;
   
   QTextStream stream(ba,IO_ReadOnly);

   if(codec)
      stream.setCodec(codec);
   else
   {
      kdWarning() << "No encoding declared or found, using UTF-8" << endl;
      stream.setEncoding( QTextStream::UnicodeUTF8 );
#ifdef __GNUC__
# warning Default UTF-8 encoding needs to be improved
#endif
      // Templates define CHARSET, so if e make it a recoverable error, the template is not loadable anymore, as for templates recoverable errors are disqualifying.
      //recoveredErrorInHeader = true;
   }
   
   QIODevice *dev = stream.device();
   int fileSize = dev->size();

   // if somethings goes wrong with the parsing, we don't have deleted the old contents
   CatalogItem tempHeader;
   QStringList tempObsolete;


   kdDebug(KBABEL) << "start parsing..." << endl;

   // first read header
   const ConversionStatus status = readHeader(stream);


   if ( status == RECOVERED_PARSE_ERROR )
   {
      kdDebug( KBABEL ) << "Recovered error in header entry" << endl;
      recoveredErrorInHeader = true;
   }
   else if ( status != OK )
   {
      emit signalClearProgressBar();

      kdDebug( KBABEL ) << "Parse error in header entry" << endl;
      return status;
   }
   
   kdDebug() << "HEADER MSGID: " << _msgid << endl;
   kdDebug() << "HEADER MSGSTR: " << _msgstr << endl;
   if ( !_msgid.isEmpty() && !_msgid.first().isEmpty() )
   {
      // The header must have an empty msgid
      kdWarning(KBABEL) << "Header entry has non-empty msgid. Creating a temporary header! " << _msgid << endl;
      tempHeader.setMsgid( QStringList() );
      QStringList tmp;
      tmp.append(
         "Content-Type: text/plain; charset=UTF-8\\n" // Unknown charset
         "Content-Transfer-Encoding: 8bit\\n"
         "Mime-Version: 1.0" );
      tempHeader.setMsgstr( tmp );
      // We keep the comment of the first entry, as it might really be a header comment (at least partially)
      const QString comment( "# Header entry was created by KBabel!\n#\n" + _comment );
      tempHeader.setComment( comment );
      recoveredErrorInHeader = true;
   }
   else
   {
      tempHeader.setMsgid( _msgid );
      tempHeader.setMsgstr( _msgstr );
      tempHeader.setComment( _comment );
   }
   if(tempHeader.isFuzzy())
   {
      tempHeader.removeFuzzy();
   }

   // check if header seems to indicate docbook content generated by xml2pot
   const bool docbookContent = (tempHeader.msgstr().find("application/x-xml2pot") != tempHeader.msgstr().end());

   // now parse the rest of the file
   uint counter=0;
   QValueList<uint> errorIndex;
   bool recoveredError=false;
   bool docbookFile=false;

   while( !stream.eof() )
   {
      kapp->processEvents(10);
      if( isStopped() )
      {
         return STOPPED;
      }

      const ConversionStatus success=readEntry(stream);

      if(success==OK)
      {
         if( _obsolete )
         {
               tempObsolete.append(_comment);
         }
         else
         {
               CatalogItem tempCatItem;
               tempCatItem.setMsgctxt( _msgctxt );
               tempCatItem.setMsgid( _msgid );
               tempCatItem.setMsgstr( _msgstr );
               tempCatItem.setComment( _comment );
               tempCatItem.setGettextPluralForm( _gettextPluralForm );
               
               // add new entry to the list of entries
               appendCatalogItem(tempCatItem);
               // check if first comment seems to indicate a docbook source file
               if(counter==0)
                  docbookFile = ( tempCatItem.comment().find(".docbook") != -1 );
         }
      }
      else if(success==RECOVERED_PARSE_ERROR)
      {
         kdDebug( KBABEL ) << "Recovered parse error in entry: " << counter << endl;
         recoveredError=true;
         errorIndex.append(counter);
         
         CatalogItem tempCatItem;
         tempCatItem.setMsgctxt( _msgctxt );
         tempCatItem.setMsgid( _msgid );
         tempCatItem.setMsgstr( _msgstr );
         tempCatItem.setComment( _comment );
         tempCatItem.setGettextPluralForm( _gettextPluralForm );

         // add new entry to the list of entries
         appendCatalogItem(tempCatItem);
      }
      else if ( success == PARSE_ERROR )
      {
         kdDebug( KBABEL ) << "Parse error in entry: " << counter << endl;
         return PARSE_ERROR;
      }
      else
      {
         kdWarning( KBABEL ) << "Unknown success status, assumig parse error " << success << endl;
         return PARSE_ERROR;
      }
      counter++;

      const uint newPercent = (100*dev->at())/fileSize;
      if(newPercent > oldPercent)
      {
         oldPercent = newPercent;
         emit signalProgress(oldPercent);
      }
   }


   // to be sure it is set to 100, if someone don't connect to
   // signalClearProgressBar()
   emit signalProgress(100);

   emit signalClearProgressBar();


   // ### TODO: can we check that there is no useful entry?
   if ( !counter )
   {
      // Empty file? (Otherwise, there would be a try of getting an entry and the count would be 1 !)
      kdDebug( KBABEL ) << k_funcinfo << " Empty file?" << endl;
      return PARSE_ERROR;
   }

   kdDebug(KBABEL) << k_funcinfo << " ready" << endl;

   // We have succesfully loaded the file (perhaps with recovered errors)
   
   setGeneratedFromDocbook(docbookContent || docbookFile);

   setHeader(tempHeader);
   setCatalogExtraData(tempObsolete);
   setErrorIndex(errorIndex);

   if(hadCodec)
      setFileCodec(codec);
   else
      setFileCodec(0);
   
   setMimeTypes( "application/x-gettext" );

   if ( recoveredErrorInHeader )
   {
      kdDebug( KBABEL ) << k_funcinfo << " Returning: header error" << endl;
      return RECOVERED_HEADER_ERROR;
   }
   else if ( recoveredError )
   {
      kdDebug( KBABEL ) << k_funcinfo << " Returning: recovered parse error" << endl;
      return RECOVERED_PARSE_ERROR;
   }
   else
   {
      kdDebug( KBABEL ) << k_funcinfo << " Returning: OK! :-)" << endl;
      return OK;
   }
}

QTextCodec* GettextImportPlugin::codecForArray(QByteArray& array, bool* hadCodec)
{
   if(hadCodec)
   {
      *hadCodec=false;
   }

   QTextStream stream( array, IO_ReadOnly );
   // ### TODO Qt4: see if it can be done with QByteArray alone, in an encoding-neutral way.
   // Set ISO-8859-1 as it is a relatively neutral encoding when reading (compared to UTF-8  or a random locale encoing)
   stream.setEncoding( QTextStream::Latin1 );

   // first read header
   ConversionStatus status = readHeader(stream);
   if(status!=OK && status != RECOVERED_PARSE_ERROR)
   {
       kdDebug(KBABEL) << "wasn't able to read header" << endl;
       return 0;
   }

   const QString head = _msgstr.first();

   QRegExp regexp("Content-Type:\\s*\\w+/[-\\w]+;?\\s*charset\\s*=\\s*(\\S+)\\s*\\\\n");
   if( regexp.search( head ) == -1 )
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
          if(hadCodec)
             *hadCodec=false;

          codec=QTextCodec::codecForName("utf8");
          kdDebug(KBABEL) 
              << QString("file seems to be a template: using utf-8 encoding.")
              << endl;
      }
      else
      {
         codec=QTextCodec::codecForName(charset.latin1());
         if(hadCodec)
            *hadCodec=true;
      }

      if(!codec)
      {
         kdWarning(KBABEL) << "charset found, but no codec available, using UTF-8 instead" << endl;
      }
   }
   else
   {
      // No charset? So it is probably ASCII, therefore UTF-8
      kdWarning(KBABEL) << "No charset defined! Assuming UTF-8!" << endl;
   }
         

   return codec;
}

ConversionStatus GettextImportPlugin::readHeader(QTextStream& stream)
{
   CatalogItem temp;
   int filePos=stream.device()->at();
   ConversionStatus status=readEntry(stream);

   if(status==OK || status==RECOVERED_PARSE_ERROR)
   {
      // test if this is the header
      if(!_msgid.first().isEmpty())
      {
         stream.device()->at(filePos);
      }

      return status;
   }

   return PARSE_ERROR;
}

ConversionStatus GettextImportPlugin::readEntry(QTextStream& stream)
{
   //kdDebug( KBABEL ) << k_funcinfo << " START" << endl;
   enum {Begin,Comment,Msgctxt,Msgid,Msgstr} part=Begin;

   QString line;
   bool error=false;
   bool recoverableError=false;
   bool seenMsgctxt=false;
   _msgstr.clear();
   _msgstr.append(QString());
   _msgid.clear();
   _msgid.append(QString());
   _msgctxt=QString();
   _comment=QString();
   _gettextPluralForm=false;
   _obsolete=false;
   
   QStringList::Iterator msgstrIt=_msgstr.begin();
   
   while( !stream.eof() )
   {
       const int pos=stream.device()->at();

       line=stream.readLine();

       //kdDebug() << "Parsing line: " << line << endl;

       // ### Qt4: no need of a such a check
       if(line.isNull()) // file end
          break;
       else if ( line.startsWith( "<<<<<<<" ) || line.startsWith( "=======" ) || line.startsWith( ">>>>>>>" ) )
       {
          // We have found a CVS/SVN conflict marker. Abort.
          // (It cannot be any useful data of the PO file, as otherwise the line would start with at least a quote)
          kdError(KBABEL) << "CVS/SVN conflict marker found! Aborting!" << endl << line << endl;
          return PARSE_ERROR;
       }
       
       // remove whitespaces from beginning and end of line
       line=line.stripWhiteSpace();

       if(part==Begin)
       {
           // ignore trailing newlines
           if(line.isEmpty())
              continue;

           if(line.startsWith("#~"))
           {
              _obsolete=true;
	      part=Comment;
	      _comment=line;
           }
           else if(line.startsWith("#"))
           {
               part=Comment;
               _comment=line;
           }
           else if(line.find(QRegExp("^msgctxt\\s*\".*\"$")) != -1)
           {
               part=Msgctxt;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgctxt\\s*\""));
               line.remove(QRegExp("\"$"));
               _msgctxt=line;
               seenMsgctxt=true;
           }
           else if(line.find(QRegExp("^msgid\\s*\".*\"$")) != -1)
           {
               part=Msgid;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgid\\s*\""));
               line.remove(QRegExp("\"$"));

               (*(_msgid).begin())=line;
           }
		     // one of the quotation marks is missing
           else if(line.find(QRegExp("^msgid\\s*\"?.*\"?$")) != -1)
           {
               part=Msgid;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgid\\s*\"?"));
               line.remove(QRegExp("\"$"));

               (*(_msgid).begin())=line;

               if(!line.isEmpty())
                  recoverableError=true;
           }
           else
           {
               kdDebug(KBABEL) << "no comment, msgctxt or msgid found after a comment: " << line << endl;
               error=true;
               break;
           }
       }
       else if(part==Comment)
       {
            if(line.isEmpty() && _obsolete ) return OK;
	    if(line.isEmpty() )
	       continue;
            else if(line.startsWith("#~"))
            {
               _comment+=("\n"+line);
	       _obsolete=true;
            }
            else if(line.startsWith("#"))
            {
               _comment+=("\n"+line);
            }
            else if(line.find(QRegExp("^msgctxt\\s*\".*\"$")) != -1)
            {
               part=Msgctxt;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgctxt\\s*\""));
               line.remove(QRegExp("\"$"));
               _msgctxt=line;
               seenMsgctxt=true;
            }
            else if(line.find(QRegExp("^msgid\\s*\".*\"$")) != -1)
            {
               part=Msgid;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgid\\s*\""));
               line.remove(QRegExp("\"$"));

               (*(_msgid).begin())=line;
            }
            // one of the quotation marks is missing
            else if(line.find(QRegExp("^msgid\\s*\"?.*\"?$")) != -1)
            {
               part=Msgid;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgid\\s*\"?"));
               line.remove(QRegExp("\"$"));

               (*(_msgid).begin())=line;
			   
               if(!line.isEmpty())
                     recoverableError=true;
            }
            else
            {
               kdDebug(KBABEL) << "no comment or msgid found after a comment while parsing: " << _comment << endl;
               error=true;
               break;
            }
        }
        else if(part==Msgctxt)
        {
            if(line.isEmpty())
               continue;
            else if(line.find(QRegExp("^\".*\\n?\"$")) != -1)
            {
               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^\""));
               line.remove(QRegExp("\"$"));
               
               // add Msgctxt line to item
               if(_msgctxt.isEmpty())
                  _msgctxt=line;
               else
                  _msgctxt+=("\n"+line);
            }
            else if(line.find(QRegExp("^msgid\\s*\".*\"$")) != -1)
            {
               part=Msgid;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgid\\s*\""));
               line.remove(QRegExp("\"$"));

               (*(_msgid).begin())=line;
            }
            // one of the quotation marks is missing
            else if(line.find(QRegExp("^msgid\\s*\"?.*\"?$")) != -1)
            {
               part=Msgid;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgid\\s*\"?"));
               line.remove(QRegExp("\"$"));

               (*(_msgid).begin())=line;
			   
               if(!line.isEmpty())
                     recoverableError=true;
            }
            else
            {
               kdDebug(KBABEL) << "no msgid found after a msgctxt while parsing: " << _msgctxt << endl;
               error=true;
               break;
            }
        }
        else if(part==Msgid)
        {
            if(line.isEmpty())
               continue;
            else if(line.find(QRegExp("^\".*\\n?\"$")) != -1)
            {
               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^\""));
               line.remove(QRegExp("\"$"));

               QStringList::Iterator it;
               if(_gettextPluralForm)
                   it = _msgid.fromLast();
               else
                   it = _msgid.begin();
               
               // add Msgid line to item
               if((*it).isEmpty())
                  (*it)=line;
               else
                  (*it)+=("\n"+line);
            }
            else if(line.find(QRegExp("^msgid_plural\\s*\".*\"$")) != -1)
            {
               part=Msgid;
               _gettextPluralForm = true;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgid_plural\\s*\""));
               line.remove(QRegExp("\"$"));

               _msgid.append(line);
            }
            // one of the quotation marks is missing
            else if(line.find(QRegExp("^msgid_plural\\s*\"?.*\"?$")) != -1)
            {
               part=Msgid;
               _gettextPluralForm = true;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgid_plural\\s*\"?"));
               line.remove(QRegExp("\"$"));

               _msgid.append(line);
			   
               if(!line.isEmpty())
                  recoverableError=true;
            }
           else if(!_gettextPluralForm 
                    && (line.find(QRegExp("^msgstr\\s*\".*\\n?\"$")) != -1))
            {
               part=Msgstr;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgstr\\s*\"?"));
               line.remove(QRegExp("\"$"));

               (*msgstrIt)=line;
            }
            else if(!_gettextPluralForm
                    && line.find(QRegExp("^msgstr\\s*\"?.*\\n?\"?$")) != -1)
            {
               part=Msgstr;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgstr\\s*\"?"));
               line.remove(QRegExp("\"$"));

               (*msgstrIt)=line;

               if(!line.isEmpty())
                  recoverableError=true;
            }
            else if( _gettextPluralForm 
                     && (line.find(QRegExp("^msgstr\\[0\\]\\s*\".*\\n?\"$")) != -1))
            {
               part=Msgstr;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgstr\\[0\\]\\s*\"?"));
               line.remove(QRegExp("\"$"));

               (*msgstrIt)=line;
            }
            else if( _gettextPluralForm
                     && (line.find(QRegExp("^msgstr\\[0\\]\\s*\"?.*\\n?\"?$")) != -1))
            {
               part=Msgstr;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgstr\\[0\\]\\s*\"?"));
               line.remove(QRegExp("\"$"));

               (*msgstrIt)=line;

               if(!line.isEmpty())
                  recoverableError=true;
            }
            else if ( line.startsWith( "#" ) )
            {
               // ### TODO: could this be considered recoverable?
               kdDebug(KBABEL) << "comment found after a msgid while parsing: " << _msgid.first() << endl;
               error=true;
               break;
            }
            else if ( line.startsWith( "msgid" ) )
            {
               kdDebug(KBABEL) << "Another msgid found after a msgid while parsing: " << _msgid.first() << endl;
               error=true;
               break;
            }
            // a line of the msgid with a missing quotation mark
            else if(line.find(QRegExp("^\"?.+\\n?\"?$")) != -1)
            {
               recoverableError=true;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^\""));
               line.remove(QRegExp("\"$"));

               QStringList::Iterator it;
               if( _gettextPluralForm )
                   it = _msgid.fromLast();
               else
                   it = _msgid.begin();
               
               // add Msgid line to item
               if((*it).isEmpty())
                  (*it)=line;
               else
                  (*it)+=("\n"+line);
            }
            else
            {
               kdDebug(KBABEL) << "no msgstr found after a msgid while parsing: " << _msgid.first() << endl;
               error=true;
               break;
            }
        }
        else if(part==Msgstr)
        {
            if(line.isEmpty())
               continue;
            // another line of the msgstr
            else if(line.find(QRegExp("^\".*\\n?\"$")) != -1)
            {
               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^\""));
               line.remove(QRegExp("\"$"));

               if((*msgstrIt).isEmpty())
                  (*msgstrIt)=line;
               else
                  (*msgstrIt)+=("\n"+line);
            }
            else if( _gettextPluralForm
                     && (line.find(QRegExp("^msgstr\\[[0-9]+\\]\\s*\".*\\n?\"$")) != -1))
            {
               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgstr\\[[0-9]+\\]\\s*\"?"));
               line.remove(QRegExp("\"$"));

               msgstrIt=_msgstr.append(line);
            }
	    else if( _gettextPluralForm 
                     && (line.find(QRegExp("^msgstr\\[[0-9]\\]\\s*\"?.*\\n?\"?$")) != -1))
            {
               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^msgstr\\[[0-9]\\]\\s*\"?"));
               line.remove(QRegExp("\"$"));

               msgstrIt=_msgstr.append(line);

               if(!line.isEmpty())
                  recoverableError=true;
            }
            else if((line.find(QRegExp("^\\s*msgid")) != -1) || (line.find(QRegExp("^\\s*#")) != -1))
            {
               // We have read successfully one entry, so end loop.
               stream.device()->at(pos);// reset position in stream to beginning of this line
               break;
            }
            else if(line.startsWith("msgstr"))
            {
               kdDebug(KBABEL) << "Another msgstr found after a msgstr while parsing: " << _msgstr.last() << endl;
               error=true;
               break;
            }
            // another line of the msgstr with a missing quotation mark
            else if(line.find(QRegExp("^\"?.+\\n?\"?$")) != -1)
            {
               recoverableError=true;

               // remove quotes at beginning and the end of the lines
               line.remove(QRegExp("^\""));
               line.remove(QRegExp("\"$"));

               if((*msgstrIt).isEmpty())
                  (*msgstrIt)=line;
               else
                  (*msgstrIt)+=("\n"+line);
            }
            else
            {
               kdDebug(KBABEL) << "no msgid or comment found after a msgstr while parsing: " << _msgstr.last() << endl;
               error=true;
               break;
            }
        }
    }

/*
   if(_gettextPluralForm)
   {
       kdDebug(KBABEL) << "gettext plural form:\n"
                 << "msgid:\n" << _msgid.first() << "\n"
                 << "msgid_plural:\n" << _msgid.last() << "\n" << endl;
       int counter=0;
       for(QStringList::Iterator it = _msgstr.begin(); it != _msgstr.end(); ++it)
       {
           kdDebug(KBABEL) << "msgstr[" << counter << "]:\n" 
                     << (*it) << endl;
           counter++;
       }
   }
  */

    //kdDebug( KBABEL ) << k_funcinfo << " NEAR RETURN" << endl;
    if(error)
       return PARSE_ERROR;
	else if(recoverableError)
		return RECOVERED_PARSE_ERROR;
    else
    {
      return OK;
    }
}
