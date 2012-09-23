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


#ifndef PMBOUNDEDBYEDIT_H
#define PMBOUNDEDBYEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmdialogeditbase.h"

class PMBoundedBy;
class QLabel;

/**
 * Dialog edit class for @ref PMBoundedBy
 */
class PMBoundedByEdit : public PMDialogEditBase
{
   Q_OBJECT
   typedef PMDialogEditBase Base;
public:
   /**
    * Creates a PMBoundedByEdit with parent and name
    */
   PMBoundedByEdit( QWidget* parent, const char* name = 0 );

   /** */
   virtual void displayObject( PMObject* o );

protected:
   /** */
   virtual void createTopWidgets( );
   
private:
   PMBoundedBy* m_pDisplayedObject;
   QLabel* m_pChildLabel;
   QLabel* m_pClippedByLabel;
};


#endif
