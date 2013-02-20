/*
    This file is part of KBugBuster.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <typeinfo>

#include <qlabel.h>
#include <qlayout.h>

#include <klineedit.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kdialog.h>

#include "kcalresource.h"
#include "resourceprefs.h"
#include "kcalresourceconfig.h"

KCalResourceConfig::KCalResourceConfig( QWidget* parent,  const char* name )
    : KRES::ConfigWidget( parent, name )
{
  resize( 245, 115 ); 

  QGridLayout *mainLayout = new QGridLayout( this, 2, 2 );
  mainLayout->setSpacing( KDialog::spacingHint() );

  QLabel *label = new QLabel( i18n("Server:"), this );
  mainLayout->addWidget( label, 0, 0 );

  mServerEdit = new KLineEdit( this );
  mainLayout->addWidget( mServerEdit, 0, 1 );


  label = new QLabel( i18n("Product:"), this );
  mainLayout->addWidget( label, 1, 0 );

  mProductEdit = new KLineEdit( this );
  mainLayout->addWidget( mProductEdit, 1, 1 );


  label = new QLabel( i18n("Component:"), this );
  mainLayout->addWidget( label, 2, 0 );

  mComponentEdit = new KLineEdit( this );
  mainLayout->addWidget( mComponentEdit, 2, 1 );
}

void KCalResourceConfig::loadSettings( KRES::Resource *resource )
{
  KCalResource *res = static_cast<KCalResource *>( resource );
  if ( res ) {
    KBB::ResourcePrefs *p = res->prefs();
    mServerEdit->setText( p->server() );
    mProductEdit->setText( p->product() );
    mComponentEdit->setText( p->component() );
  } else {
    kdError(5700) << "KCalResourceConfig::loadSettings(): no KCalResource, cast failed" << endl;
  }
}

void KCalResourceConfig::saveSettings( KRES::Resource *resource )
{
  KCalResource *res = static_cast<KCalResource*>( resource );
  if ( res ) {
    KBB::ResourcePrefs *p = res->prefs();
    p->setServer( mServerEdit->text() );
    p->setProduct( mProductEdit->text() );
    p->setComponent( mComponentEdit->text() );
  } else {
    kdError(5700) << "KCalResourceConfig::saveSettings(): no KCalResource, cast failed" << endl;
  }
}

#include "kcalresourceconfig.moc"
