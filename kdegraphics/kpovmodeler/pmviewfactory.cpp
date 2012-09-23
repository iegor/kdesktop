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

#include "pmviewfactory.h"
#include "pmglview.h"
#include "pmtreeview.h"
#include "pmdialogview.h"
#include "pmdebug.h"
#include "pmviewbase.h"
#include "pmglview.h"
#include "pmtreeview.h"
#include "pmdialogview.h"
#include "pmlibrarybrowser.h"

#include <klocale.h>

PMViewFactory* PMViewFactory::s_pInstance = 0;
KStaticDeleter<PMViewFactory> PMViewFactory::s_staticDeleter;

PMViewFactory::PMViewFactory( )
{
   m_viewTypes.setAutoDelete( true );
   addViewType( new PMTreeViewFactory( ) );
   addViewType( new PMDialogViewFactory( ) );
   addViewType( new PMGLViewFactory( ) );
   addViewType( new PMLibraryBrowserViewFactory( ) );
}

PMViewFactory::~PMViewFactory( )
{
   m_viewTypes.clear( );
}

PMViewFactory* PMViewFactory::theFactory( )
{
   if( !s_pInstance )
      s_staticDeleter.setObject( s_pInstance, new PMViewFactory( ) );
   return s_pInstance;
}

void PMViewFactory::addViewType( PMViewTypeFactory* vt )
{
   if( vt )
   {
      m_viewTypes.append( vt );
      m_dict.insert( vt->viewType( ), vt );
   }
}

PMViewBase* PMViewFactory::newViewInstance( const QString& viewType,
                                            QWidget* parent, PMPart* part ) const
{
   PMViewTypeFactory* f = m_dict.find( viewType );
   if( f )
      return f->newInstance( parent, part );

   kdError( PMArea ) << "Unknown view type \"" << viewType << "\"" << endl;
   return 0;
}

PMViewOptions* PMViewFactory::newOptionsInstance( const QString& viewType ) const
{
   PMViewTypeFactory* f = m_dict.find( viewType );
   if( f )
      return f->newOptionsInstance( );

   kdError( PMArea ) << "Unknown view type \"" << viewType << "\"" << endl;
   return 0;
}

PMViewTypeFactory* PMViewFactory::viewFactory( const QString& viewType ) const
{
   return m_dict.find( viewType );
}

const QPtrList<PMViewTypeFactory>& PMViewFactory::viewTypes( ) const
{
   return m_viewTypes;
}

