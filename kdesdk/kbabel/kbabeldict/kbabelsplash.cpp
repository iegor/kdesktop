/* 
 *
 * $Id: kbabelsplash.cpp 389288 2005-02-14 07:03:37Z nanulo $
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *               2003 Stanislav Visnovsky <visnovsky@kde.org>
 *
 * This file is part of the KBabel project. It's taken from the K3b project,
 * Copyright (C) 1998-2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of this program with any edition of
 * the Qt library by Trolltech AS, Norway (or with modified versions
 * of Qt that use the same license as Qt), and distribute linked
 * combinations including the two.  You must obey the GNU General
 * Public License in all respects for all of the code used other than
 * Qt. If you modify this file, you may extend this exception to
 * your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from
 * your version.
 *
 */

#include "kbabelsplash.h"

#include <qapplication.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qstring.h>
#include <qfontmetrics.h>
#include <qpainter.h>

#include <kstandarddirs.h>

KBabelSplash* KBabelSplash::instance = 0;

KBabelSplash::KBabelSplash( QWidget* parent, const char* name )
  : QVBox( parent, name, WType_Dialog|WShowModal|WStyle_Customize|WStyle_NoBorder|WDestructiveClose )
{
  setMargin( 0 );
  setSpacing( 0 );

  QLabel* picLabel = new QLabel( this );
  QPixmap pixmap;
  if( pixmap.load( locate( "data", "kbabel/pics/splash.png" ) ) )
    picLabel->setPixmap( pixmap );

  picLabel->setFrameStyle(QFrame::WinPanel | QFrame::Raised);

  // Set geometry, with support for Xinerama systems
  QRect r;
  r.setSize(sizeHint());
  int ps = QApplication::desktop()->primaryScreen();
  r.moveCenter( QApplication::desktop()->screenGeometry(ps).center() );
  setGeometry(r);
  
  if( instance ) delete instance;
  instance = this;
}


void KBabelSplash::mousePressEvent( QMouseEvent* )
{
  close();
}

#include "kbabelsplash.moc"
