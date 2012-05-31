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

#ifndef KPRINTDIALOGPAGE_DJVUCONVERSIONOPTIONS_H
#define KPRINTDIALOGPAGE_DJVUCONVERSIONOPTIONS_H

#include <kdeprint/kprintdialogpage.h>


class kprintDialogPage_DJVUconversionoptions_basewidget;


// This is a fairly standard KPrintDialogPage that allows the user to
// chose page size & placement options: shrink oversized pages, and
// expand small pages

class KPrintDialogPage_DJVUConversionOptions : public KPrintDialogPage
{
 public:
  KPrintDialogPage_DJVUConversionOptions( QWidget *parent = 0, const char *name = 0 );
  
  void getOptions( QMap<QString,QString>& opts, bool incldef = false );
  void setOptions( const QMap<QString,QString>& opts );
  bool isValid( QString& msg );

  kprintDialogPage_DJVUconversionoptions_basewidget* wdg;

 private:
  QVBoxLayout* kprintDialogPage_pageoptions_baseLayout;
};


#endif // KPRINTDIALOGPAGE_PAGEOPTIONS_H
