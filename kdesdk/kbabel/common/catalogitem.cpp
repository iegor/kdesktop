/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2002	  by Stanislav Visnovsky <visnovsky@nenya.ms.mff.cuni.cz>
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
#include "catalogitem.h"
#include "catalogitem_private.h"
#include "editcmd.h"
#include "resources.h"
#include "libgettext/pofiles.h"
#include "libgettext/tokens.h"

#include "regexpextractor.h"

#include <qtextstream.h>
#include <qtextcodec.h>
#include <qdom.h>

using namespace KBabel;

CatalogItem::CatalogItem(Project::Ptr project)
{
   d=0;
   clear();
   d->_project = project;
}

CatalogItem::CatalogItem(const CatalogItem& item)
{
   d=0;
   clear();
   *d=*(item.d);
}

CatalogItem::~CatalogItem()
{
    delete d;
}

QString CatalogItem::comment() const
{
    return d->_comment;
}

QString CatalogItem::msgctxt(const bool noNewlines) const
{
    if( noNewlines )
    {
        QString tmp = d->_msgctxt;
        tmp.replace("\n", " "); // ### TODO: " " or "" ?
        return tmp;
    }
    else
        return d->_msgctxt;
}

QStringList CatalogItem::msgid(const bool noNewlines) const
{
    QStringList result=d->_msgid;
    if( noNewlines )
    {
        result.gres( "\n", "" );
    }
    return result;
}

QStringList CatalogItem::msgstr(const bool noNewlines) const
{
    QStringList result=d->_msgstr;
    if( noNewlines )
    {
        result.gres( "\n", "" );
    }
    return result;
}

QStringList CatalogItem::msgstrAsList(int nr) const
{
   QString str;
   if(d->_gettextPluralForm && nr > 0)
   {
       QStringList::ConstIterator it = d->_msgstr.at(nr);
       if(it != d->_msgstr.end())
       {
           str = *it;
       }
       else
       {
           kdDebug(KBABEL) << "request for non existing plural form "
               << nr << endl;
       }
   }
   else
   {
       str = d->_msgstr.first();
   }

   QStringList list=QStringList::split("\n",str);

   if(str.left(1)=="\n")
      list.prepend("");

   if(list.isEmpty())
      list.append("");

   return list;
}

bool CatalogItem::isValid() const
{
    return d->_valid;
}

void CatalogItem::setMsgctxt(QString msg)
{
    d->_msgctxt=msg;
}

void CatalogItem::setMsgid(QString msg)
{
    d->_msgid=msg;
}

void CatalogItem::setMsgid(QStringList msg)
{
    d->_msgid=msg;
}

void CatalogItem::setMsgstr(QString msg)
{
    d->_msgstr=msg;
}

void CatalogItem::setMsgstr(QStringList msg)
{
    d->_msgstr=msg;
}

void CatalogItem::setComment(QString com)
{
    d->_comment=com;
}

void CatalogItem::setGettextPluralForm( bool _gettextPlural )
{
    d->_gettextPluralForm = _gettextPlural;
}

QStringList CatalogItem::errors() const
{
    return d->_errors;
}

QStringList CatalogItem::tagList( RegExpExtractor& te)
{
    if(!d->_haveTagList)
    {
	// FIXME: should care about plural forms in msgid
        te.setString(msgid(true).first());
        d->_tagList = QStringList(te.matches());
	d->_haveTagList = true;
    }

    return d->_tagList;
}

QStringList CatalogItem::argList( RegExpExtractor& te)
{
    if(!d->_haveArgList)
    {
	// FIXME: should care about plural forms in msgid
        te.setString(msgid(true).first());
        d->_argList = QStringList(te.matches());
    }

    return d->_argList;
}


bool CatalogItem::isFuzzy() const
{
    return d->_comment.find( QRegExp(",\\s*fuzzy") ) != -1;
}

bool CatalogItem::isCformat() const
{
    // Allow "possible-c-format" (from xgettext --debug) or "c-format"
    // Note the regexp (?: ) is similar to () but it does not capture (so it is faster)
    return d->_comment.find( QRegExp(",\\s*(?:possible-)c-format") ) == -1;
}

bool CatalogItem::isNoCformat() const
{
    return d->_comment.find( QRegExp(",\\s*no-c-format") ) == -1;
}

bool CatalogItem::isQtformat() const
{
    return d->_comment.find( QRegExp(",\\s*qt-format") ) == -1;
}

bool CatalogItem::isNoQtformat() const
{
    return d->_comment.find( QRegExp(",\\s*no-qt-format") ) == -1;
}

bool CatalogItem::isUntranslated() const
{
   return d->_msgstr.first().isEmpty();
}

PluralFormType CatalogItem::pluralForm() const
{
    if( d->_gettextPluralForm )
        return Gettext;
    if( d->_msgid.first().startsWith( "_n: " ) )
        return KDESpecific;
    else
        return NoPluralForm;
}

int CatalogItem::totalLines() const
{
   int lines=0;
   if(!d->_comment.isEmpty())
   {
      lines = d->_comment.contains('\n')+1;
   }
   int msgctxtLines=0;
   if(!d->_msgctxt.isEmpty())
   {
      msgctxtLines=d->_msgctxt.contains('\n')+1;
   }
   int msgidLines=0;
   QStringList::ConstIterator it;
   for(it=d->_msgid.begin(); it != d->_msgid.end(); ++it)
   {
      msgidLines += (*it).contains('\n')+1;
   }
   int msgstrLines=0;
   for(it=d->_msgstr.begin(); it != d->_msgstr.end(); ++it)
   {
      msgstrLines += (*it).contains('\n')+1;
   }

   if(msgctxtLines>1)
      msgctxtLines++;
   if(msgidLines>1)
      msgidLines++;
   if(msgstrLines>1)
      msgstrLines++;

   lines+=( msgctxtLines+msgidLines+msgstrLines );

   return lines;
}


void CatalogItem::setSyntaxError(bool on)
{
	if(on && !d->_errors.contains("syntax error"))
		d->_errors.append("syntax error");
	else
		d->_errors.remove("syntax error");
}

QPtrList<EditCommand> CatalogItem::removeFuzzy(bool doIt)
{
   QPtrList<EditCommand> editList;
   editList.setAutoDelete(false);

   QString comment=d->_comment;

   if(isFuzzy())
   {
       EditCommand *cmd;
       QRegExp fuzzyStr(",\\s*fuzzy");

       int offset;
       offset=comment.find(fuzzyStr);
       while(offset>=0)
       {
          cmd = new DelTextCmd(offset,fuzzyStr.cap(),0);
          cmd->setPart(Comment);
          editList.append(cmd);

          comment.remove(offset,fuzzyStr.cap().length());

          offset=comment.find(fuzzyStr,offset+1);
       }

       // remove empty comment lines
       if( comment.contains( QRegExp("^#\\s*$") ))
       {
          cmd = new DelTextCmd(0,comment,0);
          cmd->setPart(Comment);
          editList.append(cmd);

          comment="";
       }
       if( comment.contains( QRegExp("\n#\\s*$") ))
       {
          offset=comment.find( QRegExp("\n#\\s*$") );
          while(offset>=0)
          {
             cmd = new DelTextCmd(offset,comment.mid(offset),0);
             cmd->setPart(Comment);
             editList.append(cmd);

             comment.remove(offset,comment.length()-offset);

             offset=comment.find( QRegExp("\n#\\s*$"), offset+1 );
          }
       }
       if( comment.contains( QRegExp("\n#\\s*\n") ))
       {
          offset=comment.find( QRegExp("\n#\\s*\n") )+1;
          while(offset>=0)
          {
             int endIndex=comment.find("\n",offset)+1;

             cmd = new DelTextCmd(offset,comment.mid(offset,endIndex-offset),0);
             cmd->setPart(Comment);
             editList.append(cmd);

             comment.remove(offset,endIndex-offset);

             offset=comment.find( QRegExp("\n#\\s*\n"), offset+1 );
          }
       }

       if(doIt)
          d->_comment=comment;

   }

   return editList;
}



QPtrList<EditCommand> CatalogItem::addFuzzy(bool doIt)
{
   QPtrList<EditCommand> editList;
   editList.setAutoDelete(false);


   if(!isFuzzy())
   {
       EditCommand *cmd;
       int offset=d->_comment.length();

       QString addStr;
       if(offset > 0 && d->_comment[offset-1] != '\n')
       {
           addStr='\n';
       }
       addStr+="#, fuzzy";

       cmd = new InsTextCmd(offset,addStr,0);
       cmd->setPart(Comment);
       editList.append(cmd);


       if(doIt)
          d->_comment+=addStr;
   }

   return editList;
}


void CatalogItem::processCommand(EditCommand* cmd, bool undo)
{
    kdDebug(KBABEL) << "CatalogItem::processCommand" << endl;
    if(cmd->terminator()!=0)
       return;

    DelTextCmd* delcmd = (DelTextCmd*) cmd;

    bool ins =  true;
    if (delcmd->type() == EditCommand::Delete )
       ins = undo;
    else if (delcmd->type() == EditCommand::Insert )
       ins = !undo;
    else
    {
       kdDebug(KBABEL) << "what kind of edit command is this?" << endl;
       return;
    }

    if ( ins )
    {
       if(delcmd->part()==Msgstr)
       {
          if(delcmd->pluralNumber >= (int)d->_msgstr.count() )
	  {
	      for( uint i = d->_msgstr.count() ; i < (uint)(delcmd->pluralNumber+1) ; i++ )
	          d->_msgstr.append( "" );
	  }

	  (*(d->_msgstr).at(delcmd->pluralNumber)).insert(delcmd->offset,delcmd->str);
       }
       else if(delcmd->part()==Comment)
       {
          d->_comment.insert(delcmd->offset,delcmd->str);
       }
    }
    else
    { // del
       if(delcmd->part()==Msgstr)
       {
          if(delcmd->pluralNumber >= (int)d->_msgstr.count() )
	  {
	      for( uint i = d->_msgstr.count() ; i < (uint)(delcmd->pluralNumber+1) ; i++ )
	          d->_msgstr.append( "" );
	  }

          (*(d->_msgstr).at(delcmd->pluralNumber)).remove(delcmd->offset,delcmd->str.length());
       }
       else if(delcmd->part()==Comment)
       {
          d->_comment.remove(delcmd->offset,delcmd->str.length());
       }
    }
}


void CatalogItem::clear()
{
   if( !d ) d = new CatalogItemPrivate();
   else {
      d->_msgid.clear();
      d->_msgstr.clear();
      d->_errors.clear();

      d->_tagList.clear();
      d->_argList.clear();
   }
   d->_comment="";
   d->_msgctxt="";
   d->_valid=true;
   d->_gettextPluralForm=false;
   d->_haveTagList=false;
   d->_haveArgList=false;

   d->_msgid.append("");
   d->_msgstr.append("");
}

void CatalogItem::operator=(const CatalogItem& rhs)
{
    d->_comment = rhs.d->_comment;
    d->_msgctxt = rhs.d->_msgctxt;
    d->_msgid = rhs.d->_msgid;
    d->_msgstr = rhs.d->_msgstr;
    d->_valid = rhs.d->_valid;
    d->_errors = rhs.d->_errors;
    d->_gettextPluralForm = rhs.d->_gettextPluralForm;
}

QString CatalogItem::nextError() const
{
    return d->_errors.first();
}

void CatalogItem::clearErrors()
{
    d->_errors.clear();
}

void CatalogItem::appendError(const QString& error )
{
    if( !d->_errors.contains( error ) )
	d->_errors.append(error);
}

void CatalogItem::removeError(const QString& error )
{
    d->_errors.remove(error);
}

Project::Ptr CatalogItem::project() const
{
    return d->_project;
}

void CatalogItem::setProject( Project::Ptr project )
{
    d->_project = project;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
