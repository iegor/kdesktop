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


#ifndef PMSPLINEMEMENTO_H
#define PMSPLINEMEMENTO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmmemento.h"
#include "pmvector.h"
#include <qvaluelist.h>


/**
 * Memento for @ref PMLathe
 */
class PMSplineMemento : public PMMemento
{
public:
   /**
    * Creates a memento for the object originator
    */
   PMSplineMemento( PMObject* originator );
   /**
    * Deletes the memento
    */
   virtual ~PMSplineMemento( );

   /**
    * Saves the spline points
    */
   void setSplinePoints( const QValueList<PMVector>& v );
   /**
    * Returns the spline points
    */
   QValueList<PMVector> splinePoints( ) const;
   /**
    * Returns true if the spline points were saved
    */
   bool splinePointsSaved( ) const { return m_bSplinePointsSaved; }
   
private:   
   /**
    * The stored points
    */
   QValueList<PMVector> m_splinePoints;
   bool m_bSplinePointsSaved;
};

#endif
