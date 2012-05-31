/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Luis Carvalho
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


#include "pmdensityedit.h"
#include "pmdensity.h"
#include "pmlinkedit.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>


PMDensityEdit::PMDensityEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMDensityEdit::displayObject( PMObject* o )
{
   if( o->isA( "Density" ) )
   {
      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMDensityEdit: Can't display object\n";
}

#include "pmdensityedit.moc"
