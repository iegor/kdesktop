/* This file is part of the KDE project
   Copyright (C) 2001 Wilco Greven <greven@kde.org>
   Copyright (C) 2002-2004 Stefan Kebekus <kebekus@kde.org>
   Copyright (C) 2004-2005 Wilfried Huss <Wilfried.Huss@gmx.at>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <config.h>

#include <kdebug.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qrect.h>
#include <math.h>

#include "pageView.h"
#include "pageNumber.h"

PageView::PageView( QWidget* parent, const char* name )
  : QScrollView( parent, name, WStaticContents | WNoAutoErase)
{
  moveTool = true;

  widgetList = 0;
  viewport()->setFocusPolicy(QWidget::StrongFocus);

  setResizePolicy(QScrollView::Manual);

  setVScrollBarMode(QScrollView::Auto);
  setHScrollBarMode(QScrollView::Auto);

  viewport()->setBackgroundMode(Qt::NoBackground);

  setResizePolicy(Manual);
  setDragAutoScroll(false);

  enableClipper(true);
  nrCols = 1;
  nrRows = 1;
  continuousViewmode = true;
  fullScreen = false;

  connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(calculateCurrentPageNumber(int, int)));
}


void PageView::addChild( QPtrVector<DocumentWidget> *wdgList )
{
  if( wdgList == 0 ) {
    kdError(1223) << "PageView::addChild(...) called with invalid arguments" << endl;
    return;
  }

  widgetList = wdgList;
  layoutPages();
}


bool PageView::atTop() const
{
  return verticalScrollBar()->value() == verticalScrollBar()->minValue();
}


bool PageView::atBottom() const 
{
  return verticalScrollBar()->value() == verticalScrollBar()->maxValue();
}


bool PageView::readUp()
{
  if( atTop() )
    return false;
  else {
    // Coordinate of the top of the viewport
    int top = contentsY();

    DocumentWidget* widget = 0;
    // Find the widget(s) that intersect the top of the viewport
    // TODO: It would be better to use a binary search.
    for(Q_UINT16 i=0; i<widgetList->size(); i++)
    {
      widget = widgetList->at(i);
      if (childY(widget) < top && childY(widget) + widget->height() > top)
      {
        // Draw scrollguide
        widget->drawScrollGuide(top - childY(widget));
      }
    }

    int newValue = QMAX( verticalScrollBar()->value() - (int)(height() * 0.9),
                         verticalScrollBar()->minValue() );
    verticalScrollBar()->setValue( newValue );
    return true;
  }
}


bool PageView::readDown()
{
  if( atBottom() )
    return false;
  else {
    // Coordinate of the bottom of the viewport
    int bottom = contentsY() + visibleHeight();

    DocumentWidget* widget = 0;
    // Find the widget(s) that intersect the bottom of the viewport
    // TODO: It would be better to use a binary search.
    for(Q_UINT16 i=0; i<widgetList->size(); i++)
    {
      widget = widgetList->at(i);
      if (childY(widget) < bottom && childY(widget) + widget->height() > bottom)
      {
        // Draw scrollguide
        widget->drawScrollGuide(bottom - childY(widget));
      }
    }

    int newValue = QMIN( verticalScrollBar()->value() + (int)(height() * 0.9),
                         verticalScrollBar()->maxValue() );
    verticalScrollBar()->setValue( newValue );

    return true;
  }
}


void PageView::scrollRight()
{
  horizontalScrollBar()->addLine();
}

void PageView::scrollLeft()
{
  horizontalScrollBar()->subtractLine();
}

void PageView::scrollDown()
{
  verticalScrollBar()->addLine();
}

void PageView::scrollUp()
{
  verticalScrollBar()->subtractLine();
}

void PageView::scrollBottom()
{
  verticalScrollBar()->setValue( verticalScrollBar()->maxValue() );
}

void PageView::scrollTop()
{
  verticalScrollBar()->setValue( verticalScrollBar()->minValue() );
}

void PageView::keyPressEvent( QKeyEvent* e )
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

void PageView::contentsMousePressEvent( QMouseEvent* e )
{
  if (e->button() == LeftButton)
  {
    if (moveTool)
    {
      setCursor(Qt::SizeAllCursor);
      dragGrabPos = e->globalPos();
    }
    else
    {
      // we are in selection mode
    }
  }
  else
  {
    setCursor(Qt::arrowCursor);
  }
}

void PageView::contentsMouseReleaseEvent( QMouseEvent* )
{
  setCursor(Qt::arrowCursor);
}

void PageView::contentsMouseMoveEvent( QMouseEvent* e )
{
  QPoint newPos = e->globalPos();

  if (e->state() == LeftButton && moveTool)
  {
    QPoint delta = dragGrabPos - newPos;
    scrollBy(delta.x(), delta.y());
  }
  dragGrabPos = newPos;
}

void PageView::viewportResizeEvent( QResizeEvent* e )
{
  QScrollView::viewportResizeEvent( e );

  if (!widgetList)
    return;

  layoutPages();

  emit viewSizeChanged( viewport()->size() );
}

void PageView::setNrColumns( Q_UINT8 cols )
{
  nrCols = cols;
}

void PageView::setNrRows( Q_UINT8 rows )
{
  nrRows = rows;
}

void PageView::setContinuousViewMode(bool continuous)
{
  continuousViewmode = continuous;
}

bool PageView::singlePageFullScreenMode()
{
  return (nrCols == 1 && nrRows == 1 && !continuousViewmode && fullScreen);
}

void PageView::slotShowScrollbars(bool status)
{
  if (status == true) {
    setVScrollBarMode(QScrollView::Auto);
    setHScrollBarMode(QScrollView::Auto);
  } else {
    setVScrollBarMode(QScrollView::AlwaysOff);
    setHScrollBarMode(QScrollView::AlwaysOff);
  }
}

void PageView::setFullScreenMode(bool fullScreen)
{
  this -> fullScreen = fullScreen;
  if (fullScreen == true) 
  {
    setVScrollBarMode(QScrollView::AlwaysOff);
    setHScrollBarMode(QScrollView::AlwaysOff);
    oldFrameStyle = frameStyle();
    setFrameStyle(QFrame::NoFrame);
    backgroundColor = viewport()->paletteBackgroundColor();
    if (singlePageFullScreenMode())
    {
      viewport()->setPaletteBackgroundColor( Qt::black ) ;
    }
  }
  else
  {
    viewport()->setPaletteBackgroundColor( backgroundColor ) ;
    setFrameStyle(oldFrameStyle);
  }
}

void PageView::layoutPages(bool zoomChanged)
{
  // Paranoid safety check
  if (widgetList == 0)
    return;

  // If there are no widgets, e.g. because the last widget has been
  // removed, the matter is easy: set the contents size to 0. If there
  // are no widgets because previously existing widgets were removed
  // (we detect that by looking at the contentsWidth and -Height).
  if (widgetList->isEmpty()) {
    if ((contentsWidth() != 0) || (contentsHeight() != 0)) {
      QScrollView::resizeContents(0,0);
    }
    return;
  }

  // Ok, now we are in a situation where we do have some widgets that
  // shall be centered.
  int distance = distanceBetweenWidgets;
  if (singlePageFullScreenMode())
  {
    // In single page fullscreen mode we don't want a margin around the pages
    distance = 0;
  }

  QMemArray<Q_UINT32> colWidth(nrCols);
  for(Q_UINT8 i=0; i<colWidth.size(); i++)
    colWidth[i] = 0;

  Q_UINT16 numRows;
  if(nrCols <= 2)
  {
    numRows = (widgetList->size()+2*nrCols-2) / nrCols;
  }
  else
  {
    numRows = (Q_INT16)ceil(((double)widgetList->size()) / nrCols);
  }

  QMemArray<Q_UINT32> rowHeight(numRows);
  for(Q_UINT16 i=0; i<rowHeight.size(); i++)
    rowHeight[i] = 0;

  // Now find the widths and heights of the columns
  for(Q_UINT16 i=0; i<widgetList->size(); i++) 
  {
    Q_UINT8 col;
    Q_UINT16 row;

    if (nrCols == 2) {
      // In two-column display, start with the right column
      col = (i+1+nrCols) % nrCols;
      row = (i+1+nrCols) / nrCols - 1;
    } else {
      col = (i+nrCols) % nrCols;
      row = (i+nrCols) / nrCols - 1;
    }

    colWidth[col] = QMAX(colWidth[col], (Q_UINT32)widgetList->at(i)->pageSize().width());
    rowHeight[row] = QMAX(rowHeight[row], (Q_UINT32)widgetList->at(i)->pageSize().height());
  }

  // Calculate the total width and height of the display
  Q_UINT32 totalHeight = 0;
  for(Q_UINT16 i=0; i<rowHeight.size(); i++)
    totalHeight += rowHeight[i];

  totalHeight += (numRows+1)*distance;
  Q_UINT32 totalWidth = 0;
  for(Q_UINT8 i=0; i<colWidth.size(); i++)
    totalWidth += colWidth[i];

  totalWidth += (nrCols+1)*distance;
  QSize newViewportSize = viewportSize( totalWidth, totalHeight );
  Q_UINT32 centeringLeft = 0;
  if( (Q_UINT32)newViewportSize.width() > totalWidth )
    centeringLeft = ( newViewportSize.width() - totalWidth )/2;
  Q_UINT32 centeringTop = 0;
  if( (Q_UINT32)newViewportSize.height() > totalHeight )
    centeringTop = ( newViewportSize.height() - totalHeight)/2;

  // Resize the viewport
  if (((Q_UINT32)contentsWidth() != totalWidth) || ((Q_UINT32)contentsHeight() != totalHeight))
  {
    // Calculate the point in the coordinates of the contents which is currently at the center of the viewport.
    QPoint midPoint = QPoint(visibleWidth() / 2 + contentsX(), visibleHeight() / 2 + contentsY()); 
    double midPointRatioX = (double)(midPoint.x()) / contentsWidth();
    double midPointRatioY = (double)(midPoint.y()) / contentsHeight();

    resizeContents(totalWidth,totalHeight);

    // If the zoom changed recenter the former midPoint
    if (zoomChanged)
      center((int)(contentsWidth() * midPointRatioX), (int)(contentsHeight() * midPointRatioY));
  }

  // Finally, calculate the left and top coordinates of each row and
  // column, respectively
  QMemArray<Q_UINT32> colLeft(nrCols);
  colLeft[0] = distance;
  for(Q_UINT8 i=1; i<colLeft.size(); i++)
    colLeft[i] = colLeft[i-1]+colWidth[i-1]+distance;

  QMemArray<Q_UINT32> rowTop(numRows);
  rowTop[0] = distance;
  for(Q_UINT16 i=1; i<rowTop.size(); i++)
    rowTop[i] = rowTop[i-1]+rowHeight[i-1]+distance;

  for(Q_UINT16 i=0; i<widgetList->size(); i++) 
  {
    Q_UINT8 col;
    Q_UINT16 row;
    if (nrCols == 2)
    {
      // In two column-mode start with the right column.
      col = (i+nrCols-1) % nrCols;
      row = (i+nrCols-1) / nrCols;
    }
    else
    {
      col = (i+nrCols) % nrCols;
      row = i / nrCols;
    }
    if (nrCols == 2)
    {
      // in 2-column mode right justify the first column, and leftjustify the second column
      int width = widgetList->at(i)->width();
      int left;
      if (col == 0)
        left = centeringLeft + colLeft[col] + colWidth[col]-width + distance/2;
      else
        left = centeringLeft + colLeft[col];
      moveChild( widgetList->at(i), left, centeringTop+rowTop[row]);
    }
    else
    {
      // in single column and overview mode center the widgets
      int widgetWidth = widgetList->at(i)->width();
      int left = centeringLeft + colLeft[col] + ((int)colWidth[col]-widgetWidth)/2;
      moveChild(widgetList->at(i), left, centeringTop+rowTop[row]);
    }
  }
  calculateCurrentPageNumber();
}


void PageView::contentsWheelEvent ( QWheelEvent * e )
{
  emit(wheelEventReceived(e));
}


void PageView::moveViewportToWidget(QWidget* widget, int y)
{
  int verticalPos = 0;
  int verticalPosTop = 0;

  if (y != 0)
  {
    verticalPosTop = childY(widget) +  y - visibleHeight()/2;
    verticalPos = childY(widget) +  y;
  }
  else
  {
    verticalPos = childY(widget) - distanceBetweenWidgets;
    verticalPosTop = verticalPos;
  }

  if (nrCols == 1)
  {
    // In single column viewmodes, we change the vertical position only, to make it
    // easier to work with high zoomlevels where not the whole pagewidth is visible.
    // TODO: Smarter algorithm also for continuous facing viewmode.
    int top = (int)(contentsY() + 0.1 * visibleHeight());
    int bottom = (int)(contentsY() + 0.9 * visibleHeight());

    // Move the viewport if the target is currently not visible, or lies at the edge
    // of the viewport. If y = 0 always move the top of the targetpage to the top edge
    // of the viewport.
    if (verticalPos < top || verticalPos > bottom || y == 0)
    {
      setContentsPos(contentsX(), verticalPosTop);
    }
  }
  else
  {
    setContentsPos(childX(widget) - distanceBetweenWidgets, verticalPosTop);
  }
}


void PageView::viewportPaintEvent(QPaintEvent* e)
{
  // Region from which rectangles occupied by child widgets will by substracted.
  QRegion backgroundArea(e->rect());

  if (widgetList != 0)
  {
    for (unsigned int i = 0; i < widgetList->count(); i++)
    {
      DocumentWidget* item = widgetList->at(i);

      // Check if the Widget needs to be updated.
      if (!item->geometry().intersects(e->rect()))
        continue;

      QRect widgetGeometry = item->geometry();

      // Draw the widget.
      if (e->rect().intersects(widgetGeometry))
      {
        QRect widgetRect = e->rect().intersect(widgetGeometry);
        widgetRect.moveBy(-widgetGeometry.left(), -widgetGeometry.top());

        item->update(widgetRect);
      }

      // Substract the painted area.
      backgroundArea -= widgetGeometry.intersect(e->rect());
    }
  }

  // Paint the background.
  QPainter p(viewport());

  QMemArray<QRect> backgroundRects = backgroundArea.rects();

  for (unsigned int i = 0; i < backgroundRects.count(); i++)
    p.fillRect(backgroundRects[i], colorGroup().mid());
}


void PageView::calculateCurrentPageNumber(int x, int y)
{
  // Safety check
  if (widgetList == 0)
    return;

  QRect viewportRect(x, y, visibleWidth(), visibleHeight());

  //kdDebug() << "viewportRect(" << viewportRect.x() << ", " << viewportRect.y() << ", "
  //          << viewportRect.width() << ", " << viewportRect.height() << ")" << endl;

  int maxVisiblePixels = 0;
  DocumentWidget* _currentWidget = 0;

  for (Q_UINT16 i = 0; i < widgetList->size(); i++)
  {
    DocumentWidget* documentWidget = widgetList->at(i);
    // Safety check
    if (documentWidget == 0)
      continue;

    // Check if the Widget is visible
    int cx = childX(documentWidget);
    int cy = childY(documentWidget);
    QRect widgetRect(cx, cy, documentWidget->width(), documentWidget->height());
    bool isVisible = widgetRect.intersects(viewportRect);

    if (!isVisible)
      continue;

    // Calculate the number of visible pixels of the widget
    QRect visibleRect = widgetRect.intersect(viewportRect);
    int visiblePixels = visibleRect.width() * visibleRect.height();

    //kdDebug() << visiblePixels << " pixels are visible of page " << documentWidget->getPageNumber() << endl;

    // If a bigger part of this widget as of the previous widgets is visible make it the current widget.
    if (maxVisiblePixels < visiblePixels)
    {
      maxVisiblePixels = visiblePixels;
      _currentWidget = documentWidget;
    }
  }

  // No page is visible
  if (_currentWidget == 0)
    return;

  // Return the number of the current page
  emit currentPageChanged(_currentWidget->getPageNumber());
}

void PageView::calculateCurrentPageNumber()
{
  calculateCurrentPageNumber(contentsX(), contentsY());
}

void PageView::slotEnableMoveTool(bool enable)
{
  moveTool = enable;
}

#include "pageView.moc"
