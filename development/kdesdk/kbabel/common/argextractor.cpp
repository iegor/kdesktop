/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2001 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2002 by Stanislav Visnovsky <visnovsky@nenya.ms.mff.cuni.cz>
                    
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
#include "catalogsettings.h"
#include "argextractor.h"

#include <kdebug.h>
#include <kstaticdeleter.h>
#include <qregexp.h>

using namespace KBabel;

ArgExtractor::ArgExtractor(QString string) : RegExpExtractor( string )
{
}

QStringList *ArgExtractor::_argList=0;
KStaticDeleter< QStringList > sdAL;

QStringList *ArgExtractor::regExpList()
{
    if(!_argList)
    {
         sdAL.setObject( _argList, new QStringList );
        //Build the default arg list
        _argList->append("%[ndioxXucsfeEgGp]");
        _argList->append("%([0-9]+(\\$))?[-+'#0]?[0-9]*(.[0-9]+)?[hlL]?[dioxXucsfeEgGp]");  
        _argList->append("%[0-9]+");
    }

    return _argList;
}

void ArgExtractor::setArgExpressions( QStringList* list )
{
    sdAL.setObject( _argList, new QStringList );
    for( QStringList::Iterator it = list->begin() ; it != list->end() ; ++it )
        _argList->append(*it);
}
