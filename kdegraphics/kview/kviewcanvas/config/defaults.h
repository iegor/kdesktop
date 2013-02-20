/*  This file is part of the KDE project
    Copyright (C) 2003 Matthias Kretz <kretz@kde.org>

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

#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <klocale.h>
#include <qsize.h>
#include <qcolor.h>

namespace Defaults {
	static const bool smoothScaling = false;
	static const bool keepAspectRatio = true;
	static const bool centerImage = true;
	static const QColor bgColor( Qt::black );
	static const QSize minSize( 1, 1 );
	static const QSize maxSize( 10000, 10000 );
	static const unsigned int numOfBlendEffects = 4;
	static const char * blendEffectDescription[ 6 ] = {
														I18N_NOOP( "No Blending" ),
														I18N_NOOP( "Wipe From Left" ),
														I18N_NOOP( "Wipe From Right" ),
														I18N_NOOP( "Wipe From Top" ),
														I18N_NOOP( "Wipe From Bottom" ),
														I18N_NOOP( "Alpha Blend" )
													  };
}

#endif // DEFAULTS_H

// vim: sw=4 ts=4
