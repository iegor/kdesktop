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

#ifndef PMPLUGINMANAGER_H
#define PMPLUGINMANAGER_H

#include <kstaticdeleter.h>

#include <qstring.h>
#include <qptrlist.h>

class PMPart;

/**
 * Plugin info for one plugin
 */
class PMPluginInfo
{
public:
   /**
    * Default constructor
    */
   PMPluginInfo( const QString& name, const QString& description,
                 bool enabled )
   {
      m_name = name;
      m_description = description;
      m_enabled = enabled;
   }
   /**
    * Returns the plugin name
    */
   QString name( ) const { return m_name; }
   /**
    * Returns the plugin description (i18n'ed)
    */
   QString description( ) const { return m_description; }
   /**
    * Returns true if the plugin is enabled
    */
   bool enabled( ) const { return m_enabled; }
   /**
    * Enables/disables the plugin
    */
   void enable( bool en ) { m_enabled = en; }
private:
   QString m_name, m_description;
   bool m_enabled;
};

/**
 * Manager class for plugins.
 *
 * Stores a list of available plugins and loads and unloads plugins.
 */
class PMPluginManager
{
public:
   /**
    * Destructor
    */
   ~PMPluginManager( );
   /**
    * Returns the instance (singleton)
    */
   static PMPluginManager* theManager( );

   /**
    * Registers a part instance and loads the plugins for that part
    */
   void registerPart( PMPart* p );
   /**
    * Removes a part instance
    */
   void removePart( PMPart* p );

   /**
    * Returns a list of available plugins
    */
   QPtrList<PMPluginInfo> plugins( ) const { return m_plugins; }
   /**
    * Loads and unloads plugins for all parts when plugins were activated or
    * deactivated
    */
   void updatePlugins( );
private:
   /**
    * Constructor
    */
   PMPluginManager( );

   QPtrList<PMPluginInfo> m_plugins;
   QPtrList<PMPart> m_parts;

   static PMPluginManager* s_pInstance;
   static KStaticDeleter<PMPluginManager> s_staticDeleter;
};

#endif
