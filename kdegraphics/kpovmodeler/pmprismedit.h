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


#ifndef PMPRISMEDIT_H
#define PMPRISMEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmsolidobjectedit.h"
#include "pmvectoredit.h"
#include <qptrlist.h>
#include <qvaluelist.h>


class PMPrism;
class PMFloatEdit;
class PMVectorListEdit;
class QVBoxLayout;
class QComboBox;
class QCheckBox;
class QPushButton;
class QLabel;

/**
 * Dialog edit class for @ref PMPrism
 */
class PMPrismEdit : public PMSolidObjectEdit
{
   Q_OBJECT
   typedef PMSolidObjectEdit Base;
public:
   /**
    * Creates a PMPrismEdit with parent and name
    */
   PMPrismEdit( QWidget* parent, const char* name = 0 );
   /**
    * Destructor
    */
   virtual ~PMPrismEdit( );
   /** */
   virtual void displayObject( PMObject* o );
   /** */
   void updateControlPointSelection( );

   /** */
   virtual bool isDataValid( );
   
protected:
   /** */
   virtual void createTopWidgets( );
   /** */
   virtual void createBottomWidgets( );
   /** */
   virtual void saveContents( );

private:
   /**
    * Displays the spline points
    */
   void displayPoints( const QValueList< QValueList<PMVector> >& list );
   /**
    * Returns the spline points from the vector edits
    */
   QValueList< QValueList<PMVector> > splinePoints( );
   /**
    * Deletes the spline point edits
    */
   void deleteEdits( );
   /**
    * Creates the edits for the points
    */
   void createEdits( const QValueList< QValueList<PMVector> >& points );

protected slots:
   void slotTypeChanged( int );
   void slotSweepChanged( int );
   void slotAddSubPrism( );
   void slotRemoveSubPrism( );
   void slotAddPointAbove( );
   void slotAddPointBelow( );
   void slotRemovePoint( );
   void slotSelectionChanged( );
   
private:   
   PMPrism* m_pDisplayedObject;
   QPtrList< QLabel > m_labels;
   QPtrList< QPushButton > m_subPrismAddButtons;
   QPtrList< QPushButton > m_subPrismRemoveButtons;
   QPtrList< QPushButton > m_addAboveButtons;
   QPtrList< QPushButton > m_addBelowButtons;
   QPtrList< QPushButton > m_removeButtons;
   QPtrList< PMVectorListEdit> m_points;
   QWidget* m_pEditWidget;
   QComboBox* m_pSplineType;
   QComboBox* m_pSweepType;
   QCheckBox* m_pSturm;
   QCheckBox* m_pOpen;
   PMFloatEdit* m_pHeight1;
   PMFloatEdit* m_pHeight2;
   int m_lastSplineType;
};


#endif
