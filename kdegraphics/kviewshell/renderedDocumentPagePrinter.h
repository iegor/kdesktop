// -*- C++ -*-
//
// Class: RenderedDocumentPagePrinter
//
// Widget for displaying TeX DVI files.
// Part of KDVI- A previewer for TeX DVI files.
//
// (C) 2005 Stefan Kebekus. Distributed under the GPL.


#ifndef _rendereddocumentpageprinter_h_
#define _rendereddocumentpageprinter_h_

#include "renderedDocumentPage.h"
#include <qpainter.h>

class KPrinter;


class RenderedDocumentPagePrinter: public RenderedDocumentPage
{
public:
  RenderedDocumentPagePrinter(KPrinter *kp);

  virtual ~RenderedDocumentPagePrinter() { delete printerPainter; }

  /** Returns a pointer to the paintDevice, in this implementation
      either a QPrinter. The pointer returned is valid as long as
      *this RenderedDocumentPage exists. This method is used by the
      renderer to draw on the page. */
  virtual QPainter *getPainter() { return printerPainter; }

  virtual QSize size();

  /** This method does nothing. In particular, it does NOT delete the
      QPainter. The QPainter is deleted automatically when this
      RenderedDocumentPagePrinter is destructed. */
  virtual void returnPainter(QPainter *) {;}

private:
  KPrinter *printer;
  QPainter *printerPainter;
};


#endif
