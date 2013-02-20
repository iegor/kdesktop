//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2003 by Luis Carvalho
    email                : lpassos@oninetspeed.pt
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/

#ifndef PMLIBRARYMANAGER_H
#define PMLIBRARYMANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>
#include <qvaluelist.h>
#include <kstaticdeleter.h>

#include "pmlibraryhandle.h"

class KConfig;
class QDomElement;

/**
 * Class that maintains the list of available libraries for kpovmodeler
 *
 * This class is a singleton, wich means all libraries will be known to
 * all instances of the application.
 *
 * When the class is initialized, the following steps are taken:
 *    1. The global kpovmodeler library path is scanned for libraries
 *    2. The users' kpovmodeler library path is also scanned for libraries
 *
 * If more than one library has the same name, only the last one will be
 * accessible.
 */
class PMLibraryManager 
{

public:
   /**
    * Destructor
    */
   ~PMLibraryManager( );
   /**
    * Returns the manager instance (singleton)
    */
   static PMLibraryManager* theManager( );

   /**
    * Returns the list of available libraries
    */
   QValueList<QString> availableLibraries( );

   /**
    * Returns the handle for the indicated library
    */
   PMLibraryHandle* getLibraryHandle( const QString& libraryName );

   /**
    * Refreshes the list of libraries.
    * WARNING: This function invalidates all previously given PMLibraryHandle pointers
    */
   void refresh( );
   
   void saveConfig( KConfig* cfg );
   void restoreConfig( KConfig* cfg );

private:
   
   /**
    * Constructor
    */
   PMLibraryManager( );

   void scanLibraries( );

   QPtrList< PMLibraryHandle > m_libraries;

   static PMLibraryManager* s_pInstance;
   static KStaticDeleter<PMLibraryManager> s_staticDeleter;   
};

#endif
