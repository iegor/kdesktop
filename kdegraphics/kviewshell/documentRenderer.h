// -*- C++ -*-
//
// Class: documentRenderer
//
// Abstract class for rendering document types.  Needs to be
// subclassed by the actual parts using kviewshell.  Part of
// KViewshell - A generic interface for document viewers.
//
// (C) 2004-2005 Wilfried Huss, Stefan Kebekus. Distributed under the GPL.
//

#ifndef DOCUMENTRENDERER_H
#define DOCUMENTRENDERER_H

#include "bookmark.h"
#include "pageNumber.h"
#include "pageSize.h"

#include <qguardedptr.h>
#include <qcolor.h>
#include <qmutex.h>
#include <qobject.h>
#include <qvaluevector.h>

class Anchor;
class KURL;
class RenderedDocumentPage;


/** \brief loads and renders documents

This abstract class is one of the two core classes that must be
implemented by all authors who write plugins for the kviewshell
programm. It is responsible for document loading and rendering. As a
minimum, the setFile() and drawPage() must be reimplemented.

This documentation mentiones only the methods and members that are
important for authors of plugins. For full documentation, consult the
header file documentRenderer.h.

@warning Future versions of kviewshell will use threading to keep the
GUI responsive while pages are rendered. As a result, IT IS ABSOLUTELY
NECESSARY that your implementation is THREAD-SAFE, if not, this can
result in infrequent and very hard-to-find crashes of your
programm. Use the member mutex to make your implemetation
thread-safe.

@author Wilfried Huss, Stefan Kebekus
*/

class DocumentRenderer : public QObject
{
  Q_OBJECT

public:
  /** \brief default constructor */
  DocumentRenderer(QWidget* parent);

  virtual ~DocumentRenderer();

  /** \brief loading of files

  This is a purely virtual method that must be re-implemented. It is
  called when a file should be loaded. The implementation must then do
  the following
  
  - initialize internal data structures so the document pointed to by
  'fname' can be rendered quickly. It is not necessary actually load
  the file; if the implementation choses to load only parts of a large
  file and leave the rest on the disc, this is perfectly fine.
  
  - return 'true' on success and 'false' on failure. Before 'false' is
  returned, the method clear() should be called
  
  When the method returns 'true', it is expected that
  
  - the member _isModified is set to 'false'

  - the member 'numPages' is either set to 0 if the document is empty,
  or else to the number of page in the document
  
  - the vector pageSizes *must* either be empty (e.g. if your file
  format does not specify a page size), or must be of size
  totalPages(), and contain the sizes of all pages in the document.
  
  - the anchorList is filled with data; it is perfectly fine to leave
  the anchorList empty, if your file format does not support anchors,
  or if your document doesn't contain any.
  
  - the list 'bookmarks' is filled with data; it is perfectly fine to
  leave this list empty, if your file format does not support
  bookmarks or if your document doesn't contain any.
  
  - the method drawPage() works
  
  @note It is perfectly possible that setFile() is called several
  times in a row, with the same or with different filenames.
  
  @warning The signal documentIsChanged() must not be emitted while
  the method runs.

  @warning Future versions of kviewshell will use threading to keep
  the GUI responsive while pages are rendered. As a result, IT IS
  ABSOLUTELY NECESSARY that your implementation is THREAD-SAFE, if
  not, this can result in infrequent and very hard-to-find crashes of
  your programm. Use the member mutex to make your implemetation
  thread-safe.

  @param fname name of the file to open. It is not guaranteed that the
  file exists, that it is a file, or that it is readable.

  @param base original URL of the file that was opened. 

  If the program that uses this documentRenderer was asked to open
  http://www.kde.org/test.dvi.bz2, then the program would download the
  file to a temporary file and decompress it, generating
  e.g. /tmp/tmp.dvi. In that case, base would be
  http://www.kde.org/test.dvi.bz2, and fname=/tmp/tmp.dvi. The base
  can be used by the documentRenderer to handle relative URLs that
  might be contained in a file. Otherwise, it can safely be ignored.

  @returns 'true' on success and 'false' on failure. Even after this
  method returns 'false' the class must act reasonably, i.e. by
  clear()ing the document
  */
  virtual bool setFile(const QString &fname, const KURL &base) = 0;


  /** \brief clearing the document renderer

  This method clears the renderer, so that it represents an empty
  document. The standard implementation doe the following:

  - sets 'numPages' to zero

  - clears the pageSizes and the anchorList

  -  sets _isModified to 'false'

  Most authors of kviewshell-plugins will probably want to
  re-implement this to clear internal data structures of their
  implementations. 

  @warning Future versions of kviewshell will use threading to keep
  the GUI responsive while pages are rendered. As a result, IT IS
  ABSOLUTELY NECESSARY that your implementation is THREAD-SAFE, if
  not, this can result in infrequent and very hard-to-find crashes of
  your programm. Use the member mutex to make your implemetation
  thread-safe.
  */
  virtual void clear();


  /* Returns true if the current document contains 0 pages. */
  bool isEmpty() const {return numPages == 0;}

  /* Tells if the document was modified after is was loaded. */
  bool isModified() const {return _isModified; }

  /* Returns the number of pages in the document. This method can well
     return 0, e.g. if no document has been loaded yet, or if the
     current document is empty. */
  PageNumber totalPages() const {return numPages; }

  QPtrList<Bookmark> getBookmarks() const { return bookmarks; }

  /* Returns the size of page 'page'. If the document is empty, if the
     page specified is not a page of the document or if the document
     does not specify a size (which happens, e.g., for some
     DVI-files), then an invalid page size is returned. */
  SimplePageSize sizeOfPage(const PageNumber& page);

  /* Returns true if the document specifies page sizes, and false
     otherwise. NOTE: the information returned by this method is not
     always 100% reliable. Although unlikely, it is theoretically
     possible that this method returns 'true', but still some of the
     sizes returned by sizeOfPage() are invalid. */
  bool hasSpecifiedPageSizes() const {return !pageSizes.isEmpty();}

  /** rendering of documents

  This purely virtual method is the most important method in the
  DocumentRenderer class. It must be re-implemented by authors who
  want to write plugins for the kviewshell program. The purpose of
  this method is to render a graphical representation into a
  documentPage structure. More specifically, the implementation needs
  to

  - call the documentPage::clear() on *page

  and the do all of the following, in no particular order

  - obtain the pointer to the QPaintDevice from the documentPage using
    the documentPage::getPaintDevice() method and draw a graphical
    representation of the page number page->getPageNumber() into the
    QPaintDevice, using the given resolution. If the member
    accessibilityBackground is true, the accessibilityBackgroundColor
    should be used for a background color, if possible. Otherwise,
    white should be used, if possible. If you need to compute the size
    of the page in pixel, do this as follows:
    @code
    SimplePageSize psize = pageSizes[page->getPageNumber() - 1];
    if (psize.isValid()) {
      int width_in_pixel   = resolution * psize.width().getLength_in_inch();
      int height_in_pixel  = resolution * psize.height().getLength_in_inch();

      <...>
    }
    @endcode
    Don't use page->height() or page->width() to calculate the sizes
    ---KViewShell uses transformations e.g. to rotate the page, and
    sets the argument 'resolution' accordingly; these changes are not
    reflected in page->height() and page->width(). Similar problems
    occur if KViewShell required a shrunken version of the page,
    e.g. to print multiple pages on one sheet of paper.
  
  - if your document contains hyperlinks, fill the
    documentPage::hyperLinkList with HyperLinks, using pixel
    coordinates for the coordinates in the Hyperlink::box member of
    the Hyperlink.  The Hyperlink::baseline member of the Hyperlink
    can be ignored. The linkText member of the Hyperlink should either
    be an absolute URL ("http://www.kde.org"), or be of the form
    "#anch", where the string "anch" is contained in the anchorList.
  
  - if your plugin supports full-text information, fill
    documentPage::textLinkList with HyperLinks, using pixel
    coordinates for the coordinates in the Hyperlink::box and
    Hyperlink::baseline members of the Hyperlink. The entries in the
    documentPage::textLinkList should have a natural ordering, "first
    text first" (left-to-right, up-to-down for western languages,
    right-to-left for hebrew, etc.). This is important so that text
    selection with the mouse works properly, and only continuous
    blocks of text can be selected.

  @note This method is often called in a paintEvent, so that care must
  be taken to return as soon as possible. No user interaction should
  be done during the execution.

  @note If your plugin supports full-text information, you probably
  want to re-implement the method supportsTextSearch() below.

  @warning As mentioned above, it may be tempting to compute the image
  size in pixel, using page->height() and page->width(). DON'T DO
  THAT. KViewShell uses transformations e.g. to rotate the page, and
  sets the argument 'resolution' accordingly; these changes are not
  reflected in page->height() and page->width(). Similar problems
  occur if KViewShell required a shrunken version of the page, e.g. to
  print multiple pages on one sheet of paper.

  @warning The signal documentIsChanged() must not be emitted while the
  method runs.

  @warning Future versions of kviewshell will use threading to keep
  the GUI responsive while pages are rendered. As a result, IT IS
  ABSOLUTELY NECESSARY that your implementation is THREAD-SAFE, if
  not, this can result in infrequent and very hard-to-find crashes of
  your programm. Use the member mutex to make your implemetation
  thread-safe.

  @param resolution this argument contains the resolution of the
  display device. In principle. In fact, kviewshell implements zooming
  by using values that are not exactly the resolution of the display,
  but multiplied with the zoom factor. Bottom line: the
  DocumentRenderer should act as if this field indeed contains
  resolution of the display device.

  @param page pointer to a documentPage structure that this method
  rendered into.

  */
  virtual void drawPage(double resolution, RenderedDocumentPage* page) = 0;

  /** rendering of documents at thumbnail size
      
  This method is used to draw thumbnails. The standared
  implementations just calls 'drawPage' to do the job. Reimplement
  this if the used fileformat has embedded thumbnails.

  @warning Future versions of kviewshell will use threading to keep
  the GUI responsive while pages are rendered. As a result, IT IS
  ABSOLUTELY NECESSARY that your implementation is THREAD-SAFE, if
  not, this can result in infrequent and very hard-to-find crashes of
  your programm. Use the member mutex to make your implemetation
  thread-safe.
  */
  virtual void drawThumbnail(double resolution, RenderedDocumentPage* page);

  /** quick extraction of text information

  This method returns the textinformation of the current page if available.
  It is only called when the page pixmap is not of interest, so it is possible
  to implement it much more efficiently then the drawPage() method.

  The default implementation just calls drawPage().

  @warning Future versions of kviewshell will use threading to keep
  the GUI responsive while pages are rendered. As a result, IT IS
  ABSOLUTELY NECESSARY that your implementation is THREAD-SAFE, if
  not, this can result in infrequent and very hard-to-find crashes of
  your programm. Use the member mutex to make your implemetation
  thread-safe.

  @param page pointer to a documentPage structure that this method rendered into.
  */
  virtual void getText(RenderedDocumentPage* page);

  /** Flag to indicate if full text is supported

  If your implementation of the drawPage() method supports full-text
  information and writes to the documentPage::textLinkList, this
  method should be re-implemented to return 'true'. The text-search
  and text-selection utilities will then be enabled in the GUI.

  The default implementation returns 'false'.
  */
  virtual bool supportsTextSearch() const {return false;}
  
  /* This method will try to parse the reference part of the DVI
     file's URL, (either a number, which is supposed to be a page
     number, or src:(line)(filename)) and see if a corresponding
     section of the DVI file can be found. If so, it returns an anchor
     to that section. If not, it returns an invalid anchor. 
  */
  virtual Anchor        parseReference(const QString &reference);
  
  /* Looks up a anchor in the "anchorList". Returns the anchor found,
     or an invalid anchor otherwise. 
  */
  Anchor findAnchor(const QString &);
  
  /* Quick file validity check

  This method is used internally, to check if a file is valid before
  it is re-loaded. This is used e.g. by kdvi: when a the user TeXes a
  file, the file changes immediately. If the 'watch file' option is
  set, kdvi is informed immediately. At that time, however, the TeX
  typesetting program still writes to the dvi file, and reloading must
  be postphoned till TeX finishes, and the dvi file becomes vaild. If
  such considerations are not an issue for you, this method does not
  need to be re-implemented.
  
  @warning Future versions of kviewshell will use threading to keep
  the GUI responsive while pages are rendered. As a result, IT IS
  ABSOLUTELY NECESSARY that your implementation is THREAD-SAFE, if
  not, this can result in infrequent and very hard-to-find crashes of
  your programm. Use the member mutex to make your implemetation
  thread-safe.

  @param fileName name of the file that should be checked for validity

  @returns 'false' if the file 'fileName' is obviously invalid, and
  true otherwise. The default implementation always returns
  'true'. 
  */
  virtual bool isValidFile(const QString& fileName) const;

  void setAccessibleBackground(bool accessibleMode, const QColor& background = QColor(255, 255, 255));

signals:
  /** signals that the document is changed

  This signal can be emitted if the document or status of this class
  changed internally so that all associated widgets should be
  repainted. This could be emitted, e.g. if pages are removed from a
  document, or if some preferences change that have some direct
  influence on the way the document is rendered.

  When this signal is emitted, the whole GUI setup is re-computed, and
  all widgets are re-drawn. This can take considerable time.
  */
  void documentIsChanged();


  /** sets text in the statusbar

  This signal is emitted when the renderer needs to inform the user
  via the status bar. Since the status bar is not always visible, and
  since the duration that the message is shown is not quite specified,
  this should not be used for important information. */
  void setStatusBarText( const QString& );

protected:
  /** mutex used to make method thread-safe

  This is a recursive mutex that must be used to make the public
  methods of this class thread-safe. Future versions of kviewshell
  will use threading to keep the GUI responsive while pages are
  rendered. As a result, IT IS ABSOLUTELY NECESSARY that your
  implementation is THREAD-SAFE, if not, this can result in infrequent
  and very hard-to-find crashes of your programm.
  */
  QMutex mutex;


  /** number of pages in the document

  This member is set by the implementations of the setFile()
  method. It is set to zero by the constructor and in clear(). 

  @warning Only the constructor and the methods setFile() and clear()
  may write to this member.
  */
  Q_UINT16  numPages;
  
  /** page sizes

  This vector contains the size of every page in the document. To
  accomodate for file format that do not specify a page size, it is
  explicitly allowed that this vector is empty, or that entries are
  invalid page sizes. The values in this vector are set by the
  setFile() method.

  @note if the document does not specify page sizes, this vector
  should --for performance reasons-- be empty, and not set to a large
  number of invalid page sizes.

  @warning Only the constructor and the methods setFile() and clear()
  may write to this member.
  */
  QValueVector<SimplePageSize> pageSizes;
  
  /** bookmarks

  This (ordered!) list contains the bookmarks that are contained in
  the document. The values in this vector are set by the setFile()
  method, and cleared by the constructor and the clear() method.

  @warning Only the constructor and the methods setFile() and clear()
  may write to this member.
  */
  QPtrList<Bookmark> bookmarks;

  /** map of anchors in a document. 

  This map contains the anchors that are contained in the
  document. The values in this map are set by the setFile() method,
  and cleared by the constructor and the clear() method.

  @warning Only the constructor and the methods setFile() and clear()
  may write to this member.
  */
  QMap<QString, Anchor> anchorList;

  /** pointer to the parent widget

  This pointer can be used by implementations e.g. to display error
  messages. This pointer can well be zero.
  */
  QGuardedPtr<QWidget> parentWidget;

  /** specifies if accessibilityBackgroundColor should be used

  If true, the drawPage() and drawThumbnail() methods should use
  accessibilityBackgroundColor as the backgroundcolor of the
  pages. 
  */
  bool accessibilityBackground;

  /** background color, to be used for visibly impaired users

  If accessibilityBackground is true, the drawPage() and
  drawThumbnail() methods should use this color as the backgroundcolor
  of the pages.
  */
  QColor accessibilityBackgroundColor;

  /** Flag if document is modified 

  This flag indicates if the document was modified after it was
  loaded. It is set to 'false' in the constructor, in the clear() and
  setFile() method. It can be set to 'true' be methods that modify the
  document (e.g. the deletePages() method of the djvu implementation
  of this class).
   */
  bool _isModified;
};

#endif
