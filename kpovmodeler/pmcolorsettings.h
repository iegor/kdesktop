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

#ifndef PMCOLORSETTINGS_H
#define PMCOLORSETTINGS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmsettingsdialog.h"

class KColorButton;

/**
 * Colors configuration dialog page
 */
class PMColorSettings : public PMSettingsDialogPage
{
   Q_OBJECT
public:
   /**
    * Default constructor
    */
   PMColorSettings( QWidget* parent, const char* name = 0 );
   /** */
   virtual void displaySettings( );
   /** */
   virtual bool validateData( );
   /** */
   virtual void applySettings( );
   /** */
   virtual void displayDefaults( );
   
private:
   KColorButton* m_pBackgroundColor;
   KColorButton* m_pGraphicalObjectsColor[2];
   KColorButton* m_pControlPointsColor[2];
   KColorButton* m_pAxesColor[3];
   KColorButton* m_pFieldOfViewColor;
};


#endif
