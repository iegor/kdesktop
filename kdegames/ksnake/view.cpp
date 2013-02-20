/**
 * Copyright Michel Filippi <mfilippi@sade.rhein-main.de>
 *           Robert Williams
 *           Andrew Chant <andrew.chant@utoronto.ca>
 *           André Luiz dos Santos <andre@netvision.com.br>
 *           Benjamin Meyer <ben+ksnake@meyerhome.net>
 *
 * This file is part of the ksnake package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "view.h"

#include "progress.h"
#include "rattler.h"

int BRICKSIZE = 16;
int MAPWIDTH = BRICKSIZE * 35;
int MAPHEIGHT = MAPWIDTH;

#define BAR_HEIGHT 12
View::View( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    progress = new Progress(this);
    rattler = new Rattler( this);
    setMinimumSize(145,145+BAR_HEIGHT);
}

void View::resizeEvent( QResizeEvent * )
{
    // These hard coded number really need to be documented
    BRICKSIZE= (int)16* ((width() < height() - BAR_HEIGHT) ? width() : height() - BAR_HEIGHT)/ 560;
    MAPWIDTH=BRICKSIZE * BoardWidth;
    MAPHEIGHT=MAPWIDTH;
    
    progress->setGeometry(5, 0, width()-5, BAR_HEIGHT);
    rattler->setGeometry(0, BAR_HEIGHT, width(),   height()-BAR_HEIGHT);
}

QSize View::sizeHint() const
{
	return QSize(490,502);
}

#include "view.moc"
