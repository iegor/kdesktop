/** 
 * Copyright (C) 2001-2002 the KGhostView authors. See file AUTHORS.
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

#include <qdatetime.h>

#include <kdebug.h>

#include "kgvpageview.h"

KGVPageView::KGVPageView( QWidget* parent, const char* name )
  : QScrollView( parent, name )
{
    _page = 0;

    setFocusPolicy( QWidget::StrongFocus );
    viewport()->setFocusPolicy( QWidget::WheelFocus );
}

void KGVPageView::setPage( QWidget* page )
{
    if( page != 0 ) {
	addChild( page );
	centerContents();
	_page = page;
    }
}

bool KGVPageView::atTop() const
{
    return verticalScrollBar()->value() == verticalScrollBar()->minValue();
}

bool KGVPageView::atBottom() const 
{
    return verticalScrollBar()->value() == verticalScrollBar()->maxValue();
}

bool KGVPageView::eventFilter( QObject* o, QEvent* e )
{
    if ( o == _page &&  e->type() == QEvent::Resize ) {
	// We need to call QScrollView::eventFilter before centerContents,
	// otherwise a loop will be introduced.
	bool result = QScrollView::eventFilter( o, e );
	centerContents();
	emit pageSizeChanged( _page->size() );
	return result;
    }
    return QScrollView::eventFilter( o, e );
}


void KGVPageView::wheelEvent( QWheelEvent *e )
{
    int delta = e->delta();
    e->accept();
    if ((e->state() & ControlButton) == ControlButton) {
	if ( e->delta() < 0 )
	    emit zoomOut();
	else
	    emit zoomIn();
    }
    else if ( delta <= -120 && atBottom() )
    {
	emit ReadDown();
    }
    else if ( delta >= 120 && atTop())
    {
	emit ReadUp();
    }

    else
        QScrollView::wheelEvent( e );
}
void KGVPageView::mousePressEvent( QMouseEvent * e )
{
    if ( e->button() & LeftButton )
    {
	_dragGrabPos = e -> globalPos();
	setCursor( sizeAllCursor );
    }
    else if ( e->button() & MidButton )
    {
	emit ReadDown();
    }
    else if ( e -> button() & RightButton )
    {
	emit rightClick();
    }
}

void KGVPageView::mouseReleaseEvent( QMouseEvent *e )
{
    if ( e -> button() & LeftButton )
    {
	setCursor( arrowCursor );
    }
}

void KGVPageView::mouseMoveEvent( QMouseEvent * e )
{
    if ( e->state() & LeftButton )
    {
	QPoint delta = _dragGrabPos - e->globalPos();
	scrollBy( delta.x(), delta.y() );
	_dragGrabPos = e->globalPos();
    }
}

bool KGVPageView::readUp()
{
    if( atTop() )
	return false;
    else {
	int newValue = QMAX( verticalScrollBar()->value() - height() + 50,
			     verticalScrollBar()->minValue() );

	/*
	int step = 10;
	int value = verticalScrollBar()->value();
	while( value > newValue - step ) {
	    verticalScrollBar()->setValue( value );
	    value -= step;
	}
	*/
			     
	verticalScrollBar()->setValue( newValue );
	return true;
    }
}

bool KGVPageView::readDown()
{   
    if( atBottom() )
	return false;
    else {
	int newValue = QMIN( verticalScrollBar()->value() + height() - 50,
			     verticalScrollBar()->maxValue() );
	
	/*
	int step = 10;
	int value = verticalScrollBar()->value();
	while( value < newValue + step ) {
	    verticalScrollBar()->setValue( value );
	    value += step;
	}
	*/
	    
	verticalScrollBar()->setValue( newValue );
	return true;
    }
}

void KGVPageView::scrollRight()
{
    horizontalScrollBar()->addLine();
}

void KGVPageView::scrollLeft()
{
    horizontalScrollBar()->subtractLine();
}

void KGVPageView::scrollDown()
{
    verticalScrollBar()->addLine();
}

void KGVPageView::scrollUp()
{
    verticalScrollBar()->subtractLine();
}

void KGVPageView::scrollBottom()
{
    verticalScrollBar()->setValue( verticalScrollBar()->maxValue() );
}

void KGVPageView::scrollTop()
{
    verticalScrollBar()->setValue( verticalScrollBar()->minValue() );
}

void KGVPageView::enableScrollBars( bool b )
{
    setHScrollBarMode( b ? Auto : AlwaysOff );
    setVScrollBarMode( b ? Auto : AlwaysOff );
}

void KGVPageView::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() ) {
    case Key_Up:
	scrollUp();
	break;
    case Key_Down:
	scrollDown();
	break;
    case Key_Left:
	scrollLeft();
	break;
    case Key_Right:
	scrollRight();
	break;
    default:
	e->ignore();
	return;
    }
    e->accept();
}

void KGVPageView::viewportResizeEvent( QResizeEvent* e )
{
    QScrollView::viewportResizeEvent( e );
    emit viewSizeChanged( viewport()->size() );
    centerContents();
}

void KGVPageView::centerContents()
{
    if( !_page ) 
	return;

    int newX = 0;
    int newY = 0;

    QSize newViewportSize = viewportSize( _page->width(),
					  _page->height() );

    if( newViewportSize.width() > _page->width() )
	newX = ( newViewportSize.width() - _page->width() )/2;
    if( newViewportSize.height() > _page->height() )
	newY = ( newViewportSize.height() - _page->height() )/2;

    moveChild( _page, newX, newY );
}

#include "kgvpageview.moc"

// vim:sw=4:sts=4:ts=8:noet
