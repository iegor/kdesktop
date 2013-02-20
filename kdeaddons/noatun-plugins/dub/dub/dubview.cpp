/***************************************************************************
                          dubview.cpp  -  description
                             -------------------
    begin                : Tue Oct 23 01:44:51 EEST 2001
    copyright            : (C) 2001 by Eray Ozkural (exa)
    email                : erayo@cs.bilkent.edu.tr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <qprinter.h>
#include <qpainter.h>
#include <kdebug.h>

// application specific includes
#include "dubview.h"
#include "dubview.moc"
#include "dub.h"
#include "fileselectorwidget.h"

//DubView::DubView(QWidget *parent, const char *name) : QWidget(parent, name)
//{
//  setBackgroundMode(PaletteBase);
//}

DubView::DubView(QWidget *parent) : FileSelectorWidget(parent)
{
  kdDebug(90010) << "dub view cons" << endl;
  setBackgroundMode(PaletteBase);
}

DubView::~DubView()
{
}

void DubView::print(QPrinter *pPrinter)
{
  QPainter printpainter;
  printpainter.begin(pPrinter);

  // TODO: add your printing code here

  printpainter.end();
}
/** Return item list of view */
QPtrList<KFileItem>& DubView::items()
{
  return const_cast<QPtrList<KFileItem>&>( *dirOperator()->view()->items() );
}

/** Select a file */
void DubView::selectFile(KFileItem* file){
  dirOperator()->view()->setCurrentItem( file );
  dirOperator()->view()->ensureItemVisible( file );
}
