/*
    This file is part of libkcal.
    
    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>
    Copyright (c) 2002 Jan-Pascal van Best <janpascal@vanbest.org>

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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KCAL_RESOURCELOCALDIRCONFIG_H
#define KCAL_RESOURCELOCALDIRCONFIG_H

#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <kurlrequester.h>

#include <kresources/resource.h>
#include <kresources/configwidget.h>

#include "libkcal_export.h"

namespace KCal {

/**
  \internal
  
  Configuration widget for local dir resource.
  
  @see ResourceLocalDir
*/
class LIBKCAL_EXPORT ResourceLocalDirConfig : public KRES::ConfigWidget
{ 
    Q_OBJECT
  public:
    ResourceLocalDirConfig( QWidget *parent = 0, const char *name = 0 );

  public slots:
    virtual void loadSettings( KRES::Resource *resource);
    virtual void saveSettings( KRES::Resource *resource );

  private:
    KURLRequester *mURL;
    QButtonGroup *formatGroup;
    QRadioButton *icalButton;
    QRadioButton *vcalButton;

    class Private;
    Private *d;
};

}

#endif
