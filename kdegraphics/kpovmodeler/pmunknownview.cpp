/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2003 by Andreas Zehender
    email                : zehender@kde.org
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/

#include "pmunknownview.h"
#include <klocale.h>
#include <qlayout.h>

PMUnknownView::PMUnknownView( const QString& viewType,
                              QWidget* parent, const char* name )
      : PMViewBase( parent, name )
{
   QHBoxLayout* hl = new QHBoxLayout( this );
   QLabel* l;
   l = new QLabel( i18n( "Unknown view type \"%1\"" ).arg( viewType ), this );
   l->setAlignment( Qt::AlignCenter );
   hl->addWidget( l );
   m_viewType = viewType;
}

QString PMUnknownView::description( ) const
{
   return i18n( "Unknown" );
}
