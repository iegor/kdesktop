/***************************************************************************
                          kverbosoptions.h  -  description
                             -------------------
    begin                : Sat Dec 15 2001
    copyright            : (C) 2001 by Arnold Kraschinski
    email                : arnold.k67@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVERBOSOPTIONS_H
#define KVERBOSOPTIONS_H

#include "qverbosoptions.h"

/** Eine Klasse des KVerbos Projektes. Dadurch wird der Optionendialog bereitgestellt.
  * In diesem Dialog können die Zeitstufen, die gelernt werden sollen ausgewählt werden.
  *@author Arnold Kraschinski
  */

class KVerbosOptions : public QVerbosOptions  {
public:
  KVerbosOptions( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~KVerbosOptions();

public slots:
  /** Wird durch den OK-Button aufgerufen und beendet den Dialog. Änderungen
   *  werden übernommen.
   */
  virtual void slotDlgEnde();
  /** Wird durch den Cancel-Button aufgerufen und beendet den Dialog ohne
   *  Änderungen zu übernehmen.
   */
  virtual void slotCancel();	
};

#endif
