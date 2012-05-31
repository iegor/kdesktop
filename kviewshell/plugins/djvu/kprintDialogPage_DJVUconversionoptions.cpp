/***************************************************************************
 *   Copyright (C) 2005 by Stefan Kebekus                                  *
 *   kebekus@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include <klocale.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <kdebug.h>

#include "kprintDialogPage_DJVUconversionoptions.h"
#include "kprintDialogPage_DJVUconversionoptions_basewidget.h"

KPrintDialogPage_DJVUConversionOptions::KPrintDialogPage_DJVUConversionOptions( QWidget *parent, const char *name )
  : KPrintDialogPage( parent, name )
{
  setTitle( i18n("DJVU to PS Conversion") );

  kprintDialogPage_pageoptions_baseLayout = new QVBoxLayout( this, 11, 6, "kprintDialogPage_pageoptions_baseLayout"); 
  if (kprintDialogPage_pageoptions_baseLayout == 0) {
    kdError(1223) << "KPrintDialogPage_DJVUPageOptions::KPrintDialogPage_DJVUPageOptions() cannot create layout" << endl;
    return;
  }
  
  wdg = new kprintDialogPage_DJVUconversionoptions_basewidget(this, "basewdg" );
  if (wdg != 0) {
    kprintDialogPage_pageoptions_baseLayout->addWidget( wdg );
  }
}



void KPrintDialogPage_DJVUConversionOptions::getOptions( QMap<QString,QString>& opts, bool )
{
  if (wdg == 0) 
    return;

  opts["kde-kdjvu-pslevel"] = QString::number(wdg->psLevel->currentItem() + 1);

  kdDebug() << "getOptions: renderMode = " << wdg->renderMode->currentItem() << endl;
  switch (wdg->renderMode->currentItem())
  {
    case 1:
      opts["kde-kdjvu-rendermode"] = "black-and-white";
      break;
    case 2:
      opts["kde-kdjvu-rendermode"] = "foreground";
      break;
    case 3:
      opts["kde-kdjvu-rendermode"] = "background";
      break;
    default: // 0
      opts["kde-kdjvu-rendermode"] = "color";
  }
}


void KPrintDialogPage_DJVUConversionOptions::setOptions( const QMap<QString,QString>& opts )
{
  if (wdg == 0)
    return;

  bool ok;
  // Set PostScript Language Level, taking 2 as the default
  int psLevel = opts["kde-kdjvu-pslevel"].toInt(&ok);

  if (ok && psLevel >= 1 && psLevel <= 3)
  {
    wdg->psLevel->setCurrentItem(psLevel - 1);
  }
  else
  {
    wdg->psLevel->setCurrentItem(1); // PostScript Level 2
  }

  // Set render mode, taking "color" as default
  QString op = opts["kde-kdjvu-rendermode"];
  if (op == "black-and-white")
  {
    wdg->renderMode->setCurrentItem(1);
  }
  else
  {
    if (op == "foreground")
      wdg->renderMode->setCurrentItem(2);
    else
    {
      if (op == "background")
        wdg->renderMode->setCurrentItem(3);
      else
        wdg->renderMode->setCurrentItem(0);
    }
  }
}


bool KPrintDialogPage_DJVUConversionOptions::isValid( QString& )
{
  return true;
}
