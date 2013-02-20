/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002 by Stanislav Visnovsky
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
#ifndef GETTEXTIMPORTPLUGIN_H
#define GETTEXTIMPORTPLUGIN_H

#include <catalogfileplugin.h>

#include <qstringlist.h>

class KURL;
class QFile;
class QTextCodec;

/* ****************************************************************************
  The class for importing GNU gettext PO files. As an extra information,
  it stores the list of all obsolete entries.
**************************************************************************** */

class GettextImportPlugin: public KBabel::CatalogImportPlugin
{
public:
    GettextImportPlugin(QObject* parent, const char* name, const QStringList &);
    virtual KBabel::ConversionStatus load(const QString& file, const QString& mimetype);
    virtual const QString id() { return "GNU gettext"; }

private:
    QTextCodec* codecForArray(QByteArray& arary, bool* hadCodec);
    KBabel::ConversionStatus readHeader(QTextStream& stream);
    KBabel::ConversionStatus readEntry(QTextStream& stream);
    
    // description of the last read entry
    QString _msgctxt;
    QStringList _msgid;
    QStringList _msgstr;
    QString _comment;
    bool _gettextPluralForm;
    bool _obsolete;
};

#endif
