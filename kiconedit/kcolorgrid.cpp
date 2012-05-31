/*
    kiconedit - a small graphics drawing program for creating KDE icons
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

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

#include <qpainter.h>

#include <kdebug.h>

#include "kcolorgrid.h"

void KColorArray::remove(int idx)
{
  int oldsize = size();
  if(idx >= (int)size())
  {
    kdWarning() << "KColorArray::remove: Index " << idx << " out of range" << endl;
    return;
  }
  KColorArray tmp(*this);
  tmp.detach();
  resize(size()-1);
  for(int i = idx; i < oldsize-1; i++)
    at(i) = tmp[i+1];
  //kdDebug(4640) << "KColorArray::remove() " << at(idx) << "\t-\tsize: " << size() << endl;
}

void KColorArray::append(uint c)
{
  resize(size()+1);
  at(size()-1) = c;
  //kdDebug(4640) << "KColorArray::append() " << c << "\t-\tsize: " << size() << endl;
}

uint KColorArray::closestMatch(uint color)
{
  //kdDebug(4640) << "KColorArray: " << c << endl;
  uint c = color & ~OPAQUE_MASK, d = 0xffffff, t;
  //kdDebug(4640) << "KColorArray: " << c << endl;
  //kdDebug(4640) << "KColorArray: " << c|OPAQUE_MASK << endl;
  uint cb = c;
  for(uint i = 0; i < size(); i++)
  {
    if (at(i) > cb)
      t = at(i) - cb;
    else
      t = cb - at(i);
    if( t < d )
    {
      d = t;
      c = at(i);
    }
  }
  return c|OPAQUE_MASK;
}

KColorGrid::KColorGrid(QWidget *parent, const char *name, int space)
 : QWidget(parent, name, Qt::WResizeNoErase|Qt::WRepaintNoErase)
{
  //kdDebug(4640) << "KColorGrid - constructor" << endl;
  s = space;
  rows = cols = totalwidth = totalheight = 0;
  setCellSize(10);
  setGridState(Plain);
  setGrid(true);
  numcolors.resize(0);
  gridcolors.resize(0);

  //kdDebug(4640) << "KColorGrid - constructor - done" << endl;
}
/*
void KColorGrid::show()
{
  //updateScrollBars();
  QWidget::show();
}
*/
void KColorGrid::paintEvent(QPaintEvent *e)
{
  //kdDebug(4640) << "KColorGrid::paintEvent" << endl;

  //updateScrollBars();
  //QWidget::paintEvent(e);

  const QRect urect = e->rect();

  //kdDebug(4640) << "Update rect = ( " << //urect.left() << ", " << urect.top() << ", " << urect.width() << ", " << urect.height() << " )" << endl;


  int firstcol = getX(urect.x())-1;
  int firstrow = getY(urect.y())-1;
  int lastcol  = getX(urect.right())+1;
  int lastrow  = getY(urect.bottom())+1;

  QWMatrix matrix;
  QPixmap pm(urect.width(),urect.height());
  pm.fill(paletteBackgroundColor());
  QPainter p;
  p.begin( &pm );

  firstrow = (firstrow < 0) ? 0 : firstrow;
  firstcol = (firstcol < 0) ? 0 : firstcol;
  lastrow = (lastrow >= rows) ? rows : lastrow;
  lastcol = (lastcol >= cols) ? cols : lastcol;
  //kdDebug(4640) << urect.x() << " x " << urect.y() << "  -  row: " << urect.width() << " x " << urect.height() << endl;
  //kdDebug(4640) << "col: " << firstcol << " -> " << lastcol << "  -  row: " << firstrow << " -> " << lastrow << endl;

/*
  if(this->isA("KDrawGrid"))
    kdDebug(4640) << "KDrawGrid\n   firstcol: " << firstcol << "\n   lastcol: " << lastcol << "\n   firstrow: " << firstrow << "\n   lastrow: " << lastrow << endl;
*/
  for(int i = firstrow; i < lastrow; i++)
  {
    //if(this->isA("KDrawGrid"))
    //  kdDebug(4640) << "Updating row " << i << endl;
    for(int j = firstcol; j < lastcol; j++)
    {
      matrix.translate( (j*cellsize)-urect.x(), (i*cellsize)-urect.y() );
      p.setWorldMatrix( matrix );
      //p.setClipRect(j*cellsize, i*cellsize, cellsize, cellsize);
      paintCell(&p, i, j);
      //p.setClipping(FALSE);
      matrix.reset();
      p.setWorldMatrix( matrix );
    }
    //kapp->processEvents();
  }

  matrix.translate(-urect.x(),-urect.y());
  p.setWorldMatrix( matrix );
  paintForeground(&p,e);
  
  p.end();

  bitBlt(this,urect.topLeft(),&pm,QRect(0,0,pm.width(),pm.height()));

}

void KColorGrid::paintForeground(QPainter* , QPaintEvent* )
{
}

/*
void KColorGrid::resizeEvent(QResizeEvent *)
{
  //kdDebug(4640) << "resize: width:  " << width() << " - total: " << totalwidth << endl;
  //kdDebug(4640) << "resize: height: " << height() << " - total: " << totalheight << endl;
}
*/

QSize KColorGrid::sizeHint() const
{
  return QSize(totalwidth, totalheight);
}

int KColorGrid::getY( int y )
{
  if(y > (totalheight-1))
    y = totalheight;
  if(cellsize == 1)
    return y;
  return (y/cellsize);
}

int KColorGrid::getX( int x )
{
  if( x > totalwidth-1)
    x = totalwidth;
  if(cellsize == 1)
    return x;
  return (x/cellsize);
}

const QRect KColorGrid::viewRect()
{
  //kdDebug(4640) << "viewRect" << endl;
  const QRect r(0, 0, width(), height());
  //kdDebug(4640) << "viewRect - " << x << " x " << y << " - " << w << " x " << h << endl;
  return r;
}

void KColorGrid::setNumRows(int n)
{
  //kdDebug(4640) << "setNumRows" << endl;
  if(n < 0 || n == rows)
    return;

  rows = n;

  gridcolors.resize(n*numCols());  
  //QTableView::setNumRows(n);
  totalheight = (n * cellsize) + 1;
  resize(totalwidth, totalheight);
  //kdDebug(4640) << "setNumRows() - gridcolors: " << gridcolors.size() << "  size: " << numCols()*numRows() << endl;
}

void KColorGrid::setNumCols(int n)
{
  //kdDebug(4640) << "setNumCols" << endl;
  if(n < 0)
    return;
  int on = numCols();
  KColorArray gc(gridcolors);
  gc.detach();
  //kdDebug(4640) << "gc size: " << gc.size() << " numrows: " << numRows() << endl;
  gridcolors.resize(n*numRows());
  cols = n;

  totalwidth = (n * cellsize) + 1;
  resize(totalwidth, totalheight);
  //kdDebug(4640) << "numRows: " << numRows() << endl;
  //kdDebug(4640) << "gridcolor: " << gridcolors.size() << "  grid: " << numRows()*numCols() << endl;
  if(numRows() == 0)
    return;

  for(int i = 0; i < numRows(); i++)
  {
    for(int j = 0; j < n; j++)
    {
      //kdDebug(4640) << "row " << i << " , col " << j << endl;
      if(j < on ) //If there's something to read here -- i.e. we're within the original grid
      {
        //kdDebug(4640) << (i*numCols())+j << " " << (i*on)+j << endl;
        gridcolors.at((i*numCols())+j) = gc.at((i*on)+j);
      }
      else //Initialize to something..
      {
        if (gc.size()) //Have some pixels originally..
            gridcolors.at((i*numCols())+j) = gc.at(0);
        else 
            gridcolors.at((i*numCols())+j) = 0; //Picks something #### Update numcolors?
      } 
    }
  }

  //kdDebug(4640) << "setNumCols() - gridcolors: " << gridcolors.size() << "  size: " << numCols()*numRows() << endl;
}

void KColorGrid::fill( uint color)
{
  gridcolors.fill(color);
  numcolors.resize(1);
  numcolors.at(0) = color;
  emit colorschanged(numcolors.size(), numcolors.data());
}

void KColorGrid::setColor( int colNum, uint col, bool update )
{
  //kdDebug(4640) << "KColorGrid::setColor" << endl;
  uint oldcolor = gridcolors[colNum];
  gridcolors[colNum] = col;

  if(!update)
    return;

  //kdDebug(4640) << "KColorGrid::setColor - before adding" << endl;
  if(!numcolors.contains(col))
  {
    //kdDebug(4640) << "KColorGrid::setColor() - adding " << //  col << " - " << qRed(col) << " " << qGreen(col) << " " << qBlue(col) << endl;
    numcolors.append(col);
    //kdDebug(4640) << "KColorGrid::setColor() - adding done " << numcolors.size()-1 << endl;
    //numcolors++;
    emit addingcolor(col);
  }  

  //kdDebug(4640) << "KColorGrid::setColor - before removing" << endl;
  if(!gridcolors.contains(oldcolor))
  {
    int idx = numcolors.find(oldcolor);
    if(idx != -1)
    {
      //kdDebug(4640) << "KColorGrid::setColor() - removing " << //  oldcolor << " - " << qRed(oldcolor) << " " << qGreen(oldcolor) << " " << qBlue(oldcolor) << endl;
      numcolors.remove(idx);
      //kdDebug(4640) << "KColorGrid::setColor() - removing done" << endl;
      emit colorschanged(numcolors.size(), numcolors.data());
    }
    //numcolors--;
  }

  //kdDebug(4640) << "KColorGrid::setColor - before updateCell" << endl;
  repaint((colNum%numCols())*cellsize,(colNum/numCols())*cellsize,  cellsize, cellsize);
  //updateCell( colNum/numCols(), colNum%numCols(), false );
  //kdDebug(4640) << "KColorGrid::setColor - after updateCell" << endl;
}

void KColorGrid::updateCell( int row, int col, bool  )
{
    //kdDebug(4640) << "updateCell - before repaint" << endl;
  QWMatrix matrix;
  QPainter p;
  p.begin( this );
  matrix.translate( (col*cellsize), (row*cellsize) );
  p.setWorldMatrix( matrix );
  paintCell(&p, row, col);
  p.end();

}

void KColorGrid::updateColors()
{
  numcolors.resize(0);
  for(int i = 0; i < (int)gridcolors.size(); i++)
  {
    uint col = gridcolors.at(i);
    if(!numcolors.contains(col))
      numcolors.append(col);
  }
  emit colorschanged(numcolors.size(), numcolors.data());
}

void KColorGrid::setCellSize( int s )
{
  cellsize = s;
  totalwidth = (numCols() * s) + 1;
  totalheight = (numRows() * s) + 1;
  resize(totalwidth, totalheight);
  if ( isVisible() )
    repaint(viewRect(), false);
}
#include "kcolorgrid.moc"
