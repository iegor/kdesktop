/***************************************************************************

                   dialog page for adjectives (comparison)

    -----------------------------------------------------------------------

    begin          : Sat Dec 4 18:09:29 1999

    copyright      : (C) 1999-2001 Ewald Arnold <kvoctrain@ewald-arnold.de>
                     (C) 2001 The KDE-EDU team
                     (C) 2005 Peter Hedlund <peter.hedlund@kdemail.net>

    -----------------------------------------------------------------------

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#include "AdjEntryPage.h"
#include "EntryDlg.h"

#include <langset.h>

#include <qlineedit.h>


AdjEntryPage::AdjEntryPage(EntryDlg *_dlgbook, bool multi_sel, const Comparison &comp, QWidget *parent, const char *name)
  : AdjEntryPageForm( parent, name ), dlgbook(_dlgbook)
{
  comparisons = comp;

  connect( lev1Field, SIGNAL(textChanged(const QString&)), SLOT(lev1Changed(const QString&)) );
  connect( lev2Field, SIGNAL(textChanged(const QString&)), SLOT(lev2Changed(const QString&)) );
  connect( lev3Field, SIGNAL(textChanged(const QString&)), SLOT(lev3Changed(const QString&)) );

  setData(multi_sel, comp);
}


void AdjEntryPage::setData(bool multi_sel, const Comparison  &comp)
{
  if (multi_sel) {
    lev1Field->setEnabled(false);
    lev2Field->setEnabled(false);
    lev3Field->setEnabled(false);
  }
  else {
    lev1Field->setText (comp.l1());
    lev2Field->setText (comp.l2());
    lev3Field->setText (comp.l3());
  }
  setModified(false);
}


void AdjEntryPage::lev1Changed(const QString& s)
{
  setModified(true);
  comparisons.setL1 (s);
}


void AdjEntryPage::lev2Changed(const QString& s)
{
  setModified(true);
  comparisons.setL2 (s);
}


void AdjEntryPage::lev3Changed(const QString& s)
{
  setModified(true);
  comparisons.setL3 (s);
}


bool AdjEntryPage::isModified()
{
  return modified;
}


void AdjEntryPage::setEnabled(int enable)
{
  bool ena = enable == EntryDlg::EnableAll;

  lev1Field->setEnabled (ena);
  lev2Field->setEnabled (ena);
  lev3Field->setEnabled (ena);
}


void AdjEntryPage::setModified(bool mod)
{
  modified = mod;
  if (mod)
    emit sigModified();
}

#include "AdjEntryPage.moc"
