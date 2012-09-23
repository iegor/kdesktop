/***************************************************************************
 *   Copyright (C) 2006 by Martin Meredith                                 *
 *   mez@thekatapult.org.uk                                                *
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

#include "o2display.h"
#include "qembed_images.h"

K_EXPORT_COMPONENT_FACTORY( katapult_o2display,
                            KGenericFactory<o2Display>( "katapult_o2display" ) )

#define WIDTH_SINGLE 228	
#define HEIGHT_SINGLE 220
#define WIDTH_DOUBLE 392
#define HEIGHT_DOUBLE 220

#define PADDING 7
#define ICONSIZE 128

#define LEFTMARGIN 10
#define TOPMARGIN 6
#define RIGHTMARGIN 25
#define BOTTOMMARGIN 40 

o2Display::o2Display(QObject *parent, const char *name, const QStringList& list)
	: ImageDisplay(parent, name, list)
{
	setSingleBG(new QPixmap(qembed_findImage("singlebg")));
	setDoubleBG(new QPixmap(qembed_findImage("doublebg")));
	
	setSingleSize(WIDTH_SINGLE, HEIGHT_SINGLE);
	setDoubleSize(WIDTH_DOUBLE, HEIGHT_DOUBLE);
	
	setMargin(LEFTMARGIN, TOPMARGIN, RIGHTMARGIN, BOTTOMMARGIN);
	setPadding(PADDING);
	setIconSize(ICONSIZE);
}

o2Display::~o2Display()
{
}


#include "o2display.moc"
