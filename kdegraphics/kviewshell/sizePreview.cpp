// SizePreview.cpp
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2002 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#include <config.h>

#include <kdebug.h>
#include <krandomsequence.h>
#include <qevent.h>
#include <qpainter.h>

#include "sizePreview.h"

#include "simplePageSize.h"

SizePreview::SizePreview( QWidget *parent, const char *name, WFlags)
    : QWidget( parent, name, WStaticContents | WNoAutoErase )
{
  // Set a sane default
  _width = _height = 50.0;
  orientation = 0; // Portrait
}

void SizePreview::setSize(const SimplePageSize& size)
{
  _width = size.width().getLength_in_mm();
  _height = size.height().getLength_in_mm();

  if (_width < 50.0)
    _width = 50.0;
  if (_width > 1200.0)
    _width = 1200.0;

  if (_height < 50.0)
    _height = 50.0;
  if (_height > 1200.0)
    _height = 1200.0;

  update();
}

void SizePreview::resizeEvent(QResizeEvent*)
{
  update();
}

void SizePreview::paintEvent( QPaintEvent * )
{
  int displayedWidth, displayedHeight;

  // Figure the width and height of the displayed page. Tricky.
  if (orientation == 0) {
    displayedWidth  = (int)(height() * (_width/_height) + 0.5);
    displayedHeight = (int)(width() * (_height/_width) + 0.5);
  } else {
    displayedHeight  = (int)(height() * (_width/_height) + 0.5);
    displayedWidth = (int)(width() * (_height/_width) + 0.5);
  }
  if (displayedWidth <= width()) 
    displayedHeight = height();
  else
    displayedWidth = width();

  int hOffset = (width()-displayedWidth)/2;
  int vOffset = (height()-displayedHeight)/2;


  // Now draw the graphics
  pixmap.resize(width(), height());

  QPainter p(&pixmap);
  p.fillRect(rect(), colorGroup().background());
  p.setPen(Qt::black);
  p.setBrush(Qt::white);
  p.drawRect(hOffset, vOffset, displayedWidth, displayedHeight);

  // mark the textbox; we assume 25mm margin
  int margin = (int)(25.0*displayedWidth/_width + 0.5);
  QRect textBox(hOffset+margin, vOffset+margin, displayedWidth-2*margin, displayedHeight-2*margin);
  p.setPen(Qt::lightGray);
  p.drawRect(textBox);

  // Draw some dummy "text", represented by black lines
  int lineSpacing    = (int)(7.0*displayedWidth/_width + 0.5); // equiv. 7 mm
  if (lineSpacing <= 3)
    lineSpacing = 3;
  int interWordSpace = (int)(4.0*displayedWidth/_width + 0.5); // equiv. 4 mm
  if (interWordSpace <= 1)
    interWordSpace = 2;

  KRandomSequence rnd(1); // to generate word widths

  p.setClipRect(textBox);
  p.setPen(Qt::black);
  int count = 1; // Counts lines
  for (int y = vOffset+margin+lineSpacing; y <= vOffset+displayedHeight-margin; y += lineSpacing) {
    // We start each line with its own seed.
    // This means that the random sequence for each line is always the same, and this
    // results in a more steady picture when the widget is resized.
    rnd.setSeed(count);

    // Every 10th line the end of a paragraph
    int endParagraph;
    if (count++ % 10 == 0)
      endParagraph = (int)(50.0*displayedWidth/_width + 0.5);
    else
      endParagraph = 0;
    for(int x = hOffset+margin; x <= hOffset+displayedWidth-margin-endParagraph; ) {
      double wordWidthMM = rnd.getDouble()*30.0+10.0;
      int wordWidth = (int)(wordWidthMM*displayedWidth/_width + 0.5); 
      p.drawLine(x, y, x+wordWidth, y);
      x += wordWidth+interWordSpace+1;
    }
  }

  p.end();

  // Copy the pixmap onto the widget
  bitBlt(this, rect().topLeft(), &pixmap, rect(), CopyROP);
  return;
}

void SizePreview::setOrientation(int ori)
{
  orientation = ori;
  update();
}

#include "sizePreview.moc"
