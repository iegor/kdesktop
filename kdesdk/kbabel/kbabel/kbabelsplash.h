/* 
 *
 * $Id: kbabelsplash.h 389288 2005-02-14 07:03:37Z nanulo $
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *		 2003 Stanislav Visnovsky <visnovsky@kde.org>
 *
 * This file is part of the KBabel project. It's taken from the K3b project, 
 * Copyright (C) 1998-2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.

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

 */


#ifndef KBABELSPLASH_H
#define KBABELSPLASH_H

#include <qvbox.h>

class QLabel;
class QMouseEvent;
class QPaintEvent;
class QString;


class KBabelSplash : public QVBox
{
Q_OBJECT

 public:
  KBabelSplash( QWidget* parent = 0, const char* name = 0 );
  ~KBabelSplash() { instance = 0; }
  
  static KBabelSplash* instance;

 protected:
  void mousePressEvent( QMouseEvent* );
};

#endif
