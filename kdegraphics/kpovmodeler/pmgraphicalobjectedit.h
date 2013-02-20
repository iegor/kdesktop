//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2002 by Andreas Zehender
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


#ifndef PMGRAPHICALOBJECTEDIT_H
#define PMGRAPHICALOBJECTEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmdetailobjectedit.h"

class PMGraphicalObject;
class QCheckBox;
class QSpinBox;
class QLabel;

/**
 * Dialog edit class for @ref PMGraphicalObject.
 */
class PMGraphicalObjectEdit : public PMDetailObjectEdit
{
   Q_OBJECT
   typedef PMDetailObjectEdit Base;
public:
   /**
    * Creates a PMGraphicalObjectEdit with parent and name
    */
   PMGraphicalObjectEdit( QWidget* parent, const char* name = 0 );

   /** */
   virtual void displayObject( PMObject* o );

   /** */
   virtual bool isDataValid( );
protected:
   /** */
   virtual void createBottomWidgets( );
   /** */
   virtual void saveContents( );

protected slots:
   void slotRelativeChanged( );
   void slotLevelChanged( int );

private:
   void recalculateResultingVisibility( );

private:
   PMGraphicalObject* m_pDisplayedObject;
   QCheckBox* m_pNoShadowButton;
   QCheckBox* m_pNoImageButton;
   QCheckBox* m_pNoReflectionButton;
   QCheckBox* m_pDoubleIlluminateButton;
   QSpinBox* m_pVisibilityLevel;
   QCheckBox* m_pRelativeVisibility;
   QLabel* m_pResultingVisibility;
   QCheckBox* m_pExport;
};


#endif
