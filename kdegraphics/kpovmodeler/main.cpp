/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2001 by Andreas Zehender
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


#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapplication.h>

#include "pmshell.h"
#include "pmfactory.h"
#include "pmrendermanager.h"
#include "version.h"


static KCmdLineOptions options[] =
{
  { "+[file]", I18N_NOOP( "File to open" ), 0 },
  { "no-opengl", I18N_NOOP( "Disables OpenGL rendering" ), 0 },
  { "no-dri", I18N_NOOP( "Disables direct rendering" ), 0 },
  KCmdLineLastOption
};

int main( int argc, char* argv[] )
{
   PMShell* shell = 0;

   KCmdLineArgs::init( argc, argv, PMFactory::aboutData( ) );
   KCmdLineArgs::addCmdLineOptions( options );

   KApplication app;

   KCmdLineArgs* args = KCmdLineArgs::parsedArgs( );

   if( !args->isSet( "-opengl" ) )
      PMRenderManager::disableOpenGL( );
   if( !args->isSet( "-dri" ) )
      PMGLView::enableDirectRendering( false );

   if( args->count( ) > 0 )
   {
      for( int i = 0 ; i < args->count( ) ; i++ )
      {
         shell = new PMShell( args->url( i ) );
         shell->show( );
      }
   }
   else
   {
      shell = new PMShell;
      shell->show( );
   }
   args->clear();   
   return app.exec( );
}
