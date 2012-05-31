/**
 * Copyright (C) 1997-2002 the KGhostView authors. See file AUTHORS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <qdir.h>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kurl.h>

#include "kgvshell.h"
#include "kgv_view.h"
#include "displayoptions.h"

static KCmdLineOptions options[] =
{
    { "page <page-no>",   I18N_NOOP( "Page to open. Use --page=3 to show the third page, for example. Note that if the page does not exist, any other page may be displayed" ),     "1" },
    { "scale <factor>",   I18N_NOOP( "Magnification of the display" ),    "1.0" },
    { "orientation <orientation>",   I18N_NOOP( "The orientation of the shown image. Use either \"auto\", \"portrait\", \"landscape\", \"upsidedown\" or \"seascape\"" ),    "auto" },
    { "portrait",   I18N_NOOP( "Equivalent to orientation=portrait" ), 0 },
    { "landscape",   I18N_NOOP( "Equivalent to orientation=landscape" ), 0 },
    { "upsidedown",   I18N_NOOP( "Equivalent to orientation=upsidedown" ), 0 },
    { "seascape",   I18N_NOOP( "Equivalent to orientation=seascape" ), 0 },
    // { "watch",   I18N_NOOP( "Turns on watching of a file. This means that whenever the file changes while you are viewing it, kghostview automatically reloads it. This option (which can also be turned on in the menu) is especially useful if you are generating your files from latex or a similar tool." ), 0 },
    // { "page-size", I18N_NOOP( "The page size.\nUse either something like \"A4\" or you can display exact pixel size width-height" ), "auto" }
    { "+[URL]", I18N_NOOP( "Location to open" ), 0 },
    KCmdLineLastOption
};

int main( int argc, char** argv )
{
    KCmdLineArgs::init( argc, argv, KGVPart::createAboutData() );
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    if( kapp->isRestored() )
	RESTORE( KGVShell )
    else {
	KGVShell* shell = new KGVShell;
	if( args->count() == 1 ) {
	    if( QString( args->arg(0) ) == "-" ) {
		shell->openStdin();
	    } else {
		shell->openURL( args->url(0) );
	    }
	    shell->setDisplayOptions( DisplayOptions::parse( args ) );
	}
	shell->show();
    }
    args->clear();
    return app.exec();
}

// vim:sw=4:sts=4:ts=8:noet
