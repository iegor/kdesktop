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


#ifndef PMPIGMENTEDIT_H
#define PMPIGMENTEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmtexturebaseedit.h"

class PMPigment;
class QCheckBox;

/**
 * Dialog edit class for @ref PMPigment
 */
class PMPigmentEdit : public PMTextureBaseEdit
{
   Q_OBJECT
   typedef PMTextureBaseEdit Base;
public:
   /**
    * Creates a PMPigmentEdit with parent and name
    */
   PMPigmentEdit( QWidget* parent, const char* name = 0 );

   /** */
   virtual void displayObject( PMObject* o );

protected:
   /** */
   virtual void createTopWidgets( );
   /** */
   virtual void saveContents( );

private:
   PMPigment* m_pDisplayedObject;
   QCheckBox* m_pUVMapping;
};


#endif
