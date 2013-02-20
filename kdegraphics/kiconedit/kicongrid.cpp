/*
    KDE Icon Editor - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    Includes portions of code from Qt,
    Copyright (C) 1992-2000 Trolltech AS.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <stdlib.h>

#include <qpainter.h>
#include <qwhatsthis.h>
#include <qscrollview.h>
#include <qbitmap.h>
#include <qclipboard.h>
#include <qdatetime.h>

#include <kiconloader.h>
#include <kruler.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "kresize.h"
#include "properties.h"
#include "kicongrid.h"
#include "kiconedit.h"
#ifndef PICS_INCLUDED
#include "pics/logo.xpm"
#define PICS_INCLUDED
#endif

#include <X11/Xos.h>

void DrawCommand::execute()
{
	oldcolor = *((uint*)image->scanLine(y) + x);
	*((uint*)image->scanLine(y) + x) = newcolor; 
        int cell = y * grid->numCols() + x;
        grid->setUndoColor( cell, newcolor, false );
}

void DrawCommand::unexecute()
{
	*((uint*)image->scanLine(y) + x) = oldcolor; 
        int cell = y * grid->numCols() + x;
        grid->setUndoColor( cell, oldcolor, false );
}

void RepaintCommand::execute()
{
        grid->update( area);
}

KGridView::KGridView(QImage *image, KCommandHistory* history, QWidget *parent, const char *name)
: QFrame(parent, name)
{
  _corner = 0L;
  _hruler = _vruler = 0L;
  _grid = 0L;

  acceptdrop = false;

  KIconEditProperties *props = KIconEditProperties::self();

  viewport = new QScrollView(this);
  Q_CHECK_PTR(viewport);

  _grid = new KIconEditGrid(image, history, viewport->viewport());
  Q_CHECK_PTR(_grid);
  viewport->addChild(_grid);
  _grid->setGrid(props->showGrid());
  _grid->setCellSize(props->gridScale());

  QString str = i18n( "Icon draw grid\n\nThe icon grid is the area where"
      " you draw the icons.\nYou can zoom in and out using the magnifying"
      " glasses on the toolbar.\n(Tip: Hold the magnify button down for a"
      " few seconds to zoom to a predefined scale)" );
  QWhatsThis::add( _grid, str );

  if(props->bgMode() == FixedPixmap)
  {
    QPixmap pix(props->bgPixmap());
    if(pix.isNull())
    {
      QPixmap pmlogo((const char **)logo);
      pix = pmlogo;
    }
    viewport->viewport()->setBackgroundPixmap(pix);
    _grid->setBackgroundPixmap(pix);
  }
  else
  {
    viewport->viewport()->setBackgroundColor(props->bgColor());
  }

  _corner = new QFrame(this);
  _corner->setFrameStyle(QFrame::WinPanel | QFrame::Raised);

  _hruler = new KRuler(Qt::Horizontal, this);
  _hruler->setEndLabel(i18n("width"));
  _hruler->setOffset( -2 );
  _hruler->setRange(0, 1000);

  _vruler = new KRuler(Qt::Vertical, this);
  _vruler->setEndLabel(i18n("height"));
  _vruler->setOffset( -2 );
  _vruler->setRange(0, 1000);

  str = i18n( "Rulers\n\nThis is a visual representation of the current"
      " cursor position" );
  QWhatsThis::add( _hruler, str );
  QWhatsThis::add( _vruler, str );

  connect(_grid, SIGNAL(scalingchanged(int)), SLOT(scalingChange(int)));
  connect(_grid, SIGNAL(sizechanged(int, int)), SLOT(sizeChange(int, int)));
  connect(_grid, SIGNAL(needPainting()), SLOT(paintGrid()));
  connect( _grid, SIGNAL(xposchanged(int)), _hruler, SLOT(slotNewValue(int)) );
  connect( _grid, SIGNAL(yposchanged(int)), _vruler, SLOT(slotNewValue(int)) );
  connect(viewport, SIGNAL(contentsMoving(int, int)), SLOT(moving(int, int)));
  
  setSizes();
  QResizeEvent e(size(), size());
  resizeEvent(&e);
}

void KGridView::paintGrid()
{
  _grid->update(viewRect());
}

void KGridView::setSizes()
{
  if(KIconEditProperties::self()->showRulers())
  {
    _hruler->setLittleMarkDistance(_grid->scaling());
    _vruler->setLittleMarkDistance(_grid->scaling());

    _hruler->setMediumMarkDistance(5);
    _vruler->setMediumMarkDistance(5);

    _hruler->setBigMarkDistance(10);
    _vruler->setBigMarkDistance(10);

    _hruler->setShowTinyMarks(true);
    _hruler->setShowLittleMarks(false);
    _hruler->setShowMediumMarks(true);
    _hruler->setShowBigMarks(true);
    _hruler->setShowEndMarks(true);

    _vruler->setShowTinyMarks(true);
    _vruler->setShowLittleMarks(false);
    _vruler->setShowMediumMarks(true);
    _vruler->setShowBigMarks(true);
    _vruler->setShowEndMarks(true);

    _hruler->setPixelPerMark(_grid->scaling());
    _vruler->setPixelPerMark(_grid->scaling());

    _hruler->setMaxValue(_grid->width()+20);
    _vruler->setMaxValue(_grid->height()+20);

    _hruler->show();
    _vruler->show();

    _corner->show();
    //resize(_grid->width()+_vruler->width(), _grid->height()+_hruler->height());
  }
  else
  {
    _hruler->hide();
    _vruler->hide();
    _corner->hide();
    //resize(_grid->size());
  }
}

void KGridView::sizeChange(int, int)
{
    setSizes();
}

void KGridView::moving(int x, int y)
{
    _hruler->setOffset(abs(x));
    _vruler->setOffset(abs(y));
}

void KGridView::scalingChange(int)
{
    setSizes();
}

void KGridView::setShowRulers(bool mode)
{
    KIconEditProperties::self()->setShowRulers( mode );
    setSizes();
    QResizeEvent e(size(), size());
    resizeEvent(&e);
}

void KGridView::setAcceptDrop(bool a)
{
    if(a == acceptdrop) return;
    acceptdrop = a;
    paintDropSite();
}

void KGridView::checkClipboard()
{
     _grid->checkClipboard();
}

const QRect KGridView::viewRect()
{
    int x, y, cx, cy;
    if(viewport->horizontalScrollBar()->isVisible())
    {
        x = viewport->contentsX();
        cx = viewport->viewport()->width();
    }
    else
    {
        x = 0;
        cx = viewport->contentsWidth();
    }

    if(viewport->verticalScrollBar()->isVisible())
    {
        y = viewport->contentsY();
        cy = viewport->viewport()->height();
    }
    else
    {
        y = 0;
        cy = viewport->contentsHeight();
    }

    return QRect(x, y, cx, cy);
}

void KGridView::paintDropSite()
{
    QPainter p;
    p.begin( _grid );
    p.setRasterOp (NotROP);
    p.drawRect(viewRect());
    p.end();
}

void KGridView::paintEvent(QPaintEvent *)
{
  if(acceptdrop)
    paintDropSite();
}


void KGridView::resizeEvent(QResizeEvent*)
{
  kdDebug(4640) << "KGridView::resizeEvent" << endl;

  setSizes();

  if(KIconEditProperties::self()->showRulers())
  {
    _hruler->setGeometry(_vruler->width(), 0, width(), _hruler->height());
    _vruler->setGeometry(0, _hruler->height(), _vruler->width(), height());

    _corner->setGeometry(0, 0, _vruler->width(), _hruler->height());
    viewport->setGeometry(_corner->width(), _corner->height(),
                   width()-_corner->width(), height()-_corner->height());
  }
  else
    viewport->setGeometry(0, 0, width(), height());
}


KIconEditGrid::KIconEditGrid(QImage *image, KCommandHistory* h, QWidget *parent, const char *name)
 : KColorGrid(parent, name, 1)
{
    img = image;
    history = h;
    selected = 0;
    m_command = 0;

    // the 42 normal kde colors - there can be an additional
    // 18 custom colors in the custom colors palette
    for(uint i = 0; i < 42; i++)
        iconcolors.append(iconpalette[i]);

    setupImageHandlers();
    btndown = isselecting = ispasting = modified = false;

    img->create(32, 32, 32);
    img->setAlphaBuffer(true);
    clearImage(img);

    currentcolor = qRgb(0,0,0)|OPAQUE_MASK;
    emit colorSelected(currentcolor);

    setMouseTracking(true);

    setNumRows(32);
    setNumCols(32);
    fill(TRANSPARENT);

    connect( kapp->clipboard(), SIGNAL(dataChanged()), SLOT(checkClipboard()));
    connect( h, SIGNAL(commandExecuted()), this, SLOT(updatePreviewPixmap() ));    
    createCursors();

    KIconEditProperties *props = KIconEditProperties::self();

    setTransparencyDisplayType(props->transparencyDisplayType());
    setTransparencySolidColor(props->transparencySolidColor());
    setCheckerboardColor1(props->checkerboardColor1());
    setCheckerboardColor2(props->checkerboardColor2());
    setCheckerboardSize(props->checkerboardSize());
}

KIconEditGrid::~KIconEditGrid()
{
    kdDebug(4640) << "KIconEditGrid - destructor: done" << endl;
}

void KIconEditGrid::paintEvent(QPaintEvent *e)
{
  const QRect cellsRect(0, 0, numCols() * cellSize(), numRows() * cellSize());
  const QRect paintCellsRect = cellsRect.intersect(e->rect());

  if(!paintCellsRect.isEmpty())
  {
    //QTime time;

    //time.start();

    QRgb *imageBuffer = new QRgb[paintCellsRect.width() * paintCellsRect.height()];
    const int cellsize = cellSize();
    const int firstCellPixelsRemaining = cellsize - paintCellsRect.left() % cellsize;

    if(transparencyDisplayType() == TRD_SOLIDCOLOR)
    {
      const QRgb backgroundColor = transparencySolidColor().rgb();
      const int backgroundRed = transparencySolidColor().red();
      const int backgroundGreen = transparencySolidColor().green();
      const int backgroundBlue = transparencySolidColor().blue();
      const int firstCellX = paintCellsRect.left() / cellsize;

      for(int y = paintCellsRect.top(); y <= paintCellsRect.bottom(); y++)
      {
        QRgb *dest = imageBuffer + (y - paintCellsRect.top()) * paintCellsRect.width();
        
        if(y % cellsize == 0 || dest == imageBuffer)
        {
          // Paint the first scanline in each block of cellSize() identical lines.
          // The remaineder can just be copied from this one.
          const int cellY = y / cellsize;
          QRgb *src = gridcolors.data() + cellY * numCols() + firstCellX;

          QRgb sourcePixel = *src++;
          int sourceAlpha = qAlpha(sourcePixel);
          
          QRgb c;

          if(sourceAlpha == 255)
          {
            c = sourcePixel;
          }
          else
          if(sourceAlpha == 0)
          {
            c = backgroundColor;
          }
          else
          {
            const int sourceRed = qRed(sourcePixel);
            const int sourceGreen = qGreen(sourcePixel);
            const int sourceBlue = qBlue(sourcePixel);

            int r = (sourceAlpha * (sourceRed - backgroundRed)) + 0x80;
            r = backgroundRed + ((r + (r >> 8)) >> 8);

            int g = (sourceAlpha * (sourceGreen - backgroundGreen)) + 0x80;
            g = backgroundGreen + ((g + (g >> 8)) >> 8);

            int b = (sourceAlpha * (sourceBlue - backgroundBlue)) + 0x80;
            b = backgroundBlue + ((b + (b >> 8)) >> 8);

            c = qRgb(r, g, b);
          }

          int cellPixelsRemaining = firstCellPixelsRemaining;

          for(int x = paintCellsRect.left(); x <= paintCellsRect.right(); x++)
          {
            if(cellPixelsRemaining == 0)
            {
              cellPixelsRemaining = cellsize;

              // Fetch the next source pixel
              sourcePixel = *src++;
              sourceAlpha = qAlpha(sourcePixel);

              if(sourceAlpha == 255)
              {
                c = sourcePixel;
              }
              else
              if(sourceAlpha == 0)
              {
                c = backgroundColor;
              }
              else
              {
                const int sourceRed = qRed(sourcePixel);
                const int sourceGreen = qGreen(sourcePixel);
                const int sourceBlue = qBlue(sourcePixel);

                //int r = backgroundRed + (sourceAlpha * (sourceRed - backgroundRed)) / 255;
                //int g = backgroundGreen + (sourceAlpha * (sourceGreen - backgroundGreen)) / 255;
                //int b = backgroundBlue + (sourceAlpha * (sourceBlue - backgroundBlue)) / 255;

                int r = (sourceAlpha * (sourceRed - backgroundRed)) + 0x80;
                r = backgroundRed + ((r + (r >> 8)) >> 8);

                int g = (sourceAlpha * (sourceGreen - backgroundGreen)) + 0x80;
                g = backgroundGreen + ((g + (g >> 8)) >> 8);

                int b = (sourceAlpha * (sourceBlue - backgroundBlue)) + 0x80;
                b = backgroundBlue + ((b + (b >> 8)) >> 8);

                c = qRgb(r, g, b);
              }
            }

            cellPixelsRemaining--;

            *dest++ = c;
          }
        }
        else
        {
          // Copy the scanline above.
          memcpy(dest, dest - paintCellsRect.width(), paintCellsRect.width() * sizeof(QRgb));
        }
      }
    }
    else
    {
      int squareSize;
      const int fixedPointMultiplier = 4;

      if(checkerboardSize() == CHK_SMALL)
      {
        squareSize = (cellSize() * fixedPointMultiplier) / 4;
      }
      else
      if(checkerboardSize() == CHK_MEDIUM)
      {
        squareSize = (cellSize() * fixedPointMultiplier) / 2;
      }
      else
      {
        squareSize = (2 * cellSize() * fixedPointMultiplier) / 2;
      }

      QRgb *color1ScanLine = new QRgb[paintCellsRect.width()];
      QRgb *color2ScanLine = new QRgb[paintCellsRect.width()];
      QRgb *color1Buffer = color1ScanLine;
      QRgb *color2Buffer = color2ScanLine;

      for(int x = paintCellsRect.left(); x <= paintCellsRect.right(); x++)
      {
        if((((x * fixedPointMultiplier) / squareSize) & 1) == 0)
        {
          *color1Buffer++ = checkerboardColor1().rgb();
          *color2Buffer++ = checkerboardColor2().rgb();
        }
        else
        {
          *color1Buffer++ = checkerboardColor2().rgb();
          *color2Buffer++ = checkerboardColor1().rgb();
        }
      }

      const int firstCellX = paintCellsRect.left() / cellsize;
      const int firstCellPixelsRemaining = cellsize - paintCellsRect.left() % cellsize;
      int lastCellY = -1;
      int lastLineFirstSquareColour = 0;

      for(int y = paintCellsRect.top(); y <= paintCellsRect.bottom(); y++)
      {
        QRgb *dest = imageBuffer + (y - paintCellsRect.top()) * paintCellsRect.width();
        const int cellY = y / cellsize;

        int firstSquareColour;
        const QRgb *checkerboardSrc;

        if((((y * fixedPointMultiplier) / squareSize) & 1) == 0)
        {
          firstSquareColour = 1;
          checkerboardSrc = color1ScanLine;
        }
        else
        {
          firstSquareColour = 2;
          checkerboardSrc = color2ScanLine;
        }

        if(cellY == lastCellY && firstSquareColour == lastLineFirstSquareColour)
        {
          // Copy the scanline above.
          memcpy(dest, dest - paintCellsRect.width(), paintCellsRect.width() * sizeof(QRgb));
        }
        else
        {
          QRgb *src = gridcolors.data() + cellY * numCols() + firstCellX;

          QRgb sourcePixel = *src++;
          int sourceRed = qRed(sourcePixel);
          int sourceGreen = qGreen(sourcePixel);
          int sourceBlue = qBlue(sourcePixel);
          int sourceAlpha = qAlpha(sourcePixel);

          int cellPixelsRemaining = firstCellPixelsRemaining;

          for(int x = paintCellsRect.left(); x <= paintCellsRect.right(); x++)
          {
            if(cellPixelsRemaining == 0)
            {
              cellPixelsRemaining = cellsize;

              // Fetch the next source pixel
              sourcePixel = *src++;
              sourceRed = qRed(sourcePixel);
              sourceGreen = qGreen(sourcePixel);
              sourceBlue = qBlue(sourcePixel);
              sourceAlpha = qAlpha(sourcePixel);
            }

            cellPixelsRemaining--;

            QRgb c;

            if(sourceAlpha == 255)
            {
              c = sourcePixel;
            }
            else
            if(sourceAlpha == 0)
            {
              c = *checkerboardSrc;
            }
            else
            {
              const int backgroundColor = *checkerboardSrc;
              const int backgroundRed = qRed(backgroundColor);
              const int backgroundGreen = qGreen(backgroundColor);
              const int backgroundBlue = qBlue(backgroundColor);

              //int r = backgroundRed + (sourceAlpha * (sourceRed - backgroundRed)) / 255;
              //int g = backgroundGreen + (sourceAlpha * (sourceGreen - backgroundGreen)) / 255;
              //int b = backgroundBlue + (sourceAlpha * (sourceBlue - backgroundBlue)) / 255;

              int r = (sourceAlpha * (sourceRed - backgroundRed)) + 0x80;
              r = backgroundRed + ((r + (r >> 8)) >> 8);

              int g = (sourceAlpha * (sourceGreen - backgroundGreen)) + 0x80;
              g = backgroundGreen + ((g + (g >> 8)) >> 8);

              int b = (sourceAlpha * (sourceBlue - backgroundBlue)) + 0x80;
              b = backgroundBlue + ((b + (b >> 8)) >> 8);

              c = qRgb(r, g, b);
            }

            *dest++ = c;
            checkerboardSrc++;
          }
        }

        lastCellY = cellY;
        lastLineFirstSquareColour = firstSquareColour;
      }

      delete [] color1ScanLine;
      delete [] color2ScanLine;
    }

    QImage image((uchar *)(imageBuffer), paintCellsRect.width(), paintCellsRect.height(), 32, 0, 0,
#if X_BYTE_ORDER == X_LITTLE_ENDIAN
      QImage::LittleEndian);
#else
      QImage::BigEndian);
#endif
    Q_ASSERT(!image.isNull());

    QPixmap _pixmap;
    _pixmap.convertFromImage(image);

    QPainter p;
    p.begin(&_pixmap);
    paintForeground(&p, e);
    p.end();

    bitBlt(this, paintCellsRect.left(), paintCellsRect.top(), &_pixmap);

    //kdDebug(4640) << "Image render elapsed: " << time.elapsed() << endl;

    delete [] imageBuffer;
  }
}

void KIconEditGrid::paintForeground(QPainter* p, QPaintEvent* e)
{
    QWMatrix matrix;

    matrix.translate(-e->rect().x(), -e->rect().y());
    p->setWorldMatrix( matrix );

    QRect cellsRect(0, 0, numCols() * cellSize(), numRows() * cellSize());
    QRect paintCellsRect = cellsRect.intersect(e->rect());

    if(!paintCellsRect.isEmpty())
    {
      int firstColumn = paintCellsRect.left() / cellSize();
      int lastColumn = paintCellsRect.right() / cellSize();

      int firstRow = paintCellsRect.top() / cellSize();
      int lastRow = paintCellsRect.bottom() / cellSize();

      p->setPen(QColor(0, 0, 0));
      p->setBrush(QColor(0, 0, 0));

      for(int column = firstColumn; column <= lastColumn; column++)
      {
        for(int row = firstRow; row <= lastRow; row++)
        {
          int x = column * cellSize();
          int y = row * cellSize();

          if((ispasting || isselecting) && isMarked(column, row))
          {
            p->drawWinFocusRect(x + 1, y + 1, cellSize() - 2, cellSize() - 2);
          }
          else
          {
            switch( tool )
            {
              case FilledRect:
              case Rect:
              case Ellipse:
              case Circle:
              case FilledEllipse:
              case FilledCircle:
              case Line:
                if(btndown && isMarked(column, row))
                {
                  if(cellSize() > 1)
                  {
                    p->drawWinFocusRect( x + 1, y + 1, cellSize() - 2, cellSize() - 2);
                  }
                  else
                  {
                    p->drawPoint(x, y);
                  }
                }
                break;

              default:
                break;
            }
          }
        }
      }
    }

    if(hasGrid()&& !(cellSize()==1))
    {
        p->setPen(QColor(0, 0, 0));
        int x = e->rect().x() - ((e->rect().x() % cellSize()) + cellSize());
        if(x < 0) x = 0;
        int y = e->rect().y() - ((e->rect().y() % cellSize()) + cellSize());
        if(y < 0) y = 0;
        int cx = e->rect().right() + cellSize();
        int cy = e->rect().bottom() + cellSize();

        // draw grid lines
        for(int i = x; i < cx; i += cellSize())
            p->drawLine(i, y, i, cy);

        for(int i = y; i < cy; i += cellSize())
            p->drawLine(x, i, cx, i);
    }
}

void KIconEditGrid::mousePressEvent( QMouseEvent *e )
{
    if(!e || (e->button() != LeftButton))
        return;

    int row = findRow( e->pos().y() );
    int col = findCol( e->pos().x() );
    //int cell = row * numCols() + col;

    if(!img->valid(col, row))
        return;

    btndown = true;
    start.setX(col);
    start.setY(row);

    if(ispasting)
    {
        ispasting = false;
        editPaste(true);
    }

    if(isselecting)
    {
        QPointArray a(pntarray.copy());
        pntarray.resize(0);
        drawPointArray(a, Mark);
        emit selecteddata(false);
    }

    switch( tool )
    {
        case SelectRect:
        case SelectCircle:
            isselecting = true;
            break;
        default:
            break;
    }
}

void KIconEditGrid::mouseMoveEvent( QMouseEvent *e )
{
    if(!e) return;

    int row = findRow( e->pos().y() );
    int col = findCol( e->pos().x() );
    int cell = row * numCols() + col;

    if(img->valid(col, row))
    {
        //kdDebug(4640) << col << " X " << row << endl;
        emit poschanged(col, row);
        // for the rulers
        emit xposchanged((col*scaling())+scaling()/2);
        emit yposchanged((row*scaling())+scaling()/2);
    }

    QPoint tmpp(col, row);
    if(tmpp == end) return;

    // need to use intersection of rectangles to allow pasting
    // only that part of clip image which intersects -jwc-
    if(ispasting && !btndown && img->valid(col, row))
    {
        if( (col + cbsize.width()) > (numCols()-1) )
            insrect.setX(numCols()-insrect.width());
        else
            insrect.setX(col);
        if( (row + cbsize.height()) > (numRows()-1) )
            insrect.setY(numRows()-insrect.height());
        else
            insrect.setY(row);

        insrect.setSize(cbsize);
        start = insrect.topLeft();
        end = insrect.bottomRight();
        drawRect(false);
        return;
    }

    if(!img->valid(col, row) || !btndown)
        return;

    end.setX(col);
    end.setY(row);

    if(isselecting)
    {
        if(tool == SelectRect)
            drawRect(false);
        else
            drawEllipse(false);
        return;
    }

    bool erase=false;
    switch( tool )
    {
        case Eraser:
            erase=true;

        case Freehand:
        {
            if( !m_command )
                m_command = new KMacroCommand( i18n("Free Hand") );
            
            if(erase)
                setColor( cell, TRANSPARENT );
            else
                setColor( cell, currentcolor );

            if ( selected != cell )
            {
                setModified( true );
                int prevSel = selected;
                selected = cell;
                QRect area = QRect( col*cellsize,row*cellsize, cellsize, cellsize ).unite( 
                    QRect ( (prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize ) );
                    
                m_command->addCommand( new RepaintCommand( area, this ) );
                DrawCommand* dc = new DrawCommand( col, row, colorAt(cell), img, this );
                RepaintCommand* rp = new RepaintCommand( area, this );
                dc->execute();
                rp->execute();
                m_command->addCommand( dc );
                m_command->addCommand( rp );
            }
            break;
        }
        case Find:
        {
            iconcolors.closestMatch(colorAt(cell));
            if ( selected != cell )
            {
                int prevSel = selected;
                selected = cell;
                update((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize);
                update(col*cellsize,row*cellsize, cellsize, cellsize);
                emit colorSelected(colorAt(selected));
            }
            break;
        }
        case Ellipse:
        case Circle:
        case FilledEllipse:
        case FilledCircle:
        {
            drawEllipse(false);
            break;
        }
        case FilledRect:
        case Rect:
        {
            drawRect(false);
            break;
        }
        case Line:
        {
            drawLine(false, false);
            break;
        }
        case Spray:
        {
            drawSpray(QPoint(col, row));
            setModified(true);
            break;
        }
        default:
            break;
    }

    p = *img;
    emit changed(QPixmap(p));
}

void KIconEditGrid::mouseReleaseEvent( QMouseEvent *e )
{
  if(!e || (e->button() != LeftButton))
    return;

  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  btndown = false;
  end.setX(col);
  end.setY(row);
  int cell = row * numCols() + col;
  bool erase=false;
  switch( tool )
  {
    case Eraser:
        erase=true;
      //currentcolor = TRANSPARENT;
    case Freehand:
    {
      if(!img->valid(col, row))
        return;
      if(erase)
        setColor( cell, TRANSPARENT );
      else
        setColor( cell, currentcolor );
      //if ( selected != cell )
      //{
        setModified( true );
        int prevSel = selected;
        selected = cell;
        update((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize);
        update(col*cellsize,row*cellsize, cellsize, cellsize);
        //updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        //updateCell( row, col, FALSE );
        *((uint*)img->scanLine(row) + col) = colorAt(cell);
        p = *img;
      //}
      
        if( m_command ) {
            history->addCommand( m_command, false );
            m_command = 0;
        }
        
      break;
    }
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    {
      drawEllipse(true);
      break;
    }
    case FilledRect:
    case Rect:
    {
      drawRect(true);
      break;
    }
    case Line:
    {
      drawLine(true, false);
      break;
    }
    case Spray:
    {
      drawSpray(QPoint(col, row));
      break;
    }
    case FloodFill:
    {
      QApplication::setOverrideCursor(waitCursor);
      drawFlood(col, row, colorAt(cell));
      QApplication::restoreOverrideCursor();
      updateColors();
      emit needPainting();
      p = *img;
      break;
    }
    case Find:
    {
      currentcolor = colorAt(cell);
      if ( selected != cell )
      {
        int prevSel = selected;
        selected = cell;
        update((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize);
        update(col*cellsize,row*cellsize, cellsize, cellsize);
        emit colorSelected(currentcolor);
        //updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        //updateCell( row, col, FALSE );
      }

      break;
    }
    default:
      break;
  }
  
  emit changed(QPixmap(p));
  //emit colorschanged(numColors(), data());
}

//void KIconEditGrid::setColorSelection( const QColor &color )
void KIconEditGrid::setColorSelection( uint c )
{
  currentcolor = c;
  emit colorSelected(currentcolor);
}

void KIconEditGrid::loadBlank( int w, int h )
{
  img->create(w, h, 32);
  img->setAlphaBuffer(true);
  clearImage(img);
  setNumRows(h);
  setNumCols(w);
  fill(TRANSPARENT);
  emit sizechanged(numCols(), numRows());
  emit colorschanged(numColors(), data());
  history->clear();
}



void KIconEditGrid::load( QImage *image)
{
    kdDebug(4640) << "KIconEditGrid::load" << endl;

    setUpdatesEnabled(false);

    if(image == 0L)
    {
        QString msg = i18n("There was an error loading a blank image.\n");
        KMessageBox::error(this, msg);
        return;
    }

    *img = image->convertDepth(32);
    img->setAlphaBuffer(true);
    setNumRows(img->height());
    setNumCols(img->width());

    for(int y = 0; y < numRows(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < numCols(); x++, l++)
        {
            setColor((y*numCols())+x, *l, false);
        }
        //kdDebug(4640) << "Row: " << y << endl;
        kapp->processEvents(200);
    }

    updateColors();
    emit sizechanged(numCols(), numRows());
    emit colorschanged(numColors(), data());
    emit changed(pixmap());
    setUpdatesEnabled(true);
    emit needPainting();
    //repaint(viewRect(), false);
    history->clear();
}

const QPixmap &KIconEditGrid::pixmap()
{
    if(!img->isNull())
        p = *img;
    //p.convertFromImage(*img, 0);
    return(p);
}

void KIconEditGrid::getImage(QImage *image)
{
    kdDebug(4640) << "KIconEditGrid::getImage" << endl;
    *image = *img;
}

bool KIconEditGrid::zoomTo(int scale)
{
    QApplication::setOverrideCursor(waitCursor);
    setUpdatesEnabled(false);
    setCellSize( scale );
    setUpdatesEnabled(true);
    emit needPainting();
    QApplication::restoreOverrideCursor();
    emit scalingchanged(cellSize());

    if(scale == 1)
        return false;
    return true;
}

bool KIconEditGrid::zoom(Direction d)
{
    int f = (d == DirIn) ? (cellSize()+1) : (cellSize()-1);
    QApplication::setOverrideCursor(waitCursor);
    setUpdatesEnabled(false);
    setCellSize( f );
    setUpdatesEnabled(true);
    //emit needPainting();
    QApplication::restoreOverrideCursor();

    emit scalingchanged(cellSize());
    if(d == DirOut && cellSize() <= 1)
        return false;
    return true;
}

void KIconEditGrid::checkClipboard()
{
  bool ok = false;
  QImage tmp = clipboardImage(ok);
  if(ok)
    emit clipboarddata(true);
  else
  {
    emit clipboarddata(false);
  }
}

QImage KIconEditGrid::clipboardImage(bool &ok)
{
  //###### Remove me later.
  //Workaround Qt bug -- check whether format provided first.
  //Code below is from QDragObject, to match the mimetype list....

  QStrList fileFormats = QImageIO::inputFormats();
  fileFormats.first();
  bool oneIsSupported = false;
  while ( fileFormats.current() )
  {
    QCString format = fileFormats.current();
    QCString type = "image/" + format.lower();
    if (kapp->clipboard()->data()->provides(type ) )
    {
      oneIsSupported = true;
    }
    fileFormats.next();
  }
  if (!oneIsSupported)
  {
     ok = false;
     return QImage();
  }

  QImage image = kapp->clipboard()->image();
  ok = !image.isNull();
  if ( ok )
  {
      image = image.convertDepth(32);
      image.setAlphaBuffer(true);
  }
  return image;
}


void KIconEditGrid::editSelectAll()
{
    start.setX(0);
    start.setY(0);
    end.setX(numCols()-1);
    end.setY(numRows()-1);
    isselecting = true;
    drawRect(false);
    emit newmessage(i18n("All selected"));
}

void KIconEditGrid::editClear()
{
    clearImage(img);
    fill(TRANSPARENT);
    update();
    setModified(true);
    p = *img;
    emit changed(p);
    emit newmessage(i18n("Cleared"));
}

QImage KIconEditGrid::getSelection(bool cut)
{
    const QRect rect = pntarray.boundingRect();
    int nx = 0, ny = 0, nw = 0, nh = 0;
    rect.rect(&nx, &ny, &nw, &nh);

    QImage tmp(nw, nh, 32);
    tmp.setAlphaBuffer(true);
    clearImage(&tmp);

    int s = pntarray.size();

    for(int i = 0; i < s; i++)
    {
        int x = pntarray[i].x();
        int y = pntarray[i].y();
        if(img->valid(x, y) && rect.contains(QPoint(x, y)))
        {
            *((uint*)tmp.scanLine(y-ny) + (x-nx)) = *((uint*)img->scanLine(y) + x);
            if(cut)
            {
                *((uint*)img->scanLine(y) + x) = TRANSPARENT;
                setColor( (y*numCols()) + x, TRANSPARENT, false );
            }
        }
    }

    QPointArray a(pntarray.copy());
    pntarray.resize(0);
    drawPointArray(a, Mark);
    emit selecteddata(false);
    if(cut)
    {
        updateColors();
        update(rect.x()*cellSize(), rect.y()*cellSize(),
            rect.width()*cellSize(), rect.height()*cellSize());
        p = *img;
        emit changed(p);
        emit colorschanged(numColors(), data());
        emit newmessage(i18n("Selected area cut"));
        setModified(true);
    }
    else
        emit newmessage(i18n("Selected area copied"));

    return tmp;
}

void KIconEditGrid::editCopy(bool cut)
{
    kapp->clipboard()->setImage(getSelection(cut));
    isselecting = false;
}


void KIconEditGrid::editPaste(bool paste)
{
    bool ok = false;
    QImage tmp = clipboardImage(ok);

    KIconEditProperties *props = KIconEditProperties::self();

    if(ok)
    {
        if( (tmp.size().width() > img->size().width())
        || (tmp.size().height() > img->size().height()) )
        {
            if(KMessageBox::warningYesNo(this,
                i18n("The clipboard image is larger than the current"
                " image!\nPaste as new image?"),QString::null,i18n("Paste"), i18n("Do Not Paste")) == 0)
            {
                editPasteAsNew();
            }
            return;
        }
        else if(!paste)
        {
            ispasting = true;
            cbsize = tmp.size();
            return;
            // emit newmessage(i18n("Pasting"));
        }
        else
        {
            //kdDebug(4640) << "KIconEditGrid: Pasting at: " << insrect.x() << " x " << insrect.y() << endl;
            QApplication::setOverrideCursor(waitCursor);

            for(int y = insrect.y(), ny = 0; y < numRows() && ny < insrect.height(); y++, ny++)
            {
                uint *l = ((uint*)img->scanLine(y)+insrect.x());
                uint *cl = (uint*)tmp.scanLine(ny);
                for(int x = insrect.x(), nx = 0; x < numCols() && nx < insrect.width(); x++, nx++, l++, cl++)
                {
                    if(props->pasteTransparent())
                    {
                        *l = *cl;
                    }
                    else
                    {
                        // Porter-Duff Over composition
                        double alphaS = qAlpha(*cl) / 255.0;
                        double alphaD = qAlpha(*l) / 255.0;
  
                        double r = qRed(*cl) * alphaS + (1 - alphaS) * qRed(*l) * alphaD;
                        double g = qGreen(*cl) * alphaS + (1 - alphaS) * qGreen(*l) * alphaD;
                        double b = qBlue(*cl) * alphaS + (1 - alphaS) * qBlue(*l) * alphaD;
                        double a = alphaS + (1 - alphaS) * alphaD;
  
                        // Remove multiplication by alpha

                        if(a > 0)
                        {
                            r /= a;
                            g /= a;
                            b /= a;
                        }
                        else
                        {
                            r = 0;
                            g = 0;
                            b = 0;
                        }
                        
                        int ir = (int)(r + 0.5);

                        if(ir < 0)
                        {
                            ir = 0;
                        }
                        else
                        if(ir > 255)
                        {
                            ir = 255;
                        }
                        
                        int ig = (int)(g + 0.5);

                        if(ig < 0)
                        {
                            ig = 0;
                        }
                        else
                        if(ig > 255)
                        {
                            ig = 255;
                        }
                        
                        int ib = (int)(b + 0.5);

                        if(ib < 0)
                        {
                            ib = 0;
                        }
                        else
                        if(ib > 255)
                        {
                            ib = 255;
                        }
                        
                        int ia = (int)((a * 255) + 0.5);

                        if(ia < 0)
                        {
                            ia = 0;
                        }
                        else
                        if(ia > 255)
                        {
                            ia = 255;
                        }
                        
                        *l = qRgba(ir, ig, ib, ia);
                    }

                    setColor((y*numCols())+x, (uint)*l, false);
                }
            }
            updateColors();
            update(insrect.x()*cellSize(), insrect.y()*cellSize(),
                insrect.width()*cellSize(), insrect.height()*cellSize());

            QApplication::restoreOverrideCursor();

            setModified(true);
            p = *img;
            emit changed(QPixmap(p));
            emit sizechanged(numCols(), numRows());
            emit colorschanged(numColors(), data());
            emit newmessage(i18n("Done pasting"));
        }
    }
    else
    {
        QString msg = i18n("Invalid pixmap data in clipboard!\n");
        KMessageBox::sorry(this, msg);
    }
}


void KIconEditGrid::editPasteAsNew()
{
    bool ok = false;
    QImage tmp = clipboardImage(ok);

    if(ok)
    {
        if(isModified())
        {
            KIconEdit *w = new KIconEdit(tmp);
            Q_CHECK_PTR(w);
        }
        else
        {
            *img = tmp;
            load(img);
            setModified(true);
            //repaint(viewRect(), false);
  
            p = *img;
            emit changed(QPixmap(p));
            emit sizechanged(numCols(), numRows());
            emit colorschanged(numColors(), data());
            emit newmessage(i18n("Done pasting"));
            history->clear();
        }
    }
    else
    {
        QString msg = i18n("Invalid pixmap data in clipboard!\n");
        KMessageBox::error(this, msg);
    }
}


void KIconEditGrid::editResize()
{
    kdDebug(4640) << "KIconGrid::editResize" << endl;
    KResizeDialog *rs = new KResizeDialog(this, 0, QSize(numCols(), numRows()));
    if(rs->exec())
    {
        const QSize s = rs->getSize();
        *img = img->smoothScale(s.width(), s.height());
        load(img);

        setModified(true);
    }
    delete rs;
}


void KIconEditGrid::setSize(const QSize s)
{
    kdDebug(4640) << "::setSize: " << s.width() << " x " << s.height() << endl;

    img->create(s.width(), s.height(), 32);
    img->setAlphaBuffer(true);
    clearImage(img);
    load(img);
}


void KIconEditGrid::createCursors()
{
  QBitmap mask(22, 22);
  QPixmap pix;

  cursor_normal = QCursor(arrowCursor);

  pix = BarIcon("colorpicker-cursor");
  if(pix.isNull())
  {
    cursor_colorpicker = cursor_normal;
    kdDebug(4640) << "KIconEditGrid: Error loading colorpicker-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask();
    pix.setMask(mask);
    cursor_colorpicker = QCursor(pix, 1, 21);
  }

  pix = BarIcon("paintbrush-cursor");
  if(pix.isNull())
  {
    cursor_paint = cursor_normal;
    kdDebug(4640) << "KIconEditGrid: Error loading paintbrush.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask();
    pix.setMask(mask);
    cursor_paint = QCursor(pix, 0, 19);
  }

  pix = BarIcon("fill-cursor");
  if(pix.isNull())
  {
    cursor_flood = cursor_normal;
    kdDebug(4640) << "KIconEditGrid: Error loading fill-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask();
    pix.setMask(mask);
    cursor_flood = QCursor(pix, 3, 20);
  }

  pix = BarIcon("aim-cursor");
  if(pix.isNull())
  {
    cursor_aim = cursor_normal;
    kdDebug(4640) << "KIconEditGrid: Error loading aim-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask();
    pix.setMask(mask);
    cursor_aim = QCursor(pix, 10, 10);
  }

  pix = BarIcon("airbrush-cursor");
  if(pix.isNull())
  {
    cursor_spray = cursor_normal;
    kdDebug(4640) << "KIconEditGrid: Error loading airbrush-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask(true);
    pix.setMask(mask);
    cursor_spray = QCursor(pix, 0, 20);
  }

  pix = BarIcon("eraser-cursor");
  if(pix.isNull())
  {
    cursor_erase = cursor_normal;
    kdDebug(4640) << "KIconEditGrid: Error loading eraser-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask(true);
    pix.setMask(mask);
    cursor_erase = QCursor(pix, 1, 16);
  }
}



void KIconEditGrid::setTool(DrawTool t)
{
    btndown = false;
    tool = t;

    if(tool != SelectRect && tool != SelectCircle)
        isselecting = false;

    switch( tool )
    {
        case SelectRect:
            isselecting = true;
            setCursor(cursor_aim);
            break;
        case SelectCircle:
            isselecting = true;
            setCursor(cursor_aim);
            break;
        case Line:
        case Ellipse:
        case Circle:
        case FilledEllipse:
        case FilledCircle:
        case FilledRect:
        case Rect:
            setCursor(cursor_aim);
            break;
        case Freehand:
            setCursor(cursor_paint);
            break;
        case Spray:
            setCursor(cursor_spray);
            break;
        case Eraser:
            setCursor(cursor_erase);
            break;
        case FloodFill:
            setCursor(cursor_flood);
            break;
        case Find:
            setCursor(cursor_colorpicker);
            break;
        default:
            break;
    }
}


void KIconEditGrid::drawFlood(int x, int y, uint oldcolor)
{
    if((!img->valid(x, y))
    || (colorAt((y * numCols())+x) != oldcolor)
    || (colorAt((y * numCols())+x) == currentcolor))
        return;

    *((uint*)img->scanLine(y) + x) = currentcolor;
    setColor((y*numCols())+x, currentcolor, false);

    setModified(true);

    drawFlood(x, y-1, oldcolor);
    drawFlood(x, y+1, oldcolor);
    drawFlood(x-1, y, oldcolor);
    drawFlood(x+1, y, oldcolor);
    //TODO: add undo 
}


void KIconEditGrid::drawSpray(QPoint point)
{
    int x = (point.x()-5);
    int y = (point.y()-5);

    //kdDebug(4640) << "drawSpray() - " << x << " X " << y << endl;

    pntarray.resize(0);
    int points = 0;
    for(int i = 1; i < 4; i++, points++)
    {
        int dx = (rand() % 10);
        int dy = (rand() % 10);
        pntarray.putPoints(points, 1, x+dx, y+dy);
    }

    drawPointArray(pntarray, Draw);
}


//This routine is from Qt sources -- it's the branch of QPointArray::makeEllipse( int x, int y, int w, int h ) that's not normally compiled
//It seems like KIconEdit relied on the Qt1 semantics for makeEllipse, which broke
//the tool with reasonably recent Qt versions.
//Thankfully, Qt includes the old code #ifdef'd, which is hence included here
static void QPA_makeEllipse(QPointArray& ar, int x, int y, int w, int h )
{						// midpoint, 1/4 ellipse
    if ( w <= 0 || h <= 0 ) {
	if ( w == 0 || h == 0 ) {
	    ar.resize( 0 );
	    return;
	}
	if ( w < 0 ) {				// negative width
	    w = -w;
	    x -= w;
	}
	if ( h < 0 ) {				// negative height
	    h = -h;
	    y -= h;
	}
    }
    int s = (w+h+2)/2;				// max size of xx,yy array
    int *px = new int[s];			// 1/4th of ellipse
    int *py = new int[s];
    int xx, yy, i=0;
    double d1, d2;
    double a2=(w/2)*(w/2),  b2=(h/2)*(h/2);
    xx = 0;
    yy = int(h/2);
    d1 = b2 - a2*(h/2) + 0.25*a2;
    px[i] = xx;
    py[i] = yy;
    i++;
    while ( a2*(yy-0.5) > b2*(xx+0.5) ) {		// region 1
	if ( d1 < 0 ) {
	    d1 = d1 + b2*(3.0+2*xx);
	    xx++;
	} else {
	    d1 = d1 + b2*(3.0+2*xx) + 2.0*a2*(1-yy);
	    xx++;
	    yy--;
	}
	px[i] = xx;
	py[i] = yy;
	i++;
    }
    d2 = b2*(xx+0.5)*(xx+0.5) + a2*(yy-1)*(yy-1) - a2*b2;
    while ( yy > 0 ) {				// region 2
	if ( d2 < 0 ) {
	    d2 = d2 + 2.0*b2*(xx+1) + a2*(3-2*yy);
	    xx++;
	    yy--;
	} else {
	    d2 = d2 + a2*(3-2*yy);
	    yy--;
	}
	px[i] = xx;
	py[i] = yy;
	i++;
    }
    s = i;
    ar.resize( 4*s );				// make full point array
    x += w/2;
    y += h/2;
    for ( i=0; i<s; i++ ) {			// mirror
	xx = px[i];
	yy = py[i];
	ar.setPoint( s-i-1, x+xx, y-yy );
	ar.setPoint( s+i, x-xx, y-yy );
	ar.setPoint( 3*s-i-1, x-xx, y+yy );
	ar.setPoint( 3*s+i, x+xx, y+yy );
    }
    delete[] px;
    delete[] py;
}



void KIconEditGrid::drawEllipse(bool drawit)
{
    if(drawit)
    {
        drawPointArray(pntarray, Draw);
        p = *img;
        emit changed(p);
        return;
    }

    QPointArray a(pntarray.copy());
    int x = start.x(), y = start.y(), cx, cy;

    if(x > end.x())
    {
        cx = x - end.x();
        x = x - cx;
    }
    else
        cx = end.x() - x;
    if(y > end.y())
    {
        cy = y - end.y();
        y = y - cy;
    }
    else
        cy = end.y() - y;

    int d = (cx > cy) ? cx : cy;

    //kdDebug(4640) << x << ", " << y << " - " << d << " " << d << endl;
    pntarray.resize(0);
    drawPointArray(a, Mark);

    if(tool == Circle || tool == FilledCircle || tool == SelectCircle)
        QPA_makeEllipse(pntarray, x, y, d, d);
    else if(tool == Ellipse || tool == FilledEllipse)
        QPA_makeEllipse(pntarray, x, y, cx, cy);

    if((tool == FilledEllipse) || (tool == FilledCircle)
    || (tool == SelectCircle))
    {
        int s = pntarray.size();
        int points = s;
        for(int i = 0; i < s; i++)
        {
            int x = pntarray[i].x();
            int y = pntarray[i].y();
            for(int j = 0; j < s; j++)
            {
                if((pntarray[j].y() == y) && (pntarray[j].x() > x))
                {
                    for(int k = x; k < pntarray[j].x(); k++, points++)
                        pntarray.putPoints(points, 1, k, y);
                    break;
                }
            }
        }
    }

    drawPointArray(pntarray, Mark);

    if(tool == SelectCircle && pntarray.size() > 0 && !ispasting)
        emit selecteddata(true);
}


void KIconEditGrid::drawRect(bool drawit)
{
    if(drawit)
    {
        drawPointArray(pntarray, Draw);
        p = *img;
        emit changed(p);
        return;
    }

    QPointArray a(pntarray.copy());
    int x = start.x(), y = start.y(), cx, cy;

    if(x > end.x())
    {
        cx = x - end.x();
        x = x - cx;
    }
    else
        cx = end.x() - x;
    if(y > end.y())
    {
        cy = y - end.y();
        y = y - cy;
    }
    else
        cy = end.y() - y;

    //kdDebug(4640) << x << ", " << y << " - " << cx << " " << cy << endl;
    pntarray.resize(0);
    drawPointArray(a, Mark); // remove previous marking

    int points = 0;
    bool pasting = ispasting;

    if(tool == FilledRect || (tool == SelectRect))
    {
        for(int i = x; i <= x + (pasting ? cx + 1 : cx); i++)
        {
            for(int j = y; j <= y+cy; j++, points++)
            pntarray.putPoints(points, 1, i, j);
        }
    }
    else
    {
        for(int i = x; i <= x+cx; i++, points++)
            pntarray.putPoints(points, 1, i, y);
        for(int i = y; i <= y+cy; i++, points++)
            pntarray.putPoints(points, 1, x, i);
        for(int i = x; i <= x+cx; i++, points++)
            pntarray.putPoints(points, 1, i, y+cy);
        for(int i = y; i <= y+cy; i++, points++)
            pntarray.putPoints(points, 1, x+cx, i);
    }

    drawPointArray(pntarray, Mark);

    if(tool == SelectRect && pntarray.size() > 0 && !ispasting)
        emit selecteddata(true);
}


void KIconEditGrid::drawLine(bool drawit, bool drawStraight)
{
    if(drawit)
    {
        drawPointArray(pntarray, Draw);
        p = *img;
        emit changed(p);
        return;
    }

    QPointArray a(pntarray.copy());
    pntarray.resize(0);

    // remove previous marking
    drawPointArray(a, Mark);

    int x, y, dx, dy, delta;

    dx = end.x() - start.x();
    dy = end.y() - start.y();
    x = start.x();
    y = start.y();

    delta = QMAX(abs(dx), abs(dy));
    int deltaX = abs(dx);
    int deltaY = abs(dy);

    if ((drawStraight) && (delta > 0))
    {
        dx /= delta;
        dy /= delta;

        for(int i = 0; i <= delta; i++)
        {
            pntarray.putPoints(i, 1, x, y);
            x += dx;
            y += dy;
        }
    }

    else if ((delta > 0) && (deltaX >= deltaY))
    {
        for(int i = 0; i <= deltaX; i++)
        {
            pntarray.putPoints(i, 1, x, y);

            if(dx > 0)
               x++;
            else
               x--;

            if(dy >= 0)
                y = start.y() + (abs(start.x() - x) * deltaY) / deltaX;
            else
                y = start.y() - (abs(start.x() - x) * deltaY) / deltaX;
        }
    }

    else if ((delta > 0) && (deltaY > deltaX))
    {
        for(int i = 0; i <= deltaY; i++)
        {
            pntarray.putPoints(i, 1, x, y);

            if(dy > 0)
                y++;
            else
                y--;

            if(dx >= 0)
                x = start.x() + (abs(start.y() - y) * deltaX) / deltaY;
            else
                x = start.x() - (abs(start.y() - y) * deltaX) / deltaY;
        }
    }

    drawPointArray(pntarray, Mark);
}


void KIconEditGrid::drawPointArray(QPointArray a, DrawAction action)
{
    QRect area( a.boundingRect().x()*cellSize()-1, a.boundingRect().y()*cellSize()-1,
                a.boundingRect().width()*cellSize()+1, a.boundingRect().height()*cellSize()+1 );
    
    KMacroCommand* macro = 0;
    bool doupdate = false;

    if( a.size() > 0 && action == Draw ) {
	// might cause a memmory leak, if
	// macro is never used and never 
	// added to the history! TODO: Fix this
        macro = new KMacroCommand( i18n("Drawn Array") );
	RepaintCommand* rc = new RepaintCommand( area, this );
	macro->addCommand( rc );	
    }
    
    int s = a.size(); //((rect.size().width()) * (rect.size().height()));
    for(int i = 0; i < s; i++)
    {
        int x = a[i].x();
        int y = a[i].y();

        if(img->valid(x, y) && a.boundingRect().contains(a[ i ]))
        {
            //kdDebug(4640) << "x: " << x << " - y: " << y << endl;
            switch( action )
            {
                case Draw:
                {
		    DrawCommand* dc = new DrawCommand( x, y, currentcolor, img, this );
		    dc->execute();
                    //*((uint*)img->scanLine(y) + x) = currentcolor; //colors[cell]|OPAQUE;
                    //int cell = y * numCols() + x;
                    //setColor( cell, currentcolor, false );
                    doupdate = true;
                    //updateCell( y, x, FALSE );
		    macro->addCommand( dc );
                    break;
                }

                case Mark:
                case UnMark:
                    update(x*cellsize,y*cellsize, cellsize, cellsize);
                    //updateCell( y, x, true );
                    break;

                default:
                    break;
            }
        }
    }

    
    if(doupdate)
    {
        setModified( true );
        updateColors();
	RepaintCommand* rc = new RepaintCommand( area, this );
        rc->execute();
	macro->addCommand( rc );
	pntarray.resize(0);
	// add to undo/redo history	
	history->addCommand( macro, false );    }
}

void KIconEditGrid::updatePreviewPixmap()
{
    p = *img;
    emit changed(QPixmap(p));
}


bool KIconEditGrid::isMarked(QPoint point)
{
    return isMarked(point.x(), point.y());
}


bool KIconEditGrid::isMarked(int x, int y)
{
    if(((y * numCols()) + x) == selected)
        return true;

    int s = pntarray.size();
    for(int i = 0; i < s; i++)
    {
        if(y == pntarray[i].y() && x == pntarray[i].x())
            return true;
    }

    return false;
}


// Fast diffuse dither to 3x3x3 color cube
// Based on Qt's image conversion functions
static bool kdither_32_to_8( const QImage *src, QImage *dst )
{
    register QRgb *p;
    uchar  *b;
    int	    y;

	//printf("kconvert_32_to_8\n");

    if ( !dst->create(src->width(), src->height(), 8, 256) ) {
		kdWarning() << "OImage: destination image not valid" << endl;
		return FALSE;
	}

    int ncols = 256;

    static uint bm[16][16];
    static int init=0;
    if (!init)
    {
		// Build a Bayer Matrix for dithering
		init = 1;
		int n, i, j;

		bm[0][0]=0;

		for (n=1; n<16; n*=2)
        {
	    	for (i=0; i<n; i++)
            {
			    for (j=0; j<n; j++)
                {
		    	    bm[i][j]*=4;
		    	    bm[i+n][j]=bm[i][j]+2;
		    	    bm[i][j+n]=bm[i][j]+3;
		    	    bm[i+n][j+n]=bm[i][j]+1;
			    }
	    	}
		}

		for (i=0; i<16; i++)
	    	for (j=0; j<16; j++)
			    bm[i][j]<<=8;
    }

    dst->setNumColors( ncols );

#define MAX_R 2
#define MAX_G 2
#define MAX_B 2
#define INDEXOF(r,g,b) (((r)*(MAX_G+1)+(g))*(MAX_B+1)+(b))

	int rc, gc, bc;

	for ( rc=0; rc<=MAX_R; rc++ )		// build 2x2x2 color cube
	    for ( gc=0; gc<=MAX_G; gc++ )
		    for ( bc=0; bc<=MAX_B; bc++ )
            {
		        dst->setColor( INDEXOF(rc,gc,bc),
			    qRgb( rc*255/MAX_R, gc*255/MAX_G, bc*255/MAX_B ) );
		    }

	int sw = src->width();
	int* line1[3];
	int* line2[3];
	int* pv[3];

	line1[0] = new int[src->width()];
	line2[0] = new int[src->width()];
	line1[1] = new int[src->width()];
	line2[1] = new int[src->width()];
	line1[2] = new int[src->width()];
	line2[2] = new int[src->width()];
	pv[0] = new int[sw];
	pv[1] = new int[sw];
	pv[2] = new int[sw];

	for ( y=0; y < src->height(); y++ )
    {
	    p = (QRgb *)src->scanLine(y);
	    b = dst->scanLine(y);
		int endian = (QImage::systemByteOrder() == QImage::BigEndian);
		int x;
		uchar* q = src->scanLine(y);
		uchar* q2 = src->scanLine(y+1 < src->height() ? y + 1 : 0);
		for (int chan = 0; chan < 3; chan++)
        {
		    b = dst->scanLine(y);
		    int *l1 = (y&1) ? line2[chan] : line1[chan];
		    int *l2 = (y&1) ? line1[chan] : line2[chan];
		    if ( y == 0 )
            {
			    for (int i=0; i<sw; i++)
			        l1[i] = q[i*4+chan+endian];
		    }
		    if ( y+1 < src->height() )
            {
			    for (int i=0; i<sw; i++)
			        l2[i] = q2[i*4+chan+endian];
		    }
		    // Bi-directional error diffusion
		    if ( y&1 )
            {
			    for (x=0; x<sw; x++)
                {
			        int pix = QMAX(QMIN(2, (l1[x] * 2 + 128)/ 255), 0);
			        int err = l1[x] - pix * 255 / 2;
			        pv[chan][x] = pix;

			        // Spread the error around...
			        if ( x+1<sw )
                    {
				        l1[x+1] += (err*7)>>4;
				        l2[x+1] += err>>4;
			        }
			        l2[x]+=(err*5)>>4;
			        if (x>1)
				        l2[x-1]+=(err*3)>>4;
			    }
		    }
            else
            {
			    for (x=sw; x-->0; )
                {
			        int pix = QMAX(QMIN(2, (l1[x] * 2 + 128)/ 255), 0);
			        int err = l1[x] - pix * 255 / 2;
			        pv[chan][x] = pix;

			        // Spread the error around...
			        if ( x > 0 )
                    {
				        l1[x-1] += (err*7)>>4;
				        l2[x-1] += err>>4;
			        }
			        l2[x]+=(err*5)>>4;
			        if (x+1 < sw)
				        l2[x+1]+=(err*3)>>4;
			    }
		    }
		}
		if (endian)
        {
		    for (x=0; x<sw; x++)
            {
			    *b++ = INDEXOF(pv[2][x],pv[1][x],pv[0][x]);
		    }
		}
        else
        {
		    for (x=0; x<sw; x++)
            {
			    *b++ = INDEXOF(pv[0][x],pv[1][x],pv[2][x]);
		    }
		}
	}

	delete [] line1[0];
	delete [] line2[0];
	delete [] line1[1];
	delete [] line2[1];
	delete [] line1[2];
	delete [] line2[2];
	delete [] pv[0];
	delete [] pv[1];
	delete [] pv[2];

#undef MAX_R
#undef MAX_G
#undef MAX_B
#undef INDEXOF

    return TRUE;
}

// this doesn't work the way it should but the way KPixmap does.
void KIconEditGrid::mapToKDEPalette()
{
    QImage dest;

    kdither_32_to_8(img, &dest);
    *img = dest.convertDepth(32);

    for(int y = 0; y < img->height(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < img->width(); x++, l++)
        {
            if(*l < 0xff000000)
            {
                *l = *l | 0xff000000;
            }
        }
    }

    load(img);
    return;

/*
#if QT_VERSION > 140
  *img = img->convertDepthWithPalette(32, iconpalette, 42);
  load(img);
  return;
#endif
*/

    QApplication::setOverrideCursor(waitCursor);
    for(int y = 0; y < numRows(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < numCols(); x++, l++)
        {
            if(*l != TRANSPARENT)
            {
                if(!iconcolors.contains(*l))
                    *l = iconcolors.closestMatch(*l);
            }
        }
    }

    load(img);
    setModified(true);
    QApplication::restoreOverrideCursor();
}


void KIconEditGrid::grayScale()
{
    for(int y = 0; y < numRows(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < numCols(); x++, l++)
        {
            if(*l != TRANSPARENT)
            {
                uint c = qGray(*l);
                *l = qRgba(c, c, c, qAlpha(*l));
            }
        }
    }

    load(img);
    setModified(true);
}


void KIconEditGrid::clearImage(QImage *image)
{
    if(image->depth() != 32)
    {
        image->fill(TRANSPARENT);
    }
    else
    {
        // QImage::fill() does not set the alpha channel so do it
        // manually.
        for(int y = 0; y < image->height(); y++)
        {
            uint *l = (uint*)image->scanLine(y);
            for(int x = 0; x < image->width(); x++, l++)
            {
                *l = TRANSPARENT;
            }
        }
    }
}


void KIconEditGrid::setModified(bool m)
{
    if(m != modified)
    {
        modified = m;
        emit modifiedchanged(m);
    }
}


#include "kicongrid.moc"
// vim: set ts=4:
