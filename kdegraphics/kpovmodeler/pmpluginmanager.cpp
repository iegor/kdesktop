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

#include "pmpluginmanager.h"
#include "pmpart.h"
#include "pmshell.h"
#include "pmfactory.h"
#include "pmdebug.h"

#include <qmap.h>
#include <qapplication.h>

#include <kparts/plugin.h>
#include <kxmlguifactory.h>
#include <kconfig.h>

PMPluginManager* PMPluginManager::s_pInstance = 0;
KStaticDeleter<PMPluginManager> PMPluginManager::s_staticDeleter;

using namespace KParts;

// workaround for protected Plugin::pluginInfos
class PMPluginWorkaround : public Plugin
{
public:
   PMPluginWorkaround( ) : Plugin( 0, 0 ) { };
   static QValueList<Plugin::PluginInfo> installedPlugins( const KInstance* instance )
   {
      return pluginInfos( instance );
   }
};

PMPluginManager::PMPluginManager( )
{
   // find installed plugins
   KConfigGroup cfgGroup( PMFactory::instance( )->config( ),
                          "KParts Plugins" );
   QValueList<Plugin::PluginInfo> plugins
      = PMPluginWorkaround::installedPlugins( PMFactory::instance( ) );
   QValueList<Plugin::PluginInfo>::ConstIterator pIt = plugins.begin( );
   QValueList<Plugin::PluginInfo>::ConstIterator pEnd = plugins.end( );
   
   for( ; pIt != pEnd; ++pIt )
   {
      QDomElement docElem = ( *pIt ).m_document.documentElement( );
      QString name = docElem.attribute( "name" );
      QString description = docElem.attribute( "description" );
      if( !description.isEmpty( ) )
         description = i18n( description.latin1( ) );
      bool enabled = cfgGroup.readBoolEntry( name + "Enabled", false );
      m_plugins.append( new PMPluginInfo( name, description, enabled ) );
   }
}

PMPluginManager::~PMPluginManager( )
{
   m_plugins.setAutoDelete( true );
   m_plugins.clear( );
}

PMPluginManager* PMPluginManager::theManager( )
{
   if( !s_pInstance )
      s_staticDeleter.setObject( s_pInstance, new PMPluginManager( ) );
   return s_pInstance;
}

void PMPluginManager::registerPart( PMPart* p )
{
   if( !m_parts.containsRef( p ) )
   {
      m_parts.append( p );
      Plugin::loadPlugins( p, p, PMFactory::instance( ), false );
   }
}

void PMPluginManager::removePart( PMPart* p )
{
   m_parts.removeRef( p );
}

void PMPluginManager::updatePlugins( )
{
   KConfigGroup cfgGroup( PMFactory::instance( )->config( ),
                          "KParts Plugins" );
   QPtrListIterator<PMPluginInfo> pit( m_plugins );
   for( ; *pit; ++pit )
      cfgGroup.writeEntry( ( *pit )->name( ) + "Enabled",
                           ( *pit )->enabled( ) );
   cfgGroup.sync( );

   QPtrListIterator<PMPart> it( m_parts );
   for( ; *it; ++it )
   {
      Plugin::loadPlugins( *it, *it, PMFactory::instance( ), false );
      PMShell* shell = ( *it )->shell( );
      if( shell )
         shell->updateGUI( );
      // TODO find a solution to update the gui without using the shell
   }
}
