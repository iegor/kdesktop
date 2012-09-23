//-*-C++-*-
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


#ifndef PMVECTOREDIT_H
#define PMVECTOREDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include <qptrvector.h>
#include <qlineedit.h>
#include "pmvector.h"

/**
 * Edit widget for @ref PMVector
 */
class PMVectorEdit : public QWidget
{
   Q_OBJECT
public:
   /**
    * Creates an edit widget for 2D vectors.
    *
    * The labels shown are descriptionX and descriptionY
    */
   PMVectorEdit( const QString& descriptionX,
                 const QString& descriptionY,
                 QWidget* parent, const char* name = 0 );
   /**
    * Creates an edit widget for 3D vectors.
    *
    * The labels shown are descriptionX, descriptionY and descriptionZ
    */
   PMVectorEdit( const QString& descriptionX,
                 const QString& descriptionY, const QString& descriptionZ,
                 QWidget* parent, const char* name = 0 );
   /**
    * Creates an edit widget for 4D vectors.
    *
    * The labels shown are descriptionA, descriptionB, descriptionC
    * and description D
    */
   PMVectorEdit( const QString& descriptionA, const QString& descriptionB,
                 const QString& descriptionC, const QString& descriptionD,
                 QWidget* parent, const char* name = 0 );

   /**
    * Sets the displayed vector
    */
   void setVector( const PMVector& v, int precision = 5 );
   /**
    * Returns the displayed vector
    */
   PMVector vector( ) const;

   /**
    * Returns true if the text for each coordinate is a valid
    * float value. Otherwise an error message is shown.
    */
   bool isDataValid( );

   /**
    * Enables or disables read only mode
    */
   void setReadOnly( bool yes = true );
   /**
    * Reimplemented from QWidget
    */
   virtual void setEnabled( bool yes );
signals:
   /**
    * Emitted when one of the coordinates is changed
    */
   void dataChanged( );

protected slots:
   void slotTextChanged( const QString& );
private:
   QPtrVector<QLineEdit> m_edits;
};

#endif
