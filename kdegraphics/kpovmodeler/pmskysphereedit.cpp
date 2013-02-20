/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2001 by Luis Carvalho
    email                : lpassos@mail.telepac.pt
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#include "pmskysphereedit.h"
#include "pmskysphere.h"
#include "pmlinkedit.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>


PMSkySphereEdit::PMSkySphereEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMSkySphereEdit::displayObject( PMObject* o )
{
   if( o->isA( "SkySphere" ) )
   {
      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMSkySphereEdit: Can't display object\n";
}

#include "pmskysphereedit.moc"
