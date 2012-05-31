// -*- C++ -*-
//
// Class: documentPageCache
//
// Cache that holds a number of pages in a document.
// Part of KDVI- A previewer for TeX DVI files.
//
// (C) 2004 Stefan Kebekus. Distributed under the GPL.

#ifndef _documentpagecache_h_
#define _documentpagecache_h_

#include "renderedDocumentPagePixmap.h"
#include "pageNumber.h"
#include "pageSize.h"
#include "selection.h"

#include <qcache.h>
#include <qguardedptr.h>
#include <qobject.h>

class DocumentRenderer;
class QPixmap;
class RenderedDocumentPage;


class DocumentPageCache: public QObject
{
 Q_OBJECT

 public:
  DocumentPageCache();
  virtual ~DocumentPageCache();

  /** This method is used to name the DocumentRenderer, that the
      documentPageCache uses to render the page. The renderer can be
      used any time (e.g., triggered by an internal timer event), and
      must not be deleted before either the DocumentRenderer is
      deleted, or another renderer has been set. */
  void     setRenderer(DocumentRenderer *_renderer);

  void     setResolution(double res);
  double   getResolution() const {return resolutionInDPI;}

  const TextSelection& selectedText() const { return userSelection; }

  void deselectText();
  void selectText(const TextSelection& selection);

  /** Returns the size of page 'page'. If the document does not
      specify a size (which happens, e.g., for some DVI-files), then
      the userPreferredSize is returned. */
  SimplePageSize sizeOfPage(const PageNumber& page = 1) const;

  /** Returns the size of page 'page', in pixels, using the resolution
      set with setResolution(). If the document does not specify a
      size (which happens, e.g., for some DVI-files), the
      userPreferredSize is used. */
  QSize sizeOfPageInPixel(const PageNumber& page) const;

  /** Returns a pointer to a documentPage structure, or 0, if the
      documentPage could not be generated for some reason (e.g.,
      because no renderer has been set). */
  RenderedDocumentPagePixmap* getPage(const PageNumber& pageNr);

  /** Checks if the given page is already in the cache. */
  bool isPageCached(const PageNumber& pageNumber, const QSize& size);

  /** Checks if the given page is already in the cache. Here we don't care about the size
      of the page. */
  bool isPageCached(const PageNumber& pageNumber);

  /** Returns a "width" pixels width thumbnail of the given page. This
      method returns an empty QPixmap if one of the arguments is
      invalid, or if the page cannot be rendered for any reason. */
  QPixmap createThumbnail(const PageNumber& pageNr, int width);

 signals:
  /** This signal is emitted when setUserPreferredSize() or
      setUseUserPreferredSize() is called, and the page size
      changes accordingly. */
  void paperSizeChanged();

  /** This signal is emitted when the text selection of the current
      document changes. The argument is false if no text is selected, true
      otherwise. */
  void textSelected(bool);

 public slots:
  /** Clears the contents of the cache. */
  void clear();

  /** Sets the userPreferredSize, which is explained below */
  void setUserPreferredSize(const SimplePageSize& t);
  void setUseDocumentSpecifiedSize(bool);

 protected:
  /** This function creates new RenderedDocumentPagePixmap objects.
      If a multipage implementation needs additional functionality overwrite
      this function to create objects of a suitable subclass of RenderedDocumentPagePixmap.
  */
  virtual RenderedDocumentPagePixmap* createDocumentPagePixmap() const;

  /** Creates the hashing key for the cache. */
  QString createKey(const PageNumber& pageNumber, const QSize& size);

  /** Creates the hashing function. The size of the page is calculated
      based on the current resolution. */
  QString createKey(const PageNumber& pageNumber);

  QGuardedPtr<DocumentRenderer> renderer;

 private:
  /** The maximum of memory used by the cache. (32MB)
      TODO: make this configurable, or detact an appropriate value at startup. */
  Q_UINT32 maxMemory;

  /** This field contains resolution of the display device. In
      principle. In fact, kviewshell implements zooming by calling the
      setResolution()-method with values that are not exactly the
      resolution of the display, but multiplied with the zoom
      factor. Bottom line: the documentRenderer should act as if this
      field indeed contains resolution of the display device. Whene a
      documentRenderer is constructed, this field is set to the actual
      resolution to give a reasonable default value. */
  double              resolutionInDPI;

  SimplePageSize      userPreferredSize;
  bool                useDocumentSpecifiedSize;

  TextSelection userSelection;


  /** This list holds the cache. */
  QCache<RenderedDocumentPagePixmap> LRUCache;

  /** This is a dummy documentPage structure which is used internally
      by the 'createThumbnail' method. */
  RenderedDocumentPagePixmap thumbnailPage;
};


#endif
