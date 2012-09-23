//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2004 by Luis Carvalho
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


#ifndef PMPARTIFACE_H
#define PMPARTIFACE_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dcopobject.h>

#include "pmvariant.h"

/**
 * DCOP Interface for kpovmodeler
 */
class PMPartIface : virtual public DCOPObject
{
   K_DCOP

k_dcop:
   /**
    * deletes the document's contents
    */
   virtual void deleteContents( ) = 0;
   /**
    * initializes the document generally
    */
   virtual bool newDocument( ) = 0;
   /**
    * closes the actual document
    */
   virtual void closeDocument( ) = 0;
   /**
    * Inserts a new PMObject of type type
    */
   virtual void slotNewObject( const QString& type, const QString& pos ) = 0;
   /**
    * initializes the documents contents
    */
   virtual void initDocument( ) = 0;
   /**
    * clears the selection
    */
   virtual void clearSelection( ) = 0;
   /**
    * puts the marked text/object into the clipboard and removes the objects
    */
   virtual void slotEditCut( ) = 0;
   /**
    * removes the selected object
    */
   virtual void slotEditDelete( ) = 0;
   /**
    * puts the marked text/object into the clipboard
    */
   virtual void slotEditCopy( ) = 0;
   /**
    * paste the clipboard into the document
    */
   virtual void slotEditPaste( ) = 0;
   /**
    * render the current scene
    */
   virtual void slotRender( ) = 0;
   /**
    * returns the currently selected object
    */
   virtual QString activeObjectName( ) = 0;
   /**
    * set the current selected object.
    * returns true if successful or false otherwise
    */
   virtual bool setActiveObject( const QString& name ) = 0;
   /**
    * Get known properties of the currently active object
    */
   virtual QStringList getProperties( ) = 0;
   /**
    * set a property on the currently active object
    */
   virtual bool setProperty( const QString& property, const PMVariant& value ) = 0;
   /**
    * set a property on the currently active object
    */
   virtual bool setProperty( const QString& property, const QString& value ) = 0;
   /**
    * Get the value of the given property
    */
   virtual const PMVariant getProperty( const QString& property ) = 0;
   /**
    * Get the value of the given property
    */
   virtual const QString getPropertyStr( const QString& property ) = 0;
   /**
    * Get a list of known object types.
    */
   virtual QStringList getObjectTypes( ) = 0;
};

#endif
