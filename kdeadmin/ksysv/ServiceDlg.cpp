/***************************************************************************
    begin                : Sun Oct 3 1999
    copyright            : (C) 1999 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qcombobox.h>
#include <qstring.h>

#include <kapplication.h>
#include <klocale.h>
#include <kbuttonbox.h>

#include "ksvdraglist.h"
#include "ServiceDlg.h"

#define MIN_SIZE(A) A->setMinimumSize(A->sizeHint())

ServiceDlg::ServiceDlg (const QString& action, const QString& label,
						QWidget* parent, const char* name)
  : KDialogBase (parent, name, false, action, Apply|Close, Apply, true)
{
  QWidget* page = new QWidget (this);

  QBoxLayout* top = new QVBoxLayout (page, 0, spacingHint());

  mServices = new QComboBox (false, page);
  QLabel* desc = new QLabel(label, page);
  MIN_SIZE(desc);
  desc->setBuddy(mServices);
  MIN_SIZE(mServices);
  mServices->setMinimumWidth(mServices->minimumSize().width() * 2);

  QBoxLayout* serv_layout = new QHBoxLayout();
  top->addLayout (serv_layout);
  serv_layout->addWidget(desc);
  serv_layout->addWidget(mServices);
  
  setFixedSize (sizeHint());
}

ServiceDlg::~ServiceDlg()
{
}

void ServiceDlg::slotApply()
{
  emit doAction (mMapServices[mServices->currentText()]->filenameAndPath());
}

int ServiceDlg::count() const
{
  return mServices->count();
}

void ServiceDlg::resetChooser(KSVDragList* list, bool edit)
{
  mServices->clear();
  mMapServices.clear();

  if (!list)
    return;

  // initialize the combobox
  for (QListViewItemIterator it (list); 
	   it.current();
	   ++it)
    {
      const KSVItem* item = static_cast<KSVItem*> (it.current());
	  
      QFileInfo info (item->filenameAndPath());
      
      if (edit)
		{
		  if (info.isReadable())
			mServices->insertItem(item->label());
		  
		  mMapServices[item->label()] = item;
		}
      else
		{
		  if (info.isExecutable())
			mServices->insertItem(item->label());

		  mMapServices[item->label()] = item;
		}
    }
}

void ServiceDlg::show ()
{
  QDialog::show ();

  emit display (true);
}

void ServiceDlg::hide ()
{
  QDialog::hide ();

  emit display (false);
}

void ServiceDlg::toggle ()
{
  if (isHidden())
    show();
  else
    hide();
}

#include "ServiceDlg.moc"
