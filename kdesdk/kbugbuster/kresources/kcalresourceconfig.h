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
#ifndef KCALRESOURCECONFIG_H
#define KCALRESOURCECONFIG_H

#include <kresources/resource.h>
#include <kresources/configwidget.h>

class KLineEdit;

/**
  Configuration widget for remote resource.
  
  @see KCalResource
*/
class KCalResourceConfig : public KRES::ConfigWidget
{ 
    Q_OBJECT
  public:
    KCalResourceConfig( QWidget *parent = 0, const char *name = 0 );

  public slots:
    virtual void loadSettings( KRES::Resource *resource );
    virtual void saveSettings( KRES::Resource *resource );

  private:
    KLineEdit *mServerEdit;
    KLineEdit *mComponentEdit;
    KLineEdit *mProductEdit;

    class Private;
    Private *d;
};

#endif
