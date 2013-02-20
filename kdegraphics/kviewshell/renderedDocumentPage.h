// -*- C++ -*-
//
// Class: documentPage
//
// Widget for displaying TeX DVI files.
// Part of KDVI- A previewer for TeX DVI files.
//
// (C) 2004-2005 Stefan Kebekus. Distributed under the GPL.


#ifndef _rendereddocumentpage_h_
#define _rendereddocumentpage_h_

#include "pageNumber.h"

#include <qobject.h>
#include <qstring.h>
#include <qvaluevector.h>

class Hyperlink;
class QPainter;
class QRect;
class QRegion;
class TextBox;
class TextSelection;

/** @short Represents a page in a document, contains the page's graphical
    representation and resolution-dependent information.

    This abstract class represents a page in a document. Its
    implementations contain graphical representations of document pages,
    and resolution-dependent information. The most important members are

    - the page number

    - a vector that contains hyperlinks

    - a vector for source hyperlinks; this vector is currently used by
      KDVI only, and is therefore not documented much here.

    - a vector that contains the document text together with
    information about the position of the text on the page. This
    information is used in kviewshell e.g. for selecting text with the
    mouse or for the "export as text..." functionality

    - the implementations of the documentPage will also contain a
    QPaintDevice, onto which the graphical representation of the page
    is rendered. This could for instance be a QPixmap, if the page is
    to be displayed on a computer screen, or a QPrinter for printing.

    The kviewshell program uses the documentPage class in the
    following manner: first, it is constructed, the page number is
    invalid. The page number is then set to a reasonable value using
    the setPageNumber() method. After this, the
    documentRenderer.drawPage() method of an implementation of the
    documentRenderer is used to render a graphical representation into
    the QPaintDevice at a given resolution, and to fill the
    (source-)hyperlink and text vectors, which are also
    resolution-dependent. After the data has been used, the
    documentPage is clear()ed, perhaps a new page number set, and
    documentRenderer.drawPage() is used again.

    @author Stefan Kebekus <kebekus@kde.org>
    @version 1.0.0
*/

class RenderedDocumentPage : public QObject
{
  Q_OBJECT

 public:
  /** \brief Default constructor

  The default constructor constructs a RenderedDocumentPage whose pageNumber
  is invalid. It sets the member 'isEmpty' to 'true'.
  */
  RenderedDocumentPage();

  virtual ~RenderedDocumentPage();

  /** \brief Set page number

  This method sets the number of the page that this instance
  represents. A value of '0' means that this class does not represent
  any page ('invalid page'). This method also calls clear().
  */
  void setPageNumber(const PageNumber& pagenr);

  /** \brief Get page number

  @returns The number of the page that this instance represents.
  */
  PageNumber getPageNumber() const {return pageNr;}

  /** \brief Get paint device

  This method is used for instance by the documentRenderer.drawPage()
  method to draw on the page. It returns a pointer to a QPainter (in
  most implementation a painter on either a QPixmap, or a
  QPrinter). The pointer returned points to an active QPainter and is
  valid as long as *this RenderedDocumentPage exists. The pointer
  returned must not be deleted nor should QPainter::end() be
  called. Instead, the method returnPainter() must be called as soon
  as drawing is finished.

  In short, to draw on a renderedDocumentPage, you do the following:
  @code
  RenderedDocumentPage rdp;

  QPainter *paint = rdp.getPainter()
  if (paint != 0) {
  
    <Do some painting, don't call paint->end() >
  
    rdp.returnPainter(paint);
  }
  @endcode

  @warning Before the pointer has been returned by the returnPainter()
  method, all subsequent calls to getPainter() will return undefined
  results. Do not delete a renderedDocumentPage when you still hold a
  pointer. Call returnPainter() first.

  @returns A pointer to a QPainter (in most implementation a painter
  on either a QPixmap, or a QPrinter), or NULL, if painting is not
  possible. The QPainter object is owned by the RenderedDocumentPage.
  */
  virtual QPainter *getPainter() = 0;

  
  /** \brief Returns a pointer to the paint device.

  See the documentation to the getPainter() method for an explanation

  @see getPainter
  
  @warning Returning a pointer twice, or returning arbitrary pointers
  will lead to a segfault.

  @param pt a pointer to a QPainter, as returned by the getPainter()
  method
  */
  virtual void returnPainter(QPainter *pt) = 0;
  
  /** \brief Returns the size of the page in pixel. */
  virtual QSize size() = 0;

  /** \brief Returns the width of the page in pixel.

  @warning If you are implementing DocumentRenderer::drawPage(), it
  may be tempting to compute the image size in pixel, using
  page->height() and page->width(). DON'T DO THAT. KViewShell uses
  transformations e.g. to rotate the page, and sets the argument
  'resolution' accordingly; these changes are not reflected in
  page->height() and page->width(). Similar problems occur if
  KViewShell required a shrunken version of the page, e.g. to print
  multiple pages on one sheet of paper. See the documentation of
  DocumentRenderer::drawPage() to learn how to compute the sizes
  properly.
  */
  virtual int width() {return size().width(); }

  /** \brief Returns the height of the page in pixel. 

  @warning If you are implementing DocumentRenderer::drawPage(), it
  may be tempting to compute the image size in pixel, using
  page->height() and page->width(). DON'T DO THAT. KViewShell uses
  transformations e.g. to rotate the page, and sets the argument
  'resolution' accordingly; these changes are not reflected in
  page->height() and page->width(). Similar problems occur if
  KViewShell required a shrunken version of the page, e.g. to print
  multiple pages on one sheet of paper. See the documentation of
  DocumentRenderer::drawPage() to learn how to compute the sizes
  properly.
  */
  virtual int height() {return size().height(); }

  /** \brief Text contained in the document page
      
  This vector contains the document text together with information
  about the position of the text on the page. This information is
  generated by the documentRenderer.drawPage() method. It is used in
  kviewshell e.g. for selecting text with the mouse or for the "export
  as text..." functionality.
  */
  QValueVector<TextBox> textBoxList;

  /** \brief Calculates the text selected by the given rectangle. */
  TextSelection select(const QRect&);

  /** \brief Selects the character which lies at the given point. */
  TextSelection select(const QPoint&);

  QRegion selectedRegion(const TextSelection& selection);

  /** Finds the first occurence of str starting by the current index.
  If the text is found a corresponding TextSelection is returned.
  If the text is not found a empty selection is returned. */
  TextSelection find(const QString& str, int index = 0, bool caseSensitive = true);

  /** Finds the first occurence of str starting by the current index searching backwards.
  If the text is found a corresponding TextSelection is returned.
  If the text is not found a empty selection is returned.
  If index < 0 start the search at the end of the page. */
  TextSelection findRev(const QString& str, int index = 0, bool cs = true);

  /** \brief Hyperlinks on the document page

  This vector contains the hyperlinks that appear on the page. This
  information is generated by the documentRenderer.drawPage()
  method. It is used in kviewshell so users can use the mouse to
  navigate in the document through hyperlinks.
  */
  QValueVector<Hyperlink> hyperLinkList;

  // set to 'false' in the constructor, set to 'true' by the renderer,
  // if something really has been rendered
  bool isEmpty;

  /** \brief Clears the data structures
      
  Clears the contents of the class, but leaves pageNumber intact. For
  performance reasons, it does not free the memory for the
  QValueVectors so that lengthy re-allocations won't be necessary
  later.
  */
  virtual void clear();
  
 private:
  /** \brief Page number

  Number of the page that this instance represents a value of '0'
  means that this class does not represent any page.
  */
  PageNumber  pageNr;

  QString pageText;
};


#endif
