/*  This file is part of the KDE project
    Copyright (C) 2001-2002 Matthias Kretz <kretz@kde.org>

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

//$Id: kimageholder.cpp 465369 2005-09-29 14:33:08Z mueller $

#include <assert.h>

#include <qcolor.h>
#include <qwidget.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpen.h>
#include <qmovie.h>
#include <qpixmap.h>

#include <kpixmap.h>
#include <kdebug.h>

#include "kimageholder.h"

KImageHolder::KImageHolder( QWidget * parent, const char * name )
	: QWidget( parent, name, Qt::WResizeNoErase | Qt::WRepaintNoErase )
	, m_selected( false )
	, m_bSelecting( false )
	, m_scrollTimerId( 0 )
	, m_xOffset( 0 )
	, m_yOffset( 0 )
	, m_pen( new QPen( QColor( 255, 255, 255 ), 0, DashLine ) )
	, m_pPixmap( 0 )
	, m_pDoubleBuffer( 0 )
	, m_pCheckboardPixmap( 0 )
{
	setBackgroundMode( QWidget::NoBackground );
}

KImageHolder::~KImageHolder()
{
	delete m_pen;
	delete m_pPixmap;
	delete m_pDoubleBuffer;
	delete m_pCheckboardPixmap;
	m_pen = 0;
	m_pPixmap = 0;
	m_pDoubleBuffer = 0;
	m_pCheckboardPixmap = 0;
}

void KImageHolder::mousePressEvent( QMouseEvent *ev )
{
	//kdDebug( 4620 ) << k_funcinfo << " ev->state() = " << ev->state() << endl;
	// if the right mouse button is pressed emit the contextPress signal
	if ( ev->button() == RightButton )
	{
		emit contextPress( mapToGlobal( ev->pos() ) );
		return;
	}

	if( m_pPixmap == 0 )
		return;

	if( ev->button() == LeftButton || ev->button() == MidButton )
	{
		m_scrollpos = ev->globalPos();
		m_selectionStartPoint = ev->pos();
	}
}

void KImageHolder::mouseMoveEvent( QMouseEvent *ev )
{
	//FIXME: when scrolling the cursorpos shouldn't change
	if( this->rect().contains( ev->pos(), false ) )
		emit cursorPos( ev->pos() );
	//kdDebug( 4620 ) << k_funcinfo << " ev->state() = " << ev->state() << endl;
	if( ev->state() & LeftButton || ev->state() & MidButton )
	{
		// scroll when a modifier and left button or the middle button is pressed
		if( ev->state() & AltButton || ev->state() & ControlButton || ev->state() & ShiftButton || ev->state() & MidButton )
		{
			QPoint difference = m_scrollpos - ev->globalPos();
			emit wannaScroll( difference.x(), difference.y() );
		}
		else // create a selection
		{
			QWidget * parentwidget = ( QWidget* )parent();
			if( ! m_bSelecting )
			{
				m_bSelecting = true;
				if( m_selected ) {
					// remove old rubberband
					eraseSelect();
					m_selected = false;
				}

				m_selection.setLeft( m_selectionStartPoint.x() );
				m_selection.setRight( m_selectionStartPoint.x() );
				m_selection.setTop( m_selectionStartPoint.y() );
				m_selection.setBottom( m_selectionStartPoint.y() );
			}
			// currently called only on drag,
			// so assume a selection has been started
			bool erase = m_selected;

			if( !m_selected )
				m_selected = true;

			// Autoscrolling:
			// For every pixel the mouse leaves the image canvas the canvas
			// should scroll faster. The scrolling should be independent from
			// the mouse movement. Therefor I need a timer to do the scroll
			// movement:
			// e.g.: m_xOffset=-1 => scroll to the left 1 px per 50ms
			//                 -2 => scroll to the left 2 px per 50ms
			//                  2 => scroll to the right 2 px per 50ms
			//                 ...
			m_xOffset = mapTo( parentwidget, ev->pos() ).x();
			m_yOffset = mapTo( parentwidget, ev->pos() ).y();
			if( m_xOffset > 0 )
			{
				m_xOffset -= parentwidget->width();
				if( m_xOffset < 0 )
					m_xOffset = 0;
			}
			if( m_yOffset > 0 )
			{
				m_yOffset -= parentwidget->height();
				if( m_yOffset < 0 )
					m_yOffset = 0;
			}
			if( m_scrollTimerId != 0 && m_xOffset == 0 && m_yOffset == 0 )
			{
				killTimer( m_scrollTimerId );
				m_scrollTimerId = 0;
			}
			else if( m_scrollTimerId == 0 && ( m_xOffset != 0 || m_yOffset != 0 ) )
				m_scrollTimerId = startTimer( 50 );

			int r = ev->x() < width()  ? ( ev->x() < 0 ? 0 : ev->x() ) : width()  - 1;
			int b = ev->y() < height() ? ( ev->y() < 0 ? 0 : ev->y() ) : height() - 1;

			if( r != m_selection.right() || b != m_selection.bottom() )
			{
				if( erase )
					eraseSelect();

				m_selection.setRight( r );
				m_selection.setBottom( b );
				emit selected( m_selection.normalize() );

				QPainter painter( this );
				drawSelect( painter );
			}
		}
		m_scrollpos = ev->globalPos();
		m_selectionStartPoint = ev->pos();
	}
}

void KImageHolder::mouseReleaseEvent( QMouseEvent * ev )
{
	if( m_bSelecting )
	{
		m_xOffset = m_yOffset = 0;
		if( m_scrollTimerId != 0 )
		{
			killTimer( m_scrollTimerId );
			m_scrollTimerId = 0;
		}
	}
	if( ev->state() & LeftButton || ev->state() & MidButton )
		if( m_bSelecting )
			m_bSelecting = false;
		else
			clearSelection();
}

void KImageHolder::drawSelect( QPainter & painter )
{
	painter.save();
	painter.setRasterOp( XorROP );
	painter.setPen( *m_pen );
	painter.drawRect( m_selection.normalize() );
	painter.restore();
}

void KImageHolder::eraseSelect()
{
	QRegion r( m_selection.normalize() );
	QRect inner = m_selection.normalize();
	inner.rLeft() += 1;
	inner.rTop() += 1;
	inner.rRight() -= 1;
	inner.rBottom() -= 1;
	r -= inner;

	QMemArray<QRect> rects = r.rects();

	if( m_pDoubleBuffer )
		for( unsigned int i = 0; i < rects.size(); ++i )
			bitBlt( this, rects[ i ].topLeft(), m_pDoubleBuffer, rects[ i ], CopyROP );
	else
		for( unsigned int i = 0; i < rects.size(); ++i )
			bitBlt( this, rects[ i ].topLeft(), m_pPixmap, rects[ i ], CopyROP );
}

void KImageHolder::clearSelection()
{
	if( m_selected )
	{
		eraseSelect();
		m_selected = false;
	}
	m_selection.setSize( QSize( 0, 0 ) );
	emit selected( m_selection );
}

void KImageHolder::setImage( const KPixmap & pix )
{
	clearSelection();

	setPixmap( pix );
}

void KImageHolder::setImage( const QImage & image )
{
	clearSelection();
	kdDebug( 4620 ) << "converting Image to Pixmap" << endl;
	KPixmap pix( image );

	setPixmap( pix );
}

void KImageHolder::setImage( const QMovie & /*movie*/ )
{
	clearSelection();
	//setMovie( movie );
	kdWarning( 4620 ) << "setImage( QMovie ) not implemented" << endl;
}

void KImageHolder::clear()
{
	delete m_pPixmap;
	m_pPixmap = 0;
	delete m_pDoubleBuffer;
	m_pDoubleBuffer = 0;
	hide();
	clearSelection();
}

QRect KImageHolder::selection() const
{
	if( m_selected )
		return m_selection.normalize();
	else
		return QRect();
}

QSize KImageHolder::sizeHint() const
{
	if( m_pPixmap )
		return m_pPixmap->size();
	return QSize( 0, 0 );
}

void KImageHolder::paintEvent( QPaintEvent *ev )
{
	QPainter painter( this );
	painter.setClipRegion( ev->region().intersect( m_drawRect ) );
	if( m_pPixmap )
	{
		if( m_pPixmap->mask() )
		{
			if( ! m_pDoubleBuffer )
			{
				m_pDoubleBuffer = new KPixmap( m_pPixmap->size() );
				QPainter p( m_pDoubleBuffer );
				p.drawTiledPixmap( m_pDoubleBuffer->rect(), checkboardPixmap() );
				p.end();
				bitBlt( m_pDoubleBuffer, QPoint( 0, 0 ), m_pPixmap, m_pPixmap->rect() );
			}
			painter.drawPixmap( 0, 0, *m_pDoubleBuffer );
		}
		else
			painter.drawPixmap( 0, 0, *m_pPixmap );
	}
	if( m_selected )
		drawSelect( painter );
}

void KImageHolder::timerEvent( QTimerEvent * ev )
{
	if( ev->timerId() != m_scrollTimerId )
		return;

	emit wannaScroll( m_xOffset, m_yOffset );
}

void KImageHolder::setPixmap( const KPixmap & pixmap )
{
	kdDebug( 4620 ) << k_funcinfo << " " << pixmap.width() << 'x' << pixmap.height() << endl;
	delete m_pPixmap;
	delete m_pDoubleBuffer;
	m_pDoubleBuffer = 0;
	m_pPixmap = new KPixmap( pixmap );
	m_drawRect = m_pPixmap->rect();
	show();
}

const KPixmap & KImageHolder::checkboardPixmap()
{
	if( ! m_pCheckboardPixmap )
	{
		const char * xpm[] = {
			"32 32 2 1",
			"  c #666666",
			"X c #999999",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"                XXXXXXXXXXXXXXXX",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
			"XXXXXXXXXXXXXXXX                ",
		};
		m_pCheckboardPixmap = new KPixmap( xpm );
	}
	return *m_pCheckboardPixmap;
}

#include "kimageholder.moc"

// vim:sw=4:ts=4
