/***************************************************************************
                          kverbosoptions.cpp  -  description
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

#include "qlabel.h"
#include "qcheckbox.h"
#include "kverbosoptions.h"

KVerbosOptions::KVerbosOptions( QWidget* parent /*= 0*/, const char* name /*= 0*/, bool modal /*= FALSE*/, WFlags fl /*= 0*/ )
: QVerbosOptions(parent, name, modal, fl)
{
	// some times are disabled because they aren't ready yet.
	LabelSubPasado->setEnabled(false);
	LabelSubPresente->setEnabled(false);
	LabelSubFuturo->setEnabled(false);
	LabelImperativo->setEnabled(false);
	CheckBox1_6->setEnabled(false);
	CheckBox2_6->setEnabled(false);
	CheckBox3_6->setEnabled(false);
	CheckBox1_7->setEnabled(false);
	CheckBox2_7->setEnabled(false);
	CheckBox3_7->setEnabled(false);	
	CheckBox1_8->setEnabled(false);
	CheckBox2_8->setEnabled(false);
	CheckBox3_8->setEnabled(false);	
	CheckBox1_9->setEnabled(false);		
}

KVerbosOptions::~KVerbosOptions()
{
}



// Wird durch den OK-Button aufgerufen und beendet den Dialog. Änderungen
// werden übernommen.
void KVerbosOptions::slotDlgEnde()
{
  done(1);
}

// Wird durch den Cancel-Button aufgerufen und beendet den Dialog ohne
// Änderungen zu übernehmen.
void KVerbosOptions::slotCancel()
{
  done(0);
}
