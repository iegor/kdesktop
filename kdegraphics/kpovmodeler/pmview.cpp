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


#include <qprinter.h>
#include <qpainter.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <kconfig.h>
#include <kdialog.h>
#include <klocale.h>

#include <stdio.h>

// application specific includes
#include "pmview.h"
#include "pmpart.h"
#include "pmtreeview.h"
#include "pmdialogview.h"
#include "pmglview.h"

PMView::PMView( PMPart* part, QWidget* parent, const char* name )
      : QWidget( parent, name )
{
   setBackgroundMode( PaletteBase );

   QVBoxLayout* layout = new QVBoxLayout( this );
   m_pMainSplitter = new QSplitter( Qt::Horizontal, this, "MainSplitter" );
   m_pTreeEditSplitter = new QSplitter( Qt::Vertical, m_pMainSplitter,
                                        "TreeEditSplitter" );

   
   m_pTreeView = new PMTreeView( part, m_pTreeEditSplitter, "TreeView" );
   m_pTreeView->show( );
   
   m_pDialogView = new PMDialogView( part, m_pTreeEditSplitter, "EditView" );
   m_pDialogView->show( );

   
   QWidget* glArea = new QWidget( m_pMainSplitter, "GLArea" );
   glArea->show( );
   PMGLView* gl;
   
   QGridLayout* topLayout = new QGridLayout( glArea, 2, 2, 1, 1 );
   gl = new PMGLView( part, PMGLView::PMViewPosZ, glArea );
   topLayout->addWidget( gl, 0, 0 );
   gl = new PMGLView( part, PMGLView::PMViewPosX, glArea );
   topLayout->addWidget( gl, 0, 1 );
   gl = new PMGLView( part, PMGLView::PMViewNegY, glArea );
   topLayout->addWidget( gl, 1, 0 );
   gl = new PMGLView( part, PMGLView::PMViewCamera, glArea );
   topLayout->addWidget( gl, 1, 1 );
   
   m_pMainSplitter->show( );

   layout->addWidget( m_pMainSplitter );
   layout->activate( );

   m_pPart = part;
}

PMView::~PMView( )
{
}

void PMView::print( QPrinter* pPrinter )
{
   QPainter printpainter;
   printpainter.begin( pPrinter );
	
   // TODO: add your printing code here

   printpainter.end( );
}

void PMView::saveConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );

   cfg->writeEntry( "MainSplitter", m_pMainSplitter->sizes( ) );
   cfg->writeEntry( "TreeEditSplitter", m_pTreeEditSplitter->sizes( ) );
}

void PMView::restoreConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );

   m_pMainSplitter->setSizes( cfg->readIntListEntry( "MainSplitter" ) );
   m_pTreeEditSplitter->setSizes( cfg->readIntListEntry( "TreeEditSplitter" ) );
}


#include "pmview.moc"
