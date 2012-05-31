/**
 * Copyright (C) 2003, Luís Pedro Coelho
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
#include "displayoptions.h"
#include <kdebug.h>
#include <qregexp.h>
#include <kcmdlineargs.h>
#include <kconfig.h>

namespace {
	const double allowedMagnifications[] = {
		0.125,
		0.25,
		0.3333,
		0.5,
		0.6667,
		0.75,
		1,
		1.25,
		1.50,
		2,
		3,
		4,
		6,
		8
	};
	const size_t numberOfMagnifications = ( sizeof( allowedMagnifications ) / sizeof( allowedMagnifications[ 0 ] ) );
}

DisplayOptions DisplayOptions::parse( KCmdLineArgs* args )
{
	DisplayOptions res;
#define CHECK_ORIENTATION( tag, value ) \
	if ( args->isSet( tag ) ) res._overrideOrientation = value; \
	if ( args->getOption( "orientation" ) == tag ) res._overrideOrientation = value;

	CHECK_ORIENTATION( "landscape", CDSC_LANDSCAPE );
	CHECK_ORIENTATION( "seascape", CDSC_SEASCAPE );
	CHECK_ORIENTATION( "portrait", CDSC_PORTRAIT);
	CHECK_ORIENTATION( "upsidedown", CDSC_UPSIDEDOWN);

	res.setMagnification( args->getOption( "scale" ).toFloat() );
	res.setPage( args->getOption( "page" ).toInt() - 1 ); // transform from 1-based into 0-based
	//res._overridePageMedia = args->getOption( "paper" );
	kdDebug(4500 ) << "Parsed options: " << DisplayOptions::toString( res ) << endl;
	return res;
}

namespace {
	const char* const rformat = ".page: (\\d+); .magnification: ([\\d\\.]+); .orientation = (\\d+); .media = ([^;]*);";
	const char* const qformat = ".page: %1; .magnification: %2; .orientation = %3; .media = %4;";
}

QString DisplayOptions::toString( const DisplayOptions& options )
{
	return QString( qformat )
		.arg( options.page() )
		.arg( options.magnification() )
		.arg( options.overrideOrientation() )
		.arg( options.overridePageMedia().utf8() );
}

bool DisplayOptions::fromString( DisplayOptions& out, const QString& in )
{
	QRegExp regex( QString::fromLatin1( rformat ) );
	if ( regex.search( in ) < 0 ) return false;

	out.reset();
	out.setPage( regex.cap( 1 ).toInt() );
	out.setMagnification( regex.cap( 2 ).toDouble() );
	out.setOverrideOrientation( static_cast<CDSC_ORIENTATION_ENUM>( regex.cap( 3 ).toInt() ) );
	if ( !regex.cap( 4 ).isEmpty() ) out.setOverridePageMedia( regex.cap( 4 ) );
	return true;
}


bool DisplayOptions::canZoomIn() const
{
	return unsigned( closestIndex() ) < ( numberOfMagnifications - 1 );
}

bool DisplayOptions::zoomIn()
{
	if ( !canZoomIn() ) return false;
	_magnification = allowedMagnifications[ closestIndex() + 1 ];
	return true;
}

bool DisplayOptions::canZoomOut() const
{
	return closestIndex() > 0;
}

bool DisplayOptions::zoomOut()
{
	if ( !canZoomOut() ) return false;
	_magnification = allowedMagnifications[ closestIndex() - 1 ];
	return true;
}

double DisplayOptions::magnification() const
{
	return _magnification;
}


void DisplayOptions::setMagnification( double newZoom ) {
	_magnification = newZoom;
}

unsigned DisplayOptions::closestIndex() const {
	kdDebug(4500) << "DisplayOptions::closestIndex(" << _magnification << ")" << endl;
	unsigned res = 0;
	while ( res < numberOfMagnifications
			&& allowedMagnifications[ res ] < _magnification ) ++res;
	if ( res >= ( numberOfMagnifications - 1 ) ) return numberOfMagnifications - 1;
	if ( res == 0 ) return 0;

	if ( ( allowedMagnifications[ res ] - _magnification ) > ( _magnification - allowedMagnifications[ res - 1 ] ) ) {
		--res;
	}
	kdDebug(4500) << "DisplayOptions::closestIndex(" << res << "): nearest allowed magnification: "
		<< allowedMagnifications[ res ] << endl;
	return res;
}

QValueList<double> DisplayOptions::normalMagnificationValues() {
	QValueList<double> res;
	for ( const double *first = allowedMagnifications, *last = allowedMagnifications + numberOfMagnifications;
			first != last;
			++first ) {
		res << *first;
	}
	return res;
}

