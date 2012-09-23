/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2004 by Stanislav Visnovsky
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
#include "projectsettings.h"
#include "resources.h"

#include <qstring.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <kspell.h>

QString KBabel::Defaults::Spellcheck::ignoreURL()
{
    QString _ignoreURL;

    KStandardDirs * dirs = KGlobal::dirs();
    if(dirs)
    {
	_ignoreURL = dirs->saveLocation("appdata");
	if(_ignoreURL.right(1)!="/")
	    _ignoreURL+="/";
	_ignoreURL += "spellignores";
    }
    return _ignoreURL;
}

KSpellConfig* KBabel::Defaults::Spellcheck::defaultsettings = NULL;

bool KBabel::Defaults::Spellcheck::noRootAffix()
{
    if( ! defaultsettings )
    {
	defaultsettings = new KSpellConfig(0, "spellconfig");
    }
    
    return defaultsettings->noRootAffix();
}

bool KBabel::Defaults::Spellcheck::runTogether()
{
    if( ! defaultsettings )
    {
	defaultsettings = new KSpellConfig(0, "spellconfig");
    }
    
    return defaultsettings->runTogether();
}

int KBabel::Defaults::Spellcheck::spellClient()
{
    if( ! defaultsettings )
    {
	defaultsettings = new KSpellConfig(0, "spellconfig");
    }
    
    return defaultsettings->client();
}

QString KBabel::Defaults::Spellcheck::spellDictionary()
{
    if( ! defaultsettings )
    {
	defaultsettings = new KSpellConfig(0, "spellconfig");
    }
    
    return defaultsettings->dictionary();
}

int KBabel::Defaults::Spellcheck::spellEncoding()
{
    if( ! defaultsettings )
    {
	defaultsettings = new KSpellConfig(0, "spellconfig");
    }
    
    return defaultsettings->encoding();
}

QString KBabel::Defaults::CatalogManager::ignoreURL()
{
    QString _ignoreURL;

    KStandardDirs * dirs = KGlobal::dirs();
    if(dirs)
    {
	_ignoreURL = dirs->saveLocation("appdata");
	if(_ignoreURL.right(1)!="/")
	    _ignoreURL+="/";
	_ignoreURL += "validationignores";
    }
    return _ignoreURL;
}
																		
