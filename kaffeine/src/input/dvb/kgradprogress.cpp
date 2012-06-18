/*
 * kgradprogress.cpp
 *
 * Copyright (C) 2003-2005 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <qpainter.h>
#include <qlabel.h>

#include "kgradprogress.h"



KGradProgress::KGradProgress( QWidget *parent ) : QLabel( parent )
{
	barPix = 0;
	colorUp = QColor( 0, 255, 0 );
	colorDown = QColor( 255, 0, 0 );
	current = 0;
	setFrameStyle( QFrame::Box | QFrame::Plain );
	setLineWidth(1);
	setMidLineWidth(0);
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
}



void KGradProgress::setProgress( int progress )
{
	current = progress;
	draw();
	repaint(false);
}



void KGradProgress::draw()
{
	int i, j;
	int a, b;
	int off = frameWidth();
	int h = height() - ( 2*off );
	int w = width() - ( 2*off );
	int per;
	QString s;
	QFont f;

	s = s.setNum( current )+"%";
	per = current*w/100;

	if ( barPix ) delete barPix;
	barPix = new QPixmap( w, h, -1, QPixmap::BestOptim );
	QPainter p( barPix );
	p.fillRect( 0, 0, w, h, paletteBackgroundColor() );

	for ( j=0; j<per; j++ ) {
		if ( j<(w/2) ) {
			a = 255;
			b = 2*j*255/w;
		}
		else {
			a = 255-(2*(j-w/2)*255/w);
			b = 255;
		}
		p.setPen( QColor( a, b, 0 ) );
		for ( i=0; i<h; i++ ) p.drawPoint( j+off, i+off );
	}

	f = font();
	f.setPointSize( int(h/1.2) );
	p.setFont( f );
	p.setPen( QColor( 0, 0, 0 ) );
	p.drawText( w/2, (h/2)+(f.pointSize()/2), s );
	p.end();
}



void KGradProgress::resizeEvent( QResizeEvent *e )
{
	QSize s=e->size();
	draw();
}



void KGradProgress::paintEvent(QPaintEvent *event)
{
	int x, y, w, h;

	x = y = frameWidth();
	w = width() - (2*x);
	h = height() - (2*y);

	QLabel::paintEvent( event );

	bitBlt( this, x, y, barPix, 0, 0, w, h, CopyROP );
}



QSize KGradProgress::sizeHint()
{
	QLabel lab( "This is a progress bar.", 0 );
	return QSize( lab.width(), int(font().pointSize()*1.2) );
}



QSizePolicy KGradProgress::sizePolicy()
{
	return QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
}



KGradProgress::~KGradProgress()
{
	delete barPix;
}

#include "kgradprogress.moc"
