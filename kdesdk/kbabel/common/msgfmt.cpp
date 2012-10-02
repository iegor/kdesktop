/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
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
#include "msgfmt.h"

#include <kapplication.h>
#include <kprocess.h>

#include <qfileinfo.h>
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qstring.h>

#include <stdlib.h>

using namespace KBabel;

Msgfmt::Msgfmt(QObject* parent,const char* name)
      : QObject(parent , name)
{
}

Msgfmt::Status Msgfmt::checkSyntax(QString file,QString& output, bool gnu)
{
   Status stat=Ok;
   // this method does not return the right retrun values at the moment :-(

   KProcess proc;

   connect(&proc,SIGNAL(receivedStdout(KProcess*, char*, int)),
            this,SLOT(addToOutput(KProcess*,char *, int )));
   connect(&proc,SIGNAL(receivedStderr(KProcess *, char *, int)),
            this,SLOT(addToOutput(KProcess*,char *, int)));

   // remove last output
   _output="";


   proc << "msgfmt" << "--statistics" << "-o" << "/dev/null" << file;
   
   if( gnu )
   {
	proc << "-vc";
   }

   if(!proc.start(KProcess::Block,KProcess::Stderr))
   {
      stat=NoExecutable;
   }
   else if(proc.normalExit())
   {
       if( proc.exitStatus() || _output.contains(QRegExp("^.+:\\d+:")) ) // little workaround :-(
         stat=SyntaxError;
   }
   else
      stat=Error;

   output=_output;


   return stat;
}

Msgfmt::Status Msgfmt::checkSyntaxInDir(QString dir,QString regexp,QString& output)
{
   Status stat=Ok;

   // this method does not return the right return values at the moment :-(
   KProcess proc;
   proc.setUseShell(true);

   connect(&proc,SIGNAL(receivedStdout(KProcess*, char*, int)),
            this,SLOT(addToOutput(KProcess*,char *, int )));
   connect(&proc,SIGNAL(receivedStderr(KProcess *, char *, int)),
            this,SLOT(addToOutput(KProcess*,char *, int)));

   // remove last output
   _output="";

   proc << "IFS='\n'; msgfmt --statistics -o /dev/null "
           "$(find" << KProcess::quote(dir) << "-name" << KProcess::quote(regexp) << ")";

   if(!proc.start(KProcess::Block,KProcess::Stderr))
   {
      stat=NoExecutable;
   }
   else if(proc.normalExit())
   {
       if( proc.exitStatus() || _output.contains(QRegExp("^.+:\\d+:")) ) // little workaround :-(
         stat=SyntaxError;
   }
   else
      stat=Error;

   output=_output;


   return stat;
}



void Msgfmt::addToOutput(KProcess*,char *buffer, int buflen)
{
   QString newString = QString::fromLocal8Bit(buffer, buflen);

   _output+=newString;
}


QString Msgfmt::tempSaveName()
{
   return kapp->tempSaveName("/tmp/kbabel_msgfmt.po");
}

#include "msgfmt.moc"
