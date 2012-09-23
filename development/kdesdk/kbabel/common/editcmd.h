/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

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
#ifndef EDITCMD_H
#define EDITCMD_H

#include "itempart.h"

#include <qstring.h>
#include <kdemacros.h>

namespace KBabel
{

class KDE_EXPORT EditCommand
{
public:
    enum Commands { Invalid, Begin, End, Insert, Delete };

    EditCommand();
    EditCommand( const int index, const Part part );
    virtual ~EditCommand() {};
    virtual Commands type() const { return Invalid; }
    virtual int terminator() const { return 0; }

    int index() const { return _index; }
    void setIndex( int index ) { _index=index; }
    Part part() const { return _part; }
    void setPart(Part part) {_part=part;}

    virtual bool merge( EditCommand* ) { return false;}

private:
   Part _part;
   int _index;
};


class KDE_EXPORT BeginCommand : public EditCommand
{
public:
    BeginCommand( const int index, const Part part ) : EditCommand(index,part) {}
    virtual Commands type() const { return Begin; }
    virtual int terminator() const { return 1; }
};


class KDE_EXPORT EndCommand : public EditCommand
{
public:
    EndCommand( const int index, const Part part ) : EditCommand(index,part) {}
    virtual Commands type() const { return End; }
    virtual int terminator() const { return -1; }
};



class KDE_EXPORT DelTextCmd : public EditCommand
{
public:
    int offset;
    QString str;
    int pluralNumber;

    // have to handle deletion of current selection
    DelTextCmd(int offset, const QString &str, int pluralNumber );
    virtual Commands type() const { return Delete; }

    bool merge( EditCommand* other);
};

class KDE_EXPORT InsTextCmd : public DelTextCmd
{

public:
    InsTextCmd(int offset,const QString &str, int pluralNumber );
    virtual Commands type() const { return Insert; }

    bool merge( EditCommand* other);
};

}

#endif // SETTINGS_H
