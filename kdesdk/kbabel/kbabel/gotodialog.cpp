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

#include <qstring.h>
#include <klocale.h>
#include <knuminput.h>

#include "gotodialog.h"

GotoDialog::GotoDialog(int max,QWidget* parent)
   : KDialogBase(parent,0,true,i18n("Go to Entry"),Ok|Cancel)
{
   QGroupBox* box=new QGroupBox(1,Qt::Horizontal,i18n("Go to Entry"),this);
   _spinBox= new KIntSpinBox(1,max,1,1,10,box);

   setMainWidget(box);

   _spinBox->setFocus();
}

GotoDialog::~GotoDialog()
{
}


void GotoDialog::show()
{
	_spinBox->setEditFocus(true);

	KDialogBase::show();
}


int GotoDialog::number()
{
   return _spinBox->value();
}

void GotoDialog::setMax(int max)
{
   _spinBox->setRange(1,max);
   if(_spinBox->value()>max)
      _spinBox->setValue(max);
}
