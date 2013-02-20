/* This file is part of the KDE project
   Copyright (C) 2004 Will Stephenson <lists@stevello.free-online.co.uk>
   
   kimcontactmenu.cpp - Menu allowing a choice from a list of instant
   messaging contacts

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; version 2
	 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __kim_contact_menu_h
#define __kim_contact_menu_h

#include <qpopupmenu.h>
#include <qstringlist.h>

class KIMProxy;
namespace KABC {
  class AddressBook;
}

class KIMContactMenu : public QPopupMenu
{
  Q_OBJECT
public:
  KIMContactMenu( QWidget *parent, KIMProxy *proxy );
  ~KIMContactMenu();
protected slots:
  // populate menus if not already populated
  void slotAboutToShow();
  void slotAboutToHide();
  void slotItemActivated( int item );
signals:
  void contactChosen( const QString &uid );

protected:
  KIMProxy *mProxy;
  QStringList mContacts;
  KABC::AddressBook* m_addressBook;
};

#endif
