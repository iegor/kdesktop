// pageSizeDialog.cpp
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2002-2003 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#include <config.h>

#include <kapplication.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qhbox.h>



#include "pageSizeDialog.h"
#include "pageSizeWidget.h"


pageSizeDialog::pageSizeDialog( QWidget *parent, pageSize *userPrefdPageSize, const char *name, bool modal)
  :KDialogBase( parent, name, modal, i18n("Page Size"), Ok|Apply|Cancel, Ok, 
		true )
{
  userPreferredPageSize = userPrefdPageSize;
  pageSizeW = new pageSizeWidget(this, "PageSizeWidget");
  pageSizeW->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, 
					 pageSizeW->sizePolicy().hasHeightForWidth() ) );
  setMainWidget(pageSizeW);
}


void pageSizeDialog::slotOk()
{
  if (userPreferredPageSize != 0)
    *userPreferredPageSize = pageSizeW->pageSizeData();
  accept();
}


void pageSizeDialog::slotApply()
{
  if (userPreferredPageSize != 0)
    *userPreferredPageSize = pageSizeW->pageSizeData();
}


void pageSizeDialog::setPageSize(const QString& name)
{
  if (pageSizeW == 0)
    return;
  pageSizeW->setPageSize(name);
}


#include "pageSizeDialog.moc"
