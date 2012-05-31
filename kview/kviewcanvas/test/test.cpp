/*
 * kimageviewertest.cpp
 *
 * Copyright (c) 2001 Matthias Kretz <kretz@kde.org>
 */
#include "test.h"
#include <kimageviewer/canvas.h>

#include <kurl.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <kparts/componentfactory.h>

#include <qimage.h>

KImageViewerTest::KImageViewerTest()
    : KParts::MainWindow( 0L, "KImageViewerTest" )
{
	QWidget * widget = KParts::ComponentFactory::createInstanceFromQuery<QWidget>(
			"KImageViewer/Canvas", QString::null, this );
    if( widget )
    {
		m_part = dynamic_cast<KImageViewer::Canvas *>( widget );
		setCentralWidget( widget );
    }
    else
    {
        KMessageBox::error(this, "Could not find our Part!");
        kapp->quit();
    }
}

KImageViewerTest::~KImageViewerTest()
{
}

void KImageViewerTest::load(const KURL& url)
{
	QImage image( url.fileName() );
	if( m_part )
		m_part->setImage( image );
	else
        KMessageBox::error(this, "can't load the file");
}

#include "test.moc"
