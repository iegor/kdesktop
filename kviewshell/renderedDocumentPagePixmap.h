// -*- C++ -*-
//
// Class: RenderedDocumentPagePixmap
//
// Widget for displaying TeX DVI files.
// Part of KDVI- A previewer for TeX DVI files.
//
// (C) 2004-2005 Stefan Kebekus. Distributed under the GPL.


#ifndef _rendereddocumentpagepixmap_h_
#define _rendereddocumentpagepixmap_h_

#include "renderedDocumentPage.h"

#include <qpainter.h>
#include <qpixmap.h>


// This class contains everything documentRenderer needs to know 
// about a certain page.
class RenderedDocumentPagePixmap: public RenderedDocumentPage, public QPixmap
{
  Q_OBJECT

public:
  RenderedDocumentPagePixmap();

  virtual ~RenderedDocumentPagePixmap();

  /** Returns a pointer to the paintDevice (in most implementation
      either a QPixmap, or a QPrinter). The pointer returned is valid
      as long as *this RenderedDocumentPage exists. This method is
      used by the renderer to draw on the page. */
  virtual QPainter *getPainter();

  /** This implementation delete the QPainter *pt (and thereby initiates the drawing) */
  virtual void returnPainter(QPainter *pt) {delete pt;}

  QPixmap accessiblePixmap();

  void resize(const QSize& size);
  void resize(int width, int height);

  virtual QSize size() { return QPixmap::size(); }
  virtual int width() { return QPixmap::width(); }
  virtual int height() { return QPixmap::height(); }

  /** Returns the amount of memory used by this pixmap */
  unsigned int memory();

private:
  QPixmap* _accessiblePixmap;
  /** true if _accessiblePixmap needs updateing */
  bool dirty;
};


#endif
