/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002	  by Stanislav Visnovsky <visnovsky@kde.org>

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
#ifndef CATALOGITEMPRIVATE_H
#define CATALOGITEMPRIVATE_H

#include <qstringlist.h>

namespace KBabel
{

/**
* This class represents data for an entry in a catalog.
* It contains the comment, the Msgid and the Msgstr.
* It defines some functions to query the state of the entry
* (fuzzy, untranslated, cformat).
*
* @short Class, representing an entry in a catalog
* @author Matthias Kiefer <matthias.kiefer@gmx.de>
* @author Stanislav Visnovsky <visnovsky@kde.org>
*/

class CatalogItemPrivate
{

public:

   QString _comment;
   QString _msgctxt;
   QStringList _msgid;
   QStringList _msgstr;

   QStringList _tagList;
   QStringList _argList;

   bool _valid;
   bool _haveTagList;
   bool _haveArgList;
   QStringList _errors;

   /** flag, if this entry is a plural forms entry */
   bool _gettextPluralForm;
   
   friend class CatalogItem;
   
   KBabel::Project::Ptr _project;
   
   CatalogItemPrivate()
   {
	_comment = "";
	_valid=false;
	_haveTagList=false;
	_haveArgList=false;
	_gettextPluralForm=false;
   }
};

}

#endif // CATALOGITEMPRIVATE_H
