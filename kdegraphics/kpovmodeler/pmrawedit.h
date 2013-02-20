//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Andreas Zehender
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


#ifndef PMRAWEDIT_H
#define PMRAWEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmnamedobjectedit.h"

class PMRaw;
class QMultiLineEdit;

/**
 * Dialog edit class for @ref PMRaw.
 */
class PMRawEdit : public PMNamedObjectEdit
{
   Q_OBJECT
   typedef PMNamedObjectEdit Base;
public:
   /**
    * Creates a PMRawEdit with parent and name
    */
   PMRawEdit( QWidget* parent, const char* name = 0 );

   /** */
   virtual void displayObject( PMObject* o );

   /** */
   virtual bool isDataValid( );
protected:
   /** */
   virtual void createTopWidgets( );
   /** */
   virtual void saveContents( );

private:
   PMRaw* m_pDisplayedObject;
   QMultiLineEdit* m_pEdit;
};


#endif
