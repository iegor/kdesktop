/***************************************************************************
                          keduca.cpp  -  description
                             -------------------
    begin                : mié may 23 20:36:15 CEST 2001
    copyright            : (C) 2001 by Javier Campos
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "keduca.h"
#include "keduca.moc"
#include "keduca_part.h"
#include "../libkeduca/kgallerydialog.h"

#include <kapplication.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kdebug.h>

#include <kparts/componentfactory.h>

#include <qlabel.h>

Keduca::Keduca( QWidget* parent, const char *name, WFlags f )
	: KParts::MainWindow( parent, name, f ), _part( 0 )
{
	setXMLFile( "keducaui.rc" );
	
	_part = KParts::ComponentFactory::createPartInstanceFromLibrary< KParts::ReadOnlyPart >
	        ( "libkeducapart", this, name, this, "KEducaPart" );
	
	if ( _part )
	{
		setCentralWidget( _part->widget() );
		
		setupActions();
		
		configRead();
		
		createGUI( _part );
	}
	else
	{
		kdFatal() << "The library \"libkeducapart\" could not be found. Aborting." << endl;
	}
	if (!initialGeometrySet())
		resize( QSize(550, 450).expandedTo(minimumSizeHint()));
}

Keduca::~Keduca()
{
}


void Keduca::setupActions()
{
	KStdAction::open( this, SLOT( slotFileOpen() ), actionCollection() );
	(void)new KAction(  i18n( "Open &Gallery..." ), 0, 0, this, SLOT( slotGallery() ),
actionCollection(), "open_gallery" );
	_recentFiles = KStdAction::openRecent(this, SLOT ( slotFileOpenURL(const KURL &) ),
actionCollection());
	KStdAction::quit(kapp, SLOT( quit() ), actionCollection());
}

void Keduca::slotFileOpen()
{
	KURL url = KFileDialog::getOpenURL( QString::null, "application/x-edu", this,  i18n("Open Educa File") );
	if( !url.isEmpty() )
		slotFileOpenURL( url );
}

void Keduca::slotFileOpenURL( const KURL &url )
{
	if( !_part->openURL(url) ) return; 
	_recentFiles->addURL( url );
}

/*void Keduca::slotFileClose()
{
	init();
}*/

bool Keduca::queryClose()
{
	configWrite();
	return( TRUE );
}

void Keduca::configRead()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "keduca" );
	applyMainWindowSettings( config, "keduca" );
	_recentFiles->loadEntries(config);
}

void Keduca::configWrite()
{
	KConfig *config = KGlobal::config();
	
	config->setGroup( "keduca" );
	saveMainWindowSettings( config, "keduca" );
	_recentFiles->saveEntries(config);
	config->sync();
}

void Keduca::slotGallery()
{
  	initGallery();
}

void Keduca::initGallery(const KURL &urlFile)
{
	KGalleryDialog galleries;
	
	if( !urlFile.url().isEmpty() )
	galleries.putURL( urlFile );
	
	if( galleries.exec() == QDialog::Accepted )
	slotFileOpenURL( galleries.getURL() );
}
