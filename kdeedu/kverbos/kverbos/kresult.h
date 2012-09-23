/***************************************************************************
                          kresult.h  -  description
                             -------------------
    begin                : Sat Jan 5 2002
    copyright            : (C) 2002 by Arnold Kraschinski
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

#ifndef KRESULT_H
#define KRESULT_H

#include "qresult.h"
#include "kverbosuser.h"

/**In a dialog window the number of training sessions and the results of the
  *latest 10 sessions in progress bars are displayed.
  *@author Arnold Kraschinski
  */

class KResult : public QResult  {
public: 
	KResult(KVerbosUser* pU, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );	
	~KResult();
public slots:	
  /** Wird durch den OK-Button aufgerufen und beendet den Dialog.
   */
  virtual void slotDlgEnde();
};

#endif
