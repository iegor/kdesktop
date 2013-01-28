/***************************************************************************
                          dubview.h  -  description
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

#ifndef DUBVIEW_H
#define DUBVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for Qt
#include <qwidget.h>

#include <fileselectorwidget.h>

// the viewing widget for the playlist
class DubView : public FileSelectorWidget
{
 Q_OBJECT
 public:
 /** Constructor for the main view */
 DubView(QWidget *parent = 0);
 /** Destructor for the main view */
 ~DubView();


  /** contains the implementation for printing functionality */
  void print(QPrinter *pPrinter);
  /** Return item list of view */
  QPtrList<KFileItem>& items();
  /** select a particular file */
  void selectFile(KFileItem* file);

 private:

};

#endif // DUBVIEW_H
