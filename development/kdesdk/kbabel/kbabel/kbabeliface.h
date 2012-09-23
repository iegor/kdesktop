/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2002 by Stanislav Visnovsky
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
#ifndef KBABELIFACE_H
#define KBABELIFACE_H

#include <dcopobject.h>
#include <qstringlist.h>

class KBabelIface : virtual public DCOPObject
{
  K_DCOP
public:

k_dcop:
  virtual void openURL(QCString url, QCString package, WId window, int newWindow) { url = ""; window = 0; newWindow = 0; package= "";}
  virtual void openURL(QCString url, QCString package, WId window, int newWindow, QCString projectFile) 
    { url = ""; window = 0; newWindow = 0; projectFile="";  package= "";}
  virtual void openTemplate(QCString openFilename, QCString package, QCString saveFilename, int newWindow) { openFilename = ""; saveFilename = ""; newWindow = 0; package= "";}
  virtual void openTemplate(QCString openFilename, QCString package, QCString saveFilename, int newWindow, QCString projectFile) 
    { openFilename = ""; saveFilename = ""; newWindow = 0; projectFile=""; package= "";}
  /**
   * open file url, if not already opened and goto entry
   * that is equal msgid
   */
  virtual void gotoFileEntry(QCString url, QCString msgid)=0;
  virtual void gotoFileEntry(QCString url, QCString package, int msgid)=0;
  virtual void gotoFileEntry(QCString url, QCString package, int msgid, QCString projectFile)=0;
  

  virtual bool findInFile(QCString fileSource, QCString url, 
	QString findStr, int caseSensitive, int wholeWords, int isRegExp, 
	int inMsgid, int inMsgstr, int inComment, 
	int ignoreAccelMarker, int ignoreContextInfo, int askForNextFile, int askForSave)=0;
  virtual bool replaceInFile(QCString fileSource, QCString url, 
	QString findStr, QString replaceStr, int caseSensitive, int wholeWords, int isRegExp, 
	int inMsgid, int inMsgstr, int inComment, int ignoreAccelMarker, 
	int ignoreContextInfo, int ask, int askForNextFile, int askForSave)=0;
  virtual bool findInFile(QCString fileSource, QCString url, 
	QString findStr, int caseSensitive, int wholeWords, int isRegExp, 
	int inMsgid, int inMsgstr, int inComment, 
	int ignoreAccelMarker, int ignoreContextInfo, int askForNextFile, int askForSave, QCString project )=0;
  virtual bool replaceInFile(QCString fileSource, QCString url, 
	QString findStr, QString replaceStr, int caseSensitive, int wholeWords, int isRegExp, 
	int inMsgid, int inMsgstr, int inComment, int ignoreAccelMarker, 
	int ignoreContextInfo, int ask, int askForNextFile, int askForSave, QCString project )=0;
  virtual void spellcheck(QStringList fileList)=0;
};

#endif // KBABELIFACE_H
