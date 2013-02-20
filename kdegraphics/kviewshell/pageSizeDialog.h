// -*- C++ -*-
/*
 *   pageSizeDialog, for kviewshell
 *   This file: Copyright (C) 2002-2003 Stefan Kebekus, kebekus@kde.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef _PAGESIZE_DIALOG_H_
#define _PAGESIZE_DIALOG_H_

#include <kdialogbase.h>

class pageSizeWidget;
class pageSize;

class pageSizeDialog : public KDialogBase
{
  Q_OBJECT
    
public:
  // Constructs the page size Dialog. The pointer userPrefdPageSize
  // points to a pageSize object which will be set the the chosen
  // value whenever the user clicks on 'accept' or 'ok'. Programmers
  // can then connect to the signal sizeChanged() of the object to be
  // informed about the changes.
  pageSizeDialog( QWidget *parent=0, pageSize *userPrefdPageSize=0, const char *name=0, bool modal=true);
  void setPageSize(const QString&);

protected slots:
  virtual void slotOk();
  virtual void slotApply();
  
 private:
  pageSizeWidget *pageSizeW;
  pageSize       *userPreferredPageSize;
};


#endif
