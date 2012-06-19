/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2001 by Matthias Kiefer <matthias.kiefer@gmx.de>
                    
  based on code of Andrea Rizzi <rizzi@kde.org>

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
#ifndef _REGEXP_EXTRACTOR_H_
#define _REGEXP_EXTRACTOR_H_

#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qregexp.h>
#include <kdemacros.h>

namespace KBabel
{

struct KDE_EXPORT MatchedEntryInfo{
    uint index;
    QString extracted;
};


/**
 * class to extract entries based on regexps from a string
 * @author Andrea Rizzi <rizzi@kde.org>
 */
class KDE_EXPORT RegExpExtractor 
{

public:
  /**
   * Create a regexp extractor for "string"
   */ 
  RegExpExtractor(const QStringList& regexps);

  virtual ~RegExpExtractor() {}

  /**
   * Set the string of this extractor
   */
  void setString(QString string);

  /**
   * @return the number of matches found
   */
  uint countMatches();

  /**
   * @return the first match, and set the internal cursor to
   * the beginning
   */
  QString firstMatch();

  /**
   * @return the next match and move cursor forward
   */
  QString nextMatch();

  /**
   * @return the n-th match. It also moves the cursor.
   */
  QString match(uint matchnumber);

  /**
   * @return the n-th match. It also moves the cursor. -1 is there
   * is no such match.
   */
  int matchIndex(uint matchnumber);

  /**
   * @return the next match and move cursor forward
   */
  QString prevMatch();

  /**
   * @return the last match and move the cursor to the end
   */
  QString lastMatch();
  
  /**
   * @return a list of all matches
   */
  QStringList matches();

  /**
   * @return the string without matched text
   *
   * @param keepPos if false, the matches are just removed and so the
   * position of the other words in the string will change. If true,
   * the matches are replaced with ' ' and therefore the position of the
   * words will not change
   */
  QString plainString(bool keepPos=false);

  /**
   * @return the string, where matches are replaced with the given string
   */
  QString matchesReplaced(const QString& replace);

  //Functions that allow user to define his own regexps.
  
  /**
   * Add a regexp to the list of regexp identifier.
   */
  void addRegExpIdentifier(QString regexp);
		      
		      
  /** 
   * Delete from the regexp list the regexp.
   */
  void deleteRegExpIdentifier(QString regexp);
  
  void setRegExpList( const QStringList& regexps );

  QStringList regExpList();
				   
protected:

  void processString();
  QPtrList<MatchedEntryInfo> _matches;
  QString _string;
  QStringList _regExpList;
};


}

#endif
