/*  This file is part of the KDE project
    Copyright (C) 2001-2003 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include "kview.h"
#include "version.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] = I18N_NOOP( "KDE Image Viewer" );

static KCmdLineOptions options[] =
{
	{ "+[URL]", I18N_NOOP( "Image to open" ), 0 },
	KCmdLineLastOption
};

extern "C" KDE_EXPORT int kdemain( int argc, char ** argv )
{
	KAboutData about( "kview", I18N_NOOP( "KView" ),
			KVIEW_VERSION, description,
			KAboutData::License_GPL,
			I18N_NOOP( "(c) 1997-2002, The KView Developers" ) );
	about.addAuthor( "Matthias Kretz", I18N_NOOP( "Maintainer" ), "kretz@kde.org" );
	about.addAuthor( "Sirtaj Singh Kang", I18N_NOOP( "started it all" ), "taj@kde.org" );
	about.addAuthor( "Simon Hausmann", 0, "hausmann@kde.org" );
	KCmdLineArgs::init( argc, argv, &about );
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app;

	if( app.isRestored() )
		RESTORE( KView )
	else
	{
		KCmdLineArgs * args = KCmdLineArgs::parsedArgs();

		KView * kview = new KView;
		kview->show();
		// only load image in first url - there's no way I can think of
		// to tell the presenter plugin to add those urls to it's list
		if( args->count() > 0 )
		{
			if( args->url( 0 ) == QString( "-" ) )
				kview->loadFromStdin();
			else
				kview->load( args->url( 0 ) );
		}
		args->clear();
	}

	return app.exec();
}

// vim:sw=4:ts=4
