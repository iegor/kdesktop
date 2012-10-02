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
  
*/

#ifndef POINFO_H
#define POINFO_H


#include "catalogfileplugin.h"

#include <kdebug.h>

#include <qstring.h>

/**
 * @brief File cache version number.
 *
 * @note If the existing file is outdated, it will not be read,
 * instead we will overwrite it with a new file.
 */
#define POINFOCACHE_VERSION 2

class GettextFlexLexer;

namespace KBabel {

class FindOptions;

/**
 * PO-file information class, with transparent caching of information.
 */
class KDE_EXPORT PoInfo
{
public:
   int total;
   int fuzzy;
   int untranslated;

   QString project;
   QString creation;
   QString revision;
   QString lastTranslator;
   QString languageTeam;
   QString mimeVersion;
   QString contentType;
   QString encoding;
   QString others;

   QString headerComment;

   /**
    * Find PO-file information in the cache.
    *
    * @param url The URL of the PO-file.
    * @param info Returned cached information.
    * @return true if found, false otherwise.
    */
   static bool cacheFind(const QString url, PoInfo& info);

   /**
    * Save PO-file information in the cache.
    *
    * @param url The URL of the PO-file.
    * @param info Information to save.
    */
   static void cacheSave(const QString url, PoInfo& info);

   /** 
    * @brief Get information about the PO file
    * 
    * @param url The path (not an URL) of the file 
    * @param info The information retrieved from the file
    * @param wordList ???
    * @param updateWordList ???
    * @param interactive Can the function interact with the user?
    * @param msgfmt Should each file be checked with Gettext's msgfmt before
    *        being parsed?
    * @since KBabel 1.11 (KDE 3.5)
    */
   static ConversionStatus info(const QString& url,PoInfo& info, QStringList &wordList, bool updateWordList, bool interactive, bool msgfmt);

   /** 
    * @brief Get information about the PO file
    * 
    * @param url The path (not an URL) of the file 
    * @param info The information retrieved from the file
    * @param wordList ???
    * @param updateWordList ???
    * @param interactive Can the function interact with the user?
    * @deprecated
    * @note This function is missing in KBabel 1.11 (KDE 3.5). 
    *       The function was re-introduced for binary-compatibility in
    *       KBabel 1.11.1 (KDE 3.5.1)
    * @note This function always call Gettext's mgfmt before parsing each file
    */
   static ConversionStatus info(const QString& url,PoInfo& info, QStringList &wordList, bool updateWordList, bool interactive = true);

   static PoInfo headerInfo(const CatalogItem&);
   static bool findInFile(const QString& url, FindOptions options );
   
   static bool stopStaticRead;

   /**
   * reads header information from the file and searches for charset
   * information.
   * @param gettextHeader text containing gettext headers
   *
   * @return Codec for found charset or 0, if no information has been found
   */
   static QTextCodec* codecForFile(QString gettextHeader);

   /**
    * @brief Write the entire cache.
    */
   static void cacheWrite();
private:
   /**
    * Read the entire cache.
    */
   static void cacheRead();

   static ConversionStatus fastRead( CatalogItem& item, GettextFlexLexer* lexer, bool storeText );
   
   static bool _gettextPluralForm;
};

}

#endif // POINFO_H
