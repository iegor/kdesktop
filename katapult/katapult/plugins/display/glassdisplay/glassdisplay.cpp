/***************************************************************************
 *   Copyright (C) 2005 by Bastian Holst                                   *
 *   bastianholst@gmx.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <qpixmap.h>

#include "glassdisplay.h"
#include "qembed_images.h"

K_EXPORT_COMPONENT_FACTORY( katapult_glassdisplay,
                            KGenericFactory<GlassDisplay>( "katapult_glassdisplay" ) )

#define WIDTH 295
#define HEIGHT 189
#define PADDING 6
#define ICONSIZE 128

#define LEFTMARGIN 1
#define TOPMARGIN 1
#define RIGHTMARGIN 8
#define BOTTOMMARGIN 8

GlassDisplay::GlassDisplay(QObject *parent, const char *name, const QStringList& list)
	: ImageDisplay(parent, name, list)
{
	setSingleBG(new QPixmap(qembed_findImage("singlebg")));
	setDoubleBG(new QPixmap(qembed_findImage("doublebg")));
	
	setSingleSize(WIDTH, HEIGHT);
	setDoubleSize(WIDTH, HEIGHT);
	
	setMargin(LEFTMARGIN, TOPMARGIN, RIGHTMARGIN, BOTTOMMARGIN);
	setPadding(PADDING);
	setIconSize(ICONSIZE);
}

GlassDisplay::~GlassDisplay()
{
}


#include "glassdisplay.moc"
