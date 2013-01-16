/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/



#ifndef SEARCH_H
#define SEARCH_H

#include "../config.h"

// Standard Headers
#include <stdio.h>


class QLineEdit;
class QCheckBox;

// KDE headers
#include <kapplication.h>
#include <kmenubar.h>
#include <kdialogbase.h>

class Search : public KDialogBase
{
  Q_OBJECT

public:

  Search ( QWidget *parent = 0, const char * name=0);
  ~Search();

private:
  QCheckBox *substr;
  QCheckBox *wrap;
  QLineEdit *value;

signals:
    void search_signal();
    void search_done_signal();

public slots:
    void done_slot();
    void ok_slot();
    void textChanged(const QString &);
};
#endif
