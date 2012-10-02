/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001	  by Stanislav Visnovsky
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
#ifndef FINDOPTIONS_H
#define FINDOPTIONS_H

#include <qregexp.h>

class QString;
class QChar;

namespace KBabel 
{

class FindOptions
{
public:
   QString findStr;

   bool caseSensitive;
   bool wholeWords;
   bool fromCursor;
   bool backwards;
   bool isRegExp;

   bool inMsgid;
   bool inMsgstr;
   bool inComment;

   bool ignoreAccelMarker;
   bool ignoreContextInfo;
   
   bool askForNextFile;
   bool askForSave;	// whether should ask before saving or save automatically when loading next file
   bool inAllFiles;	// whether search should be done in all files
   bool inMarkedFiles;  // whether search should be restricted to marked ones
   bool inTemplates;

   // these are non-persistent options - not saved into configuration database
   bool askFile;	// whether findNext should ask catalog manager for next file
   QRegExp contextInfo;	// actual settings of context info regular expression
   QChar accelMarker;	// actual settings of accelerator marker
   
};

class ReplaceOptions : public FindOptions
{
public:
   QString replaceStr;
   bool ask;
};

}

#endif
