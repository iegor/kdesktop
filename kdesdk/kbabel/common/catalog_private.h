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
#ifndef CATALOGPRIVATE_H
#define CATALOGPRIVATE_H

#include <qintcache.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qvaluevector.h>
#include <kurl.h>

#include "msgfmt.h"
#include "catalogsettings.h"
#include "catalogitem.h"
#include "regexpextractor.h"

class QString;
class QTextStream;
class KFileInfo;
class QTextCodec;
class QFile;
class FindOptions;
class KConfig;

namespace KBabel
{

class CatalogPrivate
{

public:

   /** url of the po-file, that belongs to this catalog */
   KURL _url;
   QString _packageName;
   QString _packageDir;

   /** holds the entries in the catalog */
   QValueVector<CatalogItem> _entries;
   /** The header of the po-file. */
   CatalogItem _header;
   
   /** list of obsolete entries */
   QValueList<CatalogItem> _obsoleteEntries;
   
   /** identification string for used import filter*/
   QString _importID;
   QString _mimeTypes;

   bool _modified;
   bool _readOnly;
   bool _generatedFromDocbook;
   bool _active;
   bool _stop;

   QValueList<uint> _fuzzyIndex;
   QValueList<uint> _untransIndex;
   QValueList<uint> _errorIndex;

   QPtrList<CatalogView> _views;

   TagSettings _tagSettings;

   QPtrList<EditCommand> _undoList;
   QPtrList<EditCommand> _redoList;

   QTextCodec *fileCodec;

   QStringList msgidDiffList;
   QMap< QString, QStringList > msgstr2MsgidDiffList;
   QIntCache<QString> diffCache;
   
   int numberOfPluralForms;

   Project::Ptr _project;
   RegExpExtractor *_tagExtractor, *_argExtractor;
   
   QStringList _catalogExtra;
   
   CatalogPrivate(Project::Ptr project) : 
	_packageName( QString::null ), _packageDir( QString::null ),
	_header (project), 
	_importID( QString::null ), _mimeTypes( "text/plain" ),
	_modified(false), _readOnly(false), _generatedFromDocbook(false), 
	_active(false), _stop(false),
	fileCodec(0),  diffCache(30,76), numberOfPluralForms(-1), 
	_project(project) 
   {
	_entries.clear();
	_obsoleteEntries.clear();
	diffCache.setAutoDelete(true);
	diffCache.clear();
	
	_views.setAutoDelete(false);
	
	_undoList.setAutoDelete(true);
	_redoList.setAutoDelete(true);
	_tagExtractor = new RegExpExtractor(QStringList());
	_argExtractor = new RegExpExtractor(QStringList());
	
	_catalogExtra.clear();
   }
   
   ~CatalogPrivate()
   {
       delete _tagExtractor;
       delete _argExtractor;
   }
};

}

#endif //CATALOGPRIVATE_H
