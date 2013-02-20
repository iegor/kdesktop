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


#ifndef PMCOLOREDIT_H
#define PMCOLOREDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include "pmcolor.h"

class KColorButton;
class PMFloatEdit;
class QColor;

/**
 * Edit widget for @ref PMColor
 */
class PMColorEdit : public QWidget
{
   Q_OBJECT
public:
   /**
    * Creates an edit widget for rgbft colors.
    */
   PMColorEdit( bool filterAndTransmit, QWidget* parent, const char* name = 0 );
   
   /**
    * Sets the displayed color
    */
   void setColor( const PMColor& c );
   /**
    * Returns the color
    */
   PMColor color( ) const { return m_color; }

   /**
    * Returns true if the input is a valid
    * color value. Otherwise an error message is shown.
    */
   bool isDataValid( );

   /**
    * Enables or disables read only mode
    */
   void setReadOnly( bool yes = true );
signals:
   /**
    * Emitted when the color is changed
    */
   void dataChanged( );
   
protected slots:
   void slotColorChanged( const QColor& c );
   void slotEditChanged( );
private:
   void updateButton( );
private:
   PMFloatEdit* m_edits[5];
   KColorButton* m_pButton;
   bool m_bFilterAndTransmit;
   PMColor m_color;
};

#endif
