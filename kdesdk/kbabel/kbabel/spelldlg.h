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
#ifndef SPELLDLG_H
#define SPELLDLG_H

#include <kdialogbase.h>


class SpellDlgWidget;

class SpellDlg : public KDialogBase
{
	Q_OBJECT

public:
	SpellDlg(bool haveMarkedText, QWidget* parent, const char *name=0);
	~SpellDlg();
	
	bool all() const;
	bool current() const;
	bool begin() const;
	bool end() const;
	bool marked() const;
	bool beginCurrent() const;

private slots:
	void markedChecked(bool);

private:
	SpellDlgWidget *_mainWidget;
};

#endif // SPELLDLG_H
