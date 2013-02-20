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

#include "pmcontrolpoint.h"
#include "pmdefaults.h"
#include "pmdebug.h"

#include <kconfig.h>

double PMControlPoint::s_moveGrid = c_defaultMoveGrid;
double PMControlPoint::s_scaleGrid = c_defaultScaleGrid;
double PMControlPoint::s_rotateGrid = c_defaultRotateGrid;

PMControlPoint::PMControlPoint( int id, const QString& description )
{
   m_id = id;
   m_bChanged = false;
   m_description = description;
   m_bSelected = false;
}

PMControlPoint::~PMControlPoint( )
{
}

void PMControlPoint::startChange( const PMVector& s, const PMVector& n )
{
   m_startPoint = s;
   m_normalVector = n;
   graphicalChangeStarted( );
}

void PMControlPoint::change( const PMVector& endPoint )
{
   m_bChanged = true;
   graphicalChange( m_startPoint, m_normalVector, endPoint );
}
   
bool PMControlPoint::changed( )
{
   bool h = m_bChanged;
   m_bChanged = false;

   return h;
}

void PMControlPoint::setMoveGrid( double d )
{
   if( d > 0 )
      s_moveGrid = d;
   else
      kdError( PMArea ) << "Grid has to be greater than 0\n";
}

void PMControlPoint::setScaleGrid( double d )
{
   if( d > 0 )
      s_scaleGrid = d;
   else
      kdError( PMArea ) << "Grid has to be greater than 0\n";
}

void PMControlPoint::setRotateGrid( double d )
{
   if( d > 0 )
      s_rotateGrid = d;
   else
      kdError( PMArea ) << "Grid has to be greater than 0\n";
}

void PMControlPoint::saveConfig( KConfig* cfg )
{
   cfg->setGroup( "Grid" );
   cfg->writeEntry( "MoveGrid", s_moveGrid );
   cfg->writeEntry( "RotateGrid", s_rotateGrid );
   cfg->writeEntry( "ScaleGrid", s_scaleGrid );
}

void PMControlPoint::restoreConfig( KConfig* cfg )
{
   cfg->setGroup( "Grid" );
   s_moveGrid = cfg->readDoubleNumEntry( "MoveGrid", c_defaultMoveGrid );
   s_rotateGrid = cfg->readDoubleNumEntry( "RotateGrid", c_defaultRotateGrid );
   s_scaleGrid = cfg->readDoubleNumEntry( "ScaleGrid", c_defaultScaleGrid );
}
