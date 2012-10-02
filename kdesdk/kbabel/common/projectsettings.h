/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2005 by Stanislav Visnovsky
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
#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <qstring.h>
#include <qstringlist.h>
#include <qfont.h>
#include <qcolor.h>
#include <kurl.h>

#include "catalogsettings.h"

class KSpellConfig;

namespace KBabel {

struct SpellcheckSettings
{
   bool valid;
   bool noRootAffix;
   bool runTogether;
   int spellEncoding;
   int spellClient;
   QString spellDict;

   bool rememberIgnored;
   QString ignoreURL;

   bool onFlySpellcheck;
   
   SpellcheckSettings() { valid=false; }
};

struct CatManSettings
{
   QString poBaseDir;
   QString potBaseDir;

   bool openWindow;

   QStringList dirCommands;
   QStringList dirCommandNames;
   QStringList fileCommands;
   QStringList fileCommandNames;

   QString ignoreURL;

   bool killCmdOnExit;
   bool indexWords;
   /// Should be msgfmt be run before processing a file?
   bool msgfmt;

   bool flagColumn;
   bool fuzzyColumn;
   bool untranslatedColumn;
   bool totalColumn;
   bool cvsColumn;
   bool revisionColumn;
   bool translatorColumn;

};

struct SourceContextSettings 
{
    /**
     * A path, which can be used as @CODEROOT@ variable in @ref sourcePaths .
     * Defaults to empty string.
     */
    QString codeRoot;

    /**
     * List of paths, where the source file should be lookup. Can use @CODEROOT@ (replaced by @ref codeRoot),
     * @PACKAGE@ (replaced by package name), @PACKAGEDIR@ (replaced by the
     * longest known path of the package) and @COMMENTPATH@ (path extracted from comment specs.
     */
    QStringList sourcePaths;
    
    void SourceContextSettins() { codeRoot = QString::null; sourcePaths.clear(); }
};

/**
* This namespace provides static methods and variables to get the default
* values of configuration values
*/
namespace Defaults
{
   class KDE_EXPORT Spellcheck
   {
    public:
       static QString ignoreURL();
       static bool noRootAffix();
       static bool runTogether();
       static int spellClient();
       static QString spellDictionary();
       static int spellEncoding();
    private:
       static KSpellConfig* defaultsettings;
   };

   class KDE_EXPORT CatalogManager
   {
    public:
       static QString ignoreURL();
   };
}

}

#endif // PROJECTSETTINGS_H
