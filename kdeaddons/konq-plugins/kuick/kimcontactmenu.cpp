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

#include <qstringlist.h>

// The following enables kabc for contact name lookups instead of using Kopete's idea of their name.
//#define KIMCONTACTS_USE_KABC

#include <kimproxy.h>

#ifdef KIMCONTACTS_USE_KABC
#include <kabc/addressbook.h>
#include <kabc/stdaddressbook.h>
#endif

#include "kimcontactmenu.h"

KIMContactMenu::KIMContactMenu( QWidget *parent, KIMProxy *proxy ) 
  : QPopupMenu( parent), mProxy( proxy )
{
#ifdef KIMCONTACTS_USE_KABC
  m_addressBook = KABC::StdAddressBook::self( false );
#endif
  connect( this, SIGNAL( activated( int ) ), SLOT( slotItemActivated( int ) ) );
  connect( this, SIGNAL( aboutToShow( ) ), this, SLOT( slotAboutToShow( ) ) );
}

KIMContactMenu::~KIMContactMenu()
{
}

void KIMContactMenu::slotAboutToHide()
{
}

void KIMContactMenu::slotAboutToShow()
{
  // the pointer may be returning to the menu, if it's already populated, there's nothing to do
  if (count() >= 1) return;
  
  mContacts = mProxy->fileTransferContacts();
  
  int i = 0;
  
  for ( QStringList::Iterator it = mContacts.begin(); it != mContacts.end(); ++it, ++i )
  {
#ifdef KIMCONTACTS_USE_KABC
      insertItem( mProxy->presenceIcon( *it ), m_addressBook->findByUid( *it ).realName(), i );
#else            
      insertItem( mProxy->presenceIcon( *it ), mProxy->displayName( *it ), i );
#endif
  }
}

void KIMContactMenu::slotItemActivated( int item )
{
  // look up corresponding UID
  QString uid = mContacts[ item ];
  // emit signal
  emit contactChosen( uid );
}

#include "kimcontactmenu.moc"
