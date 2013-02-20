//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Luis Passos Carvalho
    email                : lpassos@mail.telepac.pt
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#ifndef PMPALETTEVALUEMEMENTO_H
#define PMPALETTEVALUEMEMENTO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmmemento.h"
#include "pmpalettevalue.h"
#include <qvaluelist.h>


/**
 * Memento for @ref PMImageMap
 */
class PMPaletteValueMemento : public PMMemento
{
public:
   /**
    * Creates a memento for the object originator
    */
   PMPaletteValueMemento( PMObject* originator );
   /**
    * Deletes the memento
    */
   virtual ~PMPaletteValueMemento( );

   /**
    * Saves the filter palette values
    */
   void setFilterPaletteValues( const QValueList<PMPaletteValue>& v );
   /**
    * Returns the filter palette values
    */
   QValueList<PMPaletteValue> filterPaletteValues( ) const;
   /**
    * Returns true if the filter palette values were saved
    */
   bool filterPaletteValuesSaved( ) const { return m_bFilterPaletteValuesSaved; }
   /**
    * Saves the transmit palette values
    */
   void setTransmitPaletteValues( const QValueList<PMPaletteValue>& v );
   /**
    * Returns the transmit palette values
    */
   QValueList<PMPaletteValue> transmitPaletteValues( ) const;
   /**
    * Returns true if the transmit palette values were saved
    */
   bool transmitPaletteValuesSaved( ) const { return m_bTransmitPaletteValuesSaved; }

private:
   /**
    * The stored values for filter
    */
   QValueList<PMPaletteValue> m_filterPaletteValues;
   bool m_bFilterPaletteValuesSaved;
   /**
    * The stored values for transmit
    */
   QValueList<PMPaletteValue> m_transmitPaletteValues;
   bool m_bTransmitPaletteValuesSaved;
};

#endif
