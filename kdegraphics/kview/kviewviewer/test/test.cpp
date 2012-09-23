/*
 * Copyright (c) 2001 Matthias Kretz <kretz@kde.org>
 */

#include "test.h"
#include <kimageviewer/viewer.h>

#include <kurl.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <kparts/componentfactory.h>
#include <kapplication.h>

#include <qimage.h>

Test::Test()
    : KParts::MainWindow( 0, "KView Viewer Test" ),
	m_part( 0 )
{
	m_part = KParts::ComponentFactory::createPartInstanceFromQuery<KParts::ReadWritePart>(
			"image/jpeg", "Name == 'Image Viewer Part'", this, 0, this );
    if( m_part )
	{
		setCentralWidget( m_part->widget() );
		createGUI( m_part );
	}
    else
    {
        KMessageBox::error(this, "Could not find our Part!");
        kapp->quit();
    }
}

Test::~Test()
{
}

void Test::load(const KURL& url)
{
	m_part->openURL( url );
}

#include "test.moc"
