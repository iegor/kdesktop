/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qlayout.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qstyle.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

#include "simplebutton.h"
#include "panner.h"
#include "panner.moc"

Panner::Panner( QWidget* parent, const char* name )
    : QWidget( parent, name ),
      _luSB(0),
      _rdSB(0),
      _cwidth(0), _cheight(0),
      _cx(0), _cy(0)
{
    KGlobal::locale()->insertCatalogue("libkicker");
    setBackgroundOrigin( AncestorOrigin );

    _updateScrollButtonsTimer = new QTimer(this);
    connect(_updateScrollButtonsTimer, SIGNAL(timeout()), this, SLOT(reallyUpdateScrollButtons()));

    _clipper = new QWidget(this);
    _clipper->setBackgroundOrigin(AncestorOrigin);
    _clipper->installEventFilter( this );
    _viewport = new QWidget(_clipper);
    _viewport->setBackgroundOrigin(AncestorOrigin);
    
    // layout
    _layout = new QBoxLayout(this, QBoxLayout::LeftToRight);
    _layout->addWidget(_clipper, 1);
    setOrientation(Horizontal);
}

Panner::~Panner() 
{
}

void Panner::createScrollButtons()
{
    if (_luSB)
    {
        return;
    }

    // left/up scroll button
    _luSB = new SimpleArrowButton(this);
    _luSB->installEventFilter(this);
    //_luSB->setAutoRepeat(true);
    _luSB->setMinimumSize(12, 12);
    _luSB->hide();
    _layout->addWidget(_luSB);
    connect(_luSB, SIGNAL(pressed()), SLOT(startScrollLeftUp()));
    connect(_luSB, SIGNAL(released()), SLOT(stopScroll()));

    // right/down scroll button
    _rdSB = new SimpleArrowButton(this);
    _rdSB->installEventFilter(this);
    //_rdSB->setAutoRepeat(true);
    _rdSB->setMinimumSize(12, 12);
    _rdSB->hide();
    _layout->addWidget(_rdSB);
    connect(_rdSB, SIGNAL(pressed()), SLOT(startScrollRightDown()));
    connect(_rdSB, SIGNAL(released()), SLOT(stopScroll()));

    // set up the buttons
    setupButtons();
}

void Panner::setupButtons()
{
    if (orientation() == Horizontal)
    {
        if (_luSB)
        {
            _luSB->setArrowType(Qt::LeftArrow);
            _rdSB->setArrowType(Qt::RightArrow);
            _luSB->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
            _rdSB->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
            QToolTip::add(_luSB, i18n("Scroll left"));
            QToolTip::add(_rdSB, i18n("Scroll right"));
            setMinimumSize(24, 0);
        }
        _layout->setDirection(QBoxLayout::LeftToRight);
    }
    else
    {
        if (_luSB)
        {
            _luSB->setArrowType(Qt::UpArrow);
            _rdSB->setArrowType(Qt::DownArrow);
            _luSB->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
            _rdSB->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
            QToolTip::add(_luSB, i18n("Scroll up"));
            QToolTip::add(_rdSB, i18n("Scroll down"));
            setMinimumSize(0, 24);
        }
        _layout->setDirection(QBoxLayout::TopToBottom);
    }

    if (isVisible())
    {
        // we need to manually redo the layout if we are visible
        // otherwise let the toolkit decide when to do this
        _layout->activate();
    }
}

void Panner::setOrientation(Orientation o)
{
    _orient = o;
    setupButtons();
    reallyUpdateScrollButtons();
}

void Panner::resizeEvent( QResizeEvent* )
{
    //QScrollView::resizeEvent( e );
    //updateScrollButtons();
}

void Panner::scrollRightDown()
{
    if(orientation() == Horizontal) // scroll right
        scrollBy( _step, 0 );
    else // scroll down
        scrollBy( 0, _step );
    if (_step < 64)
    _step++;
}

void Panner::scrollLeftUp()
{
    if(orientation() == Horizontal) // scroll left
        scrollBy( -_step, 0 );
    else // scroll up
        scrollBy( 0, -_step );
    if (_step < 64)
        _step++;
}

void Panner::startScrollRightDown()
{
    _scrollTimer = new QTimer(this);
    connect(_scrollTimer, SIGNAL(timeout()), SLOT(scrollRightDown()));
    _scrollTimer->start(50);
    _step = 8;
    scrollRightDown();
}

void Panner::startScrollLeftUp()
{
    _scrollTimer = new QTimer(this);
    connect(_scrollTimer, SIGNAL(timeout()), SLOT(scrollLeftUp()));
    _scrollTimer->start(50);
    _step = 8;
    scrollLeftUp();
}

void Panner::stopScroll()
{
    delete _scrollTimer;
    _scrollTimer = 0;
}

void Panner::reallyUpdateScrollButtons()
{
    int delta = 0;
    
    _updateScrollButtonsTimer->stop();

    if (orientation() == Horizontal)
    {
        delta = contentsWidth() - width();
    }
    else
    {
        delta = contentsHeight() - height();
    }

    if (delta >= 1)
    {
        createScrollButtons();

        // since the buttons may be visible but of the wrong size
        // we need to do this every single time
        _luSB->show();
        _rdSB->show();
    }
    else if (_luSB && _luSB->isVisibleTo(this))
    {
        _luSB->hide();
        _rdSB->hide();
    }
}

void Panner::updateScrollButtons()
{
    _updateScrollButtonsTimer->start(200, true);
}

void Panner::setContentsPos(int x, int y)
{
    if (x < 0)
        x = 0;
    else if (x > (contentsWidth() - visibleWidth()))
        x = contentsWidth() - visibleWidth();
    
    if (y < 0)
        y = 0;
    else if (y > (contentsHeight() - visibleHeight()))
        y = contentsHeight() - visibleHeight();
        
    if (x == contentsX() && y == contentsY())
        return;
        
    _viewport->move(-x, -y);
    emit contentsMoving(x, y);
}

void Panner::scrollBy(int dx, int dy)
{
    setContentsPos(contentsX() + dx, contentsY() + dy);
}

void Panner::resizeContents( int w, int h )
{
    _viewport->resize(w, h);
    setContentsPos(contentsX(), contentsY());
    updateScrollButtons();
}

QPoint Panner::contentsToViewport( const QPoint& p ) const
{
    return QPoint(p.x() - contentsX() - _clipper->x(), p.y() - contentsY() - _clipper->y());
}

QPoint Panner::viewportToContents( const QPoint& vp ) const
{
    return QPoint(vp.x() + contentsX() + _clipper->x(), vp.y() + contentsY() + _clipper->y());
}

void Panner::contentsToViewport( int x, int y, int& vx, int& vy ) const
{
    const QPoint v = contentsToViewport(QPoint(x,y));
    vx = v.x();
    vy = v.y();
}

void Panner::viewportToContents( int vx, int vy, int& x, int& y ) const
{
    const QPoint c = viewportToContents(QPoint(vx,vy));
    x = c.x();
    y = c.y();
}

void Panner::ensureVisible( int x, int y )
{
    ensureVisible(x, y, 50, 50);
}

void Panner::ensureVisible( int x, int y, int xmargin, int ymargin )
{
    int pw=visibleWidth();
    int ph=visibleHeight();

    int cx=-contentsX();
    int cy=-contentsY();
    int cw=contentsWidth();
    int ch=contentsHeight();

    if ( pw < xmargin*2 )
        xmargin=pw/2;
    if ( ph < ymargin*2 )
        ymargin=ph/2;

    if ( cw <= pw ) {
        xmargin=0;
        cx=0;
    }
    if ( ch <= ph ) {
        ymargin=0;
        cy=0;
    }

    if ( x < -cx+xmargin )
        cx = -x+xmargin;
    else if ( x >= -cx+pw-xmargin )
        cx = -x+pw-xmargin;

    if ( y < -cy+ymargin )
        cy = -y+ymargin;
    else if ( y >= -cy+ph-ymargin )
        cy = -y+ph-ymargin;

    if ( cx > 0 )
        cx=0;
    else if ( cx < pw-cw && cw>pw )
        cx=pw-cw;

    if ( cy > 0 )
        cy=0;
    else if ( cy < ph-ch && ch>ph )
        cy=ph-ch;

    setContentsPos( -cx, -cy );
}

bool Panner::eventFilter( QObject *obj, QEvent *e )
{
    if ( obj == _viewport || obj == _clipper ) 
    {
        switch ( e->type() ) 
        {
            case QEvent::Resize:
                viewportResizeEvent((QResizeEvent *)e);
                break;
            case QEvent::MouseButtonPress:
                viewportMousePressEvent( (QMouseEvent*)e );
                if ( ((QMouseEvent*)e)->isAccepted() )
                    return true;
                break;
            case QEvent::MouseButtonRelease:
                viewportMouseReleaseEvent( (QMouseEvent*)e );
                if ( ((QMouseEvent*)e)->isAccepted() )
                    return true;
                break;
            case QEvent::MouseButtonDblClick:
                viewportMouseDoubleClickEvent( (QMouseEvent*)e );
                if ( ((QMouseEvent*)e)->isAccepted() )
                    return true;
                break;
            case QEvent::MouseMove:
                viewportMouseMoveEvent( (QMouseEvent*)e );
                if ( ((QMouseEvent*)e)->isAccepted() )
                    return true;
                break;
            default:
                break;
        }
    }
    
    return QWidget::eventFilter( obj, e );  // always continue with standard event processing
}

void Panner::viewportResizeEvent( QResizeEvent* )
{
}

void Panner::viewportMousePressEvent( QMouseEvent* e)
{
    e->ignore();
}

void Panner::viewportMouseReleaseEvent( QMouseEvent* e )
{
    e->ignore();
}

void Panner::viewportMouseDoubleClickEvent( QMouseEvent* e )
{
    e->ignore();
}

void Panner::viewportMouseMoveEvent( QMouseEvent* e )
{
    e->ignore();
}
